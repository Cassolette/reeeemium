// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.chrome.browser.page_info;

import androidx.test.filters.MediumTest;

import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;

import org.chromium.base.test.util.CommandLineFlags;
import org.chromium.base.test.util.Feature;
import org.chromium.chrome.browser.ChromeActivity;
import org.chromium.chrome.browser.flags.ChromeSwitches;
import org.chromium.chrome.browser.vr.VrModuleProvider;
import org.chromium.chrome.test.ChromeActivityTestRule;
import org.chromium.chrome.test.ChromeJUnit4ClassRunner;
import org.chromium.components.page_info.ConnectionInfoPopup;
import org.chromium.content_public.browser.WebContents;
import org.chromium.content_public.browser.test.util.TestThreadUtils;

/**
 * Tests for ConnectionInfoPopup.
 */
@RunWith(ChromeJUnit4ClassRunner.class)
@CommandLineFlags.Add({ChromeSwitches.DISABLE_FIRST_RUN_EXPERIENCE})
public class ConnectionInfoPopupTest {
    @Rule
    public ChromeActivityTestRule<ChromeActivity> mActivityTestRule =
            new ChromeActivityTestRule<>(ChromeActivity.class);

    /**
     * Tests that ConnectionInfoPopup can be instantiated and shown.
     */
    @Test
    @MediumTest
    @Feature({"ConnectionInfoPopup"})
    public void testShow() throws InterruptedException {
        mActivityTestRule.startMainActivityOnBlankPage();
        TestThreadUtils.runOnUiThreadBlocking(() -> {
            ChromeActivity context = mActivityTestRule.getActivity();
            WebContents webContents = context.getActivityTab().getWebContents();
            ConnectionInfoPopup.show(context, webContents, context.getModalDialogManager(),
                    VrModuleProvider.getDelegate());
        });
    }
}
