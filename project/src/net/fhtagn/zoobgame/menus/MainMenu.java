package net.fhtagn.zoobgame.menus;

import android.app.Activity;
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

public class MainMenu extends MenuActivity {
	static final String TAG = "MainMenu";
	
  @Override
  public void onCreate(Bundle savedInstanceState) {
      super.onCreate(savedInstanceState);
  
      setContentView(R.layout.mainmenu);
      
      //FIXME: for a strange reason, this doesn't work from the XML, so do it programmatically
      LinearLayout container = (LinearLayout)findViewById(R.id.container);
      BitmapDrawable bg = (BitmapDrawable)getResources().getDrawable(R.drawable.backrepeat);
      bg.setTileModeX(TileMode.REPEAT);
      bg.setTileModeY(TileMode.REPEAT);
      container.setBackgroundDrawable(bg);
      
      BlurButton startBtn = (BlurButton)findViewById(R.id.start);
      startBtn.setOnClickListener(new OnClickListener() {
				@Override
        public void onClick(View view) {
        }
      });
      
      BlurButton optionsBtn = (BlurButton)findViewById(R.id.options);
      optionsBtn.setOnClickListener(new OnClickListener () {
      	@Override
      	public void onClick(View view) {
					Intent i = new Intent(getApplicationContext(), OptionsMenu.class);
					startActivityForResult(i, Common.REQUEST_OPTIONS);
      	}
      });
      
      Gallery lvlGallery = (Gallery)findViewById(R.id.lvlgallery);
      lvlGallery.setUnselectedAlpha(0.3f);
      String[] levels = new String[10];
      for (int i=0; i<10; i++) 
      	levels[i] = ""+i;
      lvlGallery.setAdapter(new GalleryTextAdapter(levels));
      
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