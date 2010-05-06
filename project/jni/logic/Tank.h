#ifndef TANK_H_
#define TANK_H_

#include "def.h"
#include "Entity.h"
#include "physics/BCircle.h"
#include "lib/Utils.h"
#include "Path.h"
#include "FireRatePolicy.h"

class Rocket;
class Bomb;

enum eTankType {
    TANK_PLAYER=0,
    TANK_STATIC,
    TANK_SIMPLE,
    TANK_SHIELD,
    TANK_BURST
};

class CollisionManager;

class Tank: public Entity {
  public:
    Tank (FireRatePolicy* pol)
      : Entity(new BCircle(TANK_BCIRCLE_R)),
        exploded(false),
        alive(true),
        firePolicy(pol),
        bombPolicy(new IntervalFirePolicy(1000)), //FIXME: should this be client-configurable ?
        path(NULL) {
      ASSERT(pol != NULL);
    }

    virtual ~Tank () {
      delete firePolicy;
      delete bombPolicy;
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
    bool canMine () { return (numMines < MAX_BOMBS_PER_TANK) && bombPolicy->canFire(); }
    //This function check that if the tank would fire now, the rocket wouldn't end up stuck in a wall
    bool checkFireDir (const Vector2& dir, const CollisionManager& colManager);

    Rocket* fireRocket (Vector2 dir);
    Bomb* dropBomb ();
    
    //A bomb this tank owned just exploded => this is used to keep track of the number of mines currently owned by this tank
    void bombExploded () {
      ASSERT(numMines > 0);
      numMines--;
    }
  protected:
    FireRatePolicy* getFireRatePolicy () const { return firePolicy; }

    //This function simulate a 90-degrees shield on the front of the tank and returns
    //true if the hit is on this shield, bouncing the entity
    bool shieldBounce (Entity* e, const Vector2& colPoint);

    void setFirePolicy (FireRatePolicy* newPol) {
      delete firePolicy;
      firePolicy = newPol;
    }
  private:

    /* Exploded is just set for the frame after the tank has exploded (for one-time stuff to be handled by game)
     * The tank should be marked as dead once this is done
     */
    bool exploded;
    bool alive;

    FireRatePolicy* firePolicy;
    FireRatePolicy* bombPolicy;

    Path* path;
    
    int numMines;
};

#endif /* TANK_H_ */
