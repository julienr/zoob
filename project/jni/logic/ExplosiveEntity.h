#ifndef EXPLOSIVEENTITY_H_
#define EXPLOSIVEENTITY_H_

#include "Entity.h"

class ExplosiveEntity : public Entity {
  public:
    ExplosiveEntity (const BoundingVolume* v)
      : Entity(v),
        exploded(false) {
    }

    bool hasExploded () {
      return exploded;
    }

    bool isSolid () const {
      return !exploded;
    }

    virtual void explode (Entity* e, const Vector2& colPoint) {
      LOGE("Explosion");
      exploded = true;
    }
  private:

    //When it has exploded, a rocket is marked as such and scheduled for explosion
    bool exploded;
};

#endif /* EXPLOSIVEENTITY_H_ */
