#ifndef ANDROIDINPUTMANAGER_H_
#define ANDROIDINPUTMANAGER_H_

#include "view/menu/Button.h"
#include "InputManager.h"
#include "PlayerFormControl.h"
#include "lib/Timer.h"
#include "logic/PlayerCommand.h"

#undef FORM_CONTROL

class AndroidInputManager : public InputManager {
  public:
    AndroidInputManager (bool gamepad, bool trackball);
    void draw ();

    //Multitouch, callback for secondary pointer
    void touchEventSecondaryDown (float x, float y);
    void touchEventSecondaryUp (float x, float y);
    void touchEventSecondaryMove (float x, float y);


    void touchEventDown (float x, float y);
    void touchEventMove (float x, float y);
    void touchEventUp (float x, float y);
    void touchEventOther (float x, float y);
    //RELATIVE (x,y)
    void trackballMove (float rx, float ry);
    void trackballClick (float rx, float ry);

    //reset all states
    void reset ();

    void think (double elapsedS, PlayerCommand& command);

    void setUseGamepad (bool use);

    void setUseTrackball (bool use);

    bool getUseGamepad () { return useGamepad; }

    bool getUseTrackball () { return useTrackball; }

    float getLeftXMargin() {
      if (useGamepad)
        return 3.2f;
      else
        return 1.0f;
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

    eState state;

    Vector2 tankMoveEnd;

    Button rocketButton;
    Button bombButton;
    Button shieldButton;
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

    bool useGamepad;
    bool useTrackball;

    //We use a local playerCommand to register player's actions
    PlayerCommand playerCommand;
};

#endif /* ANDROIDINPUTMANAGER_H_ */
