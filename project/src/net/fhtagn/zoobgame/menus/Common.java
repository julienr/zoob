package net.fhtagn.zoobgame.menus;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager.NameNotFoundException;
import android.graphics.Color;
import android.graphics.Typeface;
import android.net.Uri;
import android.util.Log;

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
	
	public static final Intent buyFullIntent () {
		Uri fullVersionURI = Uri.parse("market://details?id=net.fhtagn.zoobgame");
		Intent i = new Intent(Intent.ACTION_VIEW, fullVersionURI);
		i.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
		return i;
	}
	
	public static final Intent getEditorIntent () {
		Uri uri = Uri.parse("market://details?id=net.fhtagn.zoobeditor");
		Intent i = new Intent(Intent.ACTION_VIEW, uri);
		i.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
		return i;
	}
	
	public static boolean isEditorInstalled (Context ctx) {
		try {
	    ctx.getPackageManager().getPackageInfo("net.fhtagn.zoobeditor", 0);
	    return true;
    } catch (NameNotFoundException e) {
    	return false;
    } 
	}
	
	public static String readFromAssets(Activity activity, String fileName) {
		BufferedReader in = null;
		try {
			in = new BufferedReader(new InputStreamReader(activity.getAssets().open(fileName)));
			String line;
			StringBuilder buffer = new StringBuilder();
			while ((line = in.readLine()) != null)
				buffer.append(line).append('\n');
			return buffer.toString();
		} catch (IOException e) {
			return "Error loading " + fileName + "from assets.";
		} finally {
			//Close stream
			if (in != null) {
	      try {
	        in.close();
        } catch (IOException e) {
        	//ignore
	        e.printStackTrace();
        }
			}
		}
	}
}
