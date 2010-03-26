#ifndef TANK_H_
#define TANK_H_

#include "Entity.h"
#include "physics/BCircle.h"

class Tank: public Entity {
  public:
    Tank () : Entity (new BCircle(0.5f, this)) {}
    //FIXME: only for debug draw
    Vector2 lastColNormal;
    Vector2 lastColPoint;
};

#endif /* TANK_H_ */
