package net.fhtagn.zoobgame;

import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.GregorianCalendar;
import java.util.HashMap;
import java.util.TimeZone;
import java.util.Map.Entry;

import org.json.JSONException;
import org.json.JSONObject;

import android.content.ContentProvider;
import android.content.ContentUris;
import android.content.ContentValues;
import android.content.Context;
import android.content.UriMatcher;
import android.database.Cursor;
import android.database.SQLException;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;
import android.database.sqlite.SQLiteQueryBuilder;
import android.database.sqlite.SQLiteDatabase.CursorFactory;
import android.net.Uri;
import android.provider.BaseColumns;
import android.text.TextUtils;
import android.util.Log;

public class SerieContentProvider extends ContentProvider {
	static final String TAG = "SerieContentProvider";
	private static final String DATABASE_NAME = "zoob.db";
	private static final int DATABASE_VERSION = 14;
	private static final String SERIE_TABLE_NAME = "series";
	static final String AUTHORITY = "net.fhtagn.zoobgame.SerieContentProvider";
	private static final int SERIES = 1; //code for uri matcher
	private static final int SERIE_ID = 2;
	private static final UriMatcher uriMatcher;
	
	private static final HashMap<String, String> levelsProjectionMap;
	
	private static final SimpleDateFormat sqlDateFormat = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss"); // set the format to sql date time
	
	private static class DatabaseHelper extends SQLiteOpenHelper {
		public DatabaseHelper(Context context) {
	    super(context, DATABASE_NAME, null, DATABASE_VERSION);
    }

		@Override
    public void onCreate(SQLiteDatabase db) {
			db.execSQL("CREATE TABLE " + SERIE_TABLE_NAME + "(" 
					+ Series.ID + " INTEGER PRIMARY KEY AUTOINCREMENT, "
					+ Series.NAME + " VARCHAR(255) NOT NULL,"
					+ Series.COMMUNITY_ID + " INTEGER UNIQUE, "
					+ Series.NUM_LEVELS + " INTEGER, "
					+ Series.RATING + " FLOAT, "
					+ Series.MY_RATING + " FLOAT, "
					+ Series.AUTHOR + " VARCHAR(255), "
					+ Series.UPDATE_AVAILABLE + " BOOLEAN NOT NULL DEFAULT 0, "
					+ Series.JSON + " TEXT NOT NULL, "
					+ Series.IS_MINE + " BOOLEAN, "
					+ Series.PROGRESS + " INTEGER NOT NULL DEFAULT 0, "
					+ Series.UPLOAD_DATE + " DATE, "
					+ Series.LAST_MODIFICATION + " DATE NOT NULL DEFAULT CURRENT_TIMESTAMP);");
			
			ContentValues values = new ContentValues();
			values.put(Series.JSON, originalLevel);
			values.put(Series.IS_MINE, 0);
			cacheInfos(values);	
			long rowId = db.insert(SERIE_TABLE_NAME, Series.JSON, values);
			if (rowId != Series.ORIGINAL_ID) {
				throw new SQLException("ID for original level is not 1 but " + rowId);
			}
    }

		@Override
    public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
			//FIXME: don't do that :)
      Log.w(TAG, "Upgrading database from version " + oldVersion + " to " + newVersion
          + ", which will destroy all old data");
			db.execSQL("DROP TABLE IF EXISTS " + SERIE_TABLE_NAME);
			onCreate(db);	    
    }
	}
	
	private DatabaseHelper dbHelper;
	
	//This function will prevent any modification on the original serie record by throwing an IllegalArgumentException
	//if where/whereArgs combination would lead to such a modification
	private void preventOriginalSerieDeletion (SQLiteDatabase db, String where, String[] whereArgs) {
		Cursor c = db.query(SERIE_TABLE_NAME, new String[]{Series.ID}, Series.ID+"=="+Series.ORIGINAL_ID+" AND "+where, whereArgs, null, null, null);
		if (c.getCount() > 0) {
			c.close();
			throw new IllegalArgumentException("Query would delete original serie, aborting. DELETE WHERE " + where + " with args : " + whereArgs);
		}
		c.close();
	}
	
	private void preventOriginalSerieUpdate (ContentValues update, SQLiteDatabase db, String where, String[] whereArgs) {
		Cursor c = db.query(SERIE_TABLE_NAME, new String[]{Series.ID}, Series.ID+"=="+Series.ORIGINAL_ID+" AND "+where, whereArgs, null, null, null);
		if (c.getCount() > 0) { 
			c.close();
			if (!(update.size() == 1 && update.containsKey(Series.PROGRESS))) {
				throw new IllegalArgumentException("Updating anything else than progress on original serie is disallowed. where=[" + where+"]");	
			}
		}	
		c.close();
	}
	
	@Override
  public int delete(Uri uri, String where, String[] whereArgs) {
		SQLiteDatabase db = dbHelper.getWritableDatabase();
		int count;
		switch (uriMatcher.match(uri)) {
			case SERIES:
				preventOriginalSerieDeletion(db, where, whereArgs);
				count = db.delete(SERIE_TABLE_NAME, where, whereArgs);
				break;
			case SERIE_ID:
				String noteId = uri.getPathSegments().get(1);
				where = Series.ID + "=" + noteId + (!TextUtils.isEmpty(where) ? " AND (" + where + ')' : "");
				preventOriginalSerieDeletion(db, where, whereArgs);
				count = db.delete(SERIE_TABLE_NAME, where, whereArgs);
				break;
			default:
				throw new IllegalArgumentException("Unknown uri : " + uri);
		}
		getContext().getContentResolver().notifyChange(uri, null);
	  return count;
  }

	@Override
  public String getType(Uri uri) {
		switch (uriMatcher.match(uri)) {
			case SERIES:
				return Series.CONTENT_TYPE;
			case SERIE_ID:
				return Series.CONTENT_ITEM_TYPE;
			default:
				throw new IllegalArgumentException("Unknown uri : " + uri);
		}
  }
	
	private Date getUTCTime () {
		Calendar cal = new GregorianCalendar(TimeZone.getTimeZone("GMT"));
		return cal.getTime();
	}
	
	private static void assertNotContained (String key, ContentValues values) {
		if (values.containsKey(key)) 
			throw new IllegalArgumentException(key + " shouldn't be specified in ContentValues, it is infered from JSON");
	}
	private static void cacheInfos (ContentValues values) {
		assertNotContained(Series.NAME, values);
		assertNotContained(Series.NUM_LEVELS, values);
		assertNotContained(Series.RATING, values);
		assertNotContained(Series.AUTHOR, values);
		
		if (!values.containsKey(Series.JSON))
			return;
			
		try {
			JSONObject serieObj = new JSONObject(values.getAsString(Series.JSON));
			values.put(Series.NAME, serieObj.getString("name"));
			if (serieObj.has("levels"))
				values.put(Series.NUM_LEVELS, serieObj.getJSONArray("levels").length());
			
			if (serieObj.has("meta")) { 
				final JSONObject meta = serieObj.getJSONObject("meta");
				if (meta.has("rating"))
					values.put(Series.RATING, meta.getDouble("rating"));
				if (meta.has("author"))
					values.put(Series.AUTHOR, meta.getString("author"));
				if (meta.has("my_rating"))
					values.put(Series.MY_RATING, meta.getDouble("my_rating"));
			}
		} catch (JSONException e) {
			e.printStackTrace();
			throw new SQLException("Invalid JSON");
		}
	}

	@Override
  public Uri insert(Uri uri, ContentValues initialValues) {
		if (uriMatcher.match(uri) != SERIES)
			throw new IllegalArgumentException("Unknown uri : " + uri);
		
		ContentValues values;
		if (initialValues != null)
			values = new ContentValues(initialValues);
		else
			throw new IllegalArgumentException("insert: NULL initialValues");
		
		
		if (!values.containsKey(Series.JSON) ||
				!values.containsKey(Series.IS_MINE)) {
			throw new IllegalArgumentException(Series.JSON + " and " + Series.IS_MINE + " are required to insert a new serie");
		}
		
		cacheInfos(values);
		
		if (!values.containsKey(Series.PROGRESS))
			values.put(Series.PROGRESS, 0);
		
		values.put(Series.LAST_MODIFICATION, sqlDateFormat.format(getUTCTime()));
		
		SQLiteDatabase db = dbHelper.getWritableDatabase();
		long rowId = db.insert(SERIE_TABLE_NAME, Series.JSON, values);
		if (rowId > 0) {
			Uri levelUri = ContentUris.withAppendedId(Series.CONTENT_URI, rowId);
			getContext().getContentResolver().notifyChange(levelUri, null);
			return levelUri;
		}
		throw new SQLException("Failed to insert row into : " + uri);
  }

	@Override
  public boolean onCreate() {
		dbHelper = new DatabaseHelper(getContext());
	  return true;
  }

	@Override
  public Cursor query(Uri uri, String[] projection, String where, String[] whereArgs, String sortOrder) {
		SQLiteQueryBuilder qb = new SQLiteQueryBuilder();
		switch (uriMatcher.match(uri)) {
			case SERIES:
				qb.setTables(SERIE_TABLE_NAME);
				qb.setProjectionMap(levelsProjectionMap);
				break;
			case SERIE_ID:
				qb.setTables(SERIE_TABLE_NAME);
				qb.setProjectionMap(levelsProjectionMap);
				qb.appendWhere(Series.ID + "=" + uri.getPathSegments().get(1));
				break;
			default:
				throw new IllegalArgumentException("Unknown URI : " + uri);
		}
		
		SQLiteDatabase db = dbHelper.getReadableDatabase();
		Cursor c = qb.query(db, projection, where, whereArgs, null, null, (sortOrder==null)?Series.DEFAULT_SORT_ORDER:sortOrder);
		c.setNotificationUri(getContext().getContentResolver(), uri);
		return c;
  }

	@Override
  public int update(Uri uri, ContentValues values, String where, String[] whereArgs) {
		SQLiteDatabase db = dbHelper.getWritableDatabase();
		int count;
		//Don't consider progress update as a real update for last_modification
		if (!(values.size() == 1 && values.containsKey(Series.PROGRESS)))
				values.put(Series.LAST_MODIFICATION, sqlDateFormat.format(getUTCTime()));
		switch (uriMatcher.match(uri)) {
			case SERIES:
				preventOriginalSerieUpdate(values, db, where, whereArgs);
				cacheInfos(values); //doing that before preventOriginal would prevent it to work (because we change values.count)
				count = db.update(SERIE_TABLE_NAME, values, where, whereArgs);
				break;
			case SERIE_ID:
				String serieId = uri.getPathSegments().get(1);
				where = Series.ID + "=" + serieId + (!TextUtils.isEmpty(where) ? " AND (" + where + ')' : "");
				preventOriginalSerieUpdate(values, db, where, whereArgs);
				cacheInfos(values); //doing that before preventOriginal would prevent it to work (because we change values.count)
				count = db.update(SERIE_TABLE_NAME, values, where, whereArgs);
				break;
			default:
				throw new IllegalArgumentException("Unknown URI : " + uri);
		}
		getContext().getContentResolver().notifyChange(uri, null);
	  return count;
  }
	
	static {
		uriMatcher = new UriMatcher(UriMatcher.NO_MATCH);
		//CONTRARY to what the android docs show, the starting slash should be OMITTED ! 
		uriMatcher.addURI(AUTHORITY, "series", SERIES);
		uriMatcher.addURI(AUTHORITY, "series/#", SERIE_ID);
		
		levelsProjectionMap = new HashMap<String, String>();
		levelsProjectionMap.put(Series.ID, Series.ID);
		levelsProjectionMap.put(Series.NAME, Series.NAME);
		levelsProjectionMap.put(Series.JSON, Series.JSON);
		levelsProjectionMap.put(Series.IS_MINE, Series.IS_MINE);
		levelsProjectionMap.put(Series.PROGRESS, Series.PROGRESS);
		levelsProjectionMap.put(Series.NUM_LEVELS, Series.NUM_LEVELS);
		levelsProjectionMap.put(Series.UPDATE_AVAILABLE, Series.UPDATE_AVAILABLE);
		levelsProjectionMap.put(Series.RATING, Series.RATING);
		levelsProjectionMap.put(Series.MY_RATING, Series.MY_RATING);
		levelsProjectionMap.put(Series.AUTHOR, Series.AUTHOR);
		levelsProjectionMap.put(Series.COMMUNITY_ID, Series.COMMUNITY_ID);
		levelsProjectionMap.put(Series.LAST_MODIFICATION, Series.LAST_MODIFICATION);
		levelsProjectionMap.put(Series.UPLOAD_DATE, Series.UPLOAD_DATE);
	}
	
	//This is the original level set
	private static final String originalLevel = "{\"levels\": [{\"shadows\": false, \"tiles\": [[\"W\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"W\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"W\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"R\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"R\", \"E\", \"E\", \"W\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"W\"]], \"ydim\": 8, \"xdim\": 8, \"boss\": false, \"tanks\": [{\"type\": \"player\", \"coords\": [1, 1]}, {\"type\": \"static\", \"coords\": [3, 3]}]}, {\"shadows\": false, \"tiles\": [[\"W\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"W\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"W\", \"E\", \"W\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"W\", \"E\", \"W\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"W\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"W\"]], \"ydim\": 8, \"xdim\": 8, \"boss\": false, \"tanks\": [{\"type\": \"player\", \"coords\": [1, 1]}, {\"type\": \"simple\", \"coords\": [3, 3]}, {\"type\": \"simple\", \"coords\": [6, 6]}]}, {\"shadows\": false, \"tiles\": [[\"W\", \"T\", \"W\", \"T\", \"T\", \"T\", \"T\", \"T\", \"W\"], [\"L\", \"E\", \"R\", \"E\", \"E\", \"E\", \"M\", \"E\", \"R\"], [\"L\", \"E\", \"T\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"M\", \"E\", \"R\"], [\"L\", \"E\", \"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"M\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"W\"]], \"ydim\": 8, \"xdim\": 9, \"boss\": false, \"tanks\": [{\"type\": \"player\", \"coords\": [1, 1]}, {\"type\": \"simple\", \"coords\": [5, 3]}, {\"type\": \"simple\", \"coords\": [7, 1], \"path\": [[7, 1], [7, 6]]}]}, {\"shadows\": false, \"tiles\": [[\"W\", \"T\", \"W\", \"T\", \"T\", \"T\", \"T\", \"T\", \"W\"], [\"L\", \"E\", \"R\", \"E\", \"E\", \"E\", \"M\", \"E\", \"R\"], [\"L\", \"E\", \"T\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"M\", \"E\", \"R\"], [\"L\", \"E\", \"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"M\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"W\"]], \"ydim\": 8, \"xdim\": 9, \"boss\": false, \"tanks\": [{\"type\": \"player\", \"coords\": [1, 1]}, {\"type\": \"simple\", \"coords\": [5, 2]}, {\"type\": \"simple\", \"coords\": [5, 4]}, {\"type\": \"simple\", \"coords\": [5, 6]}, {\"type\": \"simple\", \"coords\": [7, 1], \"path\": [[7, 1], [7, 6]]}]}, {\"shadows\": false, \"tiles\": [[\"W\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"W\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"W\", \"E\", \"W\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"W\", \"E\", \"W\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"W\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"W\"]], \"ydim\": 8, \"xdim\": 9, \"boss\": false, \"tanks\": [{\"type\": \"player\", \"coords\": [1, 1]}, {\"type\": \"simple\", \"coords\": [3, 3], \"path\": [[3, 3], [5, 3], [5, 5], [3, 5]]}, {\"type\": \"simple\", \"coords\": [7, 5], \"path\": [[7, 5], [7, 1]]}, {\"type\": \"simple\", \"coords\": [6, 6], \"path\": [[6, 6], [1, 6]]}]}, {\"shadows\": true, \"tiles\": [[\"W\", \"T\", \"W\", \"T\", \"T\", \"T\", \"T\", \"T\", \"W\"], [\"L\", \"E\", \"R\", \"E\", \"E\", \"L\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"T\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"M\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"L\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"W\"]], \"ydim\": 8, \"reward_bombs\": true, \"xdim\": 9, \"boss\": true, \"tanks\": [{\"type\": \"player\", \"coords\": [1, 1]}, {\"type\": \"boss_simple\", \"coords\": [4, 4]}]}, {\"shadows\": false, \"tiles\": [[\"W\", \"T\", \"W\", \"T\", \"T\", \"T\", \"T\", \"T\", \"W\"], [\"L\", \"E\", \"R\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"T\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"W\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"W\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"W\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"W\"]], \"ydim\": 8, \"xdim\": 9, \"boss\": false, \"tanks\": [{\"type\": \"player\", \"coords\": [1, 1]}, {\"type\": \"simple\", \"coords\": [3, 1]}, {\"type\": \"simple\", \"coords\": [5, 1]}, {\"type\": \"bounce\", \"coords\": [6, 2], \"path\": [[6, 2], [6, 5]]}, {\"type\": \"bounce\", \"coords\": [4, 6], \"path\": [[4, 6], [7, 6]]}], \"bombs\": true}, {\"shadows\": false, \"tiles\": [[\"W\", \"T\", \"T\", \"T\", \"W\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"W\"], [\"L\", \"E\", \"E\", \"E\", \"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"E\", \"B\", \"B\", \"W\", \"E\", \"E\", \"E\", \"E\", \"W\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"L\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"L\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"L\", \"E\", \"E\", \"E\", \"E\", \"W\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"W\"]], \"ydim\": 8, \"xdim\": 12, \"boss\": false, \"tanks\": [{\"type\": \"player\", \"coords\": [1, 1]}, {\"type\": \"simple\", \"coords\": [2, 4]}, {\"type\": \"simple\", \"coords\": [5, 2]}, {\"type\": \"bounce\", \"coords\": [9, 3]}, {\"type\": \"bounce\", \"coords\": [9, 4]}], \"bombs\": true}, {\"shadows\": false, \"tiles\": [[\"W\", \"T\", \"W\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"W\"], [\"W\", \"E\", \"W\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"E\", \"W\", \"E\", \"E\", \"B\", \"B\", \"B\", \"B\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"T\", \"T\", \"T\", \"T\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"W\"]], \"ydim\": 8, \"xdim\": 12, \"boss\": false, \"tanks\": [{\"type\": \"player\", \"coords\": [1, 1]}, {\"type\": \"simple\", \"coords\": [5, 1], \"path\": [[5, 1], [8, 1]]}, {\"type\": \"simple\", \"coords\": [8, 6], \"path\": [[5, 6], [8, 6]]}, {\"type\": \"bounce\", \"coords\": [8, 3]}, {\"type\": \"bounce\", \"coords\": [8, 4]}], \"bombs\": true}, {\"shadows\": false, \"tiles\": [[\"W\", \"T\", \"W\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"W\"], [\"W\", \"E\", \"R\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"E\", \"R\", \"E\", \"E\", \"M\", \"E\", \"E\", \"M\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"M\", \"E\", \"E\", \"M\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"W\"]], \"ydim\": 8, \"xdim\": 12, \"boss\": false, \"tanks\": [{\"type\": \"player\", \"coords\": [1, 1]}, {\"type\": \"simple\", \"coords\": [6, 2], \"path\": [[6, 2], [5, 1], [4, 2], [5, 3]]}, {\"type\": \"simple\", \"coords\": [5, 4], \"path\": [[5, 4], [6, 5], [5, 6], [4, 5]]}, {\"type\": \"bounce\", \"coords\": [10, 2], \"path\": [[10, 2], [7, 3]]}, {\"type\": \"bounce\", \"coords\": [3, 1], \"path\": [[3, 1], [3, 3]]}], \"bombs\": true}, {\"shadows\": false, \"tiles\": [[\"W\", \"T\", \"W\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"W\"], [\"W\", \"E\", \"R\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"E\", \"R\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"R\", \"E\", \"E\", \"L\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"R\", \"E\", \"E\", \"L\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"W\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"W\"]], \"ydim\": 8, \"xdim\": 12, \"boss\": false, \"tanks\": [{\"type\": \"player\", \"coords\": [1, 1]}, {\"type\": \"bounce\", \"coords\": [4, 2], \"path\": [[4, 2], [4, 5], [6, 5], [6, 2]]}, {\"type\": \"bounce\", \"coords\": [7, 2], \"path\": [[7, 2], [7, 5], [9, 5], [9, 2]]}, {\"type\": \"simple\", \"coords\": [4, 6], \"path\": [[4, 6], [7, 6]]}], \"bombs\": true}, {\"shadows\": false, \"tiles\": [[\"W\", \"T\", \"W\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"W\"], [\"W\", \"E\", \"R\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"E\", \"R\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"R\", \"E\", \"E\", \"L\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"R\", \"E\", \"E\", \"L\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"W\"]], \"ydim\": 8, \"xdim\": 12, \"boss\": false, \"tanks\": [{\"type\": \"player\", \"coords\": [1, 1]}, {\"type\": \"bounce\", \"coords\": [6, 1]}, {\"type\": \"bounce\", \"coords\": [6, 6]}, {\"type\": \"simple\", \"coords\": [7, 3]}, {\"type\": \"simple\", \"coords\": [7, 4]}, {\"type\": \"bounce\", \"coords\": [10, 1]}, {\"type\": \"bounce\", \"coords\": [8, 6]}], \"bombs\": true}, {\"shadows\": false, \"tiles\": [[\"W\", \"T\", \"W\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"W\"], [\"W\", \"E\", \"R\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"E\", \"R\", \"E\", \"R\", \"E\", \"E\", \"R\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"R\", \"E\", \"E\", \"R\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"W\"]], \"ydim\": 8, \"xdim\": 12, \"boss\": false, \"tanks\": [{\"type\": \"player\", \"coords\": [1, 1]}, {\"type\": \"bounce\", \"coords\": [5, 2], \"path\": [[5, 2], [5, 3], [4, 3], [5, 3]]}, {\"type\": \"bounce\", \"coords\": [5, 5], \"path\": [[5, 5], [5, 4], [4, 4], [5, 4]]}, {\"type\": \"simple\", \"coords\": [8, 2], \"path\": [[8, 2], [8, 3], [7, 3], [8, 3]]}, {\"type\": \"simple\", \"coords\": [8, 5], \"path\": [[8, 5], [8, 4], [7, 4], [8, 4]]}, {\"type\": \"bounce\", \"coords\": [3, 1], \"path\": [[3, 1], [10, 1], [10, 6], [3, 6]]}], \"bombs\": true}, {\"shadows\": false, \"tiles\": [[\"W\", \"T\", \"T\", \"T\", \"W\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"W\"], [\"L\", \"E\", \"E\", \"E\", \"R\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"R\", \"E\", \"W\", \"B\", \"B\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"R\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"R\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"R\", \"E\", \"W\", \"T\", \"T\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"R\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"B\", \"B\", \"B\", \"W\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"W\"]], \"ydim\": 8, \"xdim\": 12, \"boss\": false, \"tanks\": [{\"type\": \"player\", \"coords\": [1, 3]}, {\"type\": \"simple\", \"coords\": [10, 1]}, {\"type\": \"simple\", \"coords\": [10, 6]}, {\"type\": \"simple\", \"coords\": [8, 3]}, {\"type\": \"simple\", \"coords\": [8, 4]}, {\"type\": \"bounce\", \"coords\": [5, 1]}, {\"type\": \"bounce\", \"coords\": [5, 6]}], \"bombs\": true}, {\"shadows\": false, \"tiles\": [[\"W\", \"T\", \"T\", \"T\", \"W\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"W\"], [\"L\", \"E\", \"E\", \"E\", \"R\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"R\", \"E\", \"W\", \"E\", \"E\", \"B\", \"W\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"R\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"R\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"R\", \"E\", \"W\", \"E\", \"E\", \"T\", \"W\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"R\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"B\", \"B\", \"B\", \"W\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"W\"]], \"ydim\": 8, \"xdim\": 12, \"boss\": false, \"tanks\": [{\"type\": \"player\", \"coords\": [1, 1]}, {\"type\": \"simple\", \"coords\": [7, 3]}, {\"type\": \"simple\", \"coords\": [7, 4]}, {\"type\": \"simple\", \"coords\": [3, 2]}, {\"type\": \"simple\", \"coords\": [3, 5]}, {\"type\": \"bounce\", \"coords\": [5, 1], \"path\": [[5, 1], [9, 1]]}, {\"type\": \"bounce\", \"coords\": [5, 6], \"path\": [[5, 6], [9, 6]]}], \"bombs\": true}, {\"shadows\": true, \"tiles\": [[\"W\", \"T\", \"W\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"W\"], [\"W\", \"E\", \"R\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"E\", \"R\", \"E\", \"E\", \"M\", \"E\", \"E\", \"M\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"M\", \"E\", \"E\", \"M\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"W\"]], \"ydim\": 8, \"xdim\": 12, \"boss\": true, \"reward_bounce\": true, \"tanks\": [{\"type\": \"player\", \"coords\": [1, 1]}, {\"type\": \"boss_bounce\", \"coords\": [7, 4]}], \"bombs\": true}, {\"shadows\": false, \"tiles\": [[\"W\", \"T\", \"W\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"W\"], [\"W\", \"E\", \"R\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"E\", \"R\", \"E\", \"E\", \"M\", \"E\", \"E\", \"M\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"M\", \"E\", \"E\", \"M\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"W\"]], \"ydim\": 8, \"xdim\": 12, \"boss\": false, \"bounce\": true, \"tanks\": [{\"type\": \"player\", \"coords\": [1, 1]}, {\"type\": \"bounce\", \"coords\": [3, 1]}, {\"type\": \"bounce\", \"coords\": [5, 1]}, {\"type\": \"bounce\", \"coords\": [6, 2], \"path\": [[6, 2], [6, 5]]}, {\"type\": \"shield\", \"coords\": [4, 6], \"path\": [[4, 6], [7, 6]]}], \"bombs\": true}, {\"shadows\": false, \"tiles\": [[\"W\", \"T\", \"W\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"W\"], [\"W\", \"E\", \"R\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"E\", \"R\", \"E\", \"E\", \"M\", \"E\", \"E\", \"M\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"M\", \"E\", \"E\", \"M\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"W\"]], \"ydim\": 8, \"xdim\": 12, \"boss\": false, \"bounce\": true, \"tanks\": [{\"type\": \"player\", \"coords\": [1, 1]}, {\"type\": \"bounce\", \"coords\": [3, 1]}, {\"type\": \"bounce\", \"coords\": [5, 6]}, {\"type\": \"bounce\", \"coords\": [6, 2], \"path\": [[6, 2], [6, 5]]}, {\"type\": \"shield\", \"coords\": [7, 4], \"path\": [[7, 4], [10, 4], [10, 6], [7, 6]]}, {\"type\": \"shield\", \"coords\": [9, 1], \"path\": [[9, 1], [9, 3]]}], \"bombs\": true}, {\"shadows\": false, \"tiles\": [[\"W\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"W\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"W\", \"E\", \"W\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"W\", \"E\", \"W\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"W\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"W\"]], \"ydim\": 8, \"xdim\": 9, \"boss\": false, \"bounce\": true, \"tanks\": [{\"type\": \"player\", \"coords\": [1, 1]}, {\"type\": \"shield\", \"coords\": [3, 3], \"path\": [[3, 3], [5, 3], [5, 5], [3, 5]]}, {\"type\": \"bounce\", \"coords\": [7, 5], \"path\": [[7, 5], [7, 1]]}, {\"type\": \"bounce\", \"coords\": [6, 6], \"path\": [[6, 6], [1, 6]]}], \"bombs\": true}, {\"shadows\": false, \"tiles\": [[\"W\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"W\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"W\", \"E\", \"W\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"W\", \"E\", \"W\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"W\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"W\"]], \"ydim\": 8, \"xdim\": 9, \"boss\": false, \"bounce\": true, \"tanks\": [{\"type\": \"player\", \"coords\": [1, 1]}, {\"type\": \"shield\", \"coords\": [3, 3], \"path\": [[3, 3], [5, 3], [5, 5], [3, 5]]}, {\"type\": \"shield\", \"coords\": [7, 5], \"path\": [[7, 5], [7, 1]]}, {\"type\": \"shield\", \"coords\": [6, 6], \"path\": [[6, 6], [1, 6]]}], \"bombs\": true}, {\"shadows\": false, \"tiles\": [[\"W\", \"T\", \"W\", \"T\", \"T\", \"T\", \"T\", \"T\", \"W\"], [\"L\", \"E\", \"R\", \"E\", \"E\", \"E\", \"M\", \"E\", \"R\"], [\"L\", \"E\", \"T\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"M\", \"E\", \"R\"], [\"L\", \"E\", \"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"M\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"W\"]], \"ydim\": 8, \"xdim\": 9, \"boss\": false, \"bounce\": true, \"tanks\": [{\"type\": \"player\", \"coords\": [1, 1]}, {\"type\": \"shield\", \"coords\": [5, 2]}, {\"type\": \"shield\", \"coords\": [5, 4]}, {\"type\": \"shield\", \"coords\": [5, 6]}, {\"type\": \"shield\", \"coords\": [7, 1], \"path\": [[7, 1], [7, 6]]}], \"bombs\": true}, {\"shadows\": false, \"tiles\": [[\"W\", \"T\", \"W\", \"T\", \"T\", \"T\", \"T\", \"T\", \"W\"], [\"L\", \"E\", \"R\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"T\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"W\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"W\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"W\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"W\"]], \"ydim\": 8, \"xdim\": 9, \"boss\": false, \"bounce\": true, \"tanks\": [{\"type\": \"player\", \"coords\": [1, 1]}, {\"type\": \"shield\", \"coords\": [3, 1]}, {\"type\": \"shield\", \"coords\": [5, 1]}, {\"type\": \"bounce\", \"coords\": [6, 2], \"path\": [[6, 2], [6, 5]]}, {\"type\": \"bounce\", \"coords\": [4, 6], \"path\": [[4, 6], [7, 6]]}], \"bombs\": true}, {\"shadows\": false, \"tiles\": [[\"W\", \"T\", \"W\", \"T\", \"W\", \"T\", \"T\", \"T\", \"W\"], [\"L\", \"E\", \"R\", \"E\", \"R\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"T\", \"E\", \"R\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"R\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"T\", \"T\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"W\"]], \"ydim\": 8, \"xdim\": 9, \"boss\": false, \"bounce\": true, \"tanks\": [{\"type\": \"player\", \"coords\": [1, 1]}, {\"type\": \"shield\", \"coords\": [3, 2]}, {\"type\": \"simple\", \"coords\": [5, 1]}, {\"type\": \"shield\", \"coords\": [4, 6], \"path\": [[1, 6], [4, 6]]}, {\"type\": \"bounce\", \"coords\": [5, 1]}, {\"type\": \"shield\", \"coords\": [7, 1], \"path\": [[7, 1], [7, 6]]}], \"bombs\": true}, {\"shadows\": false, \"tiles\": [[\"W\", \"T\", \"W\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"W\"], [\"W\", \"E\", \"W\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"E\", \"W\", \"E\", \"E\", \"B\", \"B\", \"B\", \"B\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"T\", \"T\", \"T\", \"T\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"W\"]], \"ydim\": 8, \"xdim\": 12, \"boss\": false, \"bounce\": true, \"tanks\": [{\"type\": \"player\", \"coords\": [1, 1]}, {\"type\": \"shield\", \"coords\": [5, 1], \"path\": [[5, 1], [8, 1]]}, {\"type\": \"shield\", \"coords\": [8, 6], \"path\": [[5, 6], [8, 6]]}, {\"type\": \"bounce\", \"coords\": [8, 3]}, {\"type\": \"bounce\", \"coords\": [8, 4]}], \"bombs\": true}, {\"shadows\": false, \"tiles\": [[\"W\", \"T\", \"W\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"W\"], [\"W\", \"E\", \"W\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"E\", \"W\", \"E\", \"R\", \"E\", \"B\", \"B\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"R\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"R\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"R\", \"E\", \"T\", \"T\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"W\"]], \"ydim\": 8, \"xdim\": 12, \"boss\": false, \"bounce\": true, \"tanks\": [{\"type\": \"player\", \"coords\": [1, 1]}, {\"type\": \"shield\", \"coords\": [5, 1], \"path\": [[4, 1], [9, 1]]}, {\"type\": \"shield\", \"coords\": [5, 6], \"path\": [[4, 6], [9, 6]]}, {\"type\": \"bounce\", \"coords\": [5, 2]}, {\"type\": \"bounce\", \"coords\": [5, 5]}, {\"type\": \"bounce\", \"coords\": [8, 3]}, {\"type\": \"bounce\", \"coords\": [8, 4]}], \"bombs\": true}, {\"shadows\": true, \"tiles\": [[\"W\", \"T\", \"W\", \"T\", \"T\", \"T\", \"T\", \"T\", \"W\"], [\"L\", \"E\", \"R\", \"E\", \"E\", \"L\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"T\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"M\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"L\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"W\"]], \"ydim\": 8, \"reward_shield\": true, \"xdim\": 9, \"boss\": true, \"bounce\": true, \"tanks\": [{\"type\": \"player\", \"coords\": [1, 1]}, {\"type\": \"boss_shield\", \"coords\": [4, 4]}], \"bombs\": true}, {\"shadows\": false, \"tiles\": [[\"W\", \"T\", \"W\", \"T\", \"T\", \"T\", \"T\", \"T\", \"W\"], [\"L\", \"E\", \"R\", \"E\", \"E\", \"E\", \"M\", \"E\", \"R\"], [\"L\", \"E\", \"T\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"M\", \"E\", \"R\"], [\"L\", \"E\", \"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"M\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"W\"]], \"ydim\": 8, \"xdim\": 9, \"boss\": false, \"bounce\": true, \"tanks\": [{\"type\": \"player\", \"coords\": [1, 1]}, {\"type\": \"burst\", \"coords\": [5, 2]}, {\"type\": \"burst\", \"coords\": [5, 4]}, {\"type\": \"shield\", \"coords\": [5, 6]}, {\"type\": \"shield\", \"coords\": [7, 1], \"path\": [[7, 1], [7, 6]]}], \"bombs\": true, \"shield\": true}, {\"shadows\": false, \"tiles\": [[\"W\", \"T\", \"W\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"W\"], [\"W\", \"E\", \"W\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"E\", \"W\", \"E\", \"R\", \"E\", \"B\", \"B\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"R\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"R\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"R\", \"E\", \"T\", \"T\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"W\"]], \"ydim\": 8, \"xdim\": 12, \"boss\": false, \"bounce\": true, \"tanks\": [{\"type\": \"player\", \"coords\": [1, 1]}, {\"type\": \"burst\", \"coords\": [5, 1], \"path\": [[4, 1], [9, 1]]}, {\"type\": \"burst\", \"coords\": [5, 6], \"path\": [[4, 6], [9, 6]]}, {\"type\": \"bounce\", \"coords\": [5, 2]}, {\"type\": \"bounce\", \"coords\": [5, 5]}, {\"type\": \"shield\", \"coords\": [8, 3]}, {\"type\": \"shield\", \"coords\": [8, 4]}], \"bombs\": true, \"shield\": true}, {\"shadows\": false, \"tiles\": [[\"W\", \"T\", \"W\", \"T\", \"T\", \"T\", \"T\", \"T\", \"W\"], [\"L\", \"E\", \"R\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"T\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"W\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"W\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"W\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"W\"]], \"ydim\": 8, \"xdim\": 9, \"boss\": false, \"bounce\": true, \"tanks\": [{\"type\": \"player\", \"coords\": [1, 1]}, {\"type\": \"shield\", \"coords\": [3, 1]}, {\"type\": \"shield\", \"coords\": [5, 1]}, {\"type\": \"burst\", \"coords\": [6, 2], \"path\": [[6, 2], [6, 5]]}, {\"type\": \"burst\", \"coords\": [4, 6], \"path\": [[4, 6], [7, 6]]}], \"bombs\": true, \"shield\": true}, {\"shadows\": false, \"tiles\": [[\"W\", \"T\", \"T\", \"T\", \"W\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"W\"], [\"L\", \"E\", \"E\", \"E\", \"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"E\", \"B\", \"B\", \"W\", \"E\", \"E\", \"E\", \"E\", \"W\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"L\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"L\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"L\", \"E\", \"E\", \"E\", \"E\", \"W\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"W\"]], \"ydim\": 8, \"xdim\": 12, \"boss\": false, \"bounce\": true, \"tanks\": [{\"type\": \"player\", \"coords\": [1, 1]}, {\"type\": \"burst\", \"coords\": [2, 4]}, {\"type\": \"burst\", \"coords\": [5, 2]}, {\"type\": \"shield\", \"coords\": [9, 3]}, {\"type\": \"shield\", \"coords\": [9, 4]}], \"bombs\": true, \"shield\": true}, {\"shadows\": false, \"tiles\": [[\"W\", \"T\", \"W\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"W\"], [\"W\", \"E\", \"W\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"E\", \"W\", \"E\", \"E\", \"B\", \"B\", \"B\", \"B\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"T\", \"T\", \"T\", \"T\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"W\"]], \"ydim\": 8, \"xdim\": 12, \"boss\": false, \"bounce\": true, \"tanks\": [{\"type\": \"player\", \"coords\": [1, 1]}, {\"type\": \"burst\", \"coords\": [5, 1], \"path\": [[5, 1], [8, 1]]}, {\"type\": \"burst\", \"coords\": [8, 6], \"path\": [[5, 6], [8, 6]]}, {\"type\": \"shield\", \"coords\": [8, 3]}, {\"type\": \"shield\", \"coords\": [8, 4]}], \"bombs\": true, \"shield\": true}, {\"shadows\": false, \"tiles\": [[\"W\", \"T\", \"W\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"W\"], [\"W\", \"E\", \"R\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"E\", \"R\", \"E\", \"E\", \"M\", \"E\", \"E\", \"M\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"M\", \"E\", \"E\", \"M\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"W\"]], \"ydim\": 8, \"xdim\": 12, \"boss\": false, \"bounce\": true, \"tanks\": [{\"type\": \"player\", \"coords\": [1, 1]}, {\"type\": \"burst\", \"coords\": [6, 2], \"path\": [[6, 2], [5, 1], [4, 2], [5, 3]]}, {\"type\": \"burst\", \"coords\": [5, 4], \"path\": [[5, 4], [6, 5], [5, 6], [4, 5]]}, {\"type\": \"bounce\", \"coords\": [10, 2], \"path\": [[10, 2], [7, 3]]}, {\"type\": \"bounce\", \"coords\": [3, 1], \"path\": [[3, 1], [3, 3]]}], \"bombs\": true, \"shield\": true}, {\"shadows\": false, \"tiles\": [[\"W\", \"T\", \"W\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"W\"], [\"W\", \"E\", \"R\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"E\", \"R\", \"E\", \"E\", \"M\", \"E\", \"E\", \"M\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"M\", \"E\", \"E\", \"M\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"W\"]], \"ydim\": 8, \"xdim\": 12, \"boss\": false, \"bounce\": true, \"tanks\": [{\"type\": \"player\", \"coords\": [1, 1]}, {\"type\": \"bounce\", \"coords\": [3, 1]}, {\"type\": \"bounce\", \"coords\": [5, 1]}, {\"type\": \"shield\", \"coords\": [6, 2], \"path\": [[6, 2], [6, 5]]}, {\"type\": \"burst\", \"coords\": [4, 6], \"path\": [[4, 6], [7, 6]]}], \"bombs\": true, \"shield\": true}, {\"shadows\": false, \"tiles\": [[\"W\", \"T\", \"W\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"W\"], [\"W\", \"E\", \"R\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"E\", \"R\", \"E\", \"E\", \"M\", \"E\", \"E\", \"M\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"M\", \"E\", \"E\", \"M\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"W\"]], \"ydim\": 8, \"xdim\": 12, \"boss\": false, \"bounce\": true, \"tanks\": [{\"type\": \"player\", \"coords\": [1, 1]}, {\"type\": \"bounce\", \"coords\": [3, 1]}, {\"type\": \"bounce\", \"coords\": [5, 6]}, {\"type\": \"shield\", \"coords\": [6, 2], \"path\": [[6, 2], [6, 5]]}, {\"type\": \"burst\", \"coords\": [7, 4], \"path\": [[7, 4], [10, 4], [10, 6], [7, 6]]}, {\"type\": \"burst\", \"coords\": [9, 1], \"path\": [[9, 1], [9, 3]]}], \"bombs\": true, \"shield\": true}, {\"shadows\": false, \"tiles\": [[\"W\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"W\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"W\", \"E\", \"W\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"W\", \"E\", \"W\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"W\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"W\"]], \"ydim\": 8, \"xdim\": 9, \"boss\": false, \"bounce\": true, \"tanks\": [{\"type\": \"player\", \"coords\": [1, 1]}, {\"type\": \"burst\", \"coords\": [3, 3], \"path\": [[3, 3], [5, 3], [5, 5], [3, 5]]}, {\"type\": \"burst\", \"coords\": [7, 5], \"path\": [[7, 5], [7, 1]]}, {\"type\": \"burst\", \"coords\": [6, 6], \"path\": [[6, 6], [1, 6]]}], \"bombs\": true, \"shield\": true}, {\"shadows\": true, \"tiles\": [[\"W\", \"T\", \"W\", \"T\", \"T\", \"T\", \"T\", \"T\", \"W\"], [\"L\", \"E\", \"R\", \"E\", \"E\", \"L\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"T\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"M\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"L\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"W\"]], \"ydim\": 8, \"reward_firing\": true, \"xdim\": 9, \"boss\": true, \"bounce\": true, \"tanks\": [{\"type\": \"player\", \"coords\": [1, 1]}, {\"type\": \"boss_burst\", \"coords\": [4, 4]}], \"bombs\": true, \"shield\": true}, {\"shadows\": false, \"tiles\": [[\"W\", \"T\", \"W\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"W\"], [\"W\", \"E\", \"R\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"E\", \"R\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"R\", \"E\", \"E\", \"L\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"R\", \"E\", \"E\", \"L\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"W\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"W\"]], \"ydim\": 8, \"xdim\": 12, \"boss\": false, \"improved_firing\": true, \"bounce\": true, \"tanks\": [{\"type\": \"player\", \"coords\": [1, 1]}, {\"type\": \"burst\", \"coords\": [4, 2], \"path\": [[4, 2], [4, 5], [6, 5], [6, 2]]}, {\"type\": \"split\", \"coords\": [7, 2], \"path\": [[7, 2], [7, 5], [9, 5], [9, 2]]}, {\"type\": \"shield\", \"coords\": [4, 6], \"path\": [[4, 6], [7, 6]]}], \"bombs\": true, \"shield\": true}, {\"shadows\": false, \"tiles\": [[\"W\", \"T\", \"W\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"W\"], [\"W\", \"E\", \"R\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"E\", \"R\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"R\", \"E\", \"E\", \"L\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"R\", \"E\", \"E\", \"L\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"W\"]], \"ydim\": 8, \"xdim\": 12, \"boss\": false, \"improved_firing\": true, \"bounce\": true, \"tanks\": [{\"type\": \"player\", \"coords\": [1, 1]}, {\"type\": \"split\", \"coords\": [6, 1]}, {\"type\": \"bounce\", \"coords\": [6, 6]}, {\"type\": \"split\", \"coords\": [7, 3]}, {\"type\": \"split\", \"coords\": [7, 4]}, {\"type\": \"bounce\", \"coords\": [10, 1]}, {\"type\": \"bounce\", \"coords\": [8, 6]}], \"bombs\": true, \"shield\": true}, {\"shadows\": false, \"tiles\": [[\"W\", \"T\", \"W\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"W\"], [\"W\", \"E\", \"R\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"E\", \"R\", \"E\", \"R\", \"E\", \"E\", \"R\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"R\", \"E\", \"E\", \"R\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"W\"]], \"ydim\": 8, \"xdim\": 12, \"boss\": false, \"improved_firing\": true, \"bounce\": true, \"tanks\": [{\"type\": \"player\", \"coords\": [1, 1]}, {\"type\": \"bounce\", \"coords\": [5, 2], \"path\": [[5, 2], [5, 3], [4, 3], [5, 3]]}, {\"type\": \"bounce\", \"coords\": [5, 5], \"path\": [[5, 5], [5, 4], [4, 4], [5, 4]]}, {\"type\": \"split\", \"coords\": [8, 2], \"path\": [[8, 2], [8, 3], [7, 3], [8, 3]]}, {\"type\": \"split\", \"coords\": [8, 5], \"path\": [[8, 5], [8, 4], [7, 4], [8, 4]]}, {\"type\": \"bounce\", \"coords\": [3, 1], \"path\": [[3, 1], [10, 1], [10, 6], [3, 6]]}], \"bombs\": true, \"shield\": true}, {\"shadows\": false, \"tiles\": [[\"W\", \"T\", \"T\", \"T\", \"W\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"W\"], [\"L\", \"E\", \"E\", \"E\", \"R\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"R\", \"E\", \"W\", \"B\", \"B\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"R\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"R\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"R\", \"E\", \"W\", \"T\", \"T\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"R\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"B\", \"B\", \"B\", \"W\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"W\"]], \"ydim\": 8, \"xdim\": 12, \"boss\": false, \"improved_firing\": true, \"bounce\": true, \"tanks\": [{\"type\": \"player\", \"coords\": [1, 3]}, {\"type\": \"split\", \"coords\": [10, 1]}, {\"type\": \"split\", \"coords\": [10, 6]}, {\"type\": \"shield\", \"coords\": [8, 3]}, {\"type\": \"shield\", \"coords\": [8, 4]}, {\"type\": \"bounce\", \"coords\": [5, 1]}, {\"type\": \"bounce\", \"coords\": [5, 6]}], \"bombs\": true, \"shield\": true}, {\"shadows\": false, \"tiles\": [[\"W\", \"T\", \"T\", \"T\", \"W\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"W\"], [\"L\", \"E\", \"E\", \"E\", \"R\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"R\", \"E\", \"W\", \"E\", \"E\", \"B\", \"W\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"R\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"R\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"R\", \"E\", \"W\", \"E\", \"E\", \"T\", \"W\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"R\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"B\", \"B\", \"B\", \"W\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"W\"]], \"ydim\": 8, \"xdim\": 12, \"boss\": false, \"improved_firing\": true, \"bounce\": true, \"tanks\": [{\"type\": \"player\", \"coords\": [1, 1]}, {\"type\": \"split\", \"coords\": [7, 3]}, {\"type\": \"split\", \"coords\": [7, 4]}, {\"type\": \"shield\", \"coords\": [3, 2]}, {\"type\": \"shield\", \"coords\": [3, 5]}, {\"type\": \"bounce\", \"coords\": [5, 1], \"path\": [[5, 1], [9, 1]]}, {\"type\": \"bounce\", \"coords\": [5, 6], \"path\": [[5, 6], [9, 6]]}], \"bombs\": true, \"shield\": true}, {\"shadows\": false, \"tiles\": [[\"W\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"W\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"W\", \"E\", \"W\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"W\", \"E\", \"W\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"W\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"W\"]], \"ydim\": 8, \"xdim\": 9, \"boss\": false, \"improved_firing\": true, \"bounce\": true, \"tanks\": [{\"type\": \"player\", \"coords\": [1, 1]}, {\"type\": \"split\", \"coords\": [3, 3], \"path\": [[3, 3], [5, 3], [5, 5], [3, 5]]}, {\"type\": \"split\", \"coords\": [7, 5], \"path\": [[7, 5], [7, 1]]}, {\"type\": \"split\", \"coords\": [6, 6], \"path\": [[6, 6], [1, 6]]}], \"bombs\": true, \"shield\": true}, {\"shadows\": false, \"tiles\": [[\"W\", \"T\", \"W\", \"T\", \"T\", \"T\", \"T\", \"T\", \"W\"], [\"L\", \"E\", \"R\", \"E\", \"E\", \"E\", \"M\", \"E\", \"R\"], [\"L\", \"E\", \"T\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"M\", \"E\", \"R\"], [\"L\", \"E\", \"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"M\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"W\"]], \"ydim\": 8, \"xdim\": 9, \"boss\": false, \"improved_firing\": true, \"bounce\": true, \"tanks\": [{\"type\": \"player\", \"coords\": [1, 1]}, {\"type\": \"split\", \"coords\": [5, 2]}, {\"type\": \"split\", \"coords\": [5, 4]}, {\"type\": \"split\", \"coords\": [5, 6]}, {\"type\": \"split\", \"coords\": [7, 1], \"path\": [[7, 1], [7, 6]]}], \"bombs\": true, \"shield\": true}, {\"shadows\": false, \"tiles\": [[\"W\", \"T\", \"W\", \"T\", \"T\", \"T\", \"T\", \"T\", \"W\"], [\"L\", \"E\", \"R\", \"E\", \"E\", \"E\", \"M\", \"E\", \"R\"], [\"L\", \"E\", \"T\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"M\", \"E\", \"R\"], [\"L\", \"E\", \"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"M\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"W\"]], \"ydim\": 8, \"xdim\": 9, \"boss\": false, \"improved_firing\": true, \"bounce\": true, \"tanks\": [{\"type\": \"player\", \"coords\": [1, 1]}, {\"type\": \"split\", \"coords\": [5, 3]}, {\"type\": \"split\", \"coords\": [7, 1], \"path\": [[7, 1], [7, 6]]}], \"bombs\": true, \"shield\": true}, {\"shadows\": false, \"tiles\": [[\"W\", \"T\", \"W\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"W\"], [\"W\", \"E\", \"R\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"E\", \"R\", \"E\", \"E\", \"M\", \"E\", \"E\", \"M\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"M\", \"E\", \"E\", \"M\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"W\"]], \"ydim\": 8, \"xdim\": 12, \"boss\": false, \"improved_firing\": true, \"bounce\": true, \"tanks\": [{\"type\": \"player\", \"coords\": [1, 1]}, {\"type\": \"split\", \"coords\": [3, 1]}, {\"type\": \"split\", \"coords\": [5, 6]}, {\"type\": \"shield\", \"coords\": [6, 2], \"path\": [[6, 2], [6, 5]]}, {\"type\": \"burst\", \"coords\": [7, 4], \"path\": [[7, 4], [10, 4], [10, 6], [7, 6]]}, {\"type\": \"burst\", \"coords\": [9, 1], \"path\": [[9, 1], [9, 3]]}], \"bombs\": true, \"shield\": true}, {\"shadows\": true, \"tiles\": [[\"W\", \"T\", \"W\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"T\", \"W\"], [\"W\", \"E\", \"R\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"E\", \"R\", \"E\", \"E\", \"M\", \"E\", \"E\", \"M\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"M\", \"E\", \"E\", \"M\", \"E\", \"E\", \"R\"], [\"L\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"E\", \"R\"], [\"W\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"B\", \"W\"]], \"ydim\": 8, \"xdim\": 12, \"boss\": true, \"improved_firing\": true, \"bounce\": true, \"tanks\": [{\"type\": \"player\", \"coords\": [1, 1]}, {\"type\": \"boss_split\", \"coords\": [3, 1]}], \"bombs\": true, \"shield\": true}], \"name\": \"official\"}";
}
