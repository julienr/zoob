#include "Game.h"
#include "lib/Math.h"

void Game::update () {
  uint64_t now = Utils::getCurrentTimeMillis();
  //Do nothing if lastTime is in the future
  //This allows the game start (and unpause) to delay
  //the start of the physics by 100ms or whatever
  if (lastTime > now) {
    LOGE("lastTime(%lu) > now(%lu)", (unsigned long)lastTime, (unsigned long)now);
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
    slideRotate(&tank, angle);
    //tank.setRotation(angle);

    const Vector2 move = dir*TANK_MOVE_SPEED*elapsedS;

    //Collision detection
    CollisionResult r;

    if (colManager.trace(&tank, move, &r)) {
      r.collidedEntity->collided = true;
      tank.collided  = true;
      tank.lastColNormal = r.normal;
      tank.lastColPoint = r.colPoint;
      //LOGE("tFirst: %f, tLast: %f, normal: (%f,%f) colPoint (%f,%f)", r.tFirst, r.tLast, r.normal.x, r.normal.y, r.colPoint.x, r.colPoint.y);
    }
    slideMove(&tank, move);
    //tank.translate(move);
  }
}

Vector2 clipVelocity (const Vector2& in, const Vector2& normal, float overbounce=1.01f) {
  float backoff = in*normal;
  if (backoff < 0)
    backoff *= overbounce;
  else
    backoff /= overbounce;
  return in-(normal*backoff);
}

void Game::slideRotate (Entity* e, float rotation) {
  e->setRotation(rotation);
}

#define MAX_BOUNCES 4
void Game::slideMove (Entity* e, Vector2 move) {
  CollisionResult r;
  unsigned bounces = 0;
  for (; colManager.trace(e, move, &r) && (bounces < MAX_BOUNCES); bounces++) {
    const Vector2 newPos = e->getPosition()+move;
    float backAmount = (r.colPoint - newPos)*r.normal;
    Vector2 backoff = backAmount*r.normal;
    /*LOGE("collision : normal(%f,%f), backoff(%f,%f), backAmount : %f", r.normal.x, r.normal.y, backoff.x, backoff.y, backAmount);
    LOGE("move (%f,%f)", move.x, move.y);*/
    move += backoff*1.1;
    //LOGE("move after backoff (%f,%f)", move.x, move.y);
  }
  e->translate(move);
}
