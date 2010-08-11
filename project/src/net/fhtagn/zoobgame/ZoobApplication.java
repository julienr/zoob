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
	static final String TAG = "ZoobApplication";
	//Deprecated: progress now saved using the content provider, only there to restore progress from previous versions when upgrading
	private static final String PREF_KEY_LEVEL = "level";
	
	private static final String PREF_KEY_DIFFICULTY = "difficulty";
	private static final String PREF_KEY_INPUT_METHOD = "input_mode";
	private static final String PREF_KEY_USE_TRACKBALL = "use_trackball";
	
	private SharedPreferences settings;
	
	private Uri currentSerie;
	
	//if true, progress will be saved to preferences. This is the normal behaviour
	//This might be turned off and the progress won't be saved when zoob closes. This is usefull when doing level edition
	private boolean progressPersistent = true;
	private int progress = -1; //only used if progressSavePersistent = false
	
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
		if (id == 1) {
			//Original serie, try to restore progress
			transferProgressFromPreferences();
		}
	}
	
	//This is a compability functions that will retrieve the progress currently saved in the user preferences (versions <= 1.0.2-2)
	//and transfer it to the contentprovider's db (versions > 1.0.2-2)
	private void transferProgressFromPreferences () {
		int settingsProgress = settings.getInt(PREF_KEY_LEVEL, 0); 
		if (settingsProgress != 0) {
			saveProgress(settingsProgress);
			SharedPreferences.Editor editor = settings.edit();
			editor.remove(PREF_KEY_LEVEL);
			editor.commit();
		}
	}
	
	public synchronized void setProgressPersistent (boolean persistent) {
		progressPersistent = persistent;
	}
	
	public synchronized int getLevel () {
		if (progressPersistent) {
			Cursor cur = getContentResolver().query(currentSerie, new String[]{Series.PROGRESS, Series.NUM_LEVELS}, null, null, null);
			if (!cur.moveToFirst()) {
				cur.close();
				return 0;
			}
			int progress = cur.getInt(cur.getColumnIndex(Series.PROGRESS));
			int numLevels = cur.getInt(cur.getColumnIndex(Series.NUM_LEVELS));
			cur.close();
			if (progress > numLevels) //This might happen during level edition
				progress = numLevels;
			Log.i(TAG, "getLevel(), persistent : " + progress);
			return progress; 
		} else {
			Log.i(TAG, "getLevel(), non-persistent : " + progress);
			return progress;
		}
	}
	
	public synchronized void saveProgress (int level) {
		if (progressPersistent) {
			ContentValues values = new ContentValues();
			values.put(Series.PROGRESS, level);
			getContentResolver().update(currentSerie, values, null, null);
		} else {
			progress = level;
		}
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
