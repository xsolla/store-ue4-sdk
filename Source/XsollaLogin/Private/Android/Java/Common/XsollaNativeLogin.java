// Copyright 2024 Xsolla Inc. All Rights Reserved.

package com.xsolla.login;

import com.xsolla.android.bridge.login.util.AnalyticsUtils;

public class XsollaNativeLogin {

    public static void configureAnalytics(String gameEngine, String gameEngineVersion) {
        AnalyticsUtils.gameEngine = gameEngine;
        AnalyticsUtils.gameEngineVersion = gameEngineVersion;
    }
}
