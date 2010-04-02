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

    Rocket* fireRocket (const Vector2& dir);

    //Callback called by a Rocket when it explodes
    void rocketExploded ();
};

#endif /* TANK_H_ */
