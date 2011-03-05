package net.fhtagn.zoobgame;

import org.json.JSONException;
import org.json.JSONObject;

import android.app.Application;
import android.content.ComponentName;
import android.content.ContentUris;
import android.content.ContentValues;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.content.SharedPreferences;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.database.Cursor;
import android.net.Uri;
import android.os.Binder;
import android.os.IBinder;
import android.os.RemoteException;
import android.preference.PreferenceManager;
import android.util.Log;

public class ZoobApplication extends Application {
	static final String TAG = "ZoobApplication";
	
	private static final String PREF_KEY_GAMEPAD = "input_gamepad";
	private static final String PREF_KEY_TRACKBALL = "input_trackball";
	private static final String PREF_KEY_DIFFICULTY = "game_difficulty";
	
	//indicate wether old prefs have been imported (second version)
	private static final String PREF_KEY_OLDPREFS_IMPORTED = "oldprefs_imported2";
	
	private SharedPreferences settings;
	
	private Zoob zoobActivity = null;
	
	private Uri currentSerie;
	
	//This is really just a cache
	private String currentJSONString = null;
	private JSONObject currentJSON = null;
	
	//if true, progress will be saved to preferences. This is the normal behaviour
	//This might be turned off and the progress won't be saved when zoob closes. This is usefull when doing level edition
	private boolean progressPersistent = true;
	private int progress = -1; //only used if progressSavePersistent = false
	
	private boolean demo;
	
	public boolean isDemo () {
		return !demo;
	}
	
	@Override
	public void onCreate () {
		super.onCreate();
		
		Log.i("ZoobApplication", "onCreate()");
		//Check for full version key
		try {
	    getPackageManager().getPackageInfo("net.fhtagn.zoobgame", 0);
	    Log.i(TAG, "full version key found");
	    demo = true;
    } catch (NameNotFoundException e) {
    	Log.i(TAG, "full version key not found");
    	demo = false;
    }
		
    settings = PreferenceManager.getDefaultSharedPreferences(getApplicationContext());
    
    //Insert default preferences
    if (!settings.contains(PREF_KEY_GAMEPAD)) {
    	SharedPreferences.Editor editor = settings.edit();
    	editor.putBoolean(PREF_KEY_GAMEPAD, true);
    	editor.putBoolean(PREF_KEY_TRACKBALL, false);
    	editor.commit();
    }
	}
	
	public void registerZoob (Zoob zoob) {
		zoobActivity = zoob;
	}
	
	public synchronized void setSerieId (long id) {
		currentSerie = ContentUris.withAppendedId(Series.CONTENT_URI, id);
		loadJSON();
		//Original serie, try to restore progress
		if (id == Series.ORIGINAL_ID && !settings.getBoolean(PREF_KEY_OLDPREFS_IMPORTED, false)) {
			Log.i(TAG, "Trying to transfer old progress");
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
	
	//This is a compability functions that will retrieve the progress currently saved in the user preferences (versions <= 1.0.2-2)
	//and transfer it to the contentprovider's db (versions > 1.0.2-2)
	private void transferProgressFromPreferences () {
		try {
			//first test to see if zoobgame is installed
	    getPackageManager().getPackageInfo("net.fhtagn.zoobgame", 0);
	    
	    //if we get here, this means zoobgame is installed
			final Intent i = new Intent();
			i.setClassName("net.fhtagn.zoobgame", "net.fhtagn.zoobgame.ProgressService");
			bindService(i, new ServiceConnection() {
				@Override
	      public void onServiceConnected(ComponentName cn, IBinder binder) {
					Log.i(TAG, "service connected : " + cn);
		      ProgressBinder b = ProgressBinder.Stub.asInterface(binder);
	        try {
		        final int progress = b.getProgress();
		        Log.i(TAG, "old progress : " + progress);
		        //just take care of not removing progress
		        if (progress > getProgress()) {
		        	saveProgress(progress);
		        
			  			SharedPreferences.Editor editor = settings.edit();
			      	editor.putBoolean(PREF_KEY_OLDPREFS_IMPORTED, true);
			      	editor.commit();
			      	
			      	//need to refresh the level gallery on the UI
			      	if (zoobActivity != null)
			      		zoobActivity.refreshLevels();
			      	else
			      		Log.i(TAG, "null zoobActivity");
		        }
		      	
		      	//try to stop the service now that we are done
		      	ZoobApplication.this.unbindService(this);
	        } catch (RemoteException e) {
		        e.printStackTrace();
	        }
	      }

				@Override
	      public void onServiceDisconnected(ComponentName cn) {
					//This is called ONLY on crash, not after unbindService()
					Log.i(TAG, "service disconnected : " + cn);
	      }
			}, Context.BIND_AUTO_CREATE);
			
    } catch (NameNotFoundException e) {
    	return;
    } 
	}
	
	public synchronized void setProgressPersistent (boolean persistent) {
		progressPersistent = persistent;
	}
	
	//Returns the max playable level (depends on player's progress) in this serie
	public synchronized int getProgress () {
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
			int currentProgress = getProgress();
			if (level > currentProgress) {
				Log.i(TAG, "saveProgress : current="+currentProgress+", new=" + level);
				values.put(Series.PROGRESS, level);
				getContentResolver().update(currentSerie, values, null, null);
			} else {
				Log.i(TAG, "saveProgress : not saving, current="+currentProgress+", new=" + level);
			}
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
