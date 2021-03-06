// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.weblayer_private;

import android.content.Context;
import android.content.Intent;

import androidx.annotation.NonNull;

import org.chromium.base.StrictModeContext;
import org.chromium.base.supplier.Supplier;
import org.chromium.components.content_settings.CookieControlsBridge;
import org.chromium.components.content_settings.CookieControlsObserver;
import org.chromium.components.embedder_support.util.UrlConstants;
import org.chromium.components.page_info.PageInfoControllerDelegate;
import org.chromium.content_public.browser.WebContents;
import org.chromium.ui.modaldialog.ModalDialogManager;
import org.chromium.url.GURL;
import org.chromium.weblayer_private.interfaces.SiteSettingsIntentHelper;

/**
 * WebLayer's customization of PageInfoControllerDelegate.
 */
public class PageInfoControllerDelegateImpl extends PageInfoControllerDelegate {
    private final Context mContext;
    private final WebContents mWebContents;
    private final String mProfileName;

    static PageInfoControllerDelegateImpl create(WebContents webContents) {
        TabImpl tab = TabImpl.fromWebContents(webContents);
        assert tab != null;
        return new PageInfoControllerDelegateImpl(tab.getBrowser().getContext(), webContents,
                tab.getProfile(), tab.getBrowser().getWindowAndroid()::getModalDialogManager);
    }

    private PageInfoControllerDelegateImpl(Context context, WebContents webContents,
            ProfileImpl profile, Supplier<ModalDialogManager> modalDialogManager) {
        super(modalDialogManager, new AutocompleteSchemeClassifierImpl(),
                /** vrHandler= */ null,
                /** isSiteSettingsAvailable= */
                isHttpOrHttps(webContents.getVisibleUrl()),
                /** cookieControlsShown= */
                CookieControlsBridge.isCookieControlsEnabled(profile));
        mContext = context;
        mWebContents = webContents;
        mProfileName = profile.getName();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void showSiteSettings(String url) {
        Intent intent =
                SiteSettingsIntentHelper.createIntentForSingleWebsite(mContext, mProfileName, url);

        // Disabling StrictMode to avoid violations (https://crbug.com/819410).
        try (StrictModeContext ignored = StrictModeContext.allowDiskReads()) {
            mContext.startActivity(intent);
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    @NonNull
    public CookieControlsBridge createCookieControlsBridge(CookieControlsObserver observer) {
        return new CookieControlsBridge(observer, mWebContents, null);
    }

    private static boolean isHttpOrHttps(GURL url) {
        String scheme = url.getScheme();
        return UrlConstants.HTTP_SCHEME.equals(scheme) || UrlConstants.HTTPS_SCHEME.equals(scheme);
    }
}
