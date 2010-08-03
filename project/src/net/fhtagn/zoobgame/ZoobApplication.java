package net.fhtagn.zoobgame;

import android.app.Application;
import android.content.ContentUris;
import android.content.ContentValues;
import android.content.Intent;
import android.content.SharedPreferences;
import android.database.Cursor;
import android.net.Uri;
import android.util.Log;

public class ZoobApplication extends Application {
	private static final String PREF_KEY_DIFFICULTY = "difficulty";
	private static final String PREF_KEY_INPUT_METHOD = "input_mode";
	private static final String PREF_KEY_USE_TRACKBALL = "use_trackball";
	
	private SharedPreferences settings;
	
	private Uri currentSerie;
	
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
	
	public synchronized void setSerieId (long id) {
		currentSerie = ContentUris.withAppendedId(Series.CONTENT_URI, id);
	}
	
	public synchronized int getLevel () {
		//FIXME: for transition, if it is stored in preferences, should get it from preferences
		//and then put it in the db, remove from pref and go on with the db
		Cursor cur = getContentResolver().query(currentSerie, new String[]{Series.PROGRESS}, null, null, null);
		if (!cur.moveToFirst())
			return 0;
		int progress = cur.getInt(cur.getColumnIndex(Series.PROGRESS));
		cur.close();
		return progress; 
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
		ContentValues values = new ContentValues();
		values.put(Series.PROGRESS, level);
		getContentResolver().update(currentSerie, values, null, null);
		return;
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
