#ifndef ROCKET_H_
#define ROCKET_H_

#include "Entity.h"

class Rocket : public Entity {
  public:
    Rocket (Tank* owner, const Vector2& pos, const Vector2& dir)
      : Entity(new BCircle(ROCKET_BCIRCLE_R)),
        owner(owner),
        numBounces(0),
        exploded(false) {
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

    bool hasExploded () {
      return exploded;
    }

    bool isSolid () const {
      return !exploded;
    }

    void explode () {
      LOGE("Rocket explosion");
      exploded = true;
    }

  private:
    Tank* owner;
    Vector2 dir;
    unsigned numBounces;
    //When it has exploded, a rocket is marked as such and scheduled for explosion
    bool exploded;
};

#endif /* ROCKET_H_ */
