package net.fhtagn.zoobgame;

import android.app.Application;
import android.content.Intent;
import android.content.SharedPreferences;
import android.net.Uri;
import android.util.Log;

public class ZoobApplication extends Application {
	private static final String PREF_KEY_DIFFICULTY = "difficulty";
	private static final String PREF_KEY_INPUT_METHOD = "input_mode";
	private static final String PREF_KEY_USE_TRACKBALL = "use_trackball";
	
	private SharedPreferences settings;
	
	private String prefKeyLevel = null;
	
	protected String getPrefsName () {
		return "net_fhtagn_zoobgame_prefs";
	}
	
	@Override
	public void onCreate () {
		super.onCreate();
		
		Log.i("ZoobApplication", "onCreate()");
		
    //Fetch level from preferences
    settings = getSharedPreferences(getPrefsName(), 0);
	}
	
	/** Serie name is used as a preference key to store progression in a particular serie */
	public synchronized void setSerieName (String name) {
		prefKeyLevel = "prog_"+name;
	}
	
	//Same as setSerieName, but for the original levelest
	public synchronized void setOriginalSerie () {
		prefKeyLevel = "level";
	}
	
	public synchronized int getLevel () {
		if (prefKeyLevel == null)
			return 0;
		return settings.getInt(prefKeyLevel, 0);
	}
	
	public synchronized int getDifficulty () {
		return settings.getInt(PREF_KEY_DIFFICULTY, 0);
	}
	
	public synchronized int getInputMethod () {
		return settings.getInt(PREF_KEY_INPUT_METHOD, 1);
	}
	
	public synchronized int getUseTrackball () {
		return settings.getInt(PREF_KEY_USE_TRACKBALL, 0);
	}
	
	private void saveIntPref (String key, int val) {
		SharedPreferences.Editor editor = settings.edit();
		editor.putInt(key, val);
		editor.commit();
		Log.i("Zoob", "saveIntPref : "+key+" = "+ val);
	}
	
	public synchronized void saveProgress (int level) {
		if (prefKeyLevel == null)
			return;
		saveIntPref(prefKeyLevel, level);
	}
	
	public synchronized void saveDifficulty (int difficulty) {
		saveIntPref(PREF_KEY_DIFFICULTY, difficulty);
	}
	
	public synchronized void saveInputMethod (int method) {
		saveIntPref(PREF_KEY_INPUT_METHOD, method);
	}

	public void saveUseTrackball(int use) {
	  saveIntPref(PREF_KEY_USE_TRACKBALL, use);
  }
}
