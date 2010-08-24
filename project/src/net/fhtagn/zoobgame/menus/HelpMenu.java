package net.fhtagn.zoobgame.menus;

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

public class HelpMenu extends MenuActivity {
	
	@Override
	public void onCreate (Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		/*setContentView(R.layout.helpmenu);
		
		OogieTextView title = (OogieTextView)findViewById(R.id.title);
		title.setTextSizeDip(25);
		title.setText("welcome to zoob ! ");
		
		OogieTextView content = (OogieTextView)findViewById(R.id.content);
		content.setTextSizeDip(15);
		//content.setTypeface(Typeface.SERIF);
		content.setText(Html.fromHtml("* the goal of the game is to destroy <font color=\"#64fb3d\">all</font> tanks<br />" +
				"* use the virtual <font color=\"#64fb3d\">gamepad</font> to move the tank<br />" +
				"* <font color=\"#64fb3d\">tap</font> anywhere to fire a rocket<br />" +
				"* have a look at the <font color=\"#64fb3d\">options</font> for more input methods<br />"), TextView.BufferType.SPANNABLE);
		
		OogieTextView tap = (OogieTextView)findViewById(R.id.tap);
		tap.setTextSizeDip(15);*/
		setFullscreenView(new MyView(this));

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
