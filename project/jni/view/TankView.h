#ifndef TANKVIEW_H_
#define TANKVIEW_H_

#include "logic/Tank.h"
#include "Sprite.h"
#include "GLW.h"

class TankView {
  public:
    TankView (const Tank& t) :
      tank(t), baseSprite("assets/sprites/tank1.png"), turretSprite("assets/sprites/turret.png") {
    }

    void draw () {
      GLW::color(tank.getColor());
      baseSprite.draw(tank, tank.getRotation());
      GLW::colorWhite();
      //turretSprite.draw(tank);
    }

    Vector2 getCenter () {
      return baseSprite.getCenter(tank);
    }

    bool touchInside (const Vector2& p) const {
      return baseSprite.touchInside(tank, p);
    }
  private:
    const Tank& tank;
    Sprite baseSprite;
    Sprite turretSprite;

};

#endif /* TANKVIEW_H_ */
