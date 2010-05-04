#include "AndroidInputManager.h"
#include "app.h"


const Vector2 fireButtonPos(13.8f, 6.0f);
const Vector2 fireButtonSize(2.5,2.5);

bool inFireButton (float x, float y) {
  return Utils::insideC(fireButtonPos, fireButtonSize, Vector2(XSG_NOTRANSX(x), YSG_NOTRANSY(y)));
}


AndroidInputManager::AndroidInputManager ()
  : InputManager(),
    state(STATE_DEFAULT),
    fireButton("assets/sprites/fire_btn.png"),
    fireButtonClicked("assets/sprites/fire_btn_clicked.png"),
    lastTouchDownTime(0) {
}

void AndroidInputManager::draw () {
  if (state == FIRING_MODE)
    fireButtonClicked.draw(fireButtonPos, fireButtonSize);
  else
    fireButton.draw(fireButtonPos, fireButtonSize);

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
  if (state == FIRING_MODE || inFireButton(x,y)) {
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
    if (state == STATE_DEFAULT && inFireButton(x,y)) {
      state = FIRING_MODE;
      getGame()->setTankMoveDir(Vector2::ZERO);
    } else if (state == FIRING_MODE) {
      getGame()->playerFire(Vector2(XSG(x),YSG(y)));
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
