package net.fhtagn.zoobgame.menus;

import net.fhtagn.zoob_demo.R;
import android.content.Context;
import android.graphics.Shader.TileMode;
import android.graphics.drawable.BitmapDrawable;
import android.widget.FrameLayout;
import android.widget.LinearLayout;

/**
 * Base class for menu view with a custom grey background.
 */
public class MenuView extends FrameLayout {
	//containerID is the top-level container for all menu elements. The background will be set
	//on it.
	private final int containerID;
	public MenuView (Context ctx, int containerID) {
		super(ctx);
		this.containerID = containerID;
	}
	
	//Set the background on the container
	protected void setBackground () {
	 	 //FIXME: for a strange reason, this doesn't work from the XML, so do it programmatically  
	   LinearLayout container = (LinearLayout)findViewById(containerID);
	   BitmapDrawable bg = (BitmapDrawable)getResources().getDrawable(R.drawable.backrepeat);
	   bg.setTileModeX(TileMode.REPEAT);
	   bg.setTileModeY(TileMode.REPEAT);
	   container.setBackgroundDrawable(bg);
	}

}
