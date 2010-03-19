#ifndef TANK_H_
#define TANK_H_

#include "Entity.h"

class Tank: public Entity {
  public:
    Tank () : Entity () {}
    //FIXME: only for debug draw
    Vector2 lastColNormal;
    Vector2 lastColPoint;
};

#endif /* TANK_H_ */
