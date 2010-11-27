#include "Explosion.h"
#include "view/primitives/Circle.h"

#define BOOM_START_SIZE 0.5f
#define BOOM_END_SIZE 3.0f
#define BOOM_SIZE_INCR ((END_SIZE-START_SIZE)/EXPLOSION_LIFE)

#define POOF_START_SIZE 0.6f
#define POOF_END_SIZE 0.1f
#define POOF_SIZE_INCR ((POOF_END_SIZE-POOF_START_SIZE)/EXPLOSION_LIFE)

#define POOF_LIFE 0.5f
#define BOOM_LIFE 0.8f

Explosion::Explosion (const ExplosionInfo& loc)
  : /*boomSprite("assets/sprites/boom.png", TEX_GROUP_GAME),*/
    poofSprite("assets/sprites/rocket.png", TEX_GROUP_GAME),
    explLight("assets/sprites/expl_light.png", TEX_GROUP_GAME),
    fireExpl("assets/sprites/expl2.png", TEX_GROUP_GAME, 16, new Animation::LinearInterpolator(), BOOM_LIFE, false),
    location(loc) {
  if (location.type == ExplosionInfo::EXPLOSION_POOF)
    timeLeft = POOF_LIFE;
  else
    timeLeft = BOOM_LIFE;
}

void Explosion::drawLighting () {
  if (location.type == ExplosionInfo::EXPLOSION_BOOM) {
    const float arg = (BOOM_LIFE-timeLeft)/BOOM_LIFE;
    const float size = Math::sin(arg*M_PI);

    explLight.bind();
    glPushMatrix();
    GLW::translate(location.position);
    GLW::scale(4*size, 4*size, 1);
    Circle::draw(true);
    glPopMatrix();
  }
}

void Explosion::draw () {
  if (location.type == ExplosionInfo::EXPLOSION_BOOM) {
    const float arg = (BOOM_LIFE-timeLeft)/BOOM_LIFE;
    const float size = BOOM_START_SIZE + (BOOM_END_SIZE-BOOM_START_SIZE)*arg;

    if (!fireExpl.isActive())
      fireExpl.start();
    fireExpl.draw(location.position, Vector2(size,size),elapsed);
    //boomSprite.draw(location.position, Vector2(size, size));
  } else {
    const float arg = (POOF_LIFE-timeLeft)/POOF_LIFE;
    const float size = POOF_START_SIZE + (POOF_END_SIZE-POOF_START_SIZE)*arg;
    poofSprite.draw(location.position, Vector2(size, size));
  }
}
