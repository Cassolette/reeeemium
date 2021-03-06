// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.chrome.browser.offlinepages.indicator;

import android.os.Handler;
import android.os.SystemClock;
import android.text.TextUtils;

import androidx.annotation.VisibleForTesting;

import org.chromium.base.ApplicationState;
import org.chromium.base.ApplicationStatus;
import org.chromium.base.Callback;
import org.chromium.base.supplier.Supplier;
import org.chromium.chrome.browser.flags.ChromeFeatureList;
import org.chromium.chrome.browser.offlinepages.indicator.ConnectivityDetector.ConnectionState;
import org.chromium.components.variations.VariationsAssociatedData;

/**
 * Class that detects if the network is offline. Waits for the network to stablize before notifying
 * the observer.
 */
class OfflineDetector
        implements ConnectivityDetector.Observer, ApplicationStatus.ApplicationStateListener {
    // If the connection is online, then we report that immediately via |mCallback|.
    // |STATUS_INDICATOR_WAIT_ON_OFFLINE_DURATION_MS| and
    // |mStatusIndicatorWaitOnSwitchOnlineToOfflineDurationMs| control the duration before
    // we report the device as offline. It's important to wait a bit before reporting connection as
    // offline since some devices may take time to establish the connection. In such cases,
    // reporting connection as offline could cause confusion to the user. Setting this to a large
    // value has a downside that if the device is actually offline, then it would take us long time
    // to report the connection as offline.

    // |STATUS_INDICATOR_WAIT_ON_OFFLINE_DURATION_MS| is the duration before we wait before
    // reporting the connection type as offline if the app has never been on an online connection.
    // In this case, we need to wait a shorter time before
    // invoking |mCallback| since the actual connection change to offline happened  much earlier
    // than when the app received the notification. Any delays in app receiving the notification of
    // connection change are only due to device's CPU constraints.
    static final long STATUS_INDICATOR_WAIT_ON_OFFLINE_DURATION_MS = 2000;

    // |mStatusIndicatorWaitOnSwitchOnlineToOfflineDurationMs| is the duration before we wait before
    // reporting the connection type as offline if the app has been on an online connection before.
    // In this case, we need to wait a longer time before invoking |mCallback| since the connection
    // change is ongoing. Any delays in app receiving the notification of connection change are due
    // to time taken by device in reestablishing the connection as well as device CPU constraints.
    // Value of |mStatusIndicatorWaitOnSwitchOnlineToOfflineDurationMs| is set to
    // |STATUS_INDICATOR_WAIT_ON_SWITCH_ONLINE_TO_OFFLINE_DEFAULT_DURATION_MS| by default, but can
    // be overridden using finch.
    static final long STATUS_INDICATOR_WAIT_ON_SWITCH_ONLINE_TO_OFFLINE_DEFAULT_DURATION_MS = 10000;
    final long mStatusIndicatorWaitOnSwitchOnlineToOfflineDurationMs;

    private static ConnectivityDetector sMockConnectivityDetector;
    private static Supplier<Long> sMockElapsedTimeSupplier;

    private ConnectivityDetector mConnectivityDetector;

    // Maintains if the connection is effectively offline.
    // Effectively offline means that all checks have been passed and the
    // |mCallback| has been invoked to notify the observers.
    private boolean mIsEffectivelyOffline;

    // True if the network is offline as detected by the connectivity detector.
    private boolean mIsOfflineLastReportedByConnectivityDetector;

    private Handler mHandler;
    private Runnable mUpdateOfflineStatusIndicatorDelayedRunnable;
    private final Callback<Boolean> mCallback;

    // Current state of the application.
    private int mApplicationState = ApplicationStatus.getStateForApplication();

    // Time when the application was last foregrounded. |callback| is invoked only when the app is
    // in foreground.
    private long mTimeWhenLastForegrounded;

    // Time when the connection was last reported as offline. |callback| is invoked only when the
    // connection has been in the ofline for |STATUS_INDICATOR_WAIT_ON_OFFLINE_DURATION_MS|.
    private long mTimeWhenLastOfflineNotificationReceived;

    // True if the |mConnectivityDetector| has been initialized.
    private boolean mConnectivityDetectorInitialized;

    // Last time when the device was online. Updated when we detect that the device is switching
    // from "online" to "offline" or when we are notified that the device is online" at the end.
    private long mTimeWhenLastOnline;

    /**
     * Constructs the offline indicator.
     * @param callback The {@link callback} is invoked when the connectivity status is stable and
     *         has changed.
     */
    OfflineDetector(Callback<Boolean> callback) {
        mCallback = callback;
        mHandler = new Handler();
        mStatusIndicatorWaitOnSwitchOnlineToOfflineDurationMs = getIntParamValueOrDefault(
                "STATUS_INDICATOR_WAIT_ON_SWITCH_ONLINE_TO_OFFLINE_DEFAULT_DURATION_MS",
                STATUS_INDICATOR_WAIT_ON_SWITCH_ONLINE_TO_OFFLINE_DEFAULT_DURATION_MS);

        mUpdateOfflineStatusIndicatorDelayedRunnable = () -> {
            // |callback| is invoked only when the app is in foreground. If the app is in
            // background, return early. When the app comes to foreground,
            // |mUpdateOfflineStatusIndicatorDelayedRunnable| would be posted.
            if (mApplicationState != ApplicationState.HAS_RUNNING_ACTIVITIES) {
                return;
            }

            // Connection state has not changed since |mUpdateOfflineStatusIndicatorDelayedRunnable|
            // was posted.
            if (mIsOfflineLastReportedByConnectivityDetector == mIsEffectivelyOffline) {
                return;
            }
            mIsEffectivelyOffline = mIsOfflineLastReportedByConnectivityDetector;
            mCallback.onResult(mIsEffectivelyOffline);
        };

        // Register as an application state observer and initialize |mTimeWhenLastForegrounded|.
        ApplicationStatus.registerApplicationStateListener(this);
        if (mApplicationState == ApplicationState.HAS_RUNNING_ACTIVITIES) {
            mTimeWhenLastForegrounded = getElapsedTime();
        }

        if (sMockConnectivityDetector != null) {
            mConnectivityDetector = sMockConnectivityDetector;
        } else {
            mConnectivityDetector = new ConnectivityDetector(this);
        }
    }

    @Override
    public void onConnectionStateChanged(int connectionState) {
        boolean previousLastReportedStateByOfflineDetector =
                mIsOfflineLastReportedByConnectivityDetector;
        mIsOfflineLastReportedByConnectivityDetector =
                (connectionState != ConnectionState.VALIDATED);
        if (previousLastReportedStateByOfflineDetector
                == mIsOfflineLastReportedByConnectivityDetector) {
            mConnectivityDetectorInitialized = true;
            return;
        }

        if (mIsOfflineLastReportedByConnectivityDetector) {
            mTimeWhenLastOfflineNotificationReceived = getElapsedTime();
        }

        // Verify that the connectivity detector is initialized before setting
        // |mTimeWhenLastOnline|. By default, |mIsOfflineLastReportedByConnectivityDetector| is
        // false, i.e., the device is assumed to be online. Tracking
        // |mConnectivityDetectorInitialized| helps us distinguish whether the connection type has
        // switched from "default online" to "offline" or "online" to "offline".
        if ((mConnectivityDetectorInitialized && !previousLastReportedStateByOfflineDetector)
                || !mIsOfflineLastReportedByConnectivityDetector) {
            mTimeWhenLastOnline = getElapsedTime();
        }

        mConnectivityDetectorInitialized = true;

        updateState();
    }

    /*
     * Returns true if the connection is offline and the connection state has been stable.
     */
    boolean isConnectionStateOffline() {
        return mIsEffectivelyOffline;
    }

    void destroy() {
        ApplicationStatus.unregisterApplicationStateListener(this);
        if (mConnectivityDetector != null) {
            mConnectivityDetector.destroy();
            mConnectivityDetector = null;
        }
        mHandler.removeCallbacks(mUpdateOfflineStatusIndicatorDelayedRunnable);
    }

    @Override
    public void onApplicationStateChange(int newState) {
        if (mApplicationState == newState) return;

        mApplicationState = newState;

        if (mApplicationState == ApplicationState.HAS_RUNNING_ACTIVITIES) {
            mTimeWhenLastForegrounded = getElapsedTime();
        }

        updateState();
    }

    private long getElapsedTime() {
        return sMockElapsedTimeSupplier != null ? sMockElapsedTimeSupplier.get()
                                                : SystemClock.elapsedRealtime();
    }

    @VisibleForTesting
    static void setMockConnectivityDetector(ConnectivityDetector connectivityDetector) {
        sMockConnectivityDetector = connectivityDetector;
    }

    @VisibleForTesting
    static void setMockElapsedTimeSupplier(Supplier<Long> supplier) {
        sMockElapsedTimeSupplier = supplier;
    }

    @VisibleForTesting
    void setHandlerForTesting(Handler handler) {
        mHandler = handler;
    }

    /*
    ** Calls |mUpdateOfflineStatusIndicatorDelayedRunnable| to update the connection state.
    */
    private void updateState() {
        mHandler.removeCallbacks(mUpdateOfflineStatusIndicatorDelayedRunnable);

        // Do not update state while the app is in background.
        if (mApplicationState != ApplicationState.HAS_RUNNING_ACTIVITIES) return;

        // Check time since the app was foregrounded and time since the offline notification was
        // received.
        final long timeSinceLastForeground = getElapsedTime() - mTimeWhenLastForegrounded;
        final long timeSinceOfflineNotificationReceived =
                getElapsedTime() - mTimeWhenLastOfflineNotificationReceived;
        final long timeSinceLastOnline = getElapsedTime() - mTimeWhenLastOnline;

        final long timeNeededForForeground =
                STATUS_INDICATOR_WAIT_ON_OFFLINE_DURATION_MS - timeSinceLastForeground;
        final long timeNeededForOffline =
                STATUS_INDICATOR_WAIT_ON_OFFLINE_DURATION_MS - timeSinceOfflineNotificationReceived;

        // If the device has been online before, then we wait up to
        // |mStatusIndicatorWaitOnSwitchOnlineToOfflineDurationMs| duration.
        final long timeNeededAfterConnectionChangeFromOnlineToOffline = mTimeWhenLastOnline > 0
                ? mStatusIndicatorWaitOnSwitchOnlineToOfflineDurationMs - timeSinceLastOnline
                : 0;

        assert mUpdateOfflineStatusIndicatorDelayedRunnable != null;

        // If the connection is online, report the state immediately. Alternatively, if the app has
        // been in foreground and connection has been offline for sufficient time, then report the
        // state immediately.
        if (!mIsOfflineLastReportedByConnectivityDetector
                || (timeNeededForForeground <= 0 && timeNeededForOffline <= 0
                        && timeNeededAfterConnectionChangeFromOnlineToOffline <= 0)) {
            mUpdateOfflineStatusIndicatorDelayedRunnable.run();
            return;
        }

        // Wait before calling |mUpdateOfflineStatusIndicatorDelayedRunnable|.
        mHandler.postDelayed(mUpdateOfflineStatusIndicatorDelayedRunnable,
                Math.max(Math.max(timeNeededForForeground, timeNeededForOffline),
                        timeNeededAfterConnectionChangeFromOnlineToOffline));
    }

    /**
     * Returns the value for a Finch parameter, or the default value if no parameter
     * exists in the current configuration.
     * @param paramName The name of the Finch parameter (or command-line switch) to get a value
     *                  for.
     * @param defaultValue The default value to return when there's no param or switch.
     * @return The value -- either the param or the default.
     */
    private static long getIntParamValueOrDefault(String paramName, long defaultValue) {
        String value;

        // May throw exception in tests.
        try {
            value = VariationsAssociatedData.getVariationParamValue(
                    ChromeFeatureList.OFFLINE_INDICATOR_V2, paramName);
        } catch (java.lang.UnsupportedOperationException e) {
            return defaultValue;
        }

        if (!TextUtils.isEmpty(value)) {
            try {
                return Integer.parseInt(value);
            } catch (NumberFormatException e) {
                return defaultValue;
            }
        }

        return defaultValue;
    }
}
