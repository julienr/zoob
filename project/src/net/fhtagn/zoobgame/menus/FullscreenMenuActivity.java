package net.fhtagn.zoobgame.menus;

import net.fhtagn.zoobgame.R;
import android.os.Bundle;
import android.view.ViewGroup.LayoutParams;
import android.widget.LinearLayout;

public abstract class FullscreenMenuActivity extends MenuActivity {

	public void onCreate (Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.menulayout);
		
		LinearLayout layout = (LinearLayout)findViewById(R.id.container);
    layout.addView(createView(), new LayoutParams(LayoutParams.FILL_PARENT, LayoutParams.FILL_PARENT));
	}
	
	protected abstract FullscreenView createView (); 
}
