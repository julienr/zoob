#include "Server.h"
#include "view/GameManager.h"
#include "levels/LevelsData.h"

Server* Server::instance = NULL;

void Server::handleConnect (const uint64_t& peerID) {

}

void Server::handleMsgHello (const uint64_t& peerID, size_t dataLen, const uint8_t* data, size_t offset) {
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

void Server::handleMsgJoin (const uint64_t& peerID, size_t dataLen, const uint8_t* data, size_t offset) {
  zoobmsg::Join join;
  zoobmsg::Join::unpack(dataLen, data, offset, join);
}

void Server::handleMsgPlayerCommand (const uint64_t& peerID, size_t dataLen, const uint8_t* data, size_t offset) {
  zoobmsg::PlayerCommands commands;
  zoobmsg::PlayerCommands::unpack(dataLen, data, offset, commands);
}

void Server::handleDisconnect (const uint64_t& peerID) {
  
}



