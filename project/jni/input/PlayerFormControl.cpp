#include "PlayerFormControl.h"
#include "view/GameManager.h"
#include "view/TankView.h"

#define BTN_NEXT 0
#define BTN_PREV 1

PlayerFormControl::PlayerFormControl ()
  : Menu(NULL),
    tankSprite("assets/sprites/tank1.png", TEX_GROUP_GAME),
    bounceTankSprite("assets/sprites/tank_bounce.png", TEX_GROUP_GAME),
    currentFormIdx(0){
  MenuItem* next = new MenuItem("assets/sprites/menuitems/next.png",
                                "assets/sprites/menuitems/next_h.png",
                                BTN_NEXT,
                                TEX_GROUP_MENU);

  const Vector2 nextPos(14.5, 0.5f);
  const Vector2 btnSize(1,1);
  const Vector2 btnBBSize(1.6f,1.5f);
  next->setPosition(nextPos);
  next->setSize(btnSize);
  next->setBB(nextPos, btnBBSize);

  MenuItem* prev = new MenuItem("assets/sprites/menuitems/prev.png",
                                "assets/sprites/menuitems/prev_h.png",
                                BTN_PREV,
                                TEX_GROUP_MENU);

  const Vector2 prevPos(12.5, 0.5f);
  prev->setPosition(prevPos);
  prev->setSize(btnSize);
  prev->setBB(prevPos, btnBBSize);

  addItem(next);
  addItem(prev);
}

void PlayerFormControl::reset () {
  currentFormIdx = 0;
}

void PlayerFormControl::actionPerformed (short touchedItem) {
  switch (touchedItem) {
    case BTN_NEXT: _actionNext(); break;
    case BTN_PREV: _actionPrev(); break;
    default:
      LOGE("unhandled menu id : %i", touchedItem);
      ASSERT(false);
      break;
  }
}

void changeForm (ePlayerForm f) {
  GameManager::getInstance()->getGame()->getPlayerTank()->changePlayerForm(f);
}

void PlayerFormControl::_actionNext () {
  const vector<ePlayerForm>& availableForms = ProgressionManager::getInstance()->getAvailablePlayerForms();
  if (currentFormIdx >= availableForms.length()-1)
    return;
  currentFormIdx++;
  changeForm(availableForms[currentFormIdx]);
}

void PlayerFormControl::_actionPrev () {
  const vector<ePlayerForm>& availableForms = ProgressionManager::getInstance()->getAvailablePlayerForms();
  if (currentFormIdx == 0)
    return;
  currentFormIdx--;
  changeForm(availableForms[currentFormIdx]);
}

void PlayerFormControl::draw () {
  const vector<ePlayerForm>& availableForms = ProgressionManager::getInstance()->getAvailablePlayerForms();
  for (size_t i=0; i<numItems(); i++) {
    MenuItem* item = getItem(i);
    short id = item->getID();
    if ((id == BTN_NEXT && currentFormIdx >= availableForms.length()-1) ||
        (id == BTN_PREV && currentFormIdx == 0))
      continue;
    item->draw();
  }

  static const Vector2 tankPos(13.5, 0.5f);
  static const Vector2 tankSize(1,1);
  const PlayerTank* playerTank = GameManager::getInstance()->getGame()->getPlayerTank();
  //GLW::color(TankView::getColor(playerTank.getTankType()));
  GLW::color(TankView::getColor((eTankType)playerTank->getCurrentForm()));
  switch (playerTank->getCurrentForm()) {
    case FORM_SIMPLE:
    case FORM_BURST:
      tankSprite.draw(tankPos, tankSize);
      break;
    case FORM_BOUNCE:
      bounceTankSprite.draw(tankPos, tankSize);
      break;
  }
  GLW::colorWhite();

}
