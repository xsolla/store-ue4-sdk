// Copyright 2024 Xsolla Inc. All Rights Reserved.

package com.xsolla.store;

import com.xsolla.android.bridge.payments.callback.BrowserCallback;

// Bridge's BrowserCallback interface has no native equivalent (UE4 has no AndroidJavaProxy) - this
// shim implements it in Java and forwards to native via JNI.
public class XsollaNativePaymentsCallback implements BrowserCallback {

    public static native void onPaymentsBrowserClosedCallback(long callback, boolean isManually);

    private final long callbackAddress;

    XsollaNativePaymentsCallback(long callbackAddress) {
        this.callbackAddress = callbackAddress;
    }

    @Override
    public void onBrowserClosed(boolean isManually) {
        onPaymentsBrowserClosedCallback(callbackAddress, isManually);
    }
}
