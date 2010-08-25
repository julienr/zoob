package net.fhtagn.zoobgame.menus;

import android.content.Context;

public abstract class InterLevelView extends FullscreenView {
	public static int GOTO_MENU = -1; //special return code for getNextLevel()
	
	private int currentLevel;
	
	public InterLevelView(Context context) {
    super(context);
	}
	
	public void setCurrentLevel (int level) {
		currentLevel = level;
	}
	
	protected int getCurrentLevel () {
		return currentLevel;
	}
	
	public abstract int getNextLevel ();
}
