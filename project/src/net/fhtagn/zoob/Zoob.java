package net.fhtagn.zoob;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.app.Activity;
import android.content.Context;
import android.content.SharedPreferences;
import android.content.pm.ActivityInfo;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.content.res.AssetFileDescriptor;
import android.content.res.AssetManager;
import android.content.res.Configuration;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MotionEvent;

public class Zoob extends Activity {
	private ZoobGLSurface mGLView;

	static {
		System.loadLibrary("zoob");
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		mGLView = new ZoobGLSurface(this, (ZoobApplication)getApplication());
		setContentView(mGLView);
		
    //Force landscape
    setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);    
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

}

class ZoobGLSurface extends GLSurfaceView {
	ZoobRenderer mRenderer;
	/**
	 * These native methods MUST NOT CALL OPENGL. OpenGL/mRenderer is run in a separate
	 * thread and calling opengl from these native methods will result in errors
	 * like "call to OpenGL ES API with no current context" 
	 */
	private static native void nativePause();
	private static native void nativeMenu();
	private static native void touchEventDown (float x, float y);
	private static native void touchEventMove (float x, float y);
	private static native void touchEventUp (float x, float y);
	private static native void touchEventOther (float x, float y);
	
	public ZoobGLSurface(Context context, ZoobApplication app) {
		super(context);
		mRenderer = new ZoobRenderer(context, app);
		setRenderer(mRenderer);
	}
	
	public void onMenu () {
		nativeMenu();
	}
	
	public void onPause () {
		nativePause();
	}

	public boolean onTouchEvent(final MotionEvent event) {
		final float x = event.getX();
		final float y = event.getY();

		switch (event.getAction()) {
			case MotionEvent.ACTION_DOWN:
				touchEventDown(x,y); 
				break;
			case MotionEvent.ACTION_MOVE:
				touchEventMove(x,y);
				break;
			case MotionEvent.ACTION_UP:
				touchEventUp(x,y);
				break;
			default:
				touchEventOther(x,y);
				break;
		}
		//This is an advice from "Writing real time games for android" Google I/O presentation
		//This avoid event flood when the screen is touched
		try {
	    Thread.sleep(16);
    } catch (InterruptedException e) {}
		return true;
	}
}

class ZoobRenderer implements GLSurfaceView.Renderer {
	private Context context;
	private ZoobApplication app;
	private String apkFilePath;

	//This constructor is not ran in the rendering thread (but in the surface thread)
	public ZoobRenderer (Context context, ZoobApplication app) {
		Log.e("ZoobRenderer", "Creating a new ZoobRenderer");
		this.context = context;
		// return apk file path (or null on error)
		ApplicationInfo appInfo = null;
		PackageManager packMgmr = context.getPackageManager();
		try {
	    appInfo = packMgmr.getApplicationInfo("net.fhtagn.zoob", 0);
    } catch (NameNotFoundException e) {
	    e.printStackTrace();
	    throw new RuntimeException("Unable to locate assets, aborting...");
    }
		apkFilePath = appInfo.sourceDir;
		this.app = app;
	}
	
	public void onSurfaceCreated(GL10 gl, EGLConfig config) {
		//Call ALL nativeInit methods here, because we want the JNIEnv of the rendering thread
		//(see comments in app-android.cpp)
		Log.i("ZoobRenderer", "Calling nativeInit");
		nativeInit(apkFilePath, this);
    nativeInitGL(app.getLevel(), app.getDifficulty());
	}

	public void onSurfaceChanged(GL10 gl, int w, int h) {
		// gl.glViewport(0, 0, w, h);
		nativeResize(w, h);
	}

	public void onDrawFrame(GL10 gl) {
		nativeRender();
	}

  private static native void nativeInitGL(int level, int difficulty);
	private static native void nativeInit(String apkPath, ZoobRenderer app);
	private static native void nativeResize(int w, int h);
	private static native void nativeRender();
	
	//These are stubs for upcall from JNI because the Application object isn't in the 
	//same thread as the JNI stuff (nativeRender) and this can lead to random crashes
	public void saveProgress (int level) {
		app.saveProgress(level);
	}
		
	public void saveDifficulty (int level) {
		app.saveDifficulty(level);
	}
}
