package net.fhtagn.zoobgame.menus;

import net.fhtagn.zoob_demo.R;
import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.drawable.Drawable;

public class ErrorView extends FullscreenView {
	private final String oops;
	private final String explanation;
	private final String tap_continue;
	private final String please_report;
	
	public ErrorView(Context context) {
    super(context);
    oops = context.getResources().getString(R.string.oops);
    explanation = context.getResources().getString(R.string.loading_error);
    please_report = context.getResources().getString(R.string.please_report);
    tap_continue = context.getResources().getString(R.string.tap_continue);
  }

	@Override
  protected void render(Canvas canvas) {
		final Paint textPaint = getTextPaint();
		Paint p = new Paint(textPaint);
		p.setColor(Common.COLOR_RED);
		canvas.drawText(oops, 7.5f, 2, p);
		canvas.drawText(explanation, 7.5f, 4, textPaint);
		canvas.drawText(please_report, 7.5f, 6, textPaint);
		canvas.drawText(tap_continue, 7.5f, 8, textPaint);
  }
}
