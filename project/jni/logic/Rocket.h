#ifndef ROCKET_H_
#define ROCKET_H_

#include "Entity.h"

class Rocket : public Entity {
  public:
    Rocket (Tank* owner, const Vector2& pos, const Vector2& dir) : Entity(new BCircle(ROCKET_BCIRCLE_R, this)), owner(owner), numBounces(0) {
      setPosition(pos);
      setDir(dir);
    }

    void explode () {
      owner->rocketExploded();
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

    unsigned getNumBounces () {
      return numBounces;
    }

  private:
    Tank* owner;
    Vector2 dir;
    unsigned numBounces;
};

#endif /* ROCKET_H_ */
