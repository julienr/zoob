#include "Explosion.h"

#define BOOM_START_SIZE 0.5f
#define BOOM_END_SIZE 3.0f
#define BOOM_SIZE_INCR ((END_SIZE-START_SIZE)/EXPLOSION_LIFE)

#define POOF_START_SIZE 0.6f
#define POOF_END_SIZE 0.1f
#define POOF_SIZE_INCR ((POOF_END_SIZE-POOF_START_SIZE)/EXPLOSION_LIFE)

void Explosion::draw () {
  if (location.type == ExplosionLocation::EXPLOSION_BOOM) {
    const float arg = (EXPLOSION_LIFE-timeLeft)/EXPLOSION_LIFE;
    const float size = BOOM_START_SIZE + (BOOM_END_SIZE-BOOM_START_SIZE)*arg;
    boomSprite.draw(location.position, Vector2(size, size));
  } else {
    const float arg = (EXPLOSION_LIFE-timeLeft)/EXPLOSION_LIFE;
    const float size = POOF_START_SIZE + (POOF_END_SIZE-POOF_START_SIZE)*arg;
    poofSprite.draw(location.position, Vector2(size, size));
  }
}
