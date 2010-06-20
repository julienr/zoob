#ifndef REWARDMENU_H_
#define REWARDMENU_H_

#include "Menu.h"


class RewardMenu : public Menu {
  public:
    RewardMenu (GameManager* gm);
    ~RewardMenu () {}
    void draw ();
  protected:
    void actionPerformed ();
  private:
    Sprite rewardText;
    Sprite rewardBomb;
    Sprite rewardShield;
    Sprite rewardBounce;
    Sprite rewardFiring;
    Sprite nextlvl;
    Sprite rewardInstr;
};

#endif /* REWARDMENU_H_ */
