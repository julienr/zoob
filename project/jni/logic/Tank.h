#ifndef TANK_H_
#define TANK_H_

#include "def.h"
#include "Entity.h"
#include "physics/BCircle.h"

class Rocket;

class Tank: public Entity {
  public:
    Tank () : Entity (new BCircle(TANK_BCIRCLE_R, this)) {}
    //FIXME: only for debug draw
    Vector2 lastColNormal;
    Vector2 lastColPoint;

    eEntityType getType () const {
      return ENTITY_TANK;
    }

    void explode () {
      LOGE("OMG, got an explosion");
    }

    Rocket* fireRocket (const Vector2& dir);
};

#endif /* TANK_H_ */
