#ifndef EXPLOSION_H_
#define EXPLOSION_H_

#include "def.h"
#include "lib/Vector2.h"
#include "Sprite.h"
#include "logic/Game.h"

//Explosion life in seconds
#define EXPLOSION_LIFE 0.5f

class Explosion {
  public:
    Explosion (const ExplosionLocation& loc) :
      timeLeft(EXPLOSION_LIFE),
      boomSprite("assets/sprites/boom.png", TEX_GROUP_GAME),
      poofSprite("assets/sprites/rocket.png", TEX_GROUP_GAME),
      location(loc) {}

    void draw ();

    void think (float elapsed) {
      timeLeft -= elapsed;
    }

    float getTimeLeft () {
      return timeLeft;
    }

  private:
    float timeLeft;
    Sprite boomSprite;
    Sprite poofSprite;
    const ExplosionLocation location;
};

#endif /* EXPLOSION_H_ */
