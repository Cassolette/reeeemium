// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.chrome.browser.share;

import org.chromium.components.browser_ui.share.ShareParams;

/**
 * A container object for passing share extras not contained in {@link ShareParams} to {@link
 * ShareDelegate}.
 *
 * <p>This class contains extras that are used only by Android Share, and should never be
 * componentized. {@link ShareParams} lives in //components and only contains parameters that are
 * used in more than one part of the Chromium codebase.
 */
public class ChromeShareExtras {
    /**
     * Whether to save the chosen activity for future direct sharing.
     */
    private final boolean mSaveLastUsed;

    /**
     * Whether it should share directly with the activity that was most recently used to share. If
     * false, the share selection will be saved.
     */
    private final boolean mShareDirectly;

    /**
     * Whether the URL is of the current visible page.
     */
    private final boolean mIsUrlOfVisiblePage;

    /**
     * Source URL of the image.
     */
    private final String mImageSrcUrl;

    private ChromeShareExtras(boolean saveLastUsed, boolean shareDirectly,
            boolean isUrlOfVisiblePage, String imageSrcUrl) {
        mSaveLastUsed = saveLastUsed;
        mShareDirectly = shareDirectly;
        mIsUrlOfVisiblePage = isUrlOfVisiblePage;
        mImageSrcUrl = imageSrcUrl;
    }

    /**
     * @return Whether to save the chosen activity for future direct sharing.
     */
    public boolean saveLastUsed() {
        return mSaveLastUsed;
    }

    /**
     * @return Whether it should share directly with the activity that was most recently used to
     * share.
     */
    public boolean shareDirectly() {
        return mShareDirectly;
    }

    /**
     * @return Whether the URL is of the current visible page.
     */
    public boolean isUrlOfVisiblePage() {
        return mIsUrlOfVisiblePage;
    }

    /**
     * @return Source URL of the image.
     */
    public String getImageSrcUrl() {
        return mImageSrcUrl;
    }

    /**
     * The builder for {@link ChromeShareExtras} objects.
     */
    public static class Builder {
        private boolean mSaveLastUsed;
        private boolean mShareDirectly;
        private boolean mIsUrlOfVisiblePage;
        private String mImageSrcUrl;

        /**
         * Sets whether to save the chosen activity for future direct sharing.
         */
        public Builder setSaveLastUsed(boolean saveLastUsed) {
            mSaveLastUsed = saveLastUsed;
            return this;
        }

        /**
         * Sets whether it should share directly with the activity that was most recently used to
         * share.
         */
        public Builder setShareDirectly(boolean shareDirectly) {
            mShareDirectly = shareDirectly;
            return this;
        }

        /**
         * Sets whether the URL is of the current visible page.
         */
        public Builder setIsUrlOfVisiblePage(boolean isUrlOfVisiblePage) {
            mIsUrlOfVisiblePage = isUrlOfVisiblePage;
            return this;
        }

        /**
         * Sets source URL of the image.
         */
        public Builder setImageSrcUrl(String imageSrcUrl) {
            mImageSrcUrl = imageSrcUrl;
            return this;
        }

        public ChromeShareExtras build() {
            return new ChromeShareExtras(
                    mSaveLastUsed, mShareDirectly, mIsUrlOfVisiblePage, mImageSrcUrl);
        }
    }
}
