#include "Game.h"
#include "lib/Math.h"

void Game::update () {
  long now = Utils::getCurrentTimeMillis();
  //Do nothing if lastTime is in the future
  //This allows the game start (and unpause) to delay
  //the start of the physics by 100ms or whatever
  if (lastTime > now) {
    LOGE("lastTime(%i) > now(%i)", lastTime, now);
    return;
  }

  double elapsedS = (now-lastTime)/1000.0;
  lastTime = now;

  //FIXME:
  //colManager.unmarkCollided();

  //Tank movement
  if (movingTank) {
    Vector2 dir = tankMoveEnd - tank.getPosition();
    dir.normalize();

    //Calculate base rotation (to face movement direction)
    //Dot product is e [0,pi], so we multiply by relative orientation of the vectors
    const float angle = acos(dir*Vector2::Y_AXIS) * Vector2::Y_AXIS.relativeOrientation(dir);
    tank.setRotation(angle);

    const Vector2 moveEnd = dir*TANK_MOVE_SPEED*elapsedS;
    //FIXME: Collision detection
    tank.translate(moveEnd);
  }
}
