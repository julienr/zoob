package net.fhtagn.zoobgame.menus;

import net.fhtagn.zoob_demo.R;
import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.drawable.Drawable;

public class RewardView extends InterLevelView {
	private final String reward;
	private final String tap_continue;
	private final String longTouchActivate;
	private final Drawable rewardIcon;
	
	private final boolean showLongTouch;
	
	public RewardView(Context context, int rewardDrawable, boolean showLongTouch) {
    super(context);
    this.showLongTouch = showLongTouch;
    reward = context.getResources().getString(R.string.reward);
    tap_continue = context.getResources().getString(R.string.tap_continue);
    longTouchActivate = context.getResources().getString(R.string.long_touch_activate);
    rewardIcon = context.getResources().getDrawable(rewardDrawable);
  }
	
	public int getNextLevel () {
		return getCurrentLevel()+1;
	}

	@Override
  protected void render(Canvas canvas) {
		final Paint textPaint = getTextPaint();
		canvas.drawText(reward, 7.5f, 1.5f, textPaint);
		renderDrawable(canvas, rewardIcon, 7.5f, 4.5f, 4, 4);
		if (showLongTouch) {
			Paint smallText = new Paint(textPaint);
			smallText.setTextSize(0.5f);
			canvas.drawText(longTouchActivate, 7.5f, 7.5f, smallText);
		}
		canvas.drawText(tap_continue, 7.5f, 9, textPaint);
  }
}
