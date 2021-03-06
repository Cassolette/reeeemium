// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.chrome.browser.customtabs;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.os.Process;
import android.support.test.InstrumentationRegistry;

import org.junit.Assert;

import org.chromium.base.task.PostTask;
import org.chromium.base.test.util.CallbackHelper;
import org.chromium.chrome.R;
import org.chromium.chrome.browser.document.ChromeLauncherActivity;
import org.chromium.content_public.browser.UiThreadTaskTraits;
import org.chromium.content_public.browser.test.util.CriteriaHelper;
import org.chromium.content_public.browser.test.util.TestThreadUtils;

import java.util.concurrent.TimeoutException;
import java.util.concurrent.atomic.AtomicReference;

import androidx.browser.customtabs.CustomTabsCallback;
import androidx.browser.customtabs.CustomTabsClient;
import androidx.browser.customtabs.CustomTabsIntent;
import androidx.browser.customtabs.CustomTabsServiceConnection;
import androidx.browser.customtabs.CustomTabsSession;

/**
 * Utility class that contains convenience calls related with custom tabs testing.
 */
public class CustomTabsTestUtils {
    /** Intent extra to specify an id to a custom tab.*/
    public static final String EXTRA_CUSTOM_TAB_ID =
            "android.support.customtabs.extra.tests.CUSTOM_TAB_ID";

    /** A plain old data class that holds the return value from {@link #bindWithCallback}. */
    public static class ClientAndSession {
        public final CustomTabsClient client;
        public final CustomTabsSession session;

        /** Creates and populates the class. */
        public ClientAndSession(CustomTabsClient client, CustomTabsSession session) {
            this.client = client;
            this.session = session;
        }
    }

    /**
     * Creates the simplest intent that is sufficient to let {@link ChromeLauncherActivity} launch
     * the {@link CustomTabActivity}.
     */
    public static Intent createMinimalCustomTabIntent(
            Context context, String url) {
        CustomTabsIntent.Builder builder = new CustomTabsIntent.Builder(
                CustomTabsSession.createMockSessionForTesting(
                        new ComponentName(context, ChromeLauncherActivity.class)));
        CustomTabsIntent customTabsIntent = builder.build();
        Intent intent = customTabsIntent.intent;
        intent.setAction(Intent.ACTION_VIEW);
        intent.setData(Uri.parse(url));
        intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        return intent;
    }

    public static CustomTabsConnection setUpConnection() {
        CustomTabsConnection connection = CustomTabsConnection.getInstance();
        connection.resetThrottling(Process.myUid());
        return connection;
    }

    public static void cleanupSessions(final CustomTabsConnection connection) {
        TestThreadUtils.runOnUiThreadBlocking(connection::cleanupAllForTesting);
    }

    public static ClientAndSession bindWithCallback(final CustomTabsCallback callback)
            throws TimeoutException {
        final AtomicReference<CustomTabsSession> sessionReference = new AtomicReference<>();
        final AtomicReference<CustomTabsClient> clientReference = new AtomicReference<>();
        final CallbackHelper waitForConnection = new CallbackHelper();
        CustomTabsClient.bindCustomTabsService(InstrumentationRegistry.getContext(),
                InstrumentationRegistry.getTargetContext().getPackageName(),
                new CustomTabsServiceConnection() {
                    @Override
                    public void onServiceDisconnected(ComponentName name) {}

                    @Override
                    public void onCustomTabsServiceConnected(
                            ComponentName name, CustomTabsClient client) {
                        clientReference.set(client);
                        sessionReference.set(client.newSession(callback));
                        waitForConnection.notifyCalled();
                    }
                });
        waitForConnection.waitForCallback(0);
        return new ClientAndSession(clientReference.get(), sessionReference.get());
    }

    /** Calls warmup() and waits for all the tasks to complete. Fails the test otherwise. */
    public static CustomTabsConnection warmUpAndWait() throws TimeoutException {
        CustomTabsConnection connection = setUpConnection();
        final CallbackHelper startupCallbackHelper = new CallbackHelper();
        CustomTabsSession session = bindWithCallback(new CustomTabsCallback() {
            @Override
            public void extraCallback(String callbackName, Bundle args) {
                if (callbackName.equals(CustomTabsConnection.ON_WARMUP_COMPLETED)) {
                    startupCallbackHelper.notifyCalled();
                }
            }
        }).session;
        Assert.assertTrue(connection.warmup(0));
        startupCallbackHelper.waitForCallback(0);
        return connection;
    }

    public static void openAppMenuAndAssertMenuShown(CustomTabActivity activity) {
        PostTask.runOrPostTask(UiThreadTaskTraits.DEFAULT,
                () -> { activity.onMenuOrKeyboardAction(R.id.show_menu, false); });

        CriteriaHelper.pollUiThread(activity.getRootUiCoordinatorForTesting()
                                            .getAppMenuCoordinatorForTesting()
                                            .getAppMenuHandler()::isAppMenuShowing,
                "App menu was not shown");
    }
}
