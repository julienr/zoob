#include "Client.h"
#include "enet/enet.h"
#include "logic/Game.h"

void Client::handleMsgWelcome (size_t dataLen, const uint8_t* data, size_t offset) {
  zoobmsg::Welcome welcome;
  zoobmsg::Welcome::unpack(dataLen, data, offset, welcome);

  LOGI("[handleMsgWelcome] playerID=%i, serverState=%i", welcome.playerID, welcome.serverState);
  LOGI("[handleMsgWelcome] level=%s", welcome.level.bytes);

  const char* json = welcome.level.bytes;
  ASSERT(json[welcome.level.numBytes-1] == '\0');
  /*const uint16_t playerID = welcome.playerID;
  const ServerState serverState = welcome.serverState;*/
  levelChangedCb(json);
}

void Client::handleMsgVersion (size_t dataLen, const uint8_t* data, size_t offset) {
  zoobmsg::Version version;
  zoobmsg::Version::unpack(dataLen, data, offset, version);

  if (version.version != PROTOCOL_VERSION) {
    //TODO: should disconnect and notify user
  }
  LOGI("[handleMsgVersion] version=%i", version.version);
}

void Client::handleMsgKicked (size_t dataLen, const uint8_t* data, size_t offset) {
  zoobmsg::Kicked kicked;
  zoobmsg::Kicked::unpack(dataLen, data, offset, kicked);

  //TODO: should disconnect and notify user
  LOGI("[handleMsgKicked] reason=%s", (char*)kicked.reason.bytes);
}

void Client::handleMsgGameState (size_t dataLen, const uint8_t* data, size_t offset) {
  zoobmsg::GameState gameState;
  zoobmsg::GameState::unpack(dataLen, data, offset, gameState);
  NetworkedGame* ng = static_cast<NetworkedGame*>(Game::getInstance());
  ng->applyGameState(gameState);
}

void Client::handleMsgSpawn (size_t dataLen, const uint8_t* data, size_t offset) {
  zoobmsg::Spawn spawn;
  zoobmsg::Spawn::unpack(dataLen, data, offset, spawn);

  LOGI("[handleMsgSpawn] spawn at (%f,%f)", spawn.position.x, spawn.position.y);

  Game* game = Game::getInstance();
  const Vector2 spawnPos = Vector2(spawn.position.x, spawn.position.y);
  uint16_t tankID = spawn.tankID;
  PlayerTank* pt = new PlayerTank(game->newPlayerFirePolicy());
  pt->setID(tankID);
  pt->setPosition(spawnPos);
  game->playerSpawned(pt);
}

void Client::wantSpawn() {
  zoobmsg::WantSpawn wantSpawn;
  sendMsgWantSpawn(wantSpawn);
}

void Client::update(NetworkedGame* game) {
}
 
void Client::sendPlayerCommand (uint16_t tankID, const PlayerCommand& cmd) {
  zoobmsg::PlayerCommands msg;
  msg.tankID = tankID;
  msg.moveDir.x = cmd.moveDir.x;
  msg.moveDir.y = cmd.moveDir.y;
  msg.fire = cmd.fire;
  msg.fireDir.x = cmd.fireDir.x;
  msg.fireDir.y = cmd.fireDir.y;
  msg.mine = cmd.mine;
  msg.shield = cmd.shield;

  sendMsgPlayerCommands(msg);
}


