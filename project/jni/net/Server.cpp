#include "Server.h"
#include "view/GameManager.h"
#include "levels/LevelsData.h"
#include "logic/Bomb.h"
#include "logic/Rocket.h"
#include "logic/NetTank.h"
#include "logic/PlayerCommand.h"

void Server::handleConnect (const uint16_t peerID) {
  LOGI("new client connected");
  connectedClients.insert(peerID);
}

void Server::handleMsgHello (const uint16_t peerID, size_t dataLen, const uint8_t* data, size_t offset) {
  zoobmsg::Hello hello;
  zoobmsg::Hello::unpack(dataLen, data, offset, hello);

  LOGI("[handleMsgHello] nickname=%s", (char*)hello.nickname.bytes);

  //Respond to hello by first sending version, then welcome message
  zoobmsg::Version version;
  version.version = PROTOCOL_VERSION;
  sendMsgVersion(peerID, version);

  zoobmsg::Welcome welcome;
  welcome.playerID = playerIDGen++;
  welcome.serverState = state;
  char* level = LevelManager::getInstance()->getLevel(GameManager::getInstance()->getCurrentLevel());
  size_t len = strlen(level)+1;
  welcome.level.numBytes = len;
  welcome.level.bytes = new char[len];
  strncpy(welcome.level.bytes, level, len);
  free(level);
  sendMsgWelcome(peerID, welcome);
}

void Server::wantSpawn () {
  spawnQueue.append(0);
}

void Server::handleMsgWantSpawn (const uint16_t peerID, size_t dataLen, const uint8_t* data, size_t offset) {
  zoobmsg::WantSpawn join;
  zoobmsg::WantSpawn::unpack(dataLen, data, offset, join);
  LOGI("[handleMsgWantSpawn] peerID=%i", peerID);
  spawnQueue.append(peerID);
}

void Server::handleMsgPlayerCommand (const uint16_t peerID, size_t dataLen, const uint8_t* data, size_t offset) {
  zoobmsg::PlayerCommands commands;
  zoobmsg::PlayerCommands::unpack(dataLen, data, offset, commands);
  PlayerCommand cmd;
  cmd.fire = commands.fire;
  cmd.fireDir.set(commands.fireDir.x, commands.fireDir.y);
  cmd.mine = commands.mine;
  cmd.moveDir.set(commands.moveDir.x, commands.moveDir.y);
  cmd.shield = commands.shield;
  //LOGI("[Server::handleMsgPlayerCommand] cmd.moveDir=(%f,%f), cmd.fire=%i", cmd.moveDir.x, cmd.moveDir.y, cmd.fire);
  Game::getInstance()->applyCommands(commands.tankID, cmd);
}

void Server::handleDisconnect (const uint16_t peerID) {
  connectedClients.remove(peerID); 
}

Tank* createNetTank () {
  return new NetTank(Game::getInstance()->newPlayerFirePolicy());
}

void Server::_doSpawns (NetworkedGame* game) {
  if (spawnQueue.empty())
    return;
  
  LOGI("[Server::_doSpawns] %ld requests in spawn queue", spawnQueue.size());
  for (list<uint16_t>::iterator i = spawnQueue.begin(); i.hasNext(); ) {
    const uint16_t peerID = *i;
    if (peerID == 0) { //special case for server spawn
      Vector2 spawnPos;
      if (game->findSpawnPosition(TANK_BCIRCLE_R, spawnPos)) {
        PlayerTank* pt = new PlayerTank(Game::getInstance()->newPlayerFirePolicy());
        pt->setPosition(spawnPos);
        game->playerSpawned(pt);
        i = spawnQueue.remove(i);
      } else {
        LOGI("[Server::_doSpawns] couldn't find a spawn position");
        i++;
      }
    } else {
      Tank* tank;
      if (game->spawnTank(TANK_BCIRCLE_R, &createNetTank, tank)) {
        //spawn successful => remove tank from spawn queue and send a spawn message to the client owning this tank
        zoobmsg::Spawn spawnMsg;
        spawnMsg.position.x = tank->getPosition().x;
        spawnMsg.position.y = tank->getPosition().y;
        spawnMsg.tankID = tank->getID();
        LOGI("[_doSpawns] spawned peerID=%d at (%f,%f)", peerID, spawnMsg.position.x, spawnMsg.position.y);
        sendMsgSpawn(peerID, spawnMsg);
        i = spawnQueue.remove(i);
      } else {
        LOGI("[Server::_doSpawns] couldn't find a spawn position");
        i++;
      }
    }
  }
}

#define BROADCAST_INTERVAL 500
void Server::update(NetworkedGame* game) {
  _doSpawns(game);
  
  const uint64_t now = Utils::getCurrentTimeMillis();
  if ((now - lastGameStateBroadcast) < BROADCAST_INTERVAL) {
    return;
  }
  lastGameStateBroadcast = now;

  //We want to read the GameState and broadcast it to clients
  zoobmsg::GameState state;
  const list<Tank*>* tanks = game->getTanks();

  state.numPlayerInfos = tanks->size();
  state.playerInfos = new zoobmsg::PlayerInfo[state.numPlayerInfos];

  int counter = 0;
  LIST_FOREACH_CONST(Tank*, (*tanks), i) {
    const Tank* tank = *i;

    zoobmsg::PlayerInfo* pinfo = &state.playerInfos[counter++];
    pinfo->playerID = tank->getID();
    //FIXME: something is wrong here
    LOGI("tank id : %i", pinfo->playerID);
    pinfo->position.x = tank->getPosition().x;
    pinfo->position.y = tank->getPosition().y;
    pinfo->velocity.x = tank->getMoveDir().x;
    pinfo->velocity.y = tank->getMoveDir().y;

    pinfo->numRocketInfos = tank->getNumRockets();
    pinfo->rocketInfos = new zoobmsg::RocketInfo[pinfo->numRocketInfos];

    pinfo->numBombInfos = tank->getNumBombs();
    pinfo->bombInfos = new zoobmsg::BombInfo[pinfo->numBombInfos];

    int rCnt = 0;
    for (set<Rocket*>::const_iterator ri=tank->getRockets(); ri.hasNext(); ri++) {
      const Rocket* rocket = *ri;
      zoobmsg::RocketInfo& rInfo = pinfo->rocketInfos[rCnt++];
      rInfo.position.x = rocket->getPosition().x;
      rInfo.position.y = rocket->getPosition().y;
      rInfo.velocity.x = rocket->getDir().x;
      rInfo.velocity.y = rocket->getDir().y;
      rInfo.speed = rocket->getSpeed();
      rInfo.rocketID = rocket->getID();
    }

    int bCnt = 0;
    for (set<Bomb*>::const_iterator bi=tank->getBombs(); bi.hasNext(); bi++) {
      const Bomb* bomb = *bi;
      zoobmsg::BombInfo& bInfo = pinfo->bombInfos[bCnt++];
      bInfo.position.x = bomb->getPosition().x;
      bInfo.position.y = bomb->getPosition().y;
      bInfo.timeleft = bomb->getTimeLeft();
      bInfo.bombID = bomb->getID();
    }
  }

  //Broadcast to connected clients
  LOGI("broadcasting to %li clients", connectedClients.size());
  for (set<uint16_t>::iterator i=connectedClients.begin(); i.hasNext(); i++) {
    const uint16_t peerID = *i;
    sendMsgGameState(peerID, state);
  }
}

void Server::sendPlayerCommand (uint16_t localPlayerID, const PlayerCommand& cmd) {
  //We don't have to do anything here, local commands are already handled by Game
  //and local Game simulation is authoritative
}




