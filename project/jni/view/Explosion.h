#ifndef EXPLOSION_H_
#define EXPLOSION_H_

#include "def.h"
#include "lib/Vector2.h"
#include "TextureManager.h"

//Explosion life in seconds
#define EXPLOSION_LIFE 1.0f

#define START_SIZE 0.5f
#define END_SIZE 3.0f
#define SIZE_INCR ((END_SIZE-START_SIZE)/EXPLOSION_LIFE)

class Explosion {
  public:
    Explosion (const Vector2& position) :
      timeLeft(EXPLOSION_LIFE),
      mainSprite("assets/sprites/boom.png"),
      position(position) {}

    void draw () {
      const float size = START_SIZE + SIZE_INCR*(EXPLOSION_LIFE-timeLeft);
      mainSprite.draw(position, Vector2(size, size));
    }

    void think (float elapsed) {
      timeLeft -= elapsed;
    }

    float getTimeLeft () {
      return timeLeft;
    }

  private:
    float timeLeft;
    Sprite mainSprite;
    const Vector2 position;
};

#endif /* EXPLOSION_H_ */
