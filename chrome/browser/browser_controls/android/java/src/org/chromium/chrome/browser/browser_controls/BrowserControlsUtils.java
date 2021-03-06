// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.chrome.browser.browser_controls;

/**
 * Static utilities related to browser controls interfaces.
 */
public class BrowserControlsUtils {
    /**
     * @return True if the browser controls are completely off screen.
     */
    public static boolean areBrowserControlsOffScreen(BrowserControlsStateProvider stateProvider) {
        return stateProvider.getBrowserControlHiddenRatio() == 1.0f;
    }

    /**
     * @return True if the browser controls are currently completely visible.
     */
    public static boolean areBrowserControlsFullyVisible(
            BrowserControlsStateProvider stateProvider) {
        return stateProvider.getBrowserControlHiddenRatio() == 0.f;
    }

    /**
     * @return Whether the browser controls should be drawn as a texture.
     */
    public static boolean drawControlsAsTexture(BrowserControlsStateProvider stateProvider) {
        return stateProvider.getBrowserControlHiddenRatio() > 0;
    }

    /**
     * TODO(jinsukkim): Move this to CompositorViewHolder.
     * @return {@code true} if browser controls shrink Blink view's size. Note that this
     *         is valid only when the browser controls are in idle state i.e. not scrolling
     *         or animating.
     */
    public static boolean controlsResizeView(BrowserControlsStateProvider stateProvider) {
        return stateProvider.getContentOffset() > stateProvider.getTopControlsMinHeight()
                || getBottomContentOffset(stateProvider)
                > stateProvider.getBottomControlsMinHeight();
    }

    /**
     * @return The content offset from the bottom of the screen, or the visible height of the bottom
     *         controls, in px.
     */
    public static int getBottomContentOffset(BrowserControlsStateProvider stateProvider) {
        return stateProvider.getBottomControlsHeight() - stateProvider.getBottomControlOffset();
    }
}
