// Copyright 2024 Xsolla Inc. All Rights Reserved.

package com.xsolla.store;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;

import com.xsolla.android.payments.XPayments;
import com.xsolla.android.payments.data.AccessToken;

public class XsollaNativePaymentsActivity extends Activity {

    public static final String ARG_TOKEN = "token";
    public static final String ARG_SANDBOX = "sandbox";
    public static final String ARG_REDIRECT_SCHEME = "redirect_scheme";
    public static final String ARG_REDIRECT_HOST = "redirect_host";
    public static final String ARG_PAY_STATION_VERSION_NUMBER = "pay_station_version_number";
    public static String CALLBACK_ADDRESS = "callback_address";
    private static final int RC_PAY_STATION = 1;

    public static native void onPaymentsBrowserClosedCallback(long callback, boolean isManually);

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (savedInstanceState != null) {
            finish();
            return;
        }


        Intent intent = getIntent();
        String token = intent.getStringExtra(ARG_TOKEN);
        boolean isSandbox = intent.getBooleanExtra(ARG_SANDBOX, false);
        String redirectScheme = intent.getStringExtra(ARG_REDIRECT_SCHEME);
        String redirectHost = intent.getStringExtra(ARG_REDIRECT_HOST);
        
        XPayments.PayStationVersion payStationVersion = XPayments.PayStationVersion.V4;
        int payStationVersionNumber = intent.getIntExtra(ARG_PAY_STATION_VERSION_NUMBER, 4);
        if(payStationVersionNumber == 3)
            payStationVersion = XPayments.PayStationVersion.V3;

        XPayments.IntentBuilder builder = XPayments.createIntentBuilder(this)
                .accessToken(new AccessToken(token))
                .payStationVersion(payStationVersion)
                .isSandbox(isSandbox);

        if (redirectScheme != null)
            builder.setRedirectUriScheme(redirectScheme);

        if (redirectHost != null)
            builder.setRedirectUriHost(redirectHost);

        startActivityForResult(builder.build(), RC_PAY_STATION);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        XPayments.Result result = XPayments.Result.fromResultIntent(data);
        XPayments.Status status = result.getStatus();
        
        boolean isManually = status == XPayments.Status.CANCELLED;
        onPaymentsBrowserClosedCallback(getIntent().getLongExtra(CALLBACK_ADDRESS, 0), isManually);

        finish();
    }
}
