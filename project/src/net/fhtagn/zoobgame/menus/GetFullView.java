package net.fhtagn.zoobgame.menus;

import com.mobclix.android.sdk.MobclixAdView;
import com.mobclix.android.sdk.MobclixIABRectangleMAdView;
import com.mobclix.android.sdk.MobclixMMABannerXLAdView;

import net.fhtagn.zoobgame.R;
import android.content.Context;
import android.content.Intent;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Shader.TileMode;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.net.Uri;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.FrameLayout;
import android.widget.LinearLayout;
import android.widget.FrameLayout.LayoutParams;
import android.widget.ScrollView;
import android.widget.TextView;

public class GetFullView extends FrameLayout {
	public GetFullView(final Context context) {
    super(context);
    
    this.setClickable(true);
    
    LayoutInflater inflater = (LayoutInflater)context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
	  this.addView(inflater.inflate(R.layout.get_full, null), 
	  		new FrameLayout.LayoutParams(LayoutParams.FILL_PARENT, LayoutParams.FILL_PARENT));
	  
	  /*ScrollView container = (ScrollView)findViewById(R.id.container);
    BitmapDrawable bg = (BitmapDrawable)getResources().getDrawable(R.drawable.backrepeat);
    bg.setTileModeX(TileMode.REPEAT);
    bg.setTileModeY(TileMode.REPEAT);
    container.setBackgroundDrawable(bg);*/
	  
	  MobclixIABRectangleMAdView adview = (MobclixIABRectangleMAdView )findViewById(R.id.advertising_rectangle_view);
	  adview.addMobclixAdViewListener(new AdViewListener());
	  
	  TextView getFull = (TextView)findViewById(R.id.get_full);
	  getFull.setOnClickListener(new OnClickListener() {
			@Override
      public void onClick(View v) {
				Uri fullVersionURI = Uri.parse("market://details?id=net.fhtagn.zoobgame");
				context.startActivity(new Intent(Intent.ACTION_VIEW, fullVersionURI));    
      }
	  });
  }
}
