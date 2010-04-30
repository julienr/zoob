#ifndef ANDROIDINPUTMANAGER_H_
#define ANDROIDINPUTMANAGER_H_

#include "InputManager.h"

/**
 * See important comment in Moob.java regarding native event-handling methods.
 * Basically, they shouldn't lead to any OpenGL call, because they'll be run in a separate
 * thread than the rendering thread.
 */


class AndroidInputManager : public InputManager {
  public:
    AndroidInputManager (GameManager* gameManager);
    void tick ();
    void draw ();

    void touchEventDown (float x, float y);
    void touchEventMove (float x, float y);
    void touchEventUp (float x, float y);
    void touchEventOther (float x, float y);
  protected:
    enum eState {
      STATE_DEFAULT=0,
      MOVING_TANK,
      MOVING_TANK_PAD, //moving tank using the gamepad
      FIRING_MODE //pressed on fire button and need to touch somewhere to fire the rocket
    };

    void startMoving (eState what, const Vector2& touchPosition);
    void setMoveTouchPoint (const Vector2& pos);
    void stopMoving ();

    void updateTankDir(const Vector2& touchPosition);

    eState state;

    Vector2 tankMoveEnd;

    bool showGamePad;
    const Sprite gamePad;
    const Sprite fireButton;
    const Sprite fireButtonClicked;

    uint64_t lastTouchDownTime;
    Vector2 lastTouchDownLocation;

};

#endif /* ANDROIDINPUTMANAGER_H_ */
