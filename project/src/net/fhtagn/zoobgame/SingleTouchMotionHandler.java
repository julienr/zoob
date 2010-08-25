package net.fhtagn.zoobgame;

import android.view.MotionEvent;

class SingleTouchMotionHandler implements MotionEventHandler {
	public Command processEvent (MotionEvent e) {
		Command c = null;
		final float x = e.getX();
		final float y = e.getY();
		switch (e.getAction()) {
			case MotionEvent.ACTION_DOWN:
				c = new Command(Command.Type.EVENT_DOWN, x, y);
				break;
			case MotionEvent.ACTION_MOVE:
				c = new Command(Command.Type.EVENT_MOVE, x, y);
				break;
			case MotionEvent.ACTION_UP:
				c = new Command(Command.Type.EVENT_UP, x, y);
				break;
			default:
				c = new Command(Command.Type.EVENT_OTHER, x, y);
				break;
		}
		return c;
	}
}
