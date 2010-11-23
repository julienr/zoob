#include "NetTank.h"
#include "Game.h"

Rocket* NetTank::createRocket (Tank* owner, const Vector2& pos, const Vector2& dir) {
  return new Rocket(owner, pos, dir, Game::getInstance()->getPlayerRocketSpeed(), BOUNCE);
}
