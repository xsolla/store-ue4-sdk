// Copyright 2024 Xsolla Inc. All Rights Reserved.

package com.xsolla.login;

import com.xsolla.android.bridge.login.XLogin;
import com.xsolla.android.bridge.login.callback.AuthCallback;

// Bridge's AuthCallback interface has no native equivalent (UE4 has no AndroidJavaProxy) - this
// shim implements it in Java and forwards to native via JNI. Shared by both xAuthSocial and
// authViaXsollaWidget, same as bridge's own AuthFlowCoordinator uses one AuthCallback type for both.
public class XsollaNativeAuthCallback implements AuthCallback {

    private static final String CANCELLED_ERROR = "CANCELLED";

    public static native void onAuthSuccessCallback(long callback, String accessToken, String refreshToken, long expiresAt, boolean rememberMe);
    public static native void onAuthCancelCallback(long callback);
    public static native void onAuthErrorCallback(long callback, String errorMessage);

    private final long callbackAddress;
    private final boolean rememberMe;

    XsollaNativeAuthCallback(long callbackAddress, boolean rememberMe) {
        this.callbackAddress = callbackAddress;
        this.rememberMe = rememberMe;
    }

    @Override
    public void onSuccess() {
        onAuthSuccessCallback(callbackAddress, XLogin.getToken(), XLogin.getRefreshToken(), XLogin.getTokenExpireTime(), rememberMe);
    }

    @Override
    public void onError(Throwable throwable, String errorMessage) {
        if (CANCELLED_ERROR.equals(errorMessage)) {
            onAuthCancelCallback(callbackAddress);
        } else {
            String message = (errorMessage != null && !errorMessage.isEmpty()) ? errorMessage : "Unknown error";
            onAuthErrorCallback(callbackAddress, message);
        }
    }
}
