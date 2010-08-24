package net.fhtagn.zoobgame.menus;

import net.fhtagn.zoobmenu.WonMenu.MyView;
import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.drawable.Drawable;
import android.os.Bundle;

public class LostMenu extends MenuActivity {
	@Override
	public void onCreate (Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setFullscreenView(new MyView(this));
	}
	
	class MyView extends FullscreenView {
		private final String gameover;
		private final String tap_continue;
		private final Drawable smileySad;
		
		public MyView(Context context) {
	    super(context);
	    gameover = context.getResources().getString(R.string.lvl_gameover);
	    tap_continue = context.getResources().getString(R.string.tap_continue);
	    smileySad = context.getResources().getDrawable(R.drawable.smiley_unhappy);
    }

		@Override
    protected void render(Canvas canvas) {
			final Paint textPaint = getTextPaint();
			canvas.drawText(gameover, 7.5f, 2, textPaint);
			renderDrawable(canvas, smileySad, 7.5f, 5, 4, 4);
			canvas.drawText(tap_continue, 7.5f, 8, textPaint);
    }
	}
}
