package net.fhtagn.zoobgame.menus;

import android.app.Activity;
import android.content.pm.ActivityInfo;
import android.os.Bundle;
import android.view.Window;
import android.view.ViewGroup.LayoutParams;
import android.widget.LinearLayout;

public class MenuActivity extends Activity {
	 @Override
	  public void onCreate(Bundle savedInstanceState) {
	      super.onCreate(savedInstanceState);
	      
	      requestWindowFeature(Window.FEATURE_NO_TITLE);
	      setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
	      
	      setContentView(R.layout.menulayout);
	 }
	 
	 protected void setFullscreenView (FullscreenView view) {
		 LinearLayout layout = (LinearLayout)findViewById(R.id.container);
		 layout.addView(view, new LayoutParams(LayoutParams.FILL_PARENT, LayoutParams.FILL_PARENT));
	 }
}
