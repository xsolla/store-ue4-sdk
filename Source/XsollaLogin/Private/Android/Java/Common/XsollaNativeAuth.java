// Copyright 2023 Xsolla Inc. All Rights Reserved.

package com.xsolla.login;

import android.app.Activity;
import android.content.Intent;

import com.xsolla.android.login.XLogin;
import com.xsolla.android.login.LoginConfig;
import com.xsolla.android.login.social.SocialNetwork;

public class XsollaNativeAuth {

    public static void xLoginInit(Activity activity, String loginID, String clientId, String facebookAppId, String facebookClientToken, String googleAppId, String wechatAppId, String qqAppId) {
        XLogin.SocialConfig socialConfig = new XLogin.SocialConfig(facebookAppId, facebookClientToken, googleAppId, wechatAppId, qqAppId);
        LoginConfig loginConfig = new LoginConfig.OauthBuilder().
                setProjectId(loginID).
                setOauthClientId(tryParseInt(clientId, 0)).
                setSocialConfig(socialConfig).
                build();
        XLogin.init(activity, loginConfig);
    }

    public static void xAuthSocial(Activity activity, String provider, boolean rememberMe, long callback) {
        SocialNetwork socialNetwork = SocialNetwork.valueOf(provider.toUpperCase());
        Intent intent = new Intent(activity, XsollaNativeAuthActivity.class);
        intent.putExtra(XsollaNativeAuthActivity.ARG_SOCIAL_NETWORK, socialNetwork.name());
        intent.putExtra(XsollaNativeAuthActivity.REMEMBER_ME, rememberMe);
        intent.putExtra(XsollaNativeAuthActivity.CALLBACK_ADDRESS, callback);
        activity.startActivity(intent);
    }

    private static int tryParseInt(String value, int defaultVal) {
        try {
            return Integer.parseInt(value);
        } catch (NumberFormatException e) {
            return defaultVal;
        }
    }
}
