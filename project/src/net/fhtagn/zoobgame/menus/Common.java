package net.fhtagn.zoobgame.menus;

import android.content.Context;
import android.graphics.Color;
import android.graphics.Typeface;

public class Common {
	//Request
	public static final int REQUEST_OPTIONS = 1;
	//Menu items
	public final static float SHADOW_DX = 1;
	public final static float SHADOW_DY = 1;
	public final static float SHADOW_RADIUS = 4;
	public final static int SHADOW_COLOR = Color.BLACK;
	public final static int SHADOW_FOCUS_COLOR = Color.parseColor("#64fb3d");
	
	public final static float MENU_ITEM_TEXT_SIZE = 35; //dip, should be the same as the one used in style.xml
	public final static float MENU_ITEM_SMALL_TEXT_SIZE = 20;
	
	// Font
	private static Typeface oogie = null;
	
	public final static Typeface getOogie(Context ctx) {
		if (oogie == null)
    	oogie = Typeface.createFromAsset(ctx.getAssets(), "fonts/OogieBoogie.ttf");
		return oogie;
	}
	
	//EpsilonEq
	public final static float EPSILON = 10e-5f;
	public final static boolean epsilonEq(float f1, float f2) {
		return Math.abs(f1-f2) < EPSILON;
	}
	
	// Colors
	public final static int COLOR_WHITE = Color.WHITE;
	public final static int COLOR_GREEN = Color.argb(255, 102, 255, 63);
}
