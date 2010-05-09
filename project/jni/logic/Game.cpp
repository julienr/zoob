#include "Game.h"
#include "lib/Math.h"
#include "logic/enemies/ShieldTank.h"
#include "logic/enemies/SimpleTank.h"
#include "logic/enemies/StaticTank.h"
#include "logic/enemies/BurstTank.h"
#include "logic/enemies/BossSimpleTank.h"
#include "logic/enemies/BossShieldTank.h"
#include "ai/algorithms/AStar.h"
#include "Bomb.h"

Game::Game (game_callback_t overCallback, game_callback_t wonCallback, Level* level)
    : colManager(level->getWidth(), level->getHeight(), GRID_CELL_SIZE),
      tank(),
      level(level),
      elapsedS(0),
      gameOverCallback(overCallback),
      gameWonCallback(wonCallback),
      gameState(GAME_RUNNING),
      godMode(false),
      playerShadows(10),
      calculateShadows(true) {
  level->addToColManager(colManager);
  tank.setPosition(level->getStartPosition());
  colManager.addEntity(&tank);

  const TankDescription* tanks = level->getTanks();
  for (size_t i=1; i<level->getNumTanks(); i++) {
    const TankDescription& desc = tanks[i];
    EnemyTank* t = NULL;
    switch (desc.tankType) {
      case TANK_SIMPLE: t = desc.path?new SimpleTank(desc.path):new SimpleTank(); break;
      case TANK_SHIELD: t = desc.path?new ShieldTank(desc.path):new ShieldTank(); break;
      case TANK_BURST: t = desc.path?new BurstTank(desc.path):new BurstTank(); break;
      case TANK_STATIC: ASSERT(!desc.path); t = new StaticTank(); break;
      case BOSS_SHIELD: ASSERT(!desc.path); t = new BossShieldTank(); break;
      case BOSS_SIMPLE: ASSERT(desc.path); t = new BossSimpleTank(desc.path); break;
      case BOSS_BURST: ASSERT(false);
      default: ASSERT(false); break;
    }
    t->setPosition(Vector2(desc.x, desc.y));
    enemies.append(t);
    colManager.addEntity(t);
  }
  lastTime = Utils::getCurrentTimeMillis();
}

Game::~Game () {
  LIST_FOREACH(EnemyTank*, enemies, i)
    delete *i;
}

void Game::update () {
  uint64_t now = Utils::getCurrentTimeMillis();
  //Do nothing if lastTime is in the future
  //This allows the game start (and unpause) to delay
  //the start of the physics by 100ms or whatever
  if (lastTime > now) {
    //LOGE("lastTime(%lu) > now(%lu)", (unsigned long)lastTime, (unsigned long)now);
    return;
  }

  //FIXME: we don't really need max refresh rate..
  //FIXME: AI could be refreshed much less often
  if (lastTime - now < 50)
    return;

  if (gameState == GAME_PAUSED)
    return;

  explosions.clear();

  elapsedS = (now-lastTime)/1000.0;
  lastTime = now;

  if (Math::epsilonEq(elapsedS, 0))
    return;

  colManager.unmarkCollided();

  //Rockets
  for (list<Rocket*>::iterator i = rockets.begin(); i.hasNext(); ) {
    Rocket* r = *i;
    if (!r->hasExploded() && (r->getNumBounces() >= ROCKET_MAX_BOUNCES)) {
      r->explode(NULL, r->getPosition());
      explosions.append(r->getPosition());
    }
    //Might have exploded because of num bounces OR because of collision
    if (r->hasExploded()) {
      colManager.removeEntity(r);
      delete r;
      i = rockets.remove(i);
    } else {
      bounceMove(r, r->getDir()*ROCKET_MOVE_SPEED*elapsedS);
      i++;
    }
  }
  
  //Bombs
  for (list<Bomb*>::iterator i = bombs.begin(); i.hasNext(); ) {
    Bomb* m = *i;
    m->think(elapsedS);
    if (m->getTimeLeft() <= 0) {
      list<Entity*>* touchedEntities = colManager.getGrid().entitiesIn(m->getPosition(), BOMB_EXPLOSION_RADIUS);
      //touch all the entities in the explosion area
      for (list<Entity*>::iterator iter = touchedEntities->begin(); iter.hasNext(); iter++)
        touch(m, *iter, m->getPosition());

      m->explode(NULL, m->getPosition());
      delete touchedEntities;
      explosions.append(m->getPosition());
      //notify the owner
      m->getOwner()->bombExploded();
      delete m;
      i = bombs.remove(i);
    } else
      i++;
  }

  //Enemies
  int numAlive = 0;
  LIST_FOREACH(EnemyTank*, enemies, i) {
    EnemyTank* t = *i;
    if (!t->isAlive())
      continue;

    numAlive++;

    if (t->hasExploded()) {
      colManager.removeEntity(t);
      t->die();
    } else {
      t->think(elapsedS);
      TankAI* ai = t->getAI();
      if (ai) {
        Vector2 moveDir;
        if (ai->decideDir(elapsedS, &moveDir, this, t))
          doTankMove(t, moveDir, elapsedS);

        Vector2 rocketDir;
        //AI fire decision is done in two time :
        //- first, AI can decide to prepare to fire
        //- after a delay, decideFire is called again and if the ai decide to actually fire, the rocket is
        // created
        //- for some firing policy, the tank will then stay in "inFiring" mode (burst mode) for some time
        if (t->isFiring()) { //burst mode, let it fire without delay
          if (t->canFire()) {
            if (ai->decideFire(elapsedS, &rocketDir, this, t) && t->checkFireDir(rocketDir, colManager))
              rockets.append(t->fireRocket(rocketDir));
            else { //ai decided to stop firing, stop the burst
              LOGE("cancelFiring");
              t->cancelFiring();
            }
          }
        } else { //normal mode or between burst mode
          if (!t->isPreparingFire() && t->canFire() && ai->decideFire(elapsedS, &rocketDir, this, t))
            t->prepareFire();

          //the enemy is ready to fire
          if (t->fireReady() && ai->decideFire(elapsedS, &rocketDir, this, t) && t->checkFireDir(rocketDir, colManager)) {
            rockets.append(t->fireRocket(rocketDir));
          }
        }


        Vector2 aim;
        if (ai->aim(elapsedS, this, t, &aim))
          t->setRotationFromDir(aim.getNormalized());
      }
    }
  }

  //End of game ?
  if (numAlive == 0) {
    gameWonCallback();
  }

  if (!godMode && tank.hasExploded()) {
    tank.unmarkExploded();
    gameOverCallback();
  }

  //Player Tank movement
  if (!tankMoveDir.isZero()) {
    Vector2 dir = tankMoveDir;
    doTankMove(&tank, dir, elapsedS);
  }

  if (calculateShadows)
    _calculatePlayerShadows();
}

void Game::_calculatePlayerShadows () {
  playerShadows.clear();
  const Vector2& lightSource = tank.getPosition();
  const unsigned w = level->getWidth();
  const unsigned h = level->getHeight();
  for (unsigned x=0; x<w; x++) {
    for (unsigned y=0; y<h; y++) {
      //Don't calculate shadows for level border tiles
      if (x == 0 || y == 0 || x == w-1 || y == h-1)
        continue;

      const Tile* tile = level->getTile(x,y);
      if (!tile->getEntity())
        continue;
      ASSERT(tile->getEntity()->getBVolume()->getType() == TYPE_AABBOX);

      const AABBox* bbox = static_cast<const AABBox*>(tile->getEntity()->getBVolume());
      playerShadows.append(ShadowPolygon(lightSource, bbox, tile->getEntity()->getPosition()));
    }
  }
}

void Game::playerFire (const Vector2& cursorPosition) {
  //Fire up rocket
  Vector2 dir = cursorPosition-tank.getPosition();
  dir.normalize();
  if (tank.canFire() && tank.checkFireDir(dir, colManager)) {
    Rocket* r = tank.fireRocket(dir);
    colManager.addEntity(r);
    rockets.append(r);
  }
}

void Game::playerDropBomb()
{
  //Fire mine
  if (tank.canMine()) {
    //FIXME: check that there isn't already a mine here ?
    LOGE("drop bomb");
    Bomb* m = tank.dropBomb();
    bombs.append(m);
  }
}

void Game::doTankMove (Tank* t, Vector2 dir, double elapsedS) {
  if (dir.isZero())
    return;

  dir.normalize();
  t->setRotationFromDir(dir);

  const Vector2 move = dir*TANK_MOVE_SPEED*elapsedS;
  slideMove(t, move);
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

  e1->explode(e2, colPoint);
  e2->explode(e1, colPoint);

  explosions.append(colPoint);
}

void Game::bounceMove (Rocket* rocket, Vector2 move) {
  CollisionResult r;
  if (colManager.trace(rocket, move, &r)) {
    if (!r.collidedEntity->bounce(rocket, r.colPoint))
      touch(rocket, r.collidedEntity, r.colPoint);
    rocket->addBounce();
    move = -2.0f*(move*r.normal)*r.normal + move;
    rocket->setDir(move);
  }
  colManager.translate(rocket, move);
}

#define MAX_BOUNCES 4
void Game::slideMove (Entity* e, Vector2 move) {
  const Vector2 origMove = move;

  //Used to store the backoffs of previous iterations, so we don't go
  //against a backoff we've already taken into account
  Vector2 backoffs[MAX_BOUNCES];

  CollisionResult r;
  for (size_t bounces = 0; colManager.trace(e, move, &r) && (bounces < MAX_BOUNCES); bounces++) {
    touch(e, r.collidedEntity, r.colPoint);

    //LOGE("position(%f,%f)\tmove(%f,%f)\ttFirst %f\ttLast %f", e->getPosition().x, e->getPosition().y, move.x, move.y, r.tFirst, r.tLast);

    const Vector2 newPos = e->getPosition()+move;
    float backAmount = (r.colPoint - newPos)*r.normal;
    //multiplication by 1.1 is to have some safety margin. If we just add backoff, we might actually go inside the 
    //wall because of rounding errors or stuff. 
    Vector2 backoff = (backAmount*r.normal)*1.1;
    move += backoff;

    backoffs[bounces] = backoff;

    //Check that we aren't going against a previous backoff
    for (size_t j=0; j<=bounces; j++) {
      //if cos(angle between move and backoff) is in [0,1], we have an angle between
      //-90 and 90
      //cos = (vel*normals[i]/(||vel||*||normals[i]||)
      //but since we only care about the sign of the cos, we can forget division
      if (move*backoffs[j] >= 0)
        continue;

      //If we reach this point, we're going against backoffs[j], just add it once more to correct this
      move += backoffs[j];
      //LOGE("move+=backoffs[%u](%f,%f)", j, backoffs[j].x, backoffs[j].y);

      //Check that our new corrected move doesn't go AGAIN against another previous backoff
      for (size_t k=0; k<=bounces; k++) {
        if (k==j)
          continue;

        if (move*backoffs[k] >= 0)
          continue;

        //LOGE("Going against backoffs[%u] : unable to find a way out!", k);
        //We're going against 2 previous backoff, just cancel the move
        move.set(0,0);
        goto end;
      }
    }

  }
end:
  colManager.translate(e, move);
}
