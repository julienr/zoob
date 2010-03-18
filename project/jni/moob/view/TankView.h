#ifndef TANKVIEW_H_
#define TANKVIEW_H_

#include "logic/Tank.h"
#include "Sprite.h"

class TankView {
  public:
    TankView (const Tank& t) :
      tank(t), baseSprite("assets/sprites/tank1.png"), turretSprite("assets/sprites/turret.png") {
    }

    void draw () {
      baseSprite.draw(tank, tank.getRotation());
      turretSprite.draw(tank);
    }

    Vector2 getCenter () {
      return baseSprite.getCenter(tank);
    }

  private:
    const Tank& tank;
    Sprite baseSprite;
    Sprite turretSprite;

};

#endif /* TANKVIEW_H_ */
