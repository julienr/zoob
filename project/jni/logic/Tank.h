#ifndef TANK_H_
#define TANK_H_

#include "def.h"
#include "Entity.h"
#include "physics/BCircle.h"
#include "lib/Color.h"
#include "lib/Utils.h"

class Rocket;

class Tank: public Entity {
  public:
    Tank (const uint64_t fireIntervalms)
      : Entity(new BCircle(TANK_BCIRCLE_R)),
        exploded(false),
        alive(true),
        lastFireTime(0),
        fireInterval(fireIntervalms) {
    }

    virtual ~Tank () {}

    //FIXME: only for debug draw
    Vector2 lastColNormal;
    Vector2 lastColPoint;

    eEntityType getType () const {
      return ENTITY_TANK;
    }

    virtual void explode (Entity* e, const Vector2& colPoint);

    bool hasExploded () const {
      return exploded;
    }

    void die () {
      alive = false;
    }

    void unmarkExploded () {
      exploded = false;
    }

    bool isAlive () const {
      return alive;
    }

    virtual eColor getColor () const = 0;

    //true if the tank can (ie is allowed by the game rules) fire a rocket
    bool canFire () { return Utils::getCurrentTimeMillis() - lastFireTime > fireInterval; }

    Rocket* fireRocket (Vector2 dir);
  private:
    /* Exploded is just set for the frame after the tank has exploded (for one-time stuff to be handled by game)
     * The tank should be marked as dead once this is done
     */
    bool exploded;
    bool alive;

    uint64_t lastFireTime;
    const uint64_t fireInterval; //fire interval in milliseconds
};

#endif /* TANK_H_ */
