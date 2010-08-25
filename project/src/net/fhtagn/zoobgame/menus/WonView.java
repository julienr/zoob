package net.fhtagn.zoobgame.menus;

import net.fhtagn.zoobgame.R;
import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.drawable.Drawable;

public class WonView extends InterLevelView {
	private final String complete;
	private final String tap_continue;
	private final Drawable smileyHappy;
	
	public WonView(Context context) {
    super(context);
    complete = context.getResources().getString(R.string.lvl_complete);
    tap_continue = context.getResources().getString(R.string.tap_continue);
    smileyHappy = context.getResources().getDrawable(R.drawable.smiley_happy);
  }
	
	public int getNextLevel () {
		return getCurrentLevel()+1;
	}

	@Override
  protected void render(Canvas canvas) {
		final Paint textPaint = getTextPaint();
		canvas.drawText(complete, 7.5f, 2, textPaint);
		renderDrawable(canvas, smileyHappy, 7.5f, 5, 4, 4);
		canvas.drawText(tap_continue, 7.5f, 8, textPaint);
  }
}
