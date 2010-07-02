package net.fhtagn.zoobgame;

import android.app.Application;
import android.content.Intent;
import android.content.SharedPreferences;
import android.net.Uri;
import android.util.Log;

public class ZoobApplication extends Application {
	private static final String PREF_KEY_LEVEL = "level";
	private static final String PREF_KEY_DIFFICULTY = "difficulty";
	private static final String PREF_KEY_INPUT_METHOD = "input_method";
	
	private SharedPreferences settings;
	
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
	
	public synchronized int getLevel () {
		return settings.getInt(PREF_KEY_LEVEL, 0);
	}
	
	public synchronized int getDifficulty () {
		return settings.getInt(PREF_KEY_DIFFICULTY, 0);
	}
	
	public synchronized int getInputMethod () {
		return settings.getInt(PREF_KEY_INPUT_METHOD, 0);
	}
	
	private void saveIntPref (String key, int val) {
		SharedPreferences.Editor editor = settings.edit();
		editor.putInt(key, val);
		editor.commit();
		Log.i("Zoob", "saveIntPref : "+key+" = "+ val);
	}
	
	public synchronized void saveProgress (int level) {
		saveIntPref(PREF_KEY_LEVEL, level);
	}
	
	public synchronized void saveDifficulty (int difficulty) {
		saveIntPref(PREF_KEY_DIFFICULTY, difficulty);
	}
	
	public synchronized void saveInputMethod (int method) {
		saveIntPref(PREF_KEY_INPUT_METHOD, method);
	}
}
