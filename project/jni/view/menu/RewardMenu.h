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
    Sprite rewardBounce;
    Sprite rewardFiring;
    Sprite rewardShield;
    Sprite nextlvl;
};

#endif /* REWARDMENU_H_ */
