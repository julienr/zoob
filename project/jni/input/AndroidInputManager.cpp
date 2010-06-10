#include "AndroidInputManager.h"
#include "app.h"
#include "view/NumberView.h"


const Vector2 rocketButtonPos(13.8f, 3.0f);
const Vector2 rocketButtonSize(2.5,2.5);
const Vector2 mineButtonPos(rocketButtonPos.x, 5.5f);
const Vector2 mineButtonSize(rocketButtonSize);
const Vector2 shieldButtonPos(rocketButtonPos.x, 8.0f);
const Vector2 shieldButtonSize(rocketButtonSize);

#define ROCKET_BUTTON_ID 0 //just used for button creation
#define MINE_BUTTON_ID 1 //just used for button creation
#define SHIELD_BUTTON_ID 2

#define SHIELD_BUTTON_COOLDOWN 20

//shortcut
ProgressionManager* _progMan () { return ProgressionManager::getInstance(); }

AndroidInputManager::AndroidInputManager ()
  : InputManager(),
    state(STATE_DEFAULT),
    rocketButton("assets/sprites/fire_rocket.png",
                 "assets/sprites/fire_rocket_clicked.png",
                 ROCKET_BUTTON_ID,
                 TEX_GROUP_GAME),
    bombButton("assets/sprites/fire_bomb.png",
               "assets/sprites/fire_bomb_clicked.png",
               MINE_BUTTON_ID,
               TEX_GROUP_GAME),
    shieldButton("assets/sprites/shield_button.png",
                 "assets/sprites/shield_button_clicked.png",
                 SHIELD_BUTTON_ID,
                 TEX_GROUP_GAME),
    lastTouchDownTime(0),
    shieldButtonTimer(SHIELD_BUTTON_COOLDOWN){
  rocketButton.setPosition(rocketButtonPos);
  rocketButton.setSize(rocketButtonSize);
  rocketButton.setBB(rocketButtonPos, 1.5f*rocketButtonSize);
  bombButton.setPosition(mineButtonPos);
  bombButton.setSize(mineButtonSize);
  bombButton.setBB(mineButtonPos, 1.5f*mineButtonSize);
  shieldButton.setPosition(shieldButtonPos);
  shieldButton.setSize(shieldButtonSize);
  shieldButton.setBB(shieldButtonPos, 1.5f*shieldButtonSize);
}

void AndroidInputManager::reset () {
  formControl.reset();
  rocketButton.setPressed(false);
  bombButton.setPressed(false);
  shieldButton.setPressed(false);
  shieldButtonTimer.stop();
  state = STATE_DEFAULT;
}


void AndroidInputManager::draw () {
  rocketButton.draw();
  if (_progMan()->hasBombs())
    bombButton.draw();
  if (_progMan()->hasShield()) {
    if (shieldButtonTimer.isActive()) {
      shieldButton.drawPressed();
      NumberView::getInstance()->drawInt((int)ceil(shieldButtonTimer.getTimeLeft()), shieldButtonPos, shieldButtonSize);
    } else
      shieldButton.draw();
  }
  if (_progMan()->hasForms())
    formControl.draw();
}

void AndroidInputManager::updateTankDir(const Vector2& touchPosition) {
  switch(state) {
    case MOVING_TANK:
      getGame()->setTankMoveDir(touchPosition-getGame()->getPlayerTank()->getPosition());
      break;
    default:
      getGame()->setTankMoveDir(Vector2::ZERO);
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
  getGame()->setTankMoveDir(Vector2::ZERO);
  state = STATE_DEFAULT;
}

void AndroidInputManager::touchEventDown (float x, float y) {
  if (!GameManager::getInstance()->inGame()) {
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
  //LOGE("time between taps : %li, dist between tap : %f", (long)(now-lastTouchDownTime), tapDist);
  if (state == FIRING_MODE || rocketButton.inside(pNoTrans)) {
    rocketButton.setPressed(true);
  } else if (bombButton.inside(pNoTrans)) {
    if (_progMan()->hasBombs())
      bombButton.setPressed(true);
  } else if (formControl.inside(pNoTrans)) {
    if (_progMan()->hasForms())
      formControl.handleTouchDown(pNoTrans);
  } else if (shieldButton.inside(pNoTrans)) {
    if (!shieldButtonTimer.isActive() && _progMan()->hasShield())
      shieldButton.setPressed(true);
  } else if (tapDist < 5 && elapsed <= 600) {
    getGame()->playerFire(p);
    startMoving(MOVING_TANK, p);
  } else {
    startMoving(MOVING_TANK, p);
  }
  lastTouchDownTime = now;
  lastTouchDownLocation = p;
  return;
}

void AndroidInputManager::touchEventMove (float x, float y) {
  if (!GameManager::getInstance()->inGame())
    return;
  const Vector2 p(XSG(x), YSG(y));
  setMoveTouchPoint(p);
}

void AndroidInputManager::touchEventUp (float x, float y) {
  if (GameManager::getInstance()->inGame()) {
    const Vector2 pNoTrans (XSG_NOTRANSX(x), YSG_NOTRANSY(y));
    if (state == FIRING_MODE) {
      getGame()->playerFire(Vector2(XSG(x),YSG(y)));
      rocketButton.setPressed(false);
      state = STATE_DEFAULT;
    } else {
      if (rocketButton.inside(pNoTrans)) {
        state = FIRING_MODE;
        getGame()->setTankMoveDir(Vector2::ZERO);
      } else {
        rocketButton.setPressed(false);
        if (_progMan()->hasBombs() && bombButton.isPressed() &&  bombButton.inside(pNoTrans))
          getGame()->playerDropBomb();
        else if (_progMan()->hasShield() && shieldButton.isPressed() && shieldButton.inside(pNoTrans)) {
          getGame()->playerActivateShield();
          shieldButtonTimer.start();
        } else if (_progMan()->hasForms() && formControl.inside(pNoTrans))
          formControl.handleTouchUp(pNoTrans);
        else
           stopMoving();
        bombButton.setPressed(false);
        shieldButton.setPressed(false);
      }
    }
  } else
    GameManager::getInstance()->handleTouchUp(Vector2(XSG_NOTRANSX(x), YSG_NOTRANSY(y)));
}

void AndroidInputManager::touchEventOther (float x, float y) {
  touchEventUp(x,y);
}
