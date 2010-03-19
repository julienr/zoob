#include "Game.h"
#include "lib/Math.h"

void Game::update () {
  long now = Utils::getCurrentTimeMillis();
  //Do nothing if lastTime is in the future
  //This allows the game start (and unpause) to delay
  //the start of the physics by 100ms or whatever
  if (lastTime > now) {
    LOGE("lastTime(%li) > now(%li)", lastTime, now);
    return;
  }

  double elapsedS = (now-lastTime)/1000.0;
  lastTime = now;

  colManager.unmarkCollided();

  //Tank movement
  if (movingTank) {
    Vector2 dir = tankMoveEnd - tank.getPosition();
    dir.normalize();

    //Calculate base rotation (to face movement direction)
    //Dot product is e [0,pi], so we multiply by relative orientation of the vectors
    const float angle = acos(dir*Vector2::Y_AXIS) * Vector2::Y_AXIS.relativeOrientation(dir);
    tank.setRotation(angle);

    const Vector2 move = dir*TANK_MOVE_SPEED*elapsedS;

    //Collision detection
    CollisionResult r;

    if (colManager.trace(&tank, move, r)) {
      r.collidedEntity->collided = true;
      tank.collided  = true;
      tank.lastColNormal = r.normal;
      tank.lastColPoint = r.colPoint;
      //LOGE("tFirst: %f, tLast: %f, normal: (%f,%f) colPoint (%f,%f)", r.tFirst, r.tLast, r.normal.x, r.normal.y, r.colPoint.x, r.colPoint.y);
    }
    tank.translate(move);
  }
}
