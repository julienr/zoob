#ifndef ROCKET_H_
#define ROCKET_H_

#include "Entity.h"

class Rocket : public Entity {
  public:
    Rocket (Tank* owner, const Vector2& pos, const Vector2& dir) : Entity(new BCircle(0.2f, this)), owner(owner), dir(dir) {
      setPosition(pos);
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

  private:
    Tank* owner;
    Vector2 dir;
};

#endif /* ROCKET_H_ */
