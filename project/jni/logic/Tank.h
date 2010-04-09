#ifndef TANK_H_
#define TANK_H_

#include "def.h"
#include "Entity.h"
#include "physics/BCircle.h"
#include "lib/Color.h"
#include "ai/TankAI.h"

class Rocket;

class Tank: public Entity {
  public:
    Tank (eColor col) : Entity (new BCircle(TANK_BCIRCLE_R, this)), color(col), ai(NULL), exploded(false), alive(true) {}
    Tank (eColor col, TankAI* ai) : Entity(new BCircle(TANK_BCIRCLE_R, this)), color(col), ai(ai), exploded(false), alive(true) {}

    ~Tank () {
      if (ai)
        delete ai;
    }

    //FIXME: only for debug draw
    Vector2 lastColNormal;
    Vector2 lastColPoint;

    eEntityType getType () const {
      return ENTITY_TANK;
    }

    void explode () {
      exploded = true;
      LOGE("OMG, got an explosion");
    }

    bool hasExploded () const {
      return exploded;
    }

    void die () {
      alive = false;
    }

    bool isAlive () const {
      return alive;
    }

    eColor getColor () const {
      return color;
    }

    //Returns NULL for the player's tank
    TankAI* getAI () { return ai; }

    Rocket* fireRocket (Vector2 dir);
  private:
    const eColor color; //This tank's color (highly symbolic, but used for rendering)
    TankAI* ai;
    /* Exploded is just set for the frame after the tank has exploded (for one-time stuff to be handled by game)
     * The tank should be marked as dead once this is done
     */
    bool exploded;
    bool alive;
};

#endif /* TANK_H_ */
