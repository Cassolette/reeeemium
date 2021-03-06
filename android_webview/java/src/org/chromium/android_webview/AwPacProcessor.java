// Copyright 2019 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.android_webview;

import org.chromium.base.annotations.JNINamespace;
import org.chromium.base.annotations.NativeMethods;

/**
 * Class to evaluate PAC scripts.
 */
@JNINamespace("android_webview")
public class AwPacProcessor {
    private long mNativePacProcessor;

    private static class LazyHolder {
        static final AwPacProcessor sInstance = new AwPacProcessor();
    }

    public static AwPacProcessor getInstance() {
        return LazyHolder.sInstance;
    }

    public AwPacProcessor() {
        mNativePacProcessor = AwPacProcessorJni.get().createNativePacProcessor();
    }

    // The calling code must not call any methods after it called destroy().
    public void destroy() {
        AwPacProcessorJni.get().destroyNative(mNativePacProcessor, this);
    }

    public boolean setProxyScript(String script) {
        return AwPacProcessorJni.get().setProxyScript(mNativePacProcessor, this, script);
    }

    public String makeProxyRequest(String url) {
        return AwPacProcessorJni.get().makeProxyRequest(mNativePacProcessor, this, url);
    }

    public static void initializeEnvironment() {
        AwPacProcessorJni.get().initializeEnvironment();
    }

    @NativeMethods
    interface Natives {
        void initializeEnvironment();
        long createNativePacProcessor();
        boolean setProxyScript(long nativeAwPacProcessor, AwPacProcessor caller, String script);
        String makeProxyRequest(long nativeAwPacProcessor, AwPacProcessor caller, String url);
        void destroyNative(long nativeAwPacProcessor, AwPacProcessor caller);
    }
}
