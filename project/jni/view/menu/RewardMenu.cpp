#include "RewardMenu.h"
#include "logic/ProgressionManager.h"
#include "view/GameManager.h"

RewardMenu::RewardMenu (GameManager* gm)
  : Menu(gm),
    rewardText("assets/sprites/menuitems/reward.png", TEX_GROUP_REWARD),
    rewardBomb("assets/sprites/menuitems/reward_bomb.png", TEX_GROUP_REWARD),
    rewardShield("assets/sprites/menuitems/reward_shield.png", TEX_GROUP_REWARD),
    rewardBounce("assets/sprites/menuitems/reward_bounce.png", TEX_GROUP_REWARD),
    rewardFiring("assets/sprites/menuitems/reward_firing.png", TEX_GROUP_REWARD),
    nextlvl("assets/sprites/menuitems/continue.png", TEX_GROUP_REWARD){

}

void RewardMenu::draw () {
  rewardText.draw(Vector2(7.5, 2), Vector2(6,3));
  const Vector2 rwPos(7.5, 5);
  const Vector2 rwSize(4,4);

  switch (ProgressionManager::getInstance()->getLastReward()) {
  case REWARD_BOMB:
    rewardBomb.draw(rwPos, rwSize);
    break;
  case REWARD_SHIELD:
    rewardShield.draw(rwPos, rwSize);
    break;
  case REWARD_BOUNCE:
    rewardBounce.draw(rwPos, rwSize);
    break;
  case REWARD_FIRING:
    rewardBounce.draw(rwPos, rwSize);
    break;
  default:
    ASSERT(false);
  }
  nextlvl.draw(Vector2(7.5, 8), Vector2(8, 2));
}

void RewardMenu::actionPerformed () {
  gameManager->nextGame();
}
