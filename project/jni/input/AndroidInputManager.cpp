#include "AndroidInputManager.h"
#include "app.h"
#include "view/NumberView.h"


//X position for buttons when they are rendered on the left or right of the game area
const float buttonsLeftX = 1.25f;
const float buttonsRightX = 13.8f;

#define ROCKET_BUTTON_ID 0 //just used for button creation
#define BOMB_BUTTON_ID 1 //just used for button creation
#define SHIELD_BUTTON_ID 2

#define SHIELD_BUTTON_COOLDOWN 20
#define MINE_BUTTON_COOLDOWN 20

#define DOUBLETAP_TIME 600
#define DOUBLETAP_DIST 5

//shortcut
ProgressionManager* _progMan () { return ProgressionManager::getInstance(); }

Vector2 gamePadPos(buttonsLeftX, 5.0f);
Vector2 gamePadSize(2.5f,2.5f);

AndroidInputManager::AndroidInputManager ()
  : InputManager(),
    inputMode(INPUT_TOUCH),
    state(STATE_DEFAULT),
    rocketButton("assets/sprites/fire_rocket.png",
                 "assets/sprites/fire_rocket_clicked.png",
                 ROCKET_BUTTON_ID,
                 TEX_GROUP_GAME),
    bombButton("assets/sprites/fire_bomb.png",
               "assets/sprites/fire_bomb_clicked.png",
               BOMB_BUTTON_ID,
               TEX_GROUP_GAME),
    shieldButton("assets/sprites/shield_button.png",
                 "assets/sprites/shield_button_clicked.png",
                 SHIELD_BUTTON_ID,
                 TEX_GROUP_GAME),
    cursor("assets/sprites/cursor.png",
             TEX_GROUP_GAME),
    cursorPosition(7.5, 5),
    gamePad("assets/sprites/control.png",
             TEX_GROUP_GAME),
    lastTouchDownTime(0),
    shieldButtonTimer(SHIELD_BUTTON_COOLDOWN),
    bombButtonTimer(MINE_BUTTON_COOLDOWN),
    pressedItem(-1) {
  setInputMode(INPUT_TOUCH);
}

void AndroidInputManager::reset () {
#ifdef FORM_CONTROL
  formControl.reset();
#endif
  rocketButton.setPressed(false);
  bombButton.setPressed(false);
  shieldButton.setPressed(false);
  shieldButtonTimer.stop();
  bombButtonTimer.stop();
  cursorPosition.set(7.5, 5);
  state = STATE_DEFAULT;
}

void AndroidInputManager::setInputMode (eInputMode mode) {
  inputMode = mode;

  if (inputMode == INPUT_TOUCH)
    rocketButton.setEnabled(true);
  else
    rocketButton.setEnabled(false);

  //BUTTONS position
  const float buttonsX = (inputMode==INPUT_TOUCH)?buttonsRightX:buttonsLeftX;
  const Vector2 buttonSize (2.5,2.5);
  rocketButton.setPosition(Vector2(buttonsX, 5.25f));
  rocketButton.setSize(buttonSize);
  rocketButton.setBB(rocketButton.getPosition(), buttonSize);
  bombButton.setPosition(Vector2(buttonsX, 2.5f));
  bombButton.setSize(buttonSize);
  bombButton.setBB(bombButton.getPosition(), buttonSize);
  shieldButton.setPosition(Vector2(buttonsX, 8.0f));
  shieldButton.setSize(buttonSize);
  shieldButton.setBB(shieldButton.getPosition(), buttonSize);
}


void AndroidInputManager::draw () {
  const uint64_t now = Utils::getCurrentTimeMillis();

  //BUTTONS
  if (inputMode == INPUT_TOUCH)
    rocketButton.draw();
  if (_progMan()->hasBombs()) {
    if (bombButtonTimer.isActive()) {
      bombButton.drawPressed();
      NumberView::getInstance()->drawInt((int)ceil(bombButtonTimer.getTimeLeft()), bombButton.getPosition(), bombButton.getSize());
    } else if (pressedItem == BOMB_BUTTON_ID) {
      GLW::color(WHITE, 1-(now-lastButtonPressTime)/(float)DOUBLETAP_TIME);
      bombButton.drawPressed();
      GLW::colorWhite();
    } else
      bombButton.draw();
  }
  if (_progMan()->hasShield()) {
    if (shieldButtonTimer.isActive()) {
      shieldButton.drawPressed();
      NumberView::getInstance()->drawInt((int)ceil(shieldButtonTimer.getTimeLeft()), shieldButton.getPosition(), shieldButton.getSize());
    } else if (pressedItem == SHIELD_BUTTON_ID) {
      GLW::color(WHITE, 1-(now-lastButtonPressTime)/(float)DOUBLETAP_TIME);
      shieldButton.drawPressed();
      GLW::colorWhite();
    } else
      shieldButton.draw();
  }

  //VISOR
  if (inputMode == INPUT_TRACKBALL) {
    cursor.draw(cursorPosition, Vector2(1,1));
    gamePad.draw(gamePadPos, gamePadSize);
  }

#ifdef FORM_CONTROL
  if (_progMan()->hasForms())
    formControl.draw();
#endif
}

bool inGamePad (Vector2 p) {
  const float hW = gamePadSize.x/2.0f;
  const float hH = gamePadSize.y/2.0f;

  return ((p.x >= gamePadPos.x-hW) && (p.x <= gamePadPos.x + hW)) &&
          ((p.y >= gamePadPos.y-hW) && (p.y <= gamePadPos.y + hH));
}

void AndroidInputManager::updateTankDir(const Vector2& touchPosition) {
  switch(state) {
    case MOVING_TANK:
      Game::getInstance()->setTankMoveDir(touchPosition-Game::getInstance()->getPlayerTank()->getPosition());
      break;
    case MOVING_TANK_PAD: {
      //When the user touch in the middle of the game pad, we shouldn't move
      const Vector2 dir = touchPosition+Vector2(transX, transY) - gamePadPos;
      if (dir.length() < 0.3f)
        Game::getInstance()->setTankMoveDir(Vector2::ZERO);
      else
        Game::getInstance()->setTankMoveDir(dir);
      break;
    }
    default:
      Game::getInstance()->setTankMoveDir(Vector2::ZERO);
      break;
  }
}

void AndroidInputManager::startMoving (eState what, const Vector2& touchPosition) {
  state = what;
  updateTankDir(touchPosition);
}

void AndroidInputManager::setMoveTouchPoint (const Vector2& pos) {
  updateTankDir(pos);
}

void AndroidInputManager::stopMoving () {
  Game* game = Game::getInstance();
  if (!game) {
    LOGE("stopMoving with game = NULL");
    return;
  }
  game->setTankMoveDir(Vector2::ZERO);
  state = STATE_DEFAULT;
}

void AndroidInputManager::think (double elapsedS) {
  const uint64_t now = Utils::getCurrentTimeMillis();
  const uint64_t elapsed = now - lastButtonPressTime;

  /*if (pressedItem != -1)
    LOGE("elapsed : %llu, pressedItem : %i, pressedItemUp: %i", elapsed, pressedItem, pressedItemUp);*/

  if ((elapsed > DOUBLETAP_TIME) && pressedItem != -1) {
    if (pressedItem == BOMB_BUTTON_ID) {
      Game::getInstance()->playerDropBomb();
      bombButtonTimer.start();
    } else if (pressedItem == SHIELD_BUTTON_ID) {
      Game::getInstance()->playerActivateShield();
      shieldButtonTimer.start();
    }
    bombButton.setPressed(false);
    shieldButton.setPressed(false);
    _setPressedItem(-1);
  }
}

void AndroidInputManager::updatePressedItem (const Vector2& p, const Vector2& pNoTrans) {
  if (bombButton.inside(pNoTrans) && !bombButtonTimer.isActive() && _progMan()->hasBombs()) {
    _setPressedItem(BOMB_BUTTON_ID);
    //LOGE("bomb");
  } else if (shieldButton.inside(pNoTrans) && !shieldButtonTimer.isActive() && _progMan()->hasShield()) {
    _setPressedItem(SHIELD_BUTTON_ID);
    //LOGE("shield");
  } else if (inputMode == INPUT_TRACKBALL && inGamePad(pNoTrans)) {
    _setPressedItem(-1);
    startMoving(MOVING_TANK_PAD, p);
  } else {
    _setPressedItem(-1);
    if (inputMode == INPUT_TOUCH)
      startMoving(MOVING_TANK, p);
  }
}

void AndroidInputManager::touchEventDown (float x, float y) {
  if (!GameManager::getInstance()->inGame() || GameManager::getInstance()->inTransition()) {
    //LOGE("touchEventDown(menu) (%f,%f) => (%f,%f)", x, y, XSG_NOTRANSX(x), YSG_NOTRANSY(y));
    GameManager::getInstance()->handleTouchDown(Vector2(XSG_NOTRANSX(x), YSG_NOTRANSY(y)));
    return;
  }

  const Vector2 p(XSG(x), YSG(y));
  const Vector2 pNoTrans (XSG_NOTRANSX(x), YSG_NOTRANSY(y));
  //Check that the double-tap was quick and not too far away (which would indicate a fast direction
  //change)
  const uint64_t now = Utils::getCurrentTimeMillis();
  const float tapDist = (p-lastTouchDownLocation).length();
  const uint64_t elapsed = now - lastTouchDownTime;

  //For buttons, we only consider events that couldn't be double tap
  if (state == FIRING_MODE || rocketButton.inside(pNoTrans)) {
    rocketButton.setPressed(true);
  } else if (inputMode == INPUT_TOUCH && (tapDist < DOUBLETAP_DIST) && (elapsed < DOUBLETAP_TIME)) {
    Game::getInstance()->playerFire(p);
    //startMoving(MOVING_TANK, p);
    //_setPressedItem(-1);
    updatePressedItem(p, pNoTrans);
  } else {
    updatePressedItem(p, pNoTrans);
  }

  //LOGE("time between taps : %li, dist between tap : %f", (long)(now-lastTouchDownTime), tapDist);
  /*if (state == FIRING_MODE || rocketButton.inside(pNoTrans)) {
    rocketButton.setPressed(true);
  } else if (bombButton.inside(pNoTrans)) {
    if (!bombButtonTimer.isActive() && _progMan()->hasBombs())
      bombButton.setPressed(true);
  }
#ifdef FORM_CONTROL
  else if (formControl.inside(pNoTrans)) {
    if (_progMan()->hasForms())
      formControl.handleTouchDown(pNoTrans);
  }
#endif
  else if (shieldButton.inside(pNoTrans)) {
    if (!shieldButtonTimer.isActive() && _progMan()->hasShield())
      shieldButton.setPressed(true);
  } else if (tapDist < 5 && elapsed <= 600) {
    getGame()->playerFire(p);
    startMoving(MOVING_TANK, p);
  } else {
    startMoving(MOVING_TANK, p);
  }*/
  lastTouchDownTime = now;
  lastTouchDownLocation = p;
  return;
}

void AndroidInputManager::touchEventMove (float x, float y) {
  if (!GameManager::getInstance()->inGame())
    return;

  if (state == FIRING_MODE)
    return;

  const Vector2 p(XSG(x), YSG(y));
  const Vector2 pNoTrans (XSG_NOTRANSX(x), YSG_NOTRANSY(y));
  //updatePressedItem(p, pNoTrans);

  setMoveTouchPoint(p);
}

void AndroidInputManager::touchEventUp (float x, float y) {
  if (GameManager::getInstance()->inGame()) {
    const Vector2 pNoTrans (XSG_NOTRANSX(x), YSG_NOTRANSY(y));
    if (state == FIRING_MODE) {
      Game::getInstance()->playerFire(Vector2(XSG(x),YSG(y)));
      rocketButton.setPressed(false);
      state = STATE_DEFAULT;
    } else {
      if (rocketButton.isPressed() && rocketButton.inside(pNoTrans)) {
        state = FIRING_MODE;
        LOGE("switching to firing mode");
        Game::getInstance()->setTankMoveDir(Vector2::ZERO);
      } else {
        rocketButton.setPressed(false);
        if ((pressedItem == BOMB_BUTTON_ID && bombButton.inside(pNoTrans)) ||
            (pressedItem == SHIELD_BUTTON_ID && shieldButton.inside(pNoTrans))) {
          const uint64_t now = Utils::getCurrentTimeMillis();
          stopMoving();
          if ((now-lastButtonPressTime) < DOUBLETAP_TIME)
            _setPressedItem(-1);
        } else {
          stopMoving();
          _setPressedItem(-1);
        }
      }
    }
    /*if (state == FIRING_MODE) {
      getGame()->playerFire(Vector2(XSG(x),YSG(y)));
      rocketButton.setPressed(false);
      state = STATE_DEFAULT;
    } else {
      if (rocketButton.inside(pNoTrans)) {
        state = FIRING_MODE;
        getGame()->setTankMoveDir(Vector2::ZERO);
      } else {
        rocketButton.setPressed(false);
        if (_progMan()->hasBombs() && bombButton.isPressed() &&  bombButton.inside(pNoTrans)) {
          getGame()->playerDropBomb();
          bombButtonTimer.start();
        } else if (_progMan()->hasShield() && shieldButton.isPressed() && shieldButton.inside(pNoTrans)) {
          getGame()->playerActivateShield();
          shieldButtonTimer.start();
        }
#ifdef FORM_CONTROL
        else if (_progMan()->hasForms() && formControl.inside(pNoTrans))
          formControl.handleTouchUp(pNoTrans);
#endif
        else
           stopMoving();
        bombButton.setPressed(false);
        shieldButton.setPressed(false);
      }
    }*/
  } else
    GameManager::getInstance()->handleTouchUp(Vector2(XSG_NOTRANSX(x), YSG_NOTRANSY(y)));
}

void AndroidInputManager::touchEventOther (float x, float y) {
  touchEventUp(x,y);
}

void AndroidInputManager::trackballMove (float rx, float ry) {
  if (GameManager::getInstance()->inGame()) {
    cursorPosition += Vector2(rx,ry);
  }
}

void AndroidInputManager::trackballClick (float rx, float ry) {
  if (GameManager::getInstance()->inGame()) {
    cursorPosition += Vector2(rx,ry);
    Game::getInstance()->playerFire(cursorPosition-Vector2(transX, transY));
  }
}
