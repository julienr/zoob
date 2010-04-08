#include "Game.h"
#include "lib/Math.h"

Game::Game (Level* level)
    : colManager(level->getWidth(), level->getHeight(), 1.0f), tank(GREY), enemies(5), level(level), elapsedS(0), movingState(MOVING_NONE) {
  level->addToColManager(colManager);
  tank.setPosition(level->getStartPosition(0));
  for (size_t i=1; i<level->getNumStartPositions(); i++) {
    Tank* t = new Tank(RED);
    t->setPosition(level->getStartPosition(i));
    enemies.add(t);
    colManager.addEntity(t);
  }
  colManager.addEntity(&tank);

  lastTime = Utils::getCurrentTimeMillis();
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

  explosions.clear();

  elapsedS = (now-lastTime)/1000.0;
  lastTime = now;

  if (Math::epsilonEq(elapsedS, 0))
    return;

  colManager.unmarkCollided();

  //Rockets
  for (list<Rocket*>::iterator i = rockets.begin(); i.hasNext(); ) {
    Rocket* r = *i;
    if (!r->hasExploded() && (r->getNumBounces() > 3))
      r->explode();
    //Might have exploded because of num bounces OR because of collision
    if (r->hasExploded()) {
      colManager.removeEntity(*i);
      delete (*i);
      i = rockets.remove(i);
    } else {
      bounceMove(r, r->getDir()*ROCKET_MOVE_SPEED*elapsedS);
      i++;
    }
  }

  //Tank movement
  if (isMovingTank()) {
    Vector2 dir = getTankMoveDir();

    if (dir.length() != 0) {
      dir.normalize();

      CollisionResult tmpR;
      //colManager.getGrid().trace(static_cast<const BCircle*>(tank.getBVolume()), tankMoveEnd-tank.getPosition(), &tmpR);

      //Calculate base rotation (to face movement direction)
      //Dot product is e [0,pi], so we multiply by relative orientation of the vectors
      const float angle = acos(dir*Vector2::Y_AXIS) * Vector2::Y_AXIS.relativeOrientation(dir);
      tank.setRotation(angle);

      const Vector2 move = dir*TANK_MOVE_SPEED*elapsedS;

      //Collision detection
      /*CollisionResult r;

      if (colManager.trace(&tank, move, &r)) {
        r.collidedEntity->collided = true;
        tank.collided  = true;
        tank.lastColNormal = r.normal;
        tank.lastColPoint = r.colPoint;
        //LOGE("tFirst: %f, tLast: %f, normal: (%f,%f) colPoint (%f,%f)", r.tFirst, r.tLast, r.normal.x, r.normal.y, r.colPoint.x, r.colPoint.y);
      }*/
      slideMove(&tank, move);
      //tank.translate(move);
    }
  }
}

void Game::startMoving (eMoveState what, const Vector2& touchPosition) {
  movingState = what;
  switch(movingState) {
    case MOVING_TANK:
      tankMoveEnd = touchPosition;
      break;
    case MOVING_TANK_PAD:
      padMoveStart = touchPosition;
      tankMoveEnd = touchPosition;
      break;
    case MOVING_CURSOR:
      cursor.setPosition(touchPosition);
      break;
    default:
      break;
  }
}

void Game::setMoveTouchPoint (const Vector2& pos) {
  switch (movingState) {
    case MOVING_TANK:
    case MOVING_TANK_PAD:
      tankMoveEnd = pos;
      break;
    case MOVING_CURSOR:
      cursor.setPosition(pos);
      break;
    default:
      break;
  }
}

void Game::stopMoving () {
  if (movingState == MOVING_CURSOR) {
    //Fire up rocket
    Vector2 dir = cursor.getPosition()-tank.getPosition();
    dir.normalize();
    rockets.append(tank.fireRocket(dir));
  }
  movingState = MOVING_NONE;
}

void Game::touch (Entity* e1, Entity* e2, const Vector2& colPoint) {
  const eEntityType t1 = e1->getType();
  const eEntityType t2 = e2->getType();
  //tank-wall and tank-tank
  if ((t1 == ENTITY_TANK || t1 == ENTITY_WALL) &&
      (t2 == ENTITY_TANK || t2 == ENTITY_WALL))
    return;
  //rocket-wall
  if ((t1 == ENTITY_ROCKET && t2 == ENTITY_WALL) ||
      (t2 == ENTITY_ROCKET && t1 == ENTITY_WALL))
    return;
  e1->explode();
  e2->explode();

  explosions.append(colPoint);
}

void Game::bounceMove (Rocket* rocket, Vector2 move) {
  CollisionResult r;
  if (colManager.trace(rocket, move, &r)) {
    touch(rocket, r.collidedEntity, r.colPoint);
    rocket->addBounce();
    move = -2.0f*(move*r.normal)*r.normal + move;
    rocket->setDir(move);
  }
  colManager.translate(rocket, move);
}

#define MAX_BOUNCES 4
void Game::slideMove (Entity* e, Vector2 move) {
  CollisionResult r;
  unsigned bounces = 0;
  for (; colManager.trace(e, move, &r) && (bounces < MAX_BOUNCES); bounces++) {
    touch(e, r.collidedEntity, r.colPoint);
    const Vector2 newPos = e->getPosition()+move;
    float backAmount = (r.colPoint - newPos)*r.normal;
    Vector2 backoff = backAmount*r.normal;
    move += backoff*1.1;
  }
  colManager.translate(e, move);
}
