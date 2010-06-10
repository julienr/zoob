#ifndef ROCKET_H_
#define ROCKET_H_

#include "ExplosiveEntity.h"
#include "logic/physics/BCircle.h"

class Game;
class Tank;

enum BouncePolicy {
    BOUNCE,
    SPLIT
};

class Rocket : public ExplosiveEntity {
  public:
    Rocket (Tank* owner, const Vector2& pos, const Vector2& dir, BouncePolicy bouncePol=BOUNCE)
      : ExplosiveEntity (new BCircle(ROCKET_BCIRCLE_R)),
        owner(owner),
        numBounces(0),
        bouncePolicy(bouncePol) {
      setPosition(pos);
      setDir(dir);
    }

    Tank* getOwner () const {
      return owner;
    }

    const Vector2& getDir () const {
      return dir;
    }

    virtual void think (double elapsedS) {}

    BouncePolicy getBouncePolicy () {
      return bouncePolicy;
    }

    void setDir (const Vector2& d) {
      dir = d.getNormalized();
    }

    virtual bool canBounce () const {
      return true;
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
    const BouncePolicy bouncePolicy;
};

#endif /* ROCKET_H_ */
