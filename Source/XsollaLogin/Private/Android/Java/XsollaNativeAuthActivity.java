// Copyright 2020 Xsolla Inc. All Rights Reserved.

package com.xsolla.login;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;

import androidx.annotation.Nullable;

import com.xsolla.android.login.XLogin;
import com.xsolla.android.login.callback.FinishSocialCallback;
import com.xsolla.android.login.callback.StartSocialCallback;
import com.xsolla.android.login.social.SocialNetwork;
import com.xsolla.android.login.token.TokenUtils;

public class XsollaNativeAuthActivity extends Activity {
    public static String ARG_SOCIAL_NETWORK = "social_network";
    public static String ARG_WITH_LOGOUT = "with_logout";
	public static String REMEMBER_ME = "remember_me";
    public static String CALLBACK_ADDRESS = "callback_address";

    private TokenUtils tokenUtils;

    public static native void onAuthSuccessCallback(long callback, String accessToken, String refreshToken, long expiresAt, boolean rememberMe);
    public static native void onAuthCancelCallback(long callback);
    public static native void onAuthErrorCallback(long callback, String errorMessage);

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        tokenUtils = new TokenUtils(this);

        SocialNetwork socialNetwork = SocialNetwork.valueOf(getIntent().getStringExtra(ARG_SOCIAL_NETWORK));
        boolean withLogout = getIntent().getBooleanExtra(ARG_WITH_LOGOUT, false);

        XLogin.startSocialAuth(this, socialNetwork, withLogout, new StartSocialCallback() {
            @Override
            public void onAuthStarted() {
                Log.d("XsollaAuthActivity", "onAuthStarted");
            }

            @Override
            public void onError(Throwable throwable, String s) {
                Log.d("XsollaAuthActivity", "onError");
                finish();
            }
        });
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        SocialNetwork socialNetwork = SocialNetwork.valueOf(getIntent().getStringExtra(ARG_SOCIAL_NETWORK));
        boolean withLogout = getIntent().getBooleanExtra(ARG_WITH_LOGOUT, false);

        XLogin.finishSocialAuth(this, socialNetwork, requestCode, resultCode, data, withLogout, new FinishSocialCallback() {
            @Override
            public void onAuthSuccess() {
                Log.d("XsollaAuthActivity", "onAuthSuccess");                
                onAuthSuccessCallback(getIntent().getLongExtra(CALLBACK_ADDRESS, 0),
					XLogin.getToken(),
					tokenUtils.getOauthRefreshToken(),
					tokenUtils.getOauthExpireTimeUnixSec(),
					getIntent().getBooleanExtra(REMEMBER_ME, false));
                finish();
            }

            @Override
            public void onAuthCancelled() {
                Log.d("XsollaAuthActivity", "onAuthCancelled");
                onAuthCancelCallback(getIntent().getLongExtra(CALLBACK_ADDRESS, 0));
                finish();
            }

            @Override
            public void onAuthError(Throwable throwable, String error) {
                Log.d("XsollaAuthActivity", "onAuthError");
                String errorMessage = (error != null && !error.isEmpty()) ? error : "Unknown error";
                onAuthErrorCallback(getIntent().getLongExtra(CALLBACK_ADDRESS, 0), errorMessage);
                finish();
            }
        });
    }
}