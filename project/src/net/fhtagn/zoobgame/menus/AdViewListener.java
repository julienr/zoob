package net.fhtagn.zoobgame.menus;

import android.util.Log;
import android.view.View;

import com.mobclix.android.sdk.MobclixAdView;
import com.mobclix.android.sdk.MobclixAdViewListener;

class AdViewListener implements MobclixAdViewListener {
  public void onSuccessfulLoad(MobclixAdView view) {
    Log.v("MobclixAdvertisingView", "The ad request was successful!");
    view.setVisibility(View.VISIBLE);
  }

  public void onFailedLoad(MobclixAdView view, int errorCode) {
    Log.v("MobclixAdvertisingView", "The ad request failed with error code: " + errorCode);
    view.setVisibility(View.GONE);
  }

  public void onAdClick(MobclixAdView adView) {
    Log.v("MobclixAdvertisingView", "Ad clicked!");
  }

  public void onCustomAdTouchThrough(MobclixAdView adView, String string) {
    Log.v("MobclixAdvertisingView", "The custom ad responded with '" + string + "' when touched!");
  }

  public boolean onOpenAllocationLoad(MobclixAdView adView, int openAllocationCode) {
    Log.v("MobclixAdvertisingView", "The ad request returned open allocation code: " + openAllocationCode);
    return false;
  }

  public String keywords()	{ return "demo,mobclix";}
  public String query()		{ return "query";}
}
