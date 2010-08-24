package net.fhtagn.zoobgame.menus;

import net.fhtagn.zoobgame.R;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.view.MotionEvent;

public class WonMenu extends FullscreenMenuActivity {
	private int currentLevel = 0;
	
	@Override
	public void onCreate (Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		Intent i = getIntent();
		if (i != null)
			currentLevel = i.getIntExtra("current_level", 0);
	}
	
	@Override
	protected FullscreenView createView () {
		return new MyView(this);
	}
	
	@Override
	public boolean onTouchEvent (MotionEvent event) {
		if (event.getAction() == MotionEvent.ACTION_UP || 
				event.getAction() == MotionEvent.ACTION_CANCEL) {
			Intent result = new Intent();
			result.putExtra("level", currentLevel+1);
			setResult(Activity.RESULT_OK, result);
			finish();
		}
		return true;
	}
	
	class MyView extends FullscreenView {
		private final String complete;
		private final String tap_continue;
		private final Drawable smileyHappy;
		
		public MyView(Context context) {
	    super(context);
	    complete = context.getResources().getString(R.string.lvl_complete);
	    tap_continue = context.getResources().getString(R.string.tap_continue);
	    smileyHappy = context.getResources().getDrawable(R.drawable.smiley_happy);
    }

		@Override
    protected void render(Canvas canvas) {
			final Paint textPaint = getTextPaint();
			canvas.drawText(complete, 7.5f, 2, textPaint);
			renderDrawable(canvas, smileyHappy, 7.5f, 5, 4, 4);
			canvas.drawText(tap_continue, 7.5f, 8, textPaint);
    }
	}
}
