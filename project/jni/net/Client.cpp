#include "Client.h"
#include "enet/enet.h"


void Client::handleMsgWelcome (size_t dataLen, const uint8_t* data, size_t offset) {
  zoobmsg::Welcome welcome;
  zoobmsg::Welcome::unpack(dataLen, data, offset, welcome);

  LOGI("[handleMsgWelcome] playerID=%i, serverState=%i", welcome.playerID, welcome.serverState);
  LOGI("[handleMsgWelcome] level=%s", welcome.level.bytes);
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

void Client::handleGameState (size_t dataLen, const uint8_t* data, size_t offset) {
  zoobmsg::GameState* gameState = new zoobmsg::GameState();
  zoobmsg::GameState::unpack(dataLen, data, offset, *gameState);
  gameStates.append(gameState);
}

void Client::update(NetworkedGame* game) {
  zoobmsg::GameState* gameState;
  while (gameStates.pop(&gameState)) {
    game->applyGameState(gameState);
    delete gameState;
  }
}
 
void Client::sendPlayerCommand (uint16_t localPlayerID, const PlayerCommand& cmd) {
  zoobmsg::PlayerCommands msg;
  msg.playerID = localPlayerID;
  msg.moveDir.x = cmd.moveDir.x;
  msg.moveDir.y = cmd.moveDir.y;
  msg.fire = cmd.fire;
  msg.fireDir.x = cmd.fireDir.x;
  msg.fireDir.y = cmd.fireDir.y;
  msg.mine = cmd.mine;
  msg.shield = cmd.shield;

  sendMsgPlayerCommands(msg);
}


