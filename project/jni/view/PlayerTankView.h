#ifndef PLAYERTANKVIEW_H_
#define PLAYERTANKVIEW_H_

#include "TankView.h"
#include "logic/PlayerTank.h"
#include "view/FadeAnim.h"

class PlayerTankView : public TankView {
  public:
    PlayerTankView (const PlayerTank* t)
      : TankView(t),
        tank(t),
        shieldSprite("assets/sprites/shield_1.png") {}

    void draw (double elapsedS) {
      if (!tank->isAlive())
        return;
      ePlayerForm form = tank->getCurrentForm();
      GLW::color(getColor(TANK_PLAYER));
      switch(form) {
        case FORM_SIMPLE:
        case FORM_BURST:
          getTankSprite()->draw(*tank, tank->getRotation());
          break;
        case FORM_SHIELD:
          getShieldTankSprite()->draw(*tank, tank->getRotation());
          break;
      }
      GLW::colorWhite();
      shieldSprite.draw(*tank, 0, 1.1f);
    }
  private:
    const PlayerTank* tank;
    const Sprite shieldSprite;
};

#endif /* PLAYERTANKVIEW_H_ */
