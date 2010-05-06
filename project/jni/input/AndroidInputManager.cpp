#include "AndroidInputManager.h"
#include "app.h"


const Vector2 rocketButtonPos(13.8f, 4.0f);
const Vector2 rocketButtonSize(2.5,1.25);
const Vector2 mineButtonPos(rocketButtonPos.x, 6.0f);
const Vector2 mineButtonSize(rocketButtonSize);

#define ROCKET_BUTTON_ID 0 //just used for button creation
#define MINE_BUTTON_ID 1 //just used for button creation

AndroidInputManager::AndroidInputManager ()
  : InputManager(),
    state(STATE_DEFAULT),
    rocketButton("assets/sprites/fire_rocket.png",
                 "assets/sprites/fire_rocket_clicked.png",
                 ROCKET_BUTTON_ID),
    bombButton("assets/sprites/fire_bomb.png",
               "assets/sprites/fire_bomb_clicked.png",
               MINE_BUTTON_ID),
    lastTouchDownTime(0) {
  rocketButton.setPosition(rocketButtonPos);
  rocketButton.setSize(rocketButtonSize);
  rocketButton.setBB(rocketButtonPos, rocketButtonSize);
  bombButton.setPosition(mineButtonPos);
  bombButton.setSize(mineButtonSize);
  bombButton.setBB(mineButtonPos, mineButtonSize);
}

void AndroidInputManager::reset () {
  formControl.reset();
  rocketButton.setPressed(false);
  bombButton.setPressed(false);
  state = STATE_DEFAULT;
}

void AndroidInputManager::draw () {
  rocketButton.draw();
  bombButton.draw();
  formControl.draw();
}

void AndroidInputManager::updateTankDir(const Vector2& touchPosition) {
  switch(state) {
    case MOVING_TANK_PAD:
      ASSERT(false); //disabled
      break;
    case MOVING_TANK:
      getGame()->setTankMoveDir(touchPosition-getGame()->getPlayerTank().getPosition());
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
    bombButton.setPressed(true);
  } else if (formControl.inside(pNoTrans)) {
    formControl.handleTouchDown(pNoTrans);
  } else if (tapDist < 1 && elapsed <= 300) {
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
    if (state == STATE_DEFAULT) {
      if (rocketButton.inside(pNoTrans)) {
        state = FIRING_MODE;
        getGame()->setTankMoveDir(Vector2::ZERO);
      } else
        rocketButton.setPressed(false);

      if (bombButton.inside(pNoTrans))
        getGame()->playerDropBomb();
      bombButton.setPressed(false);
    } else if (state == FIRING_MODE) {
      getGame()->playerFire(Vector2(XSG(x),YSG(y)));
      rocketButton.setPressed(false);
      state = STATE_DEFAULT;
    } else if (formControl.inside(pNoTrans)) {
      formControl.handleTouchUp(pNoTrans);
    } else
      stopMoving();
  } else
    GameManager::getInstance()->handleTouchUp(Vector2(XSG_NOTRANSX(x), YSG_NOTRANSY(y)));
}

void AndroidInputManager::touchEventOther (float x, float y) {
  touchEventUp(x,y);
}
