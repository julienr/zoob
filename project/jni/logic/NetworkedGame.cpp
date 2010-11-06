#include "logic/NetworkedGame.h"
#include "logic/NetTank.h"
#include "logic/Rocket.h"
#include "logic/Bomb.h"
#include "net/NetController.h"

void NetworkedGame::applyGameState (const zoobmsg::GameState* state) {
  //FIXME: should also REMOVE all rockets, tanks and bombs that are not in the update
  //anymore
  
  //The basic idea is that for each type (rocket, tank, bomb), we update
  //the entity with the same ID as in the message. If we have no entity with
  //this ID, we create a new one.
  for (uint16_t i=0; i<state->numPlayerInfos; i++) {
    const zoobmsg::PlayerInfo& pinfo = state->playerInfos[i];
    Tank* tank;
    if (!tanksByID.contains(pinfo.playerID)) {
      tank = new NetTank();
      tank->setID(pinfo.playerID);
      addTank(tank);
    } else {
      tank = tanksByID.get(pinfo.playerID); 
      tank->setPosition(Vector2(pinfo.position.x, pinfo.position.y));
    }

    for (uint16_t j=0; j<pinfo.numRocketInfos; j++) {
      const zoobmsg::RocketInfo& rinfo = pinfo.rocketInfos[j];
      const Vector2 pos(rinfo.position.x, rinfo.position.y);
      const Vector2 vel(rinfo.velocity.x, rinfo.velocity.y);
      if (!rocketsByID.contains(rinfo.rocketID)) {
        Rocket* rocket = new Rocket(tank, pos, vel, rinfo.speed);
        addRocket(rocket);
      } else {
        Rocket* rocket = rocketsByID.get(rinfo.rocketID);
        rocket->setPosition(pos);
        rocket->setDir(vel);
        rocket->setSpeed(rinfo.speed);
      }
    }

    for (uint16_t j=0; j<pinfo.numBombInfos; j++) {
      const zoobmsg::BombInfo& binfo = pinfo.bombInfos[j];
      const Vector2 pos(binfo.position.x, binfo.position.y);
      if (!bombsByID.contains(binfo.bombID)) {
        Bomb* bomb = new Bomb(tank, pos);
        bomb->setTimeLeft((double)binfo.timeleft);
      } else {
        Bomb* bomb = bombsByID.get(binfo.bombID);
        bomb->setPosition(pos);
      }
    }
  }
}

void NetworkedGame::applyCommands (Tank* tank, const PlayerCommand& cmd) {
  NetController::getInstance()->sendPlayerCommand(tank->getID(), cmd);
  Game::applyCommands(tank, cmd);
}

void NetworkedGame::update(const double elapsedS) {
  NetController::getInstance()->update(this);
  Game::update(elapsedS);
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
