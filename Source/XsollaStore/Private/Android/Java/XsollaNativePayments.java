// Copyright 2022 Xsolla Inc. All Rights Reserved.

package com.xsolla.store;

import android.app.Activity;
import android.content.Intent;
import androidx.annotation.Keep;

@Keep
public class XsollaNativePayments {

    @Keep
    public static void openPurchaseUI(Activity activity, String token, boolean sandbox, String redirectScheme, String redirectHost , long callback) {
        Intent intent = new Intent(activity, XsollaNativePaymentsActivity.class);
        intent.putExtra(XsollaNativePaymentsActivity.ARG_TOKEN, token);
        intent.putExtra(XsollaNativePaymentsActivity.ARG_SANDBOX, sandbox);
        intent.putExtra(XsollaNativePaymentsActivity.ARG_REDIRECT_HOST, redirectHost);
        intent.putExtra(XsollaNativePaymentsActivity.ARG_REDIRECT_SCHEME, redirectScheme);
        intent.putExtra(XsollaNativePaymentsActivity.CALLBACK_ADDRESS, callback);
        activity.startActivity(intent);
    }
}