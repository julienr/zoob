package net.fhtagn.zoobgame.menus;

import net.fhtagn.zoob_demo.R;
import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.drawable.Drawable;

public class EndView extends InterLevelView {
	private final String all_complete;
	private final String thx_playing;
	private final String tap_continue;
	private final Drawable star;
	
	public EndView(Context context) {
    super(context);
    all_complete = context.getResources().getString(R.string.all_complete);
    thx_playing = context.getResources().getString(R.string.thx_playing);
    tap_continue = context.getResources().getString(R.string.tap_continue);
    star = context.getResources().getDrawable(R.drawable.star);
  }

	@Override
  protected void render(Canvas canvas) {
		final Paint textPaint = getTextPaint();
		canvas.drawText(all_complete, 7.5f, 2, textPaint);
		canvas.drawText(thx_playing, 7.5f, 4, textPaint);
		renderDrawable(canvas, star, 13, 2, 2, 2);
		renderDrawable(canvas, star, 8, 5.3f, 1.5f, 1.5f, 200);
		renderDrawable(canvas, star, 3, 6, 2, 2, 150);
		canvas.drawText(tap_continue, 7.5f, 8, textPaint);
  }

	@Override
  public int getNextLevel() {
	  return GOTO_MENU;
  }
}
