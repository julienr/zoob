#include "Game.h"
#include "lib/Math.h"
#include "logic/enemies/BounceTank.h"
#include "logic/enemies/SimpleTank.h"
#include "logic/enemies/StaticTank.h"
#include "logic/enemies/BurstTank.h"
#include "logic/enemies/SplitTank.h"
#include "logic/enemies/BossSimpleTank.h"
#include "logic/enemies/BossShieldTank.h"
#include "logic/enemies/BossBounceTank.h"
#include "logic/enemies/BossBurstTank.h"
#include "logic/enemies/BossSplitTank.h"
#include "ai/algorithms/AStar.h"
#include "Bomb.h"
#include "lib/TimerManager.h"

Game* Game::instance = NULL;

Game::Game (game_callback_t overCallback, game_callback_t wonCallback, Level* level)
    : colManager(level->getWidth(), level->getHeight(), GRID_CELL_SIZE),
      playerTank(new PlayerTank()),
      level(level),
      elapsedS(0),
      gameOverCallback(overCallback),
      gameWonCallback(wonCallback),
      gameState(GAME_RUNNING),
      godMode(false),
      playerShadows(10),
      calculateShadows(level->hasShadows()),
      playerVisibility(colManager.getGrid()),
      introTimeLeft(BOSS_INTRO_TIME),
      introDone(!level->isBoss()) {
  level->addToColManager(colManager);
  playerTank->setPosition(level->getStartPosition());
  colManager.addEntity(playerTank);

  const TankDescription* tanks = level->getTanks();
  for (size_t i=1; i<level->getNumTanks(); i++) {
    const TankDescription& desc = tanks[i];
    EnemyTank* t = NULL;
    switch (desc.tankType) {
      case TANK_SIMPLE: t = desc.path?new SimpleTank(desc.path):new SimpleTank(); break;
      case TANK_BOUNCE: t = desc.path?new BounceTank(desc.path):new BounceTank(); break;
      case TANK_SHIELD: t = desc.path?new ShieldTank(desc.path):new ShieldTank(); break;
      case TANK_BURST: t = desc.path?new BurstTank(desc.path):new BurstTank(); break;
      case TANK_SPLIT: t = desc.path?new SplitTank(desc.path):new SplitTank(); break;
      case TANK_STATIC: ASSERT(!desc.path); t = new StaticTank(); break;
      case BOSS_BOUNCE: ASSERT(!desc.path); t = new BossBounceTank(); break;
      case BOSS_SIMPLE: ASSERT(!desc.path); t = new BossSimpleTank(); break;
      case BOSS_SHIELD: ASSERT(!desc.path); t = new BossShieldTank(); break;
      case BOSS_BURST: ASSERT(!desc.path); t = new BossBurstTank(); break;
      case BOSS_SPLIT: ASSERT(!desc.path); t = new BossSplitTank(); break;
      default: LOGE("unknown tank type : %i", desc.tankType); ASSERT(false); break;
    }
    t->setPosition(Vector2(desc.x, desc.y));
    enemies.append(t);
    colManager.addEntity(t);
  }
}

Game::~Game () {
  delete playerTank;
  LIST_FOREACH(EnemyTank*, enemies, i)
    delete *i;
  for (size_t i=0; i<playerShadows.length(); i++)
    delete playerShadows[i];
}

void Game::update (const double elapsedS) {
  this->elapsedS = elapsedS;
  if (gameState == GAME_PAUSED)
    return;

  if (!introDone) {
    introTimeLeft -= elapsedS;
    if (introTimeLeft <= 0)
      introDone = true;
  }

  colManager.unmarkCollided();
  if (introDone) {
    _updateRockets(elapsedS);
    _updateBombs(elapsedS);
    const int numAlives = _updateEnemies(elapsedS);
    if (numAlives == 0)
      gameWonCallback();
  }

  if (!godMode && playerTank->hasExploded()) {
    playerTank->unmarkExploded();
    gameOverCallback();
  }

  _updatePlayer(elapsedS);

  if (calculateShadows) {
    _calculatePlayerShadows();
    playerVisibility.calculateVisibility(this);
  }

  //FIXME: remove
  /*LIST_FOREACH(EnemyTank*, enemies, i) {
    EnemyTank* t = *i;
    if (!t->isAlive())
      continue;

    for (size_t i=0; i<playerShadows.length(); i++) {
      if (playerShadows[i]->fullyInside(t->getPosition(), t->getBVolume()->getWidth()/2.0f))
        LOGE("EnemyTank %p in shadow %i", t, i);
    }
  }*/
}

void Game::_updateRockets (double elapsedS) {
  //Rockets
  for (list<Rocket*>::iterator i = rockets.begin(); i.hasNext(); ) {
    Rocket* r = *i;
    r->think(elapsedS);
    if (!r->hasExploded() && (r->getNumBounces() >= ROCKET_MAX_BOUNCES)) {
      r->explode(NULL, r->getPosition());
      explosions.append(ExplosionLocation(r->getPosition(), ExplosionLocation::EXPLOSION_POOF));
    }
    //Might have exploded because of num bounces OR because of collision
    if (r->hasExploded()) {
      colManager.removeEntity(r);
      delete r;
      i = rockets.remove(i);
    } else {
      bounceMove(r, r->getDir()*r->getSpeed()*elapsedS);
      i++;
    }
  }
}

void Game::_updateBombs (double elapsedS) {
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
      explosions.append(ExplosionLocation(m->getPosition(), ExplosionLocation::EXPLOSION_BOOM));
      //notify the owner
      m->getOwner()->bombExploded();
      delete m;
      i = bombs.remove(i);
    } else
      i++;
  }
}

int Game::_updateEnemies (double elapsedS) {
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
            if (ai->confirmFire(elapsedS, &rocketDir, this, t))
              doFireRocket(t, rocketDir);
            else { //ai decided to stop firing, stop the burst
              LOGE("cancelFiring");
              t->cancelFiring();
            }
          }
        } else { //normal mode or between burst mode
          if (!t->isPreparingFire() && t->canFire() && ai->decideFire(elapsedS,
              &rocketDir, this, t))
            t->prepareFire();

          //the enemy is ready to fire
          if (t->fireReady() && ai->confirmFire(elapsedS, &rocketDir, this, t)) {
            doFireRocket(t, rocketDir);
          }
        }

        Vector2 aim;
        if (ai->aim(elapsedS, this, t, &aim))
          t->setRotationFromDir(aim.getNormalized());
      }
    }
  }
  return numAlive;
}

void Game::_updatePlayer (double elapsedS) {
  //Player Tank movement
  if (!tankMoveDir.isZero()) {
    Vector2 dir = tankMoveDir;
    doTankMove(playerTank, dir, elapsedS);
  }
}

void Game::doFireRocket (Tank* t, const Vector2& dir) {
  Rocket* r = t->fireRocket(dir);
  //if the rocket is fired into a wall, just display a poof
  CollisionResult res;
  if (!t->checkFireDir(dir, colManager, &res)) {
    touch(r, res.collidedEntity, res.colPoint);
    delete r;
  } else {
    rockets.append(r);
    colManager.addEntity(r);
  }
}

void Game::_calculatePlayerShadows () {
  for (size_t i=0; i<playerShadows.length(); i++)
    delete playerShadows[i];
  playerShadows.clear();
  const Vector2& lightSource = playerTank->getPosition();
  const unsigned w = level->getWidth();
  const unsigned h = level->getHeight();
  for (unsigned x=0; x<w; x++) {
    for (unsigned y=0; y<h; y++) {
      const Tile* tile = level->getTile(x,y);
      const eTileType type = tile->getType();


      /** Discard border tiles that cannot cast shadows.
       * For example, if we are on the right border, only R tiles cannot
       * cast shadows, but a W tile will cast (albeit small) one
       */
      if ((y == 0 && type == T) ||
          (y == h-1 && type == B) ||
          (x == 0 && type == L) ||
          (x == w-1 && type == R))
        continue;

      if (!tile->getEntity())
        continue;
      ASSERT(tile->getEntity()->getBVolume()->getType() == TYPE_AABBOX);

      const AABBox* bbox = static_cast<const AABBox*>(tile->getEntity()->getBVolume());
      playerShadows.append(new ShadowPolygon(lightSource, bbox, tile->getEntity()->getPosition()));
    }
  }
}

void Game::playerFire (const Vector2& cursorPosition) {
  //Fire up rocket
  Vector2 dir = cursorPosition-playerTank->getPosition();
  dir.normalize();
  if (playerTank->canFire()) {
    doFireRocket(playerTank, dir);
  }
}

void Game::playerDropBomb()
{
  //Fire mine
  if (playerTank->canMine()) {
    //FIXME: check that there isn't already a mine here ?
    LOGE("drop bomb");
    Bomb* m = playerTank->dropBomb();
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

  //Notify entities about the explosion
  const bool effect1 = e1->explode(e2, colPoint);
  const bool effect2 = e2->explode(e1, colPoint);

  //hasEffect is used to determine if the explosion had any effect.
  //We don't take the effect on the rocket into account (otherwise we'll obviously always have effects).
  const bool hasEffect = ((t1 != ENTITY_ROCKET) && effect1) || ((t2 != ENTITY_ROCKET) && effect2);

  explosions.append(ExplosionLocation(colPoint, hasEffect?ExplosionLocation::EXPLOSION_BOOM:ExplosionLocation::EXPLOSION_POOF));
}

void Game::bounceMove (Rocket* rocket, Vector2 move) {
  CollisionResult r;
  //FIXME: Split rockets should split instead of bouncing
  if (colManager.trace(rocket, move, &r)) {
    if (!rocket->canBounce() || !r.collidedEntity->bounce(rocket, r.colPoint)) {
      //No bounce, this rocket will gets destroyed
      touch(rocket, r.collidedEntity, r.colPoint);
      return;
    } else {
      //Bounce, check what to do based on the rocket's bounce policy
      if (rocket->getBouncePolicy() == BOUNCE) {
        rocket->addBounce();
        move = -2.0f*(move*r.normal)*r.normal + move;
        rocket->setDir(move);
      } else { //SPLIT
        rocket->addBounce();
        Rocket* newRocket = new Rocket(rocket);
        rockets.append(newRocket);
        colManager.addEntity(newRocket);
        //Now, the idea is to have the two rockets going in opposite directions each
        //30 degrees away from the normal bounce direction
        const Vector2 bounceDir = -2.0f*(move*r.normal)*r.normal + move;
        //Calculate our two new rockets movements
        move = bounceDir.getRotated(DEG_TO_RAD(30));
        Vector2 newMove = bounceDir.getRotated(DEG_TO_RAD(-30));
        //Now, to await direct collision after split, we have to move the new rockets slightly
        //away from each other. We'll therefore move them orthogonaly to the collision normal
        Vector2 normalOrtho(r.normal.y, -r.normal.x);

        //When moving the 2 rockets away from each other, just make sure they won't collide because
        //their next moves will cross
        const float t1 = normalOrtho*move;
        const float t2 = normalOrtho*newMove;
        if (t1 > t2) {
          colManager.translate(rocket, ROCKET_BCIRCLE_R*normalOrtho);
          colManager.translate(newRocket, -ROCKET_BCIRCLE_R*normalOrtho);
        } else {
          colManager.translate(rocket, -ROCKET_BCIRCLE_R*normalOrtho);
          colManager.translate(newRocket, ROCKET_BCIRCLE_R*normalOrtho);
        }

        rocket->setDir(move);
        newRocket->setDir(newMove);
        colManager.translate(newRocket, newMove);
      }
    }
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
    //If we are a bouncing tank, we shouldnt trigger a touch on the rocket we might bounce. Let the
    //rocket movement code handle that
    if (!e->bounce(r.collidedEntity, r.colPoint))
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
