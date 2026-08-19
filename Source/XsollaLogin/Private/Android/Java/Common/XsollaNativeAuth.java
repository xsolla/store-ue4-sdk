// Copyright 2024 Xsolla Inc. All Rights Reserved.

package com.xsolla.login;

import android.app.Activity;
import android.content.Intent;

import com.xsolla.android.bridge.login.LoginConfig;
import com.xsolla.android.bridge.login.XLogin;
import com.xsolla.android.bridge.login.social.SocialAuthBridgeActivity;
import com.xsolla.android.bridge.login.social.SocialNetwork;
import com.xsolla.android.bridge.login.social.XsollaWidgetBridgeActivity;

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
        SocialAuthBridgeActivity.perform(activity, socialNetwork, new XsollaNativeAuthCallback(callback, rememberMe));
    }

    public static void authViaXsollaWidget(Activity activity, String locale, boolean rememberMe, long callback) {
        XsollaWidgetBridgeActivity.perform(activity, new XsollaNativeAuthCallback(callback, rememberMe), locale);
    }

    public static void authAdditionalInfo(Activity activity, String loginUrl, String redirectUrl, long callback) {
        Intent intent = new Intent(activity, XsollaNativeAdditionalInfoAuthActivity.class);
        intent.putExtra(XsollaNativeAdditionalInfoAuthActivity.LOGIN_URL, loginUrl);
        intent.putExtra(XsollaNativeAdditionalInfoAuthActivity.REDIRECT_URL, redirectUrl);
        intent.putExtra(XsollaNativeAdditionalInfoAuthActivity.CALLBACK_ADDRESS, callback);
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
