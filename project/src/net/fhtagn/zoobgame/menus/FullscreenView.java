package net.fhtagn.zoobgame.menus;

import java.util.Timer;
import java.util.TimerTask;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.Rect;
import android.graphics.RectF;
import android.graphics.Region;
import android.graphics.Paint.Align;
import android.graphics.drawable.Drawable;
import android.util.Log;
import android.view.View;

/**
 * A specialized view that create a fixed ratio subarea of virtual width 15 and height 10
 * in which drawing can be conducted.
 * The subarea is centered in the view. 
 */
public abstract class FullscreenView extends View {
	static final String TAG = "FullscreenView";
	static final float VIRTUAL_COORDS_WIDTH = 15;
	static final float VIRTUAL_COORDS_HEIGHT = 10; 
	static final float TARGET_RATIO = VIRTUAL_COORDS_WIDTH/VIRTUAL_COORDS_HEIGHT;
	
	static final float SHADOW_RADIUS = 0.05f;
	static final float SHADOW_DX = 0.05f;
	static final float SHADOW_DY = 0.05f;
	
	static private final Paint textPaint = new Paint();
	static {
		textPaint.setTextSize(0.7f);
		textPaint.setColor(Color.WHITE);
		textPaint.setAntiAlias(true);
		textPaint.setTextAlign(Align.CENTER);
		textPaint.setShadowLayer(SHADOW_RADIUS, SHADOW_DX, SHADOW_DY, Common.SHADOW_COLOR);
	}
	
	private class RedrawTask extends TimerTask {
		@Override
    public void run() {
			FullscreenView.this.postInvalidate();
    }
	}
	
	private final Timer redrawTimer = new Timer();
	
	//Since we force the drawing in a subarea with 1.5 aspect ratio, this are the areaWidth, height and marings
	//expressed in screen pixels
	private float areaWidth;
	private float areaHeight;
	
	private float marginX;
	private float marginY;
	
	public FullscreenView(Context context) {
    super(context);
    textPaint.setTypeface(Common.getOogie(context));
  }
	
	//Call this to redraw the view at specific intervals
	protected void startLoopRedraw (long delay) {
		redrawTimer.schedule(new RedrawTask(), 0, delay);
	}
	
	protected void stopLoopRedraw () {
		redrawTimer.cancel();
	}
	
	//provide a text paint with the Oogie font
	protected Paint getTextPaint () {
		return textPaint;
	}
	
	//Utility method to render a drawable
	//(x,y) is the CENTER of the drawable
	protected void renderDrawable (Canvas canvas, Drawable drawable, float x, float y, float width, float height) {
		renderDrawable(canvas, drawable, x, y, width, height, 0);
	}
	protected void renderDrawable (Canvas canvas, Drawable drawable, float x, float y, float width, float height, float rotate) {
		canvas.save();
		canvas.translate(x-width/2.0f, y-height/2.0f);
		canvas.scale(width, height);
		canvas.rotate(rotate, 0.5f, 0.5f);
		drawable.setBounds(0,0,1,1);
		drawable.draw(canvas);
		canvas.restore();
	}
	
	@Override
	protected void onSizeChanged(int w, int h, int oldw, int oldh) {
		/**
		  * We base all our calculations on a 480/320 = 1.5 aspect ratio (that's the most
		  * common resolution for android and iphone)
		  * If we have a screen that doesn't have this aspect ratio, we use the biggest square
		  * that has this aspect ratio that we can display in this screen and we display
		  * in this square.
		  * This will of course waste some screen space, but at least the display will look good
		  */
		  float ratio = w/h;
		  //The subarea in which we'll display our objects
		  areaHeight = h;
		  areaWidth = w;
		  if (!Common.epsilonEq(ratio, TARGET_RATIO)) {
		   if (h > w/1.5) {
		     areaHeight = w/TARGET_RATIO;
		     areaWidth = w;
		   } else {
		     areaWidth = h*TARGET_RATIO;
		     areaHeight = h;
		   }
		  }
		  ratio = areaWidth/areaHeight;
		  Log.i(TAG, "ratio="+ratio+", w="+w+", h="+h+", areaWidth="+areaWidth+", areaHeight="+areaHeight);

		  //Center the viewport in the window
		  marginX = (w-areaWidth)/2.0f;
		  marginY = (h-areaHeight)/2.0f;
		  Log.i(TAG, "marginX = " + marginX + ", marginY = " + marginY);
	}

	@Override
	protected final void onDraw (Canvas canvas) {
		//canvas.drawARGB(255, 101, 101, 101);
		canvas.save();
		canvas.translate(marginX, marginY);
		canvas.clipRect(new RectF(0,0,areaWidth, areaHeight), Region.Op.REPLACE);
		
		//debug lines
		/*Paint stroke = new Paint();
		stroke.setStyle(Paint.Style.STROKE);
		stroke.setColor(Color.RED);
		stroke.setStrokeWidth(2.0f);
		canvas.drawRect(new RectF(.0f, .0f, areaWidth, areaHeight), stroke);
		canvas.drawLine(areaWidth/2.0f, 0, areaWidth/2.0f, areaHeight, stroke);
		canvas.drawLine(0, areaHeight/2.0f, areaWidth, areaHeight/2.0f, stroke);*/
		
		canvas.scale((canvas.getWidth()-2*marginX)/VIRTUAL_COORDS_WIDTH, (canvas.getHeight()-2*marginY)/VIRTUAL_COORDS_HEIGHT);
		
		render(canvas);
		
		canvas.restore();
	}
	
	//Override this to do the drawing
	protected abstract void render (Canvas canvas);
}
