package net.fhtagn.zoobgame.menus;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import net.fhtagn.zoobgame.R;
import net.fhtagn.zoobgame.Zoob;
import net.fhtagn.zoobgame.ZoobApplication;
import net.fhtagn.zoobgame.menus.MainMenu.GalleryTextAdapter;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Shader.TileMode;
import android.graphics.drawable.BitmapDrawable;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.View.OnClickListener;
import android.widget.BaseAdapter;
import android.widget.FrameLayout;
import android.widget.Gallery;
import android.widget.LinearLayout;
import android.widget.TextView;

public class MainMenuView extends FrameLayout {
	static final String TAG = "MainMenu";
	
	private final Zoob activity;
	
	public MainMenuView(Zoob activity) {
	  super(activity);
	  this.activity = activity;
	  LayoutInflater inflater = (LayoutInflater)activity.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
	  this.addView(inflater.inflate(R.layout.mainmenu, null), 
	  		new FrameLayout.LayoutParams(LayoutParams.FILL_PARENT, LayoutParams.FILL_PARENT));
	  setupButtons();
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
				Intent i = new Intent(activity, OptionsMenu.class);
				activity.startActivityForResult(i, Common.REQUEST_OPTIONS);
   	}
   });
   
   OogieTextView serieName = (OogieTextView)findViewById(R.id.serie_name);
   
   final Gallery lvlGallery = (Gallery)findViewById(R.id.lvlgallery);
   lvlGallery.setUnselectedAlpha(0.3f);
   JSONObject serieJSON = ((ZoobApplication)activity.getApplication()).getSerieJSON();
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
			 activity.play(selectedLevel);
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
				tv = new OogieTextView(activity);
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
