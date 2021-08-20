package com.xsolla.login.wxapi;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;

import androidx.annotation.Nullable;

import com.tencent.mm.opensdk.modelbase.BaseReq;
import com.tencent.mm.opensdk.modelbase.BaseResp;
import com.tencent.mm.opensdk.openapi.IWXAPI;
import com.tencent.mm.opensdk.openapi.IWXAPIEventHandler;
import com.tencent.mm.opensdk.openapi.WXAPIFactory;
import com.xsolla.android.login.social.LoginSocial;

public class WXEntryActivity extends Activity implements IWXAPIEventHandler {

    private IWXAPI iwxapi;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        iwxapi = WXAPIFactory.createWXAPI(this, LoginSocial.getWechatAppId(), false);
        iwxapi.handleIntent(getIntent(), this);
        finish();
    }

    @Override
    public void onReq(BaseReq baseReq) {
    }

    @Override
    public void onResp(BaseResp resp) {
        LoginSocial.setWechatResult(resp);
    }
}
