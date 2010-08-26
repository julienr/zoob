package net.fhtagn.zoobgame;

import org.json.JSONException;
import org.json.JSONObject;

import android.app.Application;
import android.content.ContentUris;
import android.content.ContentValues;
import android.content.Intent;
import android.content.SharedPreferences;
import android.database.Cursor;
import android.net.Uri;
import android.preference.PreferenceManager;
import android.util.Log;

public class ZoobApplication extends Application {
	static final String TAG = "ZoobApplication";
	//Deprecated: progress now saved using the content provider, only there to restore progress from previous versions when upgrading
	private static final String OLD_PREF_KEY_LEVEL = "level";
	private static final String OLD_PREF_KEY_DIFFICULTY = "difficulty";
	private static final String OLD_PREF_KEY_INPUT_METHOD = "input_mode";
	private static final String OLD_PREF_KEY_USE_TRACKBALL = "use_trackball";
	
	private static final String PREF_KEY_GAMEPAD = "input_gamepad";
	private static final String PREF_KEY_TRACKBALL = "input_trackball";
	private static final String PREF_KEY_DIFFICULTY = "game_difficulty";
	
	private SharedPreferences settings;
	
	private Uri currentSerie;
	
	//This is really just a cache
	private String currentJSONString = null;
	private JSONObject currentJSON = null;
	
	//if true, progress will be saved to preferences. This is the normal behaviour
	//This might be turned off and the progress won't be saved when zoob closes. This is usefull when doing level edition
	private boolean progressPersistent = true;
	private int progress = -1; //only used if progressSavePersistent = false
	
	//Now use getDefaultSharedPreferences()
	@Deprecated
	protected String getPrefsName () {
		return "net_fhtagn_zoobgame_prefs";
	}
	
	@Override
	public void onCreate () {
		super.onCreate();
		
		Log.i("ZoobApplication", "onCreate()");
		
    settings = PreferenceManager.getDefaultSharedPreferences(getApplicationContext());
    transferOldPref(OLD_PREF_KEY_INPUT_METHOD, PREF_KEY_GAMEPAD);
    transferOldPref(OLD_PREF_KEY_USE_TRACKBALL, PREF_KEY_TRACKBALL);
    
    //Insert default preferences
    if (!settings.contains(PREF_KEY_GAMEPAD)) {
    	SharedPreferences.Editor editor = settings.edit();
    	editor.putBoolean(PREF_KEY_GAMEPAD, true);
    	editor.putBoolean(PREF_KEY_TRACKBALL, false);
    	editor.commit();
    }
	}
	
	public synchronized void setSerieId (long id) {
		currentSerie = ContentUris.withAppendedId(Series.CONTENT_URI, id);
		loadJSON();
		if (id == 1) {
			//Original serie, try to restore progress
			transferProgressFromPreferences();
		}
	}
	
	public JSONObject getSerieJSON () {
		return currentJSON;
	}
	
	public String getSerieJSONString () {
		return currentJSONString;
	}
	
	private void loadJSON () {
		//Load serie JSON
		Cursor cur = getContentResolver().query(currentSerie, new String[]{Series.JSON}, null, null, null);
		if (cur == null || !cur.moveToFirst() || cur.getCount() != 1) {
			Log.e(TAG, "Unable to retrieve requested serie");
		} else {
			try {
				currentJSONString = cur.getString(cur.getColumnIndex(Series.JSON));
	      currentJSON = new JSONObject(currentJSONString);
	    } catch (JSONException e) {
	      e.printStackTrace();
	    }
		}
    cur.close();
	}
	
	//Transfer preferences from old sharedpreferences "getPrefsName" to new defaultsharedpreferences
	private void transferOldPref (String oldKey, String newKey) {
		SharedPreferences oldPrefs = getSharedPreferences(getPrefsName(), 0);
		if (oldPrefs.contains(oldKey)) {
			int value = oldPrefs.getInt(newKey, 0);
			saveBoolPref(newKey, value==1);
			SharedPreferences.Editor editor = oldPrefs.edit();
			editor.remove(oldKey);
			editor.commit();
		}
	}
	
	//This is a compability functions that will retrieve the progress currently saved in the user preferences (versions <= 1.0.2-2)
	//and transfer it to the contentprovider's db (versions > 1.0.2-2)
	private void transferProgressFromPreferences () {
		SharedPreferences oldPrefs = getSharedPreferences(getPrefsName(), 0);
		int settingsProgress = oldPrefs.getInt(OLD_PREF_KEY_LEVEL, 0); 
		if (settingsProgress != 0) {
			saveProgress(settingsProgress);
			SharedPreferences.Editor editor = oldPrefs.edit();
			editor.remove(OLD_PREF_KEY_LEVEL);
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
			Log.i(TAG, "saveProgress : " + level);
			values.put(Series.PROGRESS, level);
			getContentResolver().update(currentSerie, values, null, null);
		} else {
			progress = level;
		}
	}
	
	public synchronized int getDifficulty () {
		return settings.getInt(PREF_KEY_DIFFICULTY, 0);
	}
	
	public synchronized boolean usesGamepad () {
		return settings.getBoolean(PREF_KEY_GAMEPAD, true);
	}
	
	public synchronized boolean usesTrackball () {
		return settings.getBoolean(PREF_KEY_TRACKBALL, false);
	}
	
	private void saveBoolPref (String key, boolean val) {
		SharedPreferences.Editor editor = settings.edit();
		editor.putBoolean(key, val);
		editor.commit();
		Log.i("Zoob", "saveBoolPref : "+key+" = "+ val);
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
}
