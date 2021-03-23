// Copyright 2021 Xsolla Inc. All Rights Reserved.

package com.xsolla.login;

import android.app.Activity;
import android.content.Intent;

import com.xsolla.android.login.XLogin;
import com.xsolla.android.login.LoginConfig;
import com.xsolla.android.login.social.SocialNetwork;

public class XsollaNativeAuth {

    public static void xLoginInitJwt(Activity activity, String loginID, String callbackUrl, String facebookAppId, String googleAppId) {
        XLogin.SocialConfig socialConfig = new XLogin.SocialConfig(facebookAppId, googleAppId);
        LoginConfig loginConfig = new LoginConfig.JwtBuilder().
                setProjectId(loginID).
                setCallbackUrl(callbackUrl).
                setSocialConfig(socialConfig).
                build();
        XLogin.init(activity, loginConfig);
    }

    public static void xLoginInitOauth(Activity activity, String loginID, String clientId, String callbackUrl, String facebookAppId, String googleAppId) {
        XLogin.SocialConfig socialConfig = new XLogin.SocialConfig(facebookAppId, googleAppId);
        LoginConfig loginConfig = new LoginConfig.OauthBuilder().
                setProjectId(loginID).
                setOauthClientId(Integer.parseInt(clientId)).
                setCallbackUrl(callbackUrl).
                setSocialConfig(socialConfig).
                build();
        XLogin.init(activity, loginConfig);
    }

    public static void xAuthSocial(Activity activity, String provider, boolean rememberMe, boolean invalidateToken, long callback) {
        SocialNetwork socialNetwork = SocialNetwork.valueOf(provider.toUpperCase());
        Intent intent = new Intent(activity, XsollaNativeAuthActivity.class);
        intent.putExtra(XsollaNativeAuthActivity.ARG_SOCIAL_NETWORK, socialNetwork.name());
        intent.putExtra(XsollaNativeAuthActivity.ARG_WITH_LOGOUT, invalidateToken);
        intent.putExtra(XsollaNativeAuthActivity.REMEMBER_ME, rememberMe);
        intent.putExtra(XsollaNativeAuthActivity.CALLBACK_ADDRESS, callback);
        activity.startActivity(intent);
    }
}
