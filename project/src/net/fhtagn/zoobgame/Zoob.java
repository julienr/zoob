package net.fhtagn.zoobgame;

import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.List;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import net.fhtagn.zoob_demo.R;
import net.fhtagn.zoobgame.menus.Common;
import net.fhtagn.zoobgame.menus.EndView;
import net.fhtagn.zoobgame.menus.ErrorView;
import net.fhtagn.zoobgame.menus.MultiplayerMenuView;
import net.fhtagn.zoobgame.menus.StartupAdView;
import net.fhtagn.zoobgame.menus.InterLevelView;
import net.fhtagn.zoobgame.menus.LostView;
import net.fhtagn.zoobgame.menus.MainMenuView;
import net.fhtagn.zoobgame.menus.RewardView;
import net.fhtagn.zoobgame.menus.WonView;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.ContentUris;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.content.res.Configuration;
import android.database.Cursor;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.net.Uri;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnTouchListener;
import android.view.animation.AlphaAnimation;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.view.animation.LinearInterpolator;
import android.webkit.WebView;
import android.widget.Toast;
import android.widget.ViewAnimator;
import android.widget.ViewFlipper;

public class Zoob extends Activity {
	static final String TAG = "Zoob";
	private static final String ASSET_EULA = "EULA";
	static final String ACTION_PLAY = "net.fhtagn.zoobgame.PLAY";
	private ZoobGLSurface mGLView;
	
	private ViewAnimator flipper;
	
	public static final int DIALOG_HELP = 1;
	public static final int DIALOG_ABOUT = 2;
	
	//Views
	//BEGIN: Java equivalent of app.h eMenu
	static final int MENU_MAIN = 0;
	static final int MENU_WON = 1;
	static final int MENU_LOST = 2;
	static final int MENU_END = 3;
	static final int MENU_ERROR = 4;
	//END: Java equivalent of app.h eMenu
	static final int MENU_REWARD_BOMB = 5;
	static final int MENU_REWARD_BOUNCE = 6;
	static final int MENU_REWARD_SHIELD = 7;
	static final int MENU_REWARD_FIRING = 8;
	static final int MENU_GET_FULL = 9;
	static final int MENU_PLAY = 10;
	static final int MENU_MULTI = 11;
	static final int MENU_LAST = 12;
	
	//Error
	//BEGIN:Java equivalent of app.h eError
	static final int ERROR_NETWORK_TIMEOUT = 0;
	static final int ERROR_LAST = 1;
	
	private MainMenuView mainMenu;
	private WonView wonView;
	private LostView lostView;
	private EndView endView;
	private ErrorView errorView;
	private RewardView[] rewardViews = new RewardView[4]; 
	private MultiplayerMenuView multiView;
	private StartupAdView startupAdView;
	
	static {
		System.loadLibrary("zoob");
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		parseIntent(getIntent());
		
		flipper = new ViewAnimator(this);
		setContentView(flipper);
		
		// Force landscape
		setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);

		// Create GL view
		final ZoobApplication app = (ZoobApplication)getApplication();
		app.registerZoob(this);
		mGLView = new ZoobGLSurface(this, app, app.getSerieJSONString());
		
		OnClickListener interViewListener = new OnClickListener() {
			@Override
			public void onClick(View view) {
				InterLevelView iv = (InterLevelView) view;
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
		
		errorView = new ErrorView(this);
		errorView.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick (View view) {
				flipper.setDisplayedChild(MENU_MAIN);
			}
		});
		flipper.addView(errorView, MENU_ERROR);
		
		rewardViews[0] = new RewardView(this, R.drawable.reward_bomb, true);
		rewardViews[1] = new RewardView(this, R.drawable.reward_bounce, false);
		rewardViews[2] = new RewardView(this, R.drawable.reward_shield, true);
		rewardViews[3] = new RewardView(this, R.drawable.reward_firing, false);
		for (int i=0; i<4; i++) {
			rewardViews[i].setOnClickListener(interViewListener);
			flipper.addView(rewardViews[i], MENU_REWARD_BOMB+i);
		}
		
		startupAdView = new StartupAdView(this);
		startupAdView.setOnTouchListener(new OnTouchListener() {
			@Override
			public boolean onTouch(View view, MotionEvent event) {
				if (event.getAction() == MotionEvent.ACTION_CANCEL
				    || event.getAction() == MotionEvent.ACTION_UP) {
					showView(MENU_MAIN);
					return true;
				}
				return false;
			}
		});
		flipper.addView(startupAdView, MENU_GET_FULL);
		
		flipper.addView(mGLView, MENU_PLAY);
		
		multiView = new MultiplayerMenuView(this);
		flipper.addView(multiView, MENU_MULTI);
		
		//Needed to avoid getting a black screen when switching to glview
		mGLView.setBackgroundColor(Color.parseColor("#FF656565"));
		
		if (app.isDemo())
			showView(MENU_GET_FULL);
		else
			showView(MENU_MAIN);
	}
	
	//This is only called when the application first transfer the progress from previous versions to this version
	public void refreshLevels () {
		if (flipper.getDisplayedChild() == MENU_MAIN) {
			runOnUiThread(new Runnable() {
				@Override
				public void run () {
					mainMenu.refreshLvlGallery();
				}
			});
		}
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
	
	//Start a new local game
	public void play (int level) {
		showView(MENU_PLAY);
		mGLView.setLevel(level);
	}
	
	//Start a game as server
	public void playServer (int level) {
		showView(MENU_PLAY);
		mGLView.startServer(level);
	}
	
	//Start a game as client
	public void playClient (String address) {
		showView(MENU_PLAY);
		mGLView.startClient(address);
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
	public boolean onCreateOptionsMenu (Menu menu) {
		//Create the menu here anyway
		MenuInflater inflater = getMenuInflater();
		inflater.inflate(R.menu.main_optionsmenu, menu);
		ZoobApplication app = (ZoobApplication)getApplication();
		menu.setGroupVisible(R.id.demo_only, app.isDemo());
		return true;
	}
	
	//Return true if the view flipper currently displays a menu view (such as the main menu)
	private boolean isInMenu () {
		int c = flipper.getDisplayedChild();
		return c == MENU_MAIN || c == MENU_MULTI;
	}
	
	@Override
	public boolean onPrepareOptionsMenu (Menu menu) {
		if (isInMenu()) {
			return true;
		} else {
			mGLView.onMenu();
			return false;
		}
	}
	
	@Override
	protected Dialog onCreateDialog (int dialogID) {
		switch (dialogID) {
			case DIALOG_HELP:
				return createHtmlDialog(R.string.help_title, R.layout.html_dialog, getResources().getString(R.string.help_content));
			case DIALOG_ABOUT:
				return createHtmlDialog(R.string.about_title, R.layout.html_dialog, Common.readFromAssets(this, ASSET_EULA));
		}
		return null;
	}
	
	private Dialog createHtmlDialog (int titleRes, int layoutRes, String html) {
		AlertDialog.Builder builder = new AlertDialog.Builder(this);
		builder.setTitle(titleRes);
		LayoutInflater inflater = (LayoutInflater)getSystemService(LAYOUT_INFLATER_SERVICE);
		View view = inflater.inflate(layoutRes, null);
		
		WebView webView = (WebView)view.findViewById(R.id.webview);
		webView.setBackgroundColor(Color.TRANSPARENT);
		webView.loadData(html, "text/html", "utf-8");
		
		builder.setView(view);
		builder.setPositiveButton(R.string.ok, new DialogInterface.OnClickListener() {
			@Override
      public void onClick(DialogInterface dialog, int which) {
				dialog.dismiss();
      }
		});
		return builder.create();
	}
	
	@Override
	public boolean onOptionsItemSelected (MenuItem item) {
		switch (item.getItemId()) {
			case R.id.help: {
				//Toast.makeText(this, "help !", Toast.LENGTH_SHORT).show();
				showDialog(DIALOG_HELP);
				break;
			}
			case R.id.editor: {
				if (Common.isEditorInstalled(this)) {
					Intent i = new Intent();
					i.setClassName("net.fhtagn.zoobeditor", "net.fhtagn.zoobeditor.browser.Browser");
					i.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
					startActivity(i); 
				} else {
					startActivity(Common.getEditorIntent());
				}
				//Toast.makeText(this, "editor !", Toast.LENGTH_SHORT).show();
				break;
			}
			case R.id.buy: {
				startActivity(Common.buyFullIntent());
				break;
			}
			case R.id.about: {
				showDialog(DIALOG_ABOUT);
				break;
			}
			case R.id.multi: {
				showView(MENU_MULTI);
				break;
			}
		}
		return true;
	}
	
	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
    if (keyCode == KeyEvent.KEYCODE_BACK) {
    	//back pressed
    	if(!isInMenu()) { //if we are in game, go to the menu
    		mGLView.onMenu();
    		return true;
    	} else { //if we are in menu, depends on the currently displayed menu
    		int c = flipper.getDisplayedChild();
      	if (c == MENU_MAIN) //if on main menu
      		return super.onKeyDown(keyCode, event);
      	else { //if on another menu (multiplayer), go back to main
      		showView(MENU_MAIN);
      		return true;
      	}
    	}
    } else {
    	return super.onKeyDown(keyCode, event);
    }
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
	
	public void showError (final int err) {
		runOnUiThread(new Runnable() {
			public void run () {
				switch (err) {
					case ERROR_NETWORK_TIMEOUT:
						Toast.makeText(Zoob.this, R.string.error_network_timeout, Toast.LENGTH_LONG);
						showView(MENU_MAIN);
						break;
					default:
						Log.e(TAG, "showError("+err+"), err > ERROR_LAST");
						break;
				}
			}
		});
	}
}

