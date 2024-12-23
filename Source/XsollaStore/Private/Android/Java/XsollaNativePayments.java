// Copyright 2024 Xsolla Inc. All Rights Reserved.

package com.xsolla.store;

import static androidx.browser.customtabs.CustomTabsService.ACTION_CUSTOM_TABS_CONNECTION;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.pm.ResolveInfo;
import android.net.Uri;

import androidx.annotation.Keep;
import com.xsolla.android.payments.util.AnalyticsUtils;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

@Keep
public class XsollaNativePayments {

    private static List<String> allowedCustomTabsBrowsers = Arrays.asList("com.android.chrome", "com.huawei.browser", "com.sec.android.app.sbrowser");
    @Keep
    public static void openPurchaseUI(Activity activity, String token, boolean sandbox, String redirectScheme, String redirectHost , int payStationVersionNumber, long callback) {
        Intent intent = new Intent(activity, XsollaNativePaymentsActivity.class);
        intent.putExtra(XsollaNativePaymentsActivity.ARG_TOKEN, token);
        intent.putExtra(XsollaNativePaymentsActivity.ARG_SANDBOX, sandbox);
        intent.putExtra(XsollaNativePaymentsActivity.ARG_REDIRECT_HOST, redirectHost);
        intent.putExtra(XsollaNativePaymentsActivity.ARG_REDIRECT_SCHEME, redirectScheme);
        intent.putExtra(XsollaNativePaymentsActivity.ARG_PAY_STATION_VERSION_NUMBER, payStationVersionNumber);
        intent.putExtra(XsollaNativePaymentsActivity.CALLBACK_ADDRESS, callback);
        activity.startActivity(intent);
    }

    public static void configureAnalytics(String gameEngine, String gameEngineVersion) {
        AnalyticsUtils.setGameEngine(gameEngine);
        AnalyticsUtils.setGameEngineVersion(gameEngineVersion);
    }

    public static boolean isCustomTabsBrowserAvailable(Activity activity) {
        return !getAvailableCustomTabsBrowsers(activity.getApplicationContext()).isEmpty();
    }

    private static List<String> getAvailableCustomTabsBrowsers(Context context) {
        Intent browserIntent = new Intent()
                .setAction(Intent.ACTION_VIEW)
                .addCategory(Intent.CATEGORY_BROWSABLE)
                .setData(Uri.parse("https://"));
        List<ResolveInfo> activities = context.getPackageManager().queryIntentActivities(browserIntent, 0);
        // Extract the package names of the installed browsers
        List<String> installedBrowsers = new ArrayList<>();
        for (ResolveInfo resolveInfo : activities) {
            installedBrowsers.add(resolveInfo.activityInfo.packageName);
        }
        // Filter the allowed browsers based on the installed browsers
        List<String> allowedBrowsers = new ArrayList<>();
        for (String allowedBrowser : allowedCustomTabsBrowsers) {
            if (installedBrowsers.contains(allowedBrowser)) {
                allowedBrowsers.add(allowedBrowser);
            }
        }
        // Check if the allowed browsers support Custom Tabs
        List<String> customTabsBrowsers = new ArrayList<>();
        for (String packageName : allowedBrowsers) {
            Intent serviceIntent = new Intent()
                    .setAction(ACTION_CUSTOM_TABS_CONNECTION)
                    .setPackage(packageName);
            if (context.getPackageManager().resolveService(serviceIntent, 0) != null) {
                customTabsBrowsers.add(packageName);
            }
        }

        return customTabsBrowsers;
    }
}