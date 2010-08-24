package net.fhtagn.zoobgame.menus;

import net.fhtagn.zoobgame.R;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.RectF;
import android.graphics.Region;
import android.graphics.Typeface;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.text.Html;
import android.util.Log;
import android.view.SurfaceView;
import android.view.View;
import android.widget.TextView;

public class HelpMenu extends FullscreenMenuActivity {
	@Override
	protected FullscreenView createView () {
		return new MyView(this);
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
