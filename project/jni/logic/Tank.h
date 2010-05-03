#ifndef TANK_H_
#define TANK_H_

#include "def.h"
#include "Entity.h"
#include "physics/BCircle.h"
#include "lib/Utils.h"
#include "Path.h"
#include "FireRatePolicy.h"

class Rocket;

enum eTankType {
    TANK_PLAYER,
    TANK_STATIC,
    TANK_SIMPLE,
    TANK_SHIELD
};

class Tank: public Entity {
  public:
    Tank (FireRatePolicy* pol)
      : Entity(new BCircle(TANK_BCIRCLE_R)),
        exploded(false),
        alive(true),
        firePolicy(pol),
        path(NULL) {
      ASSERT(pol != NULL);
    }

    virtual ~Tank () {
      delete firePolicy;
    }

    //FIXME: only for debug draw
    Vector2 lastColNormal;
    Vector2 lastColPoint;

    eEntityType getType () const {
      return ENTITY_TANK;
    }

    virtual eTankType getTankType () const = 0;

    void explode (Entity* e, const Vector2& colPoint);

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

    void setPath (Path* p) {
      path = p;
    }

    //returns NULL if none
    Path* getPath () {
      return path;
    }

    //true if the tank can (ie is allowed by the game rules) fire a rocket
    bool canFire () { return firePolicy->canFire(); }

    Rocket* fireRocket (Vector2 dir);
  private:
    /* Exploded is just set for the frame after the tank has exploded (for one-time stuff to be handled by game)
     * The tank should be marked as dead once this is done
     */
    bool exploded;
    bool alive;

    FireRatePolicy* const firePolicy;

    Path* path;
};

#endif /* TANK_H_ */
