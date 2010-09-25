#ifndef EXPLOSION_H_
#define EXPLOSION_H_

#include "def.h"
#include "lib/Vector2.h"
#include "Sprite.h"
#include "logic/Game.h"
#include "Animation.h"

//Explosion life in seconds
#define EXPLOSION_LIFE 0.8f

class Explosion {
  public:
    Explosion (const ExplosionLocation& loc);
    void draw (float elapsed);

    float getTimeLeft () {
      return timeLeft;
    }

  private:
    float timeLeft;
    Sprite boomSprite;
    Sprite poofSprite;
    Animation fireExpl;
    const ExplosionLocation location;
};

#endif /* EXPLOSION_H_ */
