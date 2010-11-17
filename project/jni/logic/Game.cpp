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
#include "logic/Trigger.h"

Game* Game::instance = NULL;

Game::Game (game_callback_t overCallback, game_callback_t wonCallback, Level* level)
    : constructed(false),
      accumulator(0),
      colManager(level->getWidth(), level->getHeight(), GRID_CELL_SIZE),
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
      pathFinder(colManager.getGrid()),
      introTimeLeft(BOSS_INTRO_TIME),
      introDone(!level->isBoss()) {
  //Put constructor stuff in _construct
}

void Game::_construct () {
  if (constructed)
    return;
  
  level->addToColManager(colManager);
  Vector2 playerStartPosition = Vector2(1,1);

  addTank(playerTank);

  spawnTanks(level, playerStartPosition);

  playerTank->setPosition(playerStartPosition);
  ProgressionManager::getInstance()->setPlayerForm(level, playerTank);
  colManager.addEntity(playerTank);

  constructed = true;
}

void Game::spawnTanks (const Level* level, Vector2& playerStartPosition) {
  bool foundPlayer = false;

  const TankDescription* tanks = level->getTanks();
  for (size_t i=0; i<level->getNumTanks(); i++) {
    const TankDescription& desc = tanks[i];
    EnemyTank* t = NULL;
    switch (desc.tankType) {
      //Special player case, don't create a tank yet for player
      case TANK_PLAYER: foundPlayer = true; playerStartPosition.set(desc.x, desc.y); break;

      case TANK_SIMPLE: t = new SimpleTank(desc.path); break;
      case TANK_BOUNCE: t = new BounceTank(desc.path); break;
      case TANK_SHIELD: t = new ShieldTank(desc.path); break;
      case TANK_BURST: t = new BurstTank(desc.path); break;
      case TANK_SPLIT: t = new SplitTank(desc.path); break;
      case TANK_STATIC: t = new StaticTank(); break;
      case BOSS_BOUNCE: t = new BossBounceTank(desc.path); break;
      case BOSS_SIMPLE: t = new BossSimpleTank(desc.path); break;
      case BOSS_SHIELD: t = new BossShieldTank(desc.path); break;
      case BOSS_BURST: t = new BossBurstTank(desc.path); break;
      case BOSS_SPLIT: t = new BossSplitTank(desc.path); break;
      default: LOGE("unknown tank type : %i", desc.tankType); ASSERT(false); break;
    }
    if (t != NULL) { //t will be NULL when we found player, to be handled below
      t->setPosition(Vector2(desc.x, desc.y));
      addTank(t);
      colManager.addEntity(t);
      calculateShadows |= t->getAI()->requirePlayerVisibility();
    }
  }

  if (!foundPlayer)
    LOGE("No player start position defined in level, falling back on default (1,1)");
}

Game::~Game () {
  LIST_FOREACH(Tank*, tanks, i) {
    delete *i;
  }
  LIST_FOREACH(Rocket*, rockets, r) {
    delete *r;
  }
  for (size_t i=0; i<playerShadows.length(); i++)
    delete playerShadows[i];
}

void Game::applyCommands (Tank* tank, const PlayerCommand& cmd) {
  if (cmd.fire)
    _tankFire(tank, cmd.fireDir);
  if (cmd.mine)
    _tankDropBomb(tank);
  if (cmd.shield)
    _tankActivateShield(tank);
  tank->setMoveDir(cmd.moveDir);
}

void Game::update (const double elapsedS) {
  this->elapsedS = elapsedS;
  if (gameState == GAME_PAUSED)
    return;

#ifdef DEBUG
  dbg_clear(); //clear debug information
#endif

  if (!introDone) {
    introTimeLeft -= elapsedS;
    if (introTimeLeft <= 0)
      introDone = true;
  }
  //rockets and tanks
  colManager.unmarkCollided();
  if (introDone) {
    _updateRockets(elapsedS);
    _updateBombs(elapsedS);
    const int numAlives = _updateTanks(elapsedS);
    if (numAlives == 0)
      gameWonCallback();
  }

  level->removeExplodedWalls(colManager);

  //player
  if (!godMode && playerTank->hasExploded()) {
    playerTank->unmarkExploded();
    gameOverCallback();
  }

  //triggers
  _handleTriggers(); 

  //shadows
  if (calculateShadows) {
    _calculatePlayerShadows();
    playerVisibility.calculateVisibility(this);
  }
}

void Game::_handleTriggers () {
  const set<Tile*> tiles = level->getTilesWithTrigger();
  for (set<Tile*>::const_iterator i=tiles.begin(); i.hasNext(); i++) {
    Tile* tile = *i;
    const list<const Trigger*>& triggers = tile->getTriggers();
    for (list<const Trigger*>::const_iterator j=triggers.begin(); j.hasNext(); j++) {
      const Trigger* trigger = *j;
      if (trigger->think(tile))
        level->removeTrigger(tile, trigger);
    }
  }
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
      i = deleteRocket(i);
      delete r;
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
    list<Entity*>* touchedEntities = colManager.getGrid().entitiesIn(m->getPosition(), BOMB_EXPLOSION_RADIUS, m->getOwner(), ENTITY_ROCKET | ENTITY_WALL | ENTITY_BOMB);
    //A bomb will always have at least a one second lifetime. This is to avoid direct explosions after drop
    const double minBombTime = BOMB_LIFETIME - 1;
    //A mine explodes when an enemy pass on it OR after its delay
    if (m->getTimeLeft() <= 0 || ((m->getTimeLeft() <= minBombTime) && !touchedEntities->empty())) {
      //touch all the entities in the explosion area
      for (list<Entity*>::iterator iter = touchedEntities->begin(); iter.hasNext(); iter++)
        touch(m, *iter, m->getPosition());

      m->explode(NULL, m->getPosition());
      delete touchedEntities;
      explosions.append(ExplosionLocation(m->getPosition(), ExplosionLocation::EXPLOSION_BOOM));
      //notify the owner
      m->getOwner()->bombExploded();
      i = deleteBomb(i);
      delete m;
    } else
      i++;
  }
}

int Game::_updateTanks (double elapsedS) {
  int numEnemiesAlive = 0;
  LIST_FOREACH(Tank*, tanks, i) {
    Tank* t = *i;
    if (!t->isAlive())
      continue;

    if (t->getTankCategory() != CAT_PLAYER)
      numEnemiesAlive++;

    if (t->hasExploded()) {
      colManager.removeEntity(t);
      t->die();
    } else {
      t->think(elapsedS);

      //Run AI
      if (t->getTankCategory() == CAT_AI) {
        EnemyTank* et = static_cast<EnemyTank*>(t);
        _doAI(et, elapsedS);
      }

      //Move
      if (!t->getMoveDir().isZero())
        doTankMove(t, elapsedS);
    }
  }
  return numEnemiesAlive;
}

void Game::_doAI (EnemyTank* tank, double elapsedS) {
  TankAI* ai = tank->getAI();
  if (!ai)
    return;

  Vector2 moveDir;
  if (ai->decideDir(elapsedS, &moveDir, this, tank))
    tank->setMoveDir(moveDir);
  else
    tank->setMoveDir(Vector2::ZERO);

  Vector2 rocketDir;
  //AI fire decision is done in two time :
  //- first, AI can decide to prepare to fire
  //- after a delay, decideFire is called again and if the ai decide to actually fire, the rocket is
  // created
  //- for some firing policy, the tank will then stay in "inFiring" mode (burst mode) for some time
  if (tank->isFiring()) { //burst mode, let it fire without delay
    if (tank->canFire()) {
      if (ai->confirmFire(elapsedS, &rocketDir, this, tank))
        doFireRocket(tank, rocketDir);
      else { //ai decided to stop firing, stop the burst
        LOGE("cancelFiring");
        tank->cancelFiring();
      }
    }
  } else { //normal mode or between burst mode
    if (!tank->isPreparingFire() && tank->canFire() && ai->decideFire(elapsedS,
          &rocketDir, this, tank))
      tank->prepareFire();

    //the enemy is ready to fire
    if (tank->fireReady() && ai->confirmFire(elapsedS, &rocketDir, this, tank)) {
      doFireRocket(tank, rocketDir);
    }
  }

  Vector2 aim;
  if (ai->aim(elapsedS, this, tank, &aim))
    tank->setRotationFromDir(aim.getNormalized());
}


void Game::doFireRocket (Tank* t, const Vector2& dir) {
  Rocket* r = t->fireRocket(dir);
  //if the rocket is fired into a wall, just display a poof
  CollisionResult res;
  if (!t->checkFireDir(dir, colManager, &res)) {
    touch(r, res.collidedEntity, res.colPoint);
    explosions.append(ExplosionLocation(r->getPosition(), ExplosionLocation::EXPLOSION_POOF));
    delete r;
  } else {
    addRocket(r);
    colManager.addEntity(r);
  }
}

void Game::addRocket (Rocket* r) {
  rockets.append(r);
  r->getOwner()->addRocket(r);
}

list<Rocket*>::iterator Game::deleteRocket (const list<Rocket*>::iterator& i) {
  Rocket* r = *i;
  r->getOwner()->removeRocket(r);
  return rockets.remove(i);
}

void Game::addTank (Tank* t) {
  LOGI("[Game] addTank");
  tanks.append(t);
}

list<Tank*>::iterator Game::deleteTank (const list<Tank*>::iterator& i) {
  return tanks.remove(i);
}

void Game::addBomb (Bomb* b) {
  bombs.append(b);
  b->getOwner()->addBomb(b);
}

list<Bomb*>::iterator Game::deleteBomb (const list<Bomb*>::iterator& i) {
  Bomb* b = *i;
  b->getOwner()->removeBomb(b);
  return bombs.remove(i);
}

#include "containers/pair.h"

typedef pair<Vector2, const AABBox* > Occluder;
struct DistToPlayerCmp : public Compare<Occluder> {
  DistToPlayerCmp (Vector2 playerPos)
    : playerPos(playerPos) {}

  virtual int operator () (const Occluder& t1, const Occluder& t2) const {
    const float d1 = (t1.first - playerPos).length();
    const float d2 = (t2.first - playerPos).length();
    if (d1 < d2) return -1;
    else if (Math::epsilonEq(d1, d2)) return 0;
    else return 1;
  }

  Vector2 playerPos;
};

void Game::_calculatePlayerShadows () {
  for (size_t i=0; i<playerShadows.length(); i++)
    delete playerShadows[i];
  playerShadows.clear();

  const Vector2& lightSource = playerTank->getPosition();

  //First step is we find all potential occluders and keep them in a vector
  vector<Occluder> occluders(10);

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
      occluders.append(Occluder(tile->getEntity()->getPosition(), bbox));
      //playerShadows.append(new ShadowPolygon(lightSource, bbox, tile->getEntity()->getPosition()));
    }
  }

  //Second step is sorting the occluders vector based on the distance to the player
  //The idea is that the occluders closer to the player will create bigger shadows
  //and will therefore be more likely to include other shadows (see step 3)
  occluders.sort(DistToPlayerCmp(lightSource));

  //Third step is actually creating the shadows. For each newly created shadow, we check
  //if it isn't already contained in a previous shadow. We cannot check all shadows against each
  //other because this would be too costly, but since the biggest shadows are created first, this
  //works not too bad
  //int numCovered = 0;
  const size_t len = occluders.size();
  for (size_t i=0; i<len; i++) {
    const Occluder& occ = occluders[i];
    const Vector2& position = occ.first;
    const AABBox* bbox = occ.second;

    //check if it's covered by previous shadow
    bool covered = false;
    for (size_t j=0; j<playerShadows.size(); j++) {
      if (playerShadows[j]->inside(position, *bbox)) {
        covered = true;
        //numCovered++;
        break;
      }
    }

    if (!covered) {
      playerShadows.append(new ShadowPolygon(lightSource, bbox, position));
    }
  }
  //LOGE("shadow casting : %i occluders, %i discarded because already covered", occluders.size(), numCovered);
}

void Game::_tankFire (Tank* tank, const Vector2& cursorPosition) {
  //Fire up rocket
  Vector2 dir = cursorPosition-tank->getPosition();
  dir.normalize();
  if (tank->canFire()) {
    doFireRocket(tank, dir);
  }
}

void Game::_tankDropBomb(Tank* tank)
{
  //Fire mine
  if (tank->canMine()) {
    //FIXME: check that there isn't already a bomb here ?
    LOGE("drop bomb");
    Bomb* m = tank->dropBomb();
    addBomb(m);
  }
}

void Game::doTankMove (Tank* t, double elapsedS) {
  Vector2 dir = t->getMoveDir();
  if (dir.isZero())
    return;

  dir.normalize();
  t->setRotationFromDir(dir);

  const Vector2 move = dir*t->getSpeed()*elapsedS;
  slideMove(t, move);
}

void Game::touch (Entity* e1, Entity* e2, const Vector2& colPoint) {
  const eEntityType t1 = e1->getType();
  const eEntityType t2 = e2->getType();
  //tank-wall and tank-tank
  /*if ((t1 == ENTITY_TANK || t1 == ENTITY_WALL) &&
      (t2 == ENTITY_TANK || t2 == ENTITY_WALL))
    return;*/

  if (!e1->acceptsTouch(e2) || !e2->acceptsTouch(e1))
    return;

  //rocket-wall
  /*if ((t1 == ENTITY_ROCKET && t2 == ENTITY_WALL) ||
      (t2 == ENTITY_ROCKET && t1 == ENTITY_WALL))
    return;*/

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
        addRocket(newRocket);
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

//Heavily inspired by Q3's PM_SlideMove
//see http://www.soclose.de/q3doc/bg__slidemove_8c-source.htm
//for a discussion : http://www.gamedev.net/community/forums/topic.asp?topic_id=532258
#define MAX_BOUNCES 4

#define OVERCLIP 1.001f

void Game::slideMove(Entity* e, Vector2 move) {
  //Used to store the backoffs of previous iterations, so we don't go
  //against a backoff we've already taken into account
  Vector2 backoffs[MAX_BOUNCES];

  //avoid going back against original velocity
  backoffs[0] = move.getNormalized();

  CollisionResult r;
  for (size_t bounces=1; bounces < MAX_BOUNCES; bounces++) {
    if (!colManager.trace(e, move, &r))
      break;

    //touch management
    if (!e->bounce(r.collidedEntity, r.colPoint))
      touch(e, r.collidedEntity, r.colPoint);

    if (r.tFirst == 0) {
      //Probably stuck, restore last safe position
      colManager.moveTo(e, e->getLastSafePosition());
      return;
    }

    //move entity as close as possible to collider
    colManager.translate(e, (r.colPoint-e->getPosition())*0.9);

    //now that we have moved the entity, we have "used" part (r.tFirst) of the move
    move *= (1-r.tFirst);
    move -= r.normal*(move*r.normal)*OVERCLIP; //remove all components of the velociyt along the collision normal

    backoffs[bounces] = r.normal;

    //Check that we aren't going against a previous backoff
    for (size_t j = 0; j < bounces; j++) {
      //if cos(angle between move and backoff) is in [0,1], we have an angle between
      //-90 and 90
      //cos = (vel*normals[i]/(||vel||*||normals[i]||)
      //but since we only care about the sign of the cos, we can forget division
      if (move * backoffs[j] >= 0)
        continue;

      //If we reach this point, we're going against backoffs[j], just add it once more to correct this
      //move += backoffs[j];
      move -= backoffs[j]*(move*backoffs[j])*OVERCLIP;
      //LOGE("move+=backoffs[%u](%f,%f)", j, backoffs[j].x, backoffs[j].y);

      //Check that our new corrected move doesn't go AGAIN against another previous backoff
      for (size_t k = 0; k < bounces; k++) {
        if (k == j)
          continue;

        if (move * backoffs[k] >= 0)
          continue;

        //LOGE("Going against backoffs[%u] : unable to find a way out!", k);
        //We're going against 2 previous backoff, just cancel the move
        move.set(0, 0);
        return;
      }
    }
  }
  colManager.translate(e, move);
  e->saveSafePosition();
}

#ifdef DEBUG
void Game::dbg_clear () {
  dbg_overlays.clear();

  LIST_FOREACH(DebugPath*, dbg_paths, i) {
    delete (*i);
  }
  dbg_paths.clear();

  dbg_polys.clear();
}
#endif

#if 0
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

    //FIXME: should check if r.tFirst == 0, which would mean we're INSIDE an obstacle. What should we do in this case (probably just zero out the move)
    //or push in someway that gets us out of the obstacle

    //LOGE("position(%f,%f)\tmove(%f,%f)\ttFirst %f\ttLast %f", e->getPosition().x, e->getPosition().y, move.x, move.y, r.tFirst, r.tLast);

    const Vector2 newPos = e->getPosition()+move;
    float backAmount = (r.colPoint - newPos)*r.normal;
    //multiplication by 1.1 is to have some safety margin. If we just add backoff, we might actually go inside the 
    //wall because of rounding errors or stuff. 
    //Vector2 backoff = (backAmount*r.normal)*1.1;
    Vector2 backoff = (backAmount*r.normal);
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
#endif
