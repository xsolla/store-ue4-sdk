package com.xsolla.login;

import android.app.Activity;
import android.content.Intent;

import com.xsolla.android.login.XLogin;
import com.xsolla.android.login.social.SocialNetwork;

public class XsollaNativeAuth {

    public static void xLoginInitJwt(Activity activity, String loginID, String callbackUrl, String facebookAppId, String googleAppId) {
        XLogin.SocialConfig socialConfig = getSocialConfig(facebookAppId, googleAppId);
        if (callbackUrl.isEmpty())
            XLogin.initJwt(activity, loginID, socialConfig);
        else
            XLogin.initJwt(activity, loginID, callbackUrl, socialConfig);
    }

    public static void xLoginInitOauth(Activity activity, String loginID, String clientId, String callbackUrl, String facebookAppId, String googleAppId) {
        XLogin.SocialConfig socialConfig = getSocialConfig(facebookAppId, googleAppId);
        if (callbackUrl.isEmpty())
            XLogin.initOauth(activity, loginID, Integer.parseInt(clientId), socialConfig);
        else
            XLogin.initOauth(activity, loginID, Integer.parseInt(clientId), callbackUrl, socialConfig);
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

    private static XLogin.SocialConfig getSocialConfig(String facebookAppId, String googleAppId) {
        XLogin.SocialConfig.Builder socialConfigBuilder = new XLogin.SocialConfig.Builder();
        socialConfigBuilder.facebookAppId(facebookAppId);
        socialConfigBuilder.googleServerId(googleAppId);
        return socialConfigBuilder.build();
    }
}
