package net.fhtagn.zoobgame.menus;

import android.content.Context;
import android.graphics.BlurMaskFilter;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.util.AttributeSet;
import android.util.Log;
import android.view.View;
import android.widget.Gallery;
import android.widget.TextView;

public class OogieTextView extends TextView {
	private boolean focused = false;
	private final float scale;
	
	public OogieTextView(Context context) {
		this(context, null);
	}
	
	public OogieTextView(Context context, AttributeSet attrs) {
	  super(context, attrs);
	  setTypeface(Common.getOogie(context));
	  scale = getResources().getDisplayMetrics().density;
	  setTextSize(Common.MENU_ITEM_TEXT_SIZE/*scale*/);
	  setTextColor(Color.WHITE);
  }
	
	public void setTextSizeDip (float ts) {
		setTextSize(ts*scale);
	}
	
	@Override
	protected void onDraw (Canvas canvas) {
		if (focused) {
			this.getPaint().setShadowLayer(Common.SHADOW_RADIUS, Common.SHADOW_DX, Common.SHADOW_DY, Common.SHADOW_FOCUS_COLOR);
			this.getPaint().setMaskFilter(null);
		} else {
			this.getPaint().setShadowLayer(Common.SHADOW_RADIUS, Common.SHADOW_DX, Common.SHADOW_DY, Common.SHADOW_COLOR);
			this.getPaint().setMaskFilter(null);
		}
		super.onDraw(canvas);
	}
	
	@Override
	public void onFocusChanged(boolean focused, int direction, Rect previouslyFocusedRect) {
		super.onFocusChanged(focused, direction, previouslyFocusedRect);
		Log.i("OOgie", "focused : " + focused);
		this.focused = focused;
		postInvalidate();
	}

}
