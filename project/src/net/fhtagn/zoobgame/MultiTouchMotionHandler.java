package net.fhtagn.zoobgame;

import android.view.MotionEvent;

class MultiTouchMotionHandler implements MotionEventHandler {
	private static final String TAG = "SingleTouchEventProcessor";
	private static final int INVALID_POINTER_ID = -1;
	
	//We support at most 2 active pointers. The first one is always the one controlling the movements
	private int [] activePointersID = {INVALID_POINTER_ID, INVALID_POINTER_ID};
	
	private static int getActionIndex (MotionEvent e) {
		final int action = e.getAction();
		return (action & MotionEvent.ACTION_POINTER_ID_MASK) >> MotionEvent.ACTION_POINTER_ID_SHIFT;
	}
	
	public Command processEvent (MotionEvent e) {
		final int action = e.getAction();
		switch (action & MotionEvent.ACTION_MASK) {
			case MotionEvent.ACTION_DOWN:
				activePointersID[0] = e.getPointerId(0);
				//Log.v(TAG, "Primary pointer down, id : " + activePointersID[0]);
				return new Command(Command.Type.EVENT_DOWN, e.getX(0), e.getY(0));
			case MotionEvent.ACTION_POINTER_DOWN: {
				final int pointerIndex = getActionIndex(e);
				final int pointerID = e.getPointerId(pointerIndex);
				if (activePointersID[1] == INVALID_POINTER_ID) {
					activePointersID[1] = pointerID;
					//Log.v(TAG, "Secondary pointer down, id : " + pointerID + "("+e.getX(pointerIndex) + ","+e.getY(pointerIndex) +")");
					return new Command(Command.Type.EVENT_SECONDARY_DOWN, e.getX(pointerIndex), e.getY(pointerIndex));
				}
				break;
			}
			case MotionEvent.ACTION_MOVE:
				if (activePointersID[0] != INVALID_POINTER_ID) {
					final int primaryIndex = e.findPointerIndex(activePointersID[0]);
					//Log.v(TAG, "Move ("+activePointersID[0]+") ("+e.getX(primaryIndex) + ","+e.getY(primaryIndex)+")");
					return new Command(Command.Type.EVENT_MOVE, e.getX(0), e.getY(0));
				} 
				if (activePointersID[1] != INVALID_POINTER_ID) {
					final int secondaryIndex = e.findPointerIndex(activePointersID[1]);
					//Log.v(TAG, "Move ("+activePointersID[1]+") ("+e.getX(secondaryIndex) + ","+e.getY(secondaryIndex)+")");
					return new Command(Command.Type.EVENT_SECONDARY_MOVE, e.getX(secondaryIndex), e.getY(secondaryIndex));
				}
				break;
			case MotionEvent.ACTION_UP:
			case MotionEvent.ACTION_CANCEL:
			case MotionEvent.ACTION_POINTER_UP: {
				final int pointerIndex = getActionIndex(e);
				final int pointerID = e.getPointerId(pointerIndex);
				if (activePointersID[0] == pointerID) {
					//Log.v(TAG, "Primary pointer up, id : " + activePointersID[0]);
					activePointersID[0] = INVALID_POINTER_ID;
					return new Command(Command.Type.EVENT_UP, e.getX(0), e.getY(0));
				}
				if (activePointersID[1] == pointerID) {
					//Log.v(TAG, "Secondary pointer up, id : " + e.getPointerId(pointerIndex));
					activePointersID[1] = INVALID_POINTER_ID;
					return new Command(Command.Type.EVENT_SECONDARY_UP, e.getX(pointerIndex), e.getY(pointerIndex));
				}
				break;
			}
		}
		return null;
	}
}