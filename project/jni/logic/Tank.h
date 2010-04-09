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
    Tank (eColor col) : Entity (new BCircle(TANK_BCIRCLE_R, this)), color(col), ai(NULL), exploded(false) {}
    Tank (eColor col, TankAI* ai) : Entity(new BCircle(TANK_BCIRCLE_R, this)), color(col), ai(ai), exploded(false) {}

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

    bool hasExploded () {
      return exploded;
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
    bool exploded;
};

#endif /* TANK_H_ */
