#include "Explosion.h"

#define BOOM_START_SIZE 0.5f
#define BOOM_END_SIZE 3.0f
#define BOOM_SIZE_INCR ((END_SIZE-START_SIZE)/EXPLOSION_LIFE)

#define POOF_START_SIZE 0.6f
#define POOF_END_SIZE 0.1f
#define POOF_SIZE_INCR ((POOF_END_SIZE-POOF_START_SIZE)/EXPLOSION_LIFE)

#define POOF_LIFE 0.5f
#define BOOM_LIFE 0.8f

Explosion::Explosion (const ExplosionLocation& loc) 
  : boomSprite("assets/sprites/boom.png", TEX_GROUP_GAME),
    poofSprite("assets/sprites/rocket.png", TEX_GROUP_GAME),
    fireExpl("assets/sprites/expl2.png", TEX_GROUP_GAME, 16, new Animation::LinearInterpolator(), EXPLOSION_LIFE, false),
    location(loc) {
  if (location.type == ExplosionLocation::EXPLOSION_POOF)
    timeLeft = POOF_LIFE;
  else
    timeLeft = BOOM_LIFE;
}

void Explosion::draw (float elapsed) {
  timeLeft -= elapsed;

  if (location.type == ExplosionLocation::EXPLOSION_BOOM) {
    const float arg = (EXPLOSION_LIFE-timeLeft)/EXPLOSION_LIFE;
    const float size = BOOM_START_SIZE + (BOOM_END_SIZE-BOOM_START_SIZE)*arg;

    if (!fireExpl.isActive())
      fireExpl.start();
    fireExpl.draw(location.position, Vector2(size,size),elapsed);
    //boomSprite.draw(location.position, Vector2(size, size));
  } else {
    const float arg = (EXPLOSION_LIFE-timeLeft)/EXPLOSION_LIFE;
    const float size = POOF_START_SIZE + (POOF_END_SIZE-POOF_START_SIZE)*arg;
    poofSprite.draw(location.position, Vector2(size, size));
  }
}
