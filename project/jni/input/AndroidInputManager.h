#ifndef ANDROIDINPUTMANAGER_H_
#define ANDROIDINPUTMANAGER_H_

#include "view/menu/MenuItem.h"
#include "InputManager.h"
#include "PlayerFormControl.h"

/**
 * See important comment in Moob.java regarding native event-handling methods.
 * Basically, they shouldn't lead to any OpenGL call, because they'll be run in a separate
 * thread than the rendering thread.
 */


class AndroidInputManager : public InputManager {
  public:
    AndroidInputManager ();
    void tick ();
    void draw ();

    void touchEventDown (float x, float y);
    void touchEventMove (float x, float y);
    void touchEventUp (float x, float y);
    void touchEventOther (float x, float y);

    //reset all states
    void reset ();
  protected:
    enum eState {
      STATE_DEFAULT=0,
      MOVING_TANK,
      FIRING_MODE //pressed on fire button and need to touch somewhere to fire the rocket
    };

    void startMoving (eState what, const Vector2& touchPosition);
    void setMoveTouchPoint (const Vector2& pos);
    void stopMoving ();

    void updateTankDir(const Vector2& touchPosition);

    eState state;

    Vector2 tankMoveEnd;

    MenuItem rocketButton;
    MenuItem bombButton;

    PlayerFormControl formControl;

    uint64_t lastTouchDownTime;
    Vector2 lastTouchDownLocation;

};

#endif /* ANDROIDINPUTMANAGER_H_ */
