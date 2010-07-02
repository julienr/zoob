#ifndef ANDROIDINPUTMANAGER_H_
#define ANDROIDINPUTMANAGER_H_

#include "view/menu/MenuItem.h"
#include "InputManager.h"
#include "PlayerFormControl.h"
#include "lib/Timer.h"

#undef FORM_CONTROL

enum eInputMode {
    INPUT_TOUCH, //touch to move, double-tap to fire
    INPUT_TRACKBALL //gamePad to move, trackball to fire
};

class AndroidInputManager : public InputManager {
  public:
    AndroidInputManager ();
    void draw ();

    void touchEventDown (float x, float y);
    void touchEventMove (float x, float y);
    void touchEventUp (float x, float y);
    void touchEventOther (float x, float y);
    //RELATIVE (x,y)
    void trackballMove (float rx, float ry);
    void trackballClick (float rx, float ry);

    //reset all states
    void reset ();

    void think (double elapsedS);

    void setInputMode (eInputMode mode);

    eInputMode getInputMode () { return inputMode; }

    float getLeftXMargin() {
      switch (inputMode) {
        case INPUT_TOUCH:
          return 1.0f;
        case INPUT_TRACKBALL:
          return 3.2f;
      }
    }

  protected:
    enum eState {
      STATE_DEFAULT=0,
      MOVING_TANK,
      MOVING_TANK_PAD,
      FIRING_MODE //pressed on fire button and need to touch somewhere to fire the rocket
    };

    void startMoving (eState what, const Vector2& touchPosition);
    void setMoveTouchPoint (const Vector2& pos);
    void stopMoving ();

    void updatePressedItem (const Vector2& p, const Vector2& pNoTrans);

    void updateTankDir(const Vector2& touchPosition);

    void _setPressedItem (int item) {
      if (item != pressedItem)
        lastButtonPressTime = Utils::getCurrentTimeMillis();
      pressedItem = item;
    }

    eInputMode inputMode;
    eState state;

    Vector2 tankMoveEnd;

    MenuItem rocketButton;
    MenuItem bombButton;
    MenuItem shieldButton;
#ifdef FORM_CONTROL
    PlayerFormControl formControl;
#endif

    Sprite cursor;
    Vector2 cursorPosition;

    Sprite gamePad;

    uint64_t lastTouchDownTime;
    Vector2 lastTouchDownLocation;

    Timer shieldButtonTimer;
    Timer bombButtonTimer;

    int pressedItem;
    uint64_t lastButtonPressTime;
};

#endif /* ANDROIDINPUTMANAGER_H_ */
