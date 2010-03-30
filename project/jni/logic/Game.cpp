#include "Game.h"
#include "lib/Math.h"

Game::Game (Level* level)
    : colManager(level->getWidth(), level->getHeight(), 1.0f), enemies(5), level(level),movingTank(false) {
  level->addToColManager(colManager);
  tank.setPosition(level->getStartPosition(0));
  for (size_t i=1; i<level->getNumStartPositions(); i++) {
    Tank* t = new Tank();
    t->setPosition(level->getStartPosition(i));
    enemies.add(t);
    colManager.addEntity(t);
  }
  colManager.addEntity(&tank);
}

Game::~Game () {
  for (size_t i=0; i<enemies.length(); i++)
    delete enemies[i];
}

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

    CollisionResult tmpR;
    //colManager.getGrid().trace(static_cast<const BCircle*>(tank.getBVolume()), tankMoveEnd-tank.getPosition(), &tmpR);

    //Calculate base rotation (to face movement direction)
    //Dot product is e [0,pi], so we multiply by relative orientation of the vectors
    const float angle = acos(dir*Vector2::Y_AXIS) * Vector2::Y_AXIS.relativeOrientation(dir);
    tank.setRotation(angle);

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
  colManager.translate(e, move);
}
