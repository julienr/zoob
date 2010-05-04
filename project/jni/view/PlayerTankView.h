#ifndef PLAYERTANKVIEW_H_
#define PLAYERTANKVIEW_H_

#include "TankView.h"
#include "logic/PlayerTank.h"

class PlayerTankView : public TankView {
  public:
    PlayerTankView (const PlayerTank& t)
      : TankView(t),
        tank(t) {
    }

    void draw () {
      if (!tank.isAlive())
        return;
      ePlayerForm form = tank.getCurrentForm();
      GLW::color(getColor(TANK_PLAYER));
      switch(form) {
        case FORM_SIMPLE:
        case FORM_BURST:
          getTankSprite().draw(tank, tank.getRotation());
          break;
        case FORM_SHIELD:
          getShieldTankSprite().draw(tank, tank.getRotation());
          break;
      }
      GLW::colorWhite();
    }
  private:
    const PlayerTank& tank;
};

#endif /* PLAYERTANKVIEW_H_ */
