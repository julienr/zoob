package net.fhtagn.zoobgame.menus;

import net.fhtagn.zoob_demo.R;
import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.ViewGroup.LayoutParams;
import android.widget.LinearLayout;

public abstract class FullscreenMenuActivity extends MenuActivity {
	private int currentLevel = -1;
	
	public void onCreate (Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.menulayout);
		
		Intent i = getIntent();
		if (i != null)
			currentLevel = i.getIntExtra("current_level", 0);
		
		LinearLayout layout = (LinearLayout)findViewById(R.id.container);
    layout.addView(createView(), new LayoutParams(LayoutParams.FILL_PARENT, LayoutParams.FILL_PARENT));
	}
	
	protected int getCurrentLevel () {
		return currentLevel;
	}
	
	protected abstract FullscreenView createView ();
	
	protected Intent prepareResult () {
		return null;
	}
	
	@Override
	public boolean onTouchEvent (MotionEvent event) {
		if (event.getAction() == MotionEvent.ACTION_UP || 
				event.getAction() == MotionEvent.ACTION_CANCEL) {
			Intent result = prepareResult();
			setResult(Activity.RESULT_OK, result);
			finish();
		}
		return true;
	}
}
