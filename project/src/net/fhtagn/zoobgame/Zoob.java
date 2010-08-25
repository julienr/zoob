package net.fhtagn.zoobgame;

import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.List;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import net.fhtagn.zoobgame.menus.EndView;
import net.fhtagn.zoobgame.menus.HelpView;
import net.fhtagn.zoobgame.menus.InterLevelView;
import net.fhtagn.zoobgame.menus.LostView;
import net.fhtagn.zoobgame.menus.MainMenu;
import net.fhtagn.zoobgame.menus.MainMenuView;
import net.fhtagn.zoobgame.menus.WonView;

import android.app.Activity;
import android.content.ContentUris;
import android.content.Context;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.content.res.Configuration;
import android.database.Cursor;
import android.graphics.Color;
import android.net.Uri;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.animation.AlphaAnimation;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.view.animation.LinearInterpolator;
import android.widget.ViewAnimator;
import android.widget.ViewFlipper;

public class Zoob extends Activity {
	static final String TAG = "Zoob";
	static final String ACTION_PLAY = "net.fhtagn.zoobgame.PLAY";
	private ZoobGLSurface mGLView;
	
	private ViewAnimator flipper;
	
	//Views
	static final int MENU_MAIN = 0;
	static final int MENU_WON = 1;
	static final int MENU_LOST = 2;
	static final int MENU_END = 3;
	static final int MENU_HELP = 4;
	static final int MENU_PLAY = 5;
	static final int MENU_LAST = 6;
	
	private MainMenuView mainMenu;
	private WonView wonView;
	private LostView lostView;
	private EndView endView;
	private HelpView helpView;
	static {
		System.loadLibrary("zoob");
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		parseIntent(getIntent());
		
		flipper = new ViewAnimator(this);
		flipper.setBackgroundColor(Color.parseColor("#656565"));
		Animation fadeIn = AnimationUtils.loadAnimation(this,android.R.anim.slide_in_left);
		//fadeIn.setDuration(1000);
		Animation fadeOut = AnimationUtils.loadAnimation(this,android.R.anim.slide_out_right);
		//fadeOut.setDuration(1000);
		/*flipper.setInAnimation(fadeIn);
		flipper.setOutAnimation(fadeOut);*/
		setContentView(flipper);
		
    //Force landscape
    setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);   

    //Create GL view
		ZoobApplication app = (ZoobApplication)getApplication();
		mGLView = new ZoobGLSurface(this, app, app.getSerieJSONString());
		
		OnClickListener interViewListener = new OnClickListener() {
			@Override
      public void onClick(View view) {
				InterLevelView iv = (InterLevelView)view;
				mGLView.setLevel(iv.getNextLevel());
				flipper.setDisplayedChild(MENU_PLAY);
      }
		};
		
		mainMenu = new MainMenuView(this);
		flipper.addView(mainMenu, MENU_MAIN);
		
		wonView = new WonView(this);
		wonView.setOnClickListener(interViewListener);
		flipper.addView(wonView, MENU_WON);
		
		lostView = new LostView(this);
		lostView.setOnClickListener(interViewListener);
		flipper.addView(lostView, MENU_LOST);
		
		endView = new EndView(this);
		endView.setOnClickListener(interViewListener);
		flipper.addView(endView, MENU_END);
		
		helpView = new HelpView(this);
		helpView.setOnClickListener(interViewListener);
		flipper.addView(helpView, MENU_HELP);
		
		flipper.addView(mGLView, MENU_PLAY);
		
		showView(MENU_MAIN);
	}
	
	private void showView (int id) {
		flipper.setDisplayedChild(id);
		flipper.requestLayout();
	}
	
	public void parseIntent (Intent intent) {
		ZoobApplication app = (ZoobApplication)getApplication();
		app.setProgressPersistent(true);
		/** Intent resolution **/
		int serieID;
		if (intent != null && intent.getAction() != null && intent.getAction().equals(ACTION_PLAY)) { //Play a serie, specified in the intent
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
		} else if (app.getSerieJSON() == null) {
			//We only load original serie if we haven't yet loaded any serie
			//because a "back" intent (sent from Zoob) won't have any serie information and we should just keep the same serie
			Log.i(TAG, "No PLAY intent received, launching original serie");
			serieID = 1;
			app.setSerieId(serieID);
		}
	}
	
	public void play (int level) {
		flipper.setDisplayedChild(MENU_PLAY);
		mGLView.setLevel(level);
	}
	
	@Override
	protected void onPause() {
		super.onPause();
		mGLView.onPause();
	}

	@Override
	protected void onResume() {
		super.onResume();
		mGLView.onResume();
	}
	
	@Override
	public boolean onPrepareOptionsMenu (Menu menu) {
		mGLView.onMenu();
		return false;
	}
	
	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
    if (keyCode == KeyEvent.KEYCODE_BACK) {
    	mGLView.onMenu();
      return true;
    }
    return super.onKeyDown(keyCode, event);
	}
	
	@Override
	public void onConfigurationChanged(Configuration newConfig) {
		super.onConfigurationChanged(newConfig);
	}
	
	public void showMenu (final int id, final int currentLevel) {
		if (id < 0 || id >= MENU_LAST) {
			Log.e(TAG, "showMenu("+id+"), id > MENU_LAST("+MENU_LAST+")");
			return;
		}
		
		runOnUiThread(new Runnable() {
			public void run () {
				View child = flipper.getChildAt(id);
				if (child instanceof InterLevelView) {
					InterLevelView iv = (InterLevelView)child;
					iv.setCurrentLevel(currentLevel);
				}
				showView(id);
			}
		});
	}
}

