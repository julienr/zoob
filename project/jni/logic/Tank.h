#ifndef TANK_H_
#define TANK_H_

#include "def.h"
#include "Entity.h"
#include "physics/BCircle.h"
#include "lib/Utils.h"
#include "Path.h"
#include "FireRatePolicy.h"
#include "logic/physics/CollisionResult.h"
#include "lib/Timer.h"
#include "ai/TankAI.h" 
#include "Difficulty.h"
#include "containers/set.h"


class Rocket;
class Bomb;

enum eTankCategory {
  CAT_PLAYER,
  CAT_NET,
  CAT_AI
};

enum eTankType {
    TANK_PLAYER=0,
    TANK_STATIC,
    TANK_SIMPLE,
    BOSS_SIMPLE,
    TANK_BOUNCE,
    BOSS_BOUNCE,
    TANK_SHIELD,
    BOSS_SHIELD,
    TANK_BURST,
    BOSS_BURST,
    TANK_SPLIT,
    BOSS_SPLIT,
};


#define TANK_MOVE_SPEED 1.5f

class CollisionManager;

class Tank: public Entity {
  public:
    //radius is the radius of the tank
    Tank (float radius, FireRatePolicy* pol, float speed=TANK_MOVE_SPEED)
      : Entity(new BCircle(radius)),
        maxLives(1),
        numLives(1),
        tankRadius(radius),
        exploded(false),
        alive(true),
        firePolicy(pol),
        //FIXME: move the whole bomb stuff to PlayerTank
        bombPolicy(new IntervalFirePolicy(1000)), //FIXME: should this be client-configurable ?
        path(NULL),
        numMines(0),
        forceDirTimer(0.3),
        shieldTimer(Difficulty::getInstance()->getPlayerShieldTime()),
        speed(speed) {
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

    virtual eTankCategory getTankCategory () const = 0;

    virtual int explode (Entity* e, const Vector2& colPoint);

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

    virtual bool acceptsTouch (Entity* other) {
      return other->getType() == ENTITY_ROCKET ||
             other->getType() == ENTITY_BOMB; 
    }

    //returns NULL if none
    Path* getPath () {
      return path;
    }

    //returns NULL if no AI
    virtual TankAI* getAI () const { return NULL; }

    virtual void think (double elapsedS) {}

    //true if the tank can (ie is allowed by the game rules) fire a rocket
    bool canFire () { return firePolicy->canFire(); }
    bool canMine () { return (numMines < MAX_BOMBS_PER_TANK) && bombPolicy->canFire(); }
    //This function check that if the tank would fire now, the rocket wouldn't end up stuck in a wall
    bool checkFireDir (const Vector2& dir, const CollisionManager& colManager, CollisionResult* r);

    Rocket* fireRocket (Vector2 dir);
    Bomb* dropBomb ();
    
    //A bomb this tank owned just exploded => this is used to keep track of the number of mines currently owned by this tank
    void bombExploded () {
      ASSERT(numMines > 0);
      numMines--;
    }

    void setRotationFromDir (const Vector2& dir) {
      if (forceDirTimer.isActive())
        return;
      Entity::setRotationFromDir(dir);
    }

    void startShield () {
      shieldTimer.start();
    }

    double getShieldTimeLeft () const {
      return shieldTimer.getTimeLeft();
    }

    bool shieldActive () const {
      return shieldTimer.isActive();
    }

    void setMoveDir (const Vector2& v) {
      moveDir.set(v);
    }

    const Vector2& getMoveDir () const {
      return moveDir;
    }

    unsigned getLivesLeft () const { return numLives; }
    unsigned getMaxLives () const { return maxLives; }

    float getSpeed () const { return speed; }

    void addRocket (Rocket* r) {
      rockets.insert(r);
    }

    void removeRocket (Rocket* r) {
      rockets.remove(r);
    }

    void addBomb (Bomb* b) {
      bombs.insert(b);
    }

    void removeBomb (Bomb* b) {
      bombs.remove(b);
    }

    size_t getNumRockets () const {
      return rockets.size();
    }

    set<Rocket*>::const_iterator getRockets () const {
      return rockets.begin();
    }

    size_t getNumBombs () const {
      return bombs.size();
    }

    set<Bomb*>::const_iterator getBombs () const {
      return bombs.begin();
    }

  protected:
    virtual Rocket* createRocket (Tank* owner, const Vector2& pos, const Vector2& dir) = 0;

    FireRatePolicy* getFireRatePolicy () const { return firePolicy; }

    //This function simulate a 90-degrees shield on the front of the tank and returns
    //true if the hit is on this shield, bouncing the entity
    bool shieldBounce (Entity* e, const Vector2& colPoint);

    void setFirePolicy (FireRatePolicy* newPol) {
      delete firePolicy;
      firePolicy = newPol;
    }

    void setLives (unsigned num) { maxLives = numLives = num; }
  private:
    Vector2 moveDir;
    unsigned maxLives;
    unsigned numLives;
    float tankRadius;

    /* Exploded is just set for the frame after the tank has exploded (for one-time stuff to be handled by game)
     * The tank should be marked as dead once this is done
     */
    bool exploded;
    bool alive;

    FireRatePolicy* firePolicy;
    FireRatePolicy* bombPolicy;

    Path* path;
    
    int numMines;

    //This is a timer just used to force the direction in which the tank is viewing after it has fired (to have a visual feedback
    //of the direction in which the tank just fired)
    Timer forceDirTimer;

    Timer shieldTimer;

    const float speed; //movement speed

    //rockets and bombs owned by this tank
    set<Rocket*> rockets;
    set<Bomb*> bombs;
};

#endif /* TANK_H_ */
