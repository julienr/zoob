package net.fhtagn.zoob;

import android.app.Application;
import android.content.SharedPreferences;
import android.util.Log;

public class ZoobApplication extends Application {
	private static final String PREFS_NAME = "net_fhtagn_zoob_prefs";
	private static final String PREF_KEY_LEVEL = "level";
	private static final String PREF_KEY_DIFFICULTY = "difficulty";
	
	private SharedPreferences settings;
	
	private int level;
	private int difficulty;
	
	@Override
	public void onCreate () {
		super.onCreate();
		
		Log.i("ZoobApplication", "onCreate()");
		
    //Fetch level from preferences
    settings = getSharedPreferences(PREFS_NAME, 0);
    level = settings.getInt(PREF_KEY_LEVEL, 0);
    level = settings.getInt(PREF_KEY_DIFFICULTY, 1);
	}
	
	private void saveIntPref (String key, int val) {
		SharedPreferences.Editor editor = settings.edit();
		editor.putInt(key, val);
		editor.commit();
		Log.i("Zoob", "saveIntPref : "+key+" = "+ val);
	}
	
	public void saveProgress (int level) {
		saveIntPref(PREF_KEY_LEVEL, level);
	}
	
	public void saveDifficulty (int difficulty) {
		saveIntPref(PREF_KEY_DIFFICULTY, difficulty);
	}
}
