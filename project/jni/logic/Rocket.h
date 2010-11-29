#ifndef ROCKET_H_
#define ROCKET_H_

#include "Entity.h"
#include "logic/physics/BCircle.h"

class Game;
class Tank;

enum BouncePolicy {
    BOUNCE,
    SPLIT
};

class Rocket : public Entity {
  public:
    Rocket (Tank* owner, const Vector2& pos, const Vector2& dir, float speed, BouncePolicy bouncePol=BOUNCE)
      : Entity (new BCircle(ROCKET_BCIRCLE_R)),
        owner(owner),
        numBounces(0),
        bouncePolicy(bouncePol),
        speed(speed) {
      setPosition(pos);
      setDir(dir);
    }

    Rocket (const Rocket* other)
      : Entity (new BCircle(ROCKET_BCIRCLE_R)),
        owner(other->getOwner()),
        numBounces(other->getNumBounces()),
        bouncePolicy(other->getBouncePolicy()),
        speed(other->getSpeed()){
      setPosition(other->getPosition());
      setDir(other->getDir());
    }

    Tank* getOwner () const {
      return owner;
    }

    const Vector2& getDir () const {
      return dir;
    }

    virtual void think (double UNUSED(elapsedS)) {}

    BouncePolicy getBouncePolicy () const {
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

    unsigned getNumBounces () const {
      return numBounces;
    }

    void setSpeed (float speed) {
      this->speed = speed;
    }

    float getSpeed () const {
      return speed;
    }

  private:
    Tank* owner;
    Vector2 dir;
    unsigned numBounces;
    const BouncePolicy bouncePolicy;
    float speed;
};

#endif /* ROCKET_H_ */
