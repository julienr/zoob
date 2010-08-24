package net.fhtagn.zoobgame.menus;

import net.fhtagn.zoobgame.R;
import android.content.pm.ActivityInfo;
import android.graphics.Shader.TileMode;
import android.graphics.drawable.BitmapDrawable;
import android.os.Bundle;
import android.preference.PreferenceActivity;
import android.view.Window;

public class OptionsMenu extends PreferenceActivity {
	public void onCreate (Bundle savedInstanceState) {
		requestWindowFeature(Window.FEATURE_NO_TITLE);
    setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
		super.onCreate(savedInstanceState);
		addPreferencesFromResource(R.layout.optionsmenu);
	}
}
