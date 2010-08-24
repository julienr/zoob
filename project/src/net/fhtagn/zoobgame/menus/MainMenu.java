package net.fhtagn.zoobgame.menus;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import net.fhtagn.zoobgame.Eula;
import net.fhtagn.zoobgame.R;
import net.fhtagn.zoobgame.Series;
import net.fhtagn.zoobgame.Zoob;
import net.fhtagn.zoobgame.ZoobApplication;
import android.app.Activity;
import android.content.ContentUris;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.database.Cursor;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Typeface;
import android.graphics.Paint.FontMetrics;
import android.graphics.Shader.TileMode;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.View.OnClickListener;
import android.widget.BaseAdapter;
import android.widget.Gallery;
import android.widget.LinearLayout;
import android.widget.SimpleCursorAdapter;
import android.widget.SpinnerAdapter;
import android.widget.TextView;
import android.widget.Toast;

public class MainMenu extends MenuActivity {
	static final String TAG = "MainMenu";
	static final String ACTION_PLAY = "net.fhtagn.zoobgame.PLAY";
	
	static final int REQUEST_PLAY = 1;
	
	static JSONObject serieJSON = null;
	
  @Override
  public void onCreate(Bundle savedInstanceState) {
      super.onCreate(savedInstanceState);
      
  		//EULA
  		Eula.show(this);
  		
  		ZoobApplication app = (ZoobApplication)getApplication();
  		app.setProgressPersistent(true);
  		/** Intent resolution **/
  		Intent intent = getIntent();
  		int serieID;
  		if (intent != null && intent.getAction().equals(ACTION_PLAY)) { //Play a serie, specified in the intent
  			String lastSegment = intent.getData().getLastPathSegment();
  			
  			if (lastSegment == null) {
  				Log.e(TAG, "lastSegment = null when resolving PLAY intent : " + intent.getData());
  				serieID = 1;
  			} else {
  				serieID = Integer.parseInt(lastSegment);
  				Log.i(TAG, "PLAY intent received, serieID = " + serieID);
  			}
  			app.setSerieId(serieID);
  			//The URI can have a query parameter ?startlevel=<level>
  			String qp = intent.getData().getQueryParameter("startlevel");
  			int startLevel;
  			if (qp == null)
  				startLevel = 0;
  			else {
  				startLevel = Integer.parseInt(qp);
  				app.setProgressPersistent(false);
  				Log.i(TAG, "Got startlevel = " + startLevel);
  			}
  			app.saveProgress(startLevel);
  		} else {
  			Log.i(TAG, "No PLAY intent received, launching original serie");
  			serieID = 1;
  			app.setSerieId(serieID);
  		}
  		
  		//Load serie JSON
  		Uri serieURI = ContentUris.withAppendedId(Series.CONTENT_URI, serieID); //original serie has id 1
  		Cursor cur = managedQuery(serieURI, new String[]{Series.JSON}, null, null, null);
  		if (cur == null || !cur.moveToFirst() || cur.getCount() != 1) {
  			Log.e(TAG, "Unable to retrieve requested serie");
  			finish();
  			return;
  		}
  		try {
	      serieJSON = new JSONObject(cur.getString(cur.getColumnIndex(Series.JSON)));
      } catch (JSONException e) {
      	jsonError();
	      e.printStackTrace();
      }
      
  		setContentView(R.layout.mainmenu);
  		setupButtons();
  }
  
  private void jsonError () {
  	Toast toast = Toast.makeText(this, getResources().getString(R.string.error_loading), Toast.LENGTH_LONG);
  	toast.show();
  }
  
  private void setupButtons () {
  	 //FIXME: for a strange reason, this doesn't work from the XML, so do it programmatically
    LinearLayout container = (LinearLayout)findViewById(R.id.container);
    BitmapDrawable bg = (BitmapDrawable)getResources().getDrawable(R.drawable.backrepeat);
    bg.setTileModeX(TileMode.REPEAT);
    bg.setTileModeY(TileMode.REPEAT);
    container.setBackgroundDrawable(bg);
    
    BlurButton optionsBtn = (BlurButton)findViewById(R.id.options);
    optionsBtn.setOnClickListener(new OnClickListener () {
    	@Override
    	public void onClick(View view) {
				Intent i = new Intent(getApplicationContext(), OptionsMenu.class);
				startActivityForResult(i, Common.REQUEST_OPTIONS);
    	}
    });
    
    OogieTextView serieName = (OogieTextView)findViewById(R.id.serie_name);
    
    final Gallery lvlGallery = (Gallery)findViewById(R.id.lvlgallery);
    lvlGallery.setUnselectedAlpha(0.3f);
    try {
	    JSONArray lvlArray = serieJSON.getJSONArray("levels");
	    String[] levels = new String[lvlArray.length()];
	    for (int i=0; i<lvlArray.length(); i++) 
	    	levels[i] = ""+i;
	    lvlGallery.setAdapter(new GalleryTextAdapter(levels));
	    
	    serieName.setText(serieJSON.getString("name"));
    } catch (JSONException e) {
    	lvlGallery.setEnabled(false);
    	serieName.setTextColor(Color.RED);
    	serieName.setText(getResources().getString(R.string.error_loading));
	    e.printStackTrace();
    }
    
    BlurButton startBtn = (BlurButton)findViewById(R.id.start);
    startBtn.setOnClickListener(new OnClickListener() {
			@Override
      public void onClick(View view) {
				int selectedLevel = lvlGallery.getSelectedItemPosition();
				Intent i = new Intent(getApplicationContext(), Zoob.class);
        i.putExtra("json", serieJSON.toString());
        i.putExtra("level", selectedLevel);
        startActivityForResult(i, REQUEST_PLAY);
      }
    });
    
    Gallery diffGallery = (Gallery)findViewById(R.id.diffgallery);
    diffGallery.setUnselectedAlpha(0.3f);
    String[] difficulties = {"easy", "medium", "hard"};
    diffGallery.setAdapter(new GalleryTextAdapter(difficulties));
  }
  
  class GalleryTextAdapter extends BaseAdapter {
  	private final String[] elements;
  	
  	public GalleryTextAdapter (String [] elements) {
  		this.elements = elements;
  	}
  	
		@Override
    public int getCount() {
			return elements.length;
    }

		@Override
    public Object getItem(int position) {
			return position;
    }

		@Override
    public long getItemId(int id) {
			return id;
    }

		@Override
    public View getView(int position, View convertView, ViewGroup parent) {
			TextView tv;
			if (convertView == null) {
				tv = new OogieTextView(MainMenu.this);
				/*tv = new TextView(ZoobMenu.this);
				tv.setTypeface(oogie);
				tv.setTextSize(35*scale);
				tv.setShadowLayer(4, 1, 1, Color.BLACK);
				tv.setTextColor(Color.WHITE);*/
				final float scale = getResources().getDisplayMetrics().density;
				Paint paint = new Paint();
				paint.setTypeface(tv.getTypeface());
				paint.setTextSize(tv.getTextSize());
				tv.setLayoutParams(new Gallery.LayoutParams((int)(paint.measureText(elements[position])+40*scale), 60));
			} else {
				tv = (TextView)convertView;
			}
			tv.setText(elements[position]);
			return tv;
    }
  	
  }
}