// Copyright 2024 Xsolla Inc. All Rights Reserved.

package com.xsolla.login;

import android.app.Activity;
import android.net.Uri;
import android.os.Bundle;
import android.text.TextUtils;
import android.util.Log;
import android.webkit.WebResourceRequest;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebViewClient;

import androidx.annotation.Nullable;

import java.util.ArrayList;
import java.util.List;

public class XsollaNativeAdditionalInfoAuthActivity extends Activity {
    public static String CALLBACK_ADDRESS = "callback_address";
    public static String LOGIN_URL = "login_url";
    public static String REDIRECT_URL = "redirect_url";

    public static native void onAuthSuccessCallback(long callback, String code, String token);
    public static native void onAuthCancelCallback(long callback);
    public static native void onAuthErrorCallback(long callback, String errorMessage);

    private WebView authWebView;
    private boolean isCompleted = false;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        if (savedInstanceState != null) {
            onAuthCancelCallback(getIntent().getLongExtra(CALLBACK_ADDRESS, 0));
            finish();
            return;
        }

        final String loginUrl = getIntent().getStringExtra(LOGIN_URL);
        final String redirectUrl = getIntent().getStringExtra(REDIRECT_URL);
        if (TextUtils.isEmpty(loginUrl) || TextUtils.isEmpty(redirectUrl)) {
            onAuthError("Missing login or redirect URL");
            return;
        }

        authWebView = new WebView(this);
        setContentView(authWebView);

        WebSettings settings = authWebView.getSettings();
        settings.setJavaScriptEnabled(true);
        settings.setDomStorageEnabled(true);
        settings.setLoadsImagesAutomatically(true);

        authWebView.setWebViewClient(new WebViewClient() {
            @Override
            public boolean shouldOverrideUrlLoading(WebView view, WebResourceRequest request) {
                if (request == null || request.getUrl() == null) {
                    return false;
                }
                return handlePotentialCompletionUrl(request.getUrl().toString(), redirectUrl);
            }

            @Override
            public boolean shouldOverrideUrlLoading(WebView view, String url) {
                return handlePotentialCompletionUrl(url, redirectUrl);
            }
        });

        String browserUrl = ensureRedirectUri(loginUrl, redirectUrl);
        authWebView.loadUrl(browserUrl);
    }

    @Override
    public void onBackPressed() {
        if (!isCompleted) {
            isCompleted = true;
            onAuthCancelCallback(getIntent().getLongExtra(CALLBACK_ADDRESS, 0));
        }
        finish();
    }

    @Override
    protected void onDestroy() {
        if (authWebView != null) {
            authWebView.stopLoading();
            authWebView.destroy();
            authWebView = null;
        }
        super.onDestroy();
    }

    private boolean handlePotentialCompletionUrl(String url, String redirectUrl) {
        if (isCompleted || TextUtils.isEmpty(url)) {
            return false;
        }

        if (!isRedirectMatch(url, redirectUrl)) {
            return false;
        }

        Uri uri = Uri.parse(url);
        String token = uri.getQueryParameter("token");
        String code = uri.getQueryParameter("code");
        String error = uri.getQueryParameter("error");
        String errorDescription = uri.getQueryParameter("error_description");

        if (!TextUtils.isEmpty(token) || !TextUtils.isEmpty(code)) {
            isCompleted = true;
            onAuthSuccessCallback(getIntent().getLongExtra(CALLBACK_ADDRESS, 0), code, token);
            finish();
            return true;
        }

        if (!TextUtils.isEmpty(error) || !TextUtils.isEmpty(errorDescription)) {
            String normalizedError = !TextUtils.isEmpty(errorDescription)
                    ? errorDescription
                    : (!TextUtils.isEmpty(error) ? error : "Unknown authentication error");
            onAuthError(normalizedError);
            return true;
        }

        onAuthError("Authentication completed without code or token");
        return true;
    }

    private void onAuthError(String errorMessage) {
        if (isCompleted) {
            return;
        }

        isCompleted = true;
        String normalizedError = TextUtils.isEmpty(errorMessage) ? "Unknown authentication error" : errorMessage;
        Log.d("XsollaNativeAdditionalInfoAuthActivity", "onAuthError: " + normalizedError);
        onAuthErrorCallback(getIntent().getLongExtra(CALLBACK_ADDRESS, 0), normalizedError);
        finish();
    }

    private static boolean isRedirectMatch(String currentUrl, String redirectUrl) {
        return currentUrl.startsWith(redirectUrl);
    }

    private static String ensureRedirectUri(String loginUrl, String redirectUrl) {
        Uri loginUri = Uri.parse(loginUrl);
        List<String> queryParams = new ArrayList<>();
        for (String paramName : loginUri.getQueryParameterNames()) {
            if (!"redirect_uri".equalsIgnoreCase(paramName)) {
                for (String value : loginUri.getQueryParameters(paramName)) {
                    queryParams.add(paramName + "=" + Uri.encode(value));
                }
            }
        }
        queryParams.add("redirect_uri=" + Uri.encode(redirectUrl));

        StringBuilder builder = new StringBuilder();
        String scheme = loginUri.getScheme();
        if (!TextUtils.isEmpty(scheme)) {
            builder.append(scheme).append("://");
        }
        String authority = loginUri.getEncodedAuthority();
        if (!TextUtils.isEmpty(authority)) {
            builder.append(authority);
        }
        String path = loginUri.getEncodedPath();
        if (!TextUtils.isEmpty(path)) {
            builder.append(path);
        }
        if (!queryParams.isEmpty()) {
            builder.append("?");
            for (int i = 0; i < queryParams.size(); i++) {
                if (i > 0) {
                    builder.append("&");
                }
                builder.append(queryParams.get(i));
            }
        }
        String fragment = loginUri.getEncodedFragment();
        if (!TextUtils.isEmpty(fragment)) {
            builder.append("#").append(fragment);
        }
        return builder.toString();
    }
}
