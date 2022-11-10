// Copyright 2022 Xsolla Inc. All Rights Reserved.

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
    public static String CALLBACK_ADDRESS = "callback_address";
    private static final int RC_PAY_STATION = 1;

    public static native void onPaymentsSuccessCallback(long callback);
    public static native void onPaymentsErrorCallback(long callback, String errorMessage);

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Intent intent = getIntent();
        String token = intent.getStringExtra(ARG_TOKEN);
        boolean isSandbox = intent.getBooleanExtra(ARG_SANDBOX, false);
        String redirectScheme = intent.getStringExtra(ARG_REDIRECT_SCHEME);
        String redirectHost = intent.getStringExtra(ARG_REDIRECT_HOST);

        XPayments.IntentBuilder builder = XPayments.createIntentBuilder(this)
                .accessToken(new AccessToken(token))
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
        
        if(status == XPayments.Status.COMPLETED)
        {
            onPaymentsSuccessCallback(getIntent().getLongExtra(CALLBACK_ADDRESS, 0));
        }

        if(status == XPayments.Status.UNKNOWN)
        {
            String errorMessage = "Unknown error";
            onPaymentsErrorCallback(getIntent().getLongExtra(CALLBACK_ADDRESS, 0), errorMessage);
        }

        if(status == XPayments.Status.CANCELLED)
        {
            String errorMessage = "Cancelled";
            onPaymentsErrorCallback(getIntent().getLongExtra(CALLBACK_ADDRESS, 0), errorMessage);
        }
         
        finish();
    }
}
