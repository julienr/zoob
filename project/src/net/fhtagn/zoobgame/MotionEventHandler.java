package net.fhtagn.zoobgame;

import android.view.MotionEvent;

interface MotionEventHandler {
	public Command processEvent (MotionEvent e);
}
