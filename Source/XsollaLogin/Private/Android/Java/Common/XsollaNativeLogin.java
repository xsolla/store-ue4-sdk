// Copyright 2023 Xsolla Inc. All Rights Reserved.

package com.xsolla.login;

import com.xsolla.android.login.util.AnalyticsUtils;

public class XsollaNativeLogin {

    public static void configureAnalytics(String gameEngine, String gameEngineVersion) {
        AnalyticsUtils.setGameEngine(gameEngine);
	    AnalyticsUtils.setGameEngineVersion(gameEngineVersion);
    }
}
