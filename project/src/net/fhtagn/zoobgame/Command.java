package net.fhtagn.zoobgame;

/** Since the event listening thread doesn't run in the rendering thread,
 * we are buffering events in the rendering thread and they are processed (by
 * calling their JNI callbacks) just before rendering 
 * 
 * This class is used to store an event */
class Command {
	public enum Type {
		EVENT_DOWN, EVENT_MOVE, EVENT_UP, EVENT_OTHER,
		EVENT_PAUSE, EVENT_MENU, EVENT_TRACKBALL, EVENT_TRACKBALL_CLICK,
		EVENT_SECONDARY_DOWN, EVENT_SECONDARY_MOVE, EVENT_SECONDARY_UP
	}
	
	public final float x;
	public final float y;
	
	public final Type type;
	
	public Command (Type type) {
		assert(type == Type.EVENT_PAUSE || type == Type.EVENT_MENU);
		this.type = type;
		this.x = this.y = 0;
	}
	
	public Command (Type type, float x, float y) {
		this.x = x;
		this.y = y;
		this.type = type;
	}
}
