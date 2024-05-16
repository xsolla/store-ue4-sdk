// Copyright 2023 Xsolla Inc. All Rights Reserved.

package com.xsolla.login;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import androidx.annotation.Nullable;

import com.xsolla.android.login.XLogin;
import com.xsolla.android.login.callback.FinishXsollaWidgetAuthCallback;
import com.xsolla.android.login.callback.StartXsollaWidgetAuthCallback;

public class XsollaNativeXsollaWidgetAuthActivity extends Activity {

    public static String CALLBACK_ADDRESS = "callback_address";
    public static String REMEMBER_ME = "remember_me";
    public static String LOCALE = "locale";

	public static native void onAuthSuccessCallback(long callback, String accessToken, String refreshToken, long expiresAt, boolean rememberMe);
    public static native void onAuthCancelCallback(long callback);

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        if (savedInstanceState != null) {
            finish();
            return;
        }

        String locale = getIntent().getStringExtra(LOCALE);

        XLogin.startAuthWithXsollaWidget(this, new StartXsollaWidgetAuthCallback() {
            @Override
            public void onAuthStarted() {
                Log.d("XsollaNativeXsollaWidgetAuthActivity", "onAuthStarted");
            }

            @Override
            public void onError(Throwable throwable, String s) {
                Log.d("XsollaNativeXsollaWidgetAuthActivity", "onError");
                finish();
            }
        }, locale);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        XLogin.finishAuthWithXsollaWidget(this, requestCode, resultCode, data, new FinishXsollaWidgetAuthCallback() {
            @Override
            public void onAuthSuccess() {
				Log.d("XsollaNativeXsollaWidgetAuthActivity", "onAuthSuccess");
                onAuthSuccessCallback(getIntent().getLongExtra(CALLBACK_ADDRESS, 0),
				XLogin.getToken(),
				XLogin.getRefreshToken(),
				XLogin.getTokenExpireTime(),
				getIntent().getBooleanExtra(REMEMBER_ME, false));
                finish();
            }

            @Override
            public void onAuthCancelled() {
                Log.d("XsollaNativeXsollaWidgetAuthActivity", "onAuthCanceled");
                onAuthCancelCallback(getIntent().getLongExtra(CALLBACK_ADDRESS, 0));
                finish();
            }

            @Override
            public void onAuthError(Throwable throwable, String error) {
                String errorMessage = (error != null && !error.isEmpty()) ? error : "Unknown error";
                Log.d("XsollaNativeXsollaWidgetAuthActivity", "onAuthError. errorMessage = " + errorMessage);
                onAuthCancelCallback(getIntent().getLongExtra(CALLBACK_ADDRESS, 0));
                finish();
            }
        });
    }
}