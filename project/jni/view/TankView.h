#ifndef TANKVIEW_H_
#define TANKVIEW_H_

#include "logic/Tank.h"
#include "Sprite.h"
#include "GLW.h"

class TankView {
  public:
    TankView (const Tank& t) :
      tank(t),
      tankSprite("assets/sprites/tank1.png"),
      shieldTankSprite("assets/sprites/tank_shield.png") {
    }

    void draw () {
      if (!tank.isAlive())
        return;
      eColor c = tank.getColor();
      GLW::color(tank.getColor());
      switch(c) {
        case GREY:
        case RED:
          tankSprite.draw(tank, tank.getRotation());
          break;
        case GREEN:
          shieldTankSprite.draw(tank, tank.getRotation());
          break;
        default:
          LOGE("Unhandled color %i", c);
      }
      GLW::colorWhite();
    }

    Vector2 getCenter () {
      return tankSprite.getCenter(tank);
    }

    bool touchInside (const Vector2& p) const {
      return tankSprite.touchInside(tank, p);
    }
  private:
    const Tank& tank;
    Sprite tankSprite;
    Sprite shieldTankSprite;

};

#endif /* TANKVIEW_H_ */
