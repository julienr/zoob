#include "logic/NetworkedGame.h"
#include "logic/NetTank.h"
#include "logic/Rocket.h"
#include "logic/Bomb.h"
#include "net/NetController.h"

void NetworkedGame::applyGameState (const zoobmsg::GameState& state) {
  //FIXME: should also REMOVE all rockets, tanks and bombs that are not in the update
  //anymore
  //We keep track of all the rockets, bombs and tanks that are not in the update
  //but still in our local game. We can use this information to detect some bad
  //network problem or disconnection
  set<uint16_t> orphanRockets;
  set<uint16_t> orphanBombs;
  set<uint16_t> orphanTanks;

  getRocketsID(orphanRockets);
  getBombsID(orphanBombs);
  getTanksID(orphanTanks);
  
  //The basic idea is that for each type (rocket, tank, bomb), we update
  //the entity with the same ID as in the message. If we have no entity with
  //this ID, we create a new one.
  LOGI("[applyGameState] %i tanks", state.numTankInfos);
  for (uint16_t i=0; i<state.numTankInfos; i++) {
    const zoobmsg::TankInfo& tinfo = state.tankInfos[i];
    Tank* tank;
    orphanTanks.remove(tinfo.tankID);
    if (!tanksByID.contains(tinfo.tankID)) {
      LOGI("[applyGameState] creating new NetTank");
      tank = new NetTank(newPlayerFirePolicy());
      tank->setID(tinfo.tankID);
      addTank(tank);
    } else {
      tank = tanksByID.get(tinfo.tankID);
    }
    setAuthoritativeTankPosition(tank, Vector2(tinfo.position.x, tinfo.position.y));

    LOGI("[applyGameState] tank %i pos=(%f,%f), %i rockets, %i bombs", tinfo.tankID, tinfo.position.x, tinfo.position.y, tinfo.numRocketInfos, tinfo.numBombInfos);

    for (uint16_t j=0; j<tinfo.numRocketInfos; j++) {
      const zoobmsg::RocketInfo& rinfo = tinfo.rocketInfos[j];
      const Vector2 pos(rinfo.position.x, rinfo.position.y);
      const Vector2 vel(rinfo.velocity.x, rinfo.velocity.y);
      orphanRockets.remove(rinfo.rocketID);
      if (!rocketsByID.contains(rinfo.rocketID)) {
        Rocket* rocket = new Rocket(tank, pos, vel, rinfo.speed);
        addRocket(rocket);
      } else {
        Rocket* rocket = rocketsByID.get(rinfo.rocketID);
        setAuthoritativeRocketPosition(rocket, pos);
        rocket->setDir(vel);
        rocket->setSpeed(rinfo.speed);
      }
    }

    for (uint16_t j=0; j<tinfo.numBombInfos; j++) {
      const zoobmsg::BombInfo& binfo = tinfo.bombInfos[j];
      const Vector2 pos(binfo.position.x, binfo.position.y);
      orphanBombs.remove(binfo.bombID);
      if (!bombsByID.contains(binfo.bombID)) {
        Bomb* bomb = new Bomb(tank, pos);
        bomb->setTimeLeft((double)binfo.timeleft);
      } else {
        Bomb* bomb = bombsByID.get(binfo.bombID);
        setAuthoritativeBombPosition(bomb, pos);
      }
    }
  }

  SET_FOREACH(uint16_t, orphanRockets, i) {
    LOGI("[NetworkedGame::applyGameState] rocket %d is orphan", *i);
  }

  SET_FOREACH(uint16_t, orphanBombs, i) {
    LOGI("[NetworkedGame::applyGameState] bomb %d is orphan", *i);
  }

  SET_FOREACH(uint16_t, orphanTanks, i) {
    LOGI("[NetworkedGame::applyGameState] tank %d is orphan", *i);
  }

  //Explosions
  LOGI("[applyGameState] %i explosions", state.numExplosions);
  for (uint16_t i=0; i<state.numExplosions; i++) {
    const zoobmsg::Explosion& expl = state.explosions[i];
    ExplosionInfo::eType type = expl.boom?ExplosionInfo::EXPLOSION_BOOM:ExplosionInfo::EXPLOSION_POOF;
    Vector2 explPos(expl.position.x, expl.position.y);

    ExplosionInfo explInfo (explPos, type);
    for (uint16_t j=0; j<expl.numDamages; j++) {
      const zoobmsg::Damage& damage = expl.damages[j];
      //FIXME: would require getEntityByID ()
      explInfo.damagedEntities.append(pair<Entity*, int>(getEntityByID(damage.entityID), damage.damages));

      applyDamages(damage.entityID, damage.damages);
    }
    //Apply it immediatly
    Game::explode(explInfo);
  }
}

void NetworkedGame::applyDamages (uint16_t entityID, int damages) {
  //TODO
  Entity* entity = getEntityByID(entityID);
  LOGI("[NetworkedGame::applyDamages] %i damages for entity (%p) with id %i", damages, entity, entityID);
}

Entity* NetworkedGame::getEntityByID (uint16_t id) {
  if (tanksByID.contains(id))
    return tanksByID.get(id);
  if (rocketsByID.contains(id))
    return rocketsByID.get(id);
  if (bombsByID.contains(id))
    return bombsByID.get(id);
  return NULL;
}

void NetworkedGame::applyCommands (uint16_t id, const PlayerCommand& cmd) {
  Tank* t = tanksByID.get(id);
  if (t == NULL) {
    LOGE("[NetworkedGame::applyCommands] id (%i) has no corresponding tank", id);
  } else {
    applyCommands(t, cmd);
  }
}

void NetworkedGame::explode (const ExplosionInfo& info) {
  NetController::getInstance()->sendExplosion(info);
  Game::explode(info);
}

void NetworkedGame::applyCommands (Tank* tank, const PlayerCommand& cmd) {
  NetController::getInstance()->sendPlayerCommand(tank->getID(), cmd);
  Game::applyCommands(tank, cmd);
}

void NetworkedGame::update(const double elapsedS) {
  NetController::getInstance()->update(this);
  Game::update(elapsedS);
}

void NetworkedGame::spawnTanks (const Level* level, Vector2& playerStartPosition) {
  //Only spawn if we are server
  if (!NetController::getInstance()->isClient()) {
    Game::spawnTanks(level, playerStartPosition);
  }
}

void NetworkedGame::spawnPlayer() {
  NetController::getInstance()->wantSpawn();
}


void NetworkedGame::touch (Entity* e1, Entity* e2, const Vector2& colPoint) {
  //If we are the client, we dont't perform ANY touch, just wait for the
  //server to send the damage/explosion/... events
  if (NetController::getInstance()->isClient())
    return;
  Game::touch(e1, e2, colPoint);
}

void NetworkedGame::multiTouch (Entity* source, const list<Entity*>& touched, const Vector2& colPoint) {
  if (NetController::getInstance()->isClient())
    return;
  Game::multiTouch(source, touched, colPoint);
}

void NetworkedGame::addRocket (Rocket* r) {
  Game::addRocket(r);
  rocketsByID.insert(r->getID(), r);
}

list<Rocket*>::iterator NetworkedGame::deleteRocket (const list<Rocket*>::iterator& i) {
  rocketsByID.remove((*i)->getID());
  return Game::deleteRocket(i);
}

void NetworkedGame::addTank (Tank* t) {
  LOGI("[NetworkedGame] addTank");
  NetController::getInstance()->assignID(t);
  LOGI("assigned id %i to tank %p", t->getID(), t);
  Game::addTank(t);
  tanksByID.insert(t->getID(), t);
}

list<Tank*>::iterator NetworkedGame::deleteTank (const list<Tank*>::iterator& i) {
  tanksByID.remove((*i)->getID());
  return Game::deleteTank(i);
}

void NetworkedGame::addBomb (Bomb* b) {
  Game::addBomb(b);
  bombsByID.insert(b->getID(), b);
}

list<Bomb*>::iterator NetworkedGame::deleteBomb (const list<Bomb*>::iterator& i) {
  bombsByID.remove((*i)->getID());
  return Game::deleteBomb(i);
}

bool NetworkedGame::isGameOver () const {
  /*if (NetController::getInstance()->isClient()) {
    return false;
  } else {
    return Game::isGameOver();
  }*/
  //FIXME: only for debugging. Should implement some real round management
  return false;
}

bool NetworkedGame::isGameWon (int numAlives) const {
  /*if (NetController::getInstance()->isClient()) {
    return false;
  } else {
    return Game::isGameWon(numAlives);
  }*/
  return false;
}