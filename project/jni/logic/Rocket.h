#ifndef ROCKET_H_
#define ROCKET_H_

#include "ExplosiveEntity.h"

class Rocket : public ExplosiveEntity {
  public:
    Rocket (Tank* owner, const Vector2& pos, const Vector2& dir)
      : ExplosiveEntity (new BCircle(ROCKET_BCIRCLE_R)),
        owner(owner),
        numBounces(0) {
      setPosition(pos);
      setDir(dir);
    }

    Tank* getOwner () {
      return owner;
    }

    const Vector2& getDir () const {
      return dir;
    }

    void setDir (const Vector2& d) {
      dir = d.getNormalized();
    }

    void addBounce () {
      numBounces++;
    }

    eEntityType getType () const {
      return ENTITY_ROCKET;
    }

    unsigned getNumBounces () {
      return numBounces;
    }

  private:
    Tank* owner;
    Vector2 dir;
    unsigned numBounces;
};

#endif /* ROCKET_H_ */
