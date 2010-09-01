package net.fhtagn.zoobgame.menus;

import net.fhtagn.zoob_demo.R;
import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.drawable.Drawable;

public class LostView extends InterLevelView {
	private final String gameover;
	private final String tap_continue;
	private final Drawable smileySad;
	
	public LostView(Context context) {
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

	@Override
  public int getNextLevel() {
	  return getCurrentLevel();
  }
}
