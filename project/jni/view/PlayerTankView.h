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
        shieldSprite("assets/sprites/shield_1.png", TEX_GROUP_GAME)
        {}

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
        case FORM_BOUNCE:
          getBounceTankSprite()->draw(*tank, tank->getRotation());
          break;
      }
      if (tank->getShieldTimeLeft() > 0) {
        drawShield(elapsedS);
      } else
        shieldState = SHIELD_ACTIVE;

      GLW::colorWhite();
    }

    void drawShield (double elapsedS) {
      if (shieldState == SHIELD_FLASH_FULL) {
        shieldStateTimeLeft -= elapsedS;
        if (shieldStateTimeLeft < 0) {
          shieldState = SHIELD_FLASH_TRANSPARENT;
          shieldStateTimeLeft = 0.5;
        }
        GLW::color(getColor(TANK_PLAYER), 0.5f);
      } else if (shieldState == SHIELD_FLASH_TRANSPARENT) {
        shieldStateTimeLeft -= elapsedS;
        if (shieldStateTimeLeft < 0) {
          shieldState = SHIELD_FLASH_FULL;
          shieldStateTimeLeft = 0.5;
        }
        GLW::color(getColor(TANK_PLAYER));
      } else {
        if (tank->getShieldTimeLeft() < 2.5) {
          shieldState = SHIELD_FLASH_FULL;
          shieldStateTimeLeft = 0.5;
        }
        GLW::color(getColor(TANK_PLAYER));
      }
      shieldSprite.draw(*tank, 0, 1.1f);
    }
  private:
    const PlayerTank* tank;
    Sprite shieldSprite;

    //We are animating the shield when the period comes to an end. It will flash
    enum ShieldState {
        SHIELD_FLASH_FULL,
        SHIELD_FLASH_TRANSPARENT,
        SHIELD_ACTIVE
    } shieldState;
    double shieldStateTimeLeft;
};

#endif /* PLAYERTANKVIEW_H_ */
