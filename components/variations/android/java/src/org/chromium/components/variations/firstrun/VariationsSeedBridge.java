// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.components.variations.firstrun;

import android.util.Base64;

import org.chromium.base.ContextUtils;
import org.chromium.base.annotations.CalledByNative;
import org.chromium.base.metrics.RecordHistogram;
import org.chromium.components.variations.firstrun.VariationsSeedFetcher.SeedInfo;

import java.text.ParseException;

/**
 * VariationsSeedBridge is a class which is used to pass variations first run seed that was fetched
 * before the actual Chrome first run to Chromium core. Class provides methods to store the seed
 * in SharedPreferences and to get the seed from there. To store raw seed data class serializes
 * byte[] to Base64 encoded string and decodes this string before passing to C++ side.
 */
public class VariationsSeedBridge {
    protected static final String VARIATIONS_FIRST_RUN_SEED_BASE64 = "variations_seed_base64";
    protected static final String VARIATIONS_FIRST_RUN_SEED_SIGNATURE = "variations_seed_signature";
    protected static final String VARIATIONS_FIRST_RUN_SEED_COUNTRY = "variations_seed_country";
    protected static final String VARIATIONS_FIRST_RUN_SEED_DATE_HEADER = "variations_seed_date";
    protected static final String VARIATIONS_FIRST_RUN_SEED_DATE = "variations_seed_date_ms";
    protected static final String VARIATIONS_FIRST_RUN_SEED_IS_GZIP_COMPRESSED =
            "variations_seed_is_gzip_compressed";

    // This pref is used to store information about successful seed storing on the C++ side, in
    // order to not fetch the seed again.
    protected static final String VARIATIONS_FIRST_RUN_SEED_NATIVE_STORED =
            "variations_seed_native_stored";

    // These must be kept in sync with VariationsFirstRunPrefEvents in enums.xml.
    private static final int DEBUG_PREFS_STORED = 0;
    private static final int DEBUG_PREFS_CLEARED = 1;
    private static final int DEBUG_PREFS_RETRIEVED_DATA_EMPTY = 2;
    private static final int DEBUG_PREFS_RETRIEVED_DATA_NON_EMPTY = 3;
    private static final int DEBUG_PREFS_CLEARED_NON_EMPTY = 4;
    private static final int DEBUG_PREFS_MAX = 5;

    // TODO(crbug.com/1090968): Debug histogram to investigate a regression. Remove when resolved.
    private static void logDebugHistogram(int value) {
        RecordHistogram.recordEnumeratedHistogram(
                "Variations.FirstRunPrefsDebug", value, DEBUG_PREFS_MAX);
    }

    protected static String getVariationsFirstRunSeedPref(String prefName) {
        return ContextUtils.getAppSharedPreferences().getString(prefName, "");
    }

    /**
     * Stores variations seed data (raw data, seed signature and country code) in SharedPreferences.
     * CalledByNative attribute is used by unit tests code to set test data.
     */
    @CalledByNative
    public static void setVariationsFirstRunSeed(
            byte[] rawSeed, String signature, String country, long date, boolean isGzipCompressed) {
        ContextUtils.getAppSharedPreferences()
                .edit()
                .putString(VARIATIONS_FIRST_RUN_SEED_BASE64,
                        Base64.encodeToString(rawSeed, Base64.NO_WRAP))
                .putString(VARIATIONS_FIRST_RUN_SEED_SIGNATURE, signature)
                .putString(VARIATIONS_FIRST_RUN_SEED_COUNTRY, country)
                .putLong(VARIATIONS_FIRST_RUN_SEED_DATE, date)
                .putBoolean(VARIATIONS_FIRST_RUN_SEED_IS_GZIP_COMPRESSED, isGzipCompressed)
                .apply();
        logDebugHistogram(DEBUG_PREFS_STORED);
    }

    @CalledByNative
    private static void clearFirstRunPrefs() {
        if (hasJavaPref()) {
            logDebugHistogram(DEBUG_PREFS_CLEARED_NON_EMPTY);
        }
        ContextUtils.getAppSharedPreferences()
                .edit()
                .remove(VARIATIONS_FIRST_RUN_SEED_BASE64)
                .remove(VARIATIONS_FIRST_RUN_SEED_SIGNATURE)
                .remove(VARIATIONS_FIRST_RUN_SEED_COUNTRY)
                .remove(VARIATIONS_FIRST_RUN_SEED_DATE)
                .remove(VARIATIONS_FIRST_RUN_SEED_DATE_HEADER)
                .remove(VARIATIONS_FIRST_RUN_SEED_IS_GZIP_COMPRESSED)
                .apply();
        logDebugHistogram(DEBUG_PREFS_CLEARED);
    }

    /**
     * Returns the status of the variations first run fetch: was it successful or not.
     */
    public static boolean hasJavaPref() {
        return !ContextUtils.getAppSharedPreferences()
                        .getString(VARIATIONS_FIRST_RUN_SEED_BASE64, "")
                        .isEmpty();
    }

    /**
     * Returns the status of the variations seed storing on the C++ side: was it successful or not.
     */
    public static boolean hasNativePref() {
        return ContextUtils.getAppSharedPreferences().getBoolean(
                VARIATIONS_FIRST_RUN_SEED_NATIVE_STORED, false);
    }

    @CalledByNative
    private static void markVariationsSeedAsStored() {
        ContextUtils.getAppSharedPreferences()
                .edit()
                .putBoolean(VARIATIONS_FIRST_RUN_SEED_NATIVE_STORED, true)
                .apply();
    }

    @CalledByNative
    private static byte[] getVariationsFirstRunSeedData() {
        byte[] data = Base64.decode(
                getVariationsFirstRunSeedPref(VARIATIONS_FIRST_RUN_SEED_BASE64), Base64.NO_WRAP);
        logDebugHistogram(data.length == 0 ? DEBUG_PREFS_RETRIEVED_DATA_EMPTY
                                           : DEBUG_PREFS_RETRIEVED_DATA_NON_EMPTY);
        return data;
    }

    @CalledByNative
    private static String getVariationsFirstRunSeedSignature() {
        return getVariationsFirstRunSeedPref(VARIATIONS_FIRST_RUN_SEED_SIGNATURE);
    }

    @CalledByNative
    private static String getVariationsFirstRunSeedCountry() {
        return getVariationsFirstRunSeedPref(VARIATIONS_FIRST_RUN_SEED_COUNTRY);
    }

    @CalledByNative
    private static long getVariationsFirstRunSeedDate() {
        long date =
                ContextUtils.getAppSharedPreferences().getLong(VARIATIONS_FIRST_RUN_SEED_DATE, 0);
        if (date > 0) return date;
        // VARIATIONS_FIRST_RUN_SEED_DATE_HEADER is deprecated in favor of
        // VARIATIONS_FIRST_RUN_SEED_DATE, but fall back on the old value in case the prefs were
        // written by an old version.
        // TODO(crbug.com/1013390): Remove this fallback logic.
        String header = getVariationsFirstRunSeedPref(VARIATIONS_FIRST_RUN_SEED_DATE_HEADER);
        if (header.isEmpty()) return 0;
        try {
            return SeedInfo.parseDateHeader(header);
        } catch (ParseException e) {
            // Shouldn't happen as the date will have been verified in VariationsSeedFetcher.
            throw new RuntimeException("Invalid date in first run seed pref", e);
        }
    }

    @CalledByNative
    private static boolean getVariationsFirstRunSeedIsGzipCompressed() {
        return ContextUtils.getAppSharedPreferences().getBoolean(
                VARIATIONS_FIRST_RUN_SEED_IS_GZIP_COMPRESSED, false);
    }
}
