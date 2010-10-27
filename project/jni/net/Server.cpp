#include "Server.h"

Server* Server::instance = NULL;

void Server::handleConnect (const uint64_t& peerID) {

}

void Server::handleMsgHello (size_t dataLen, const uint8_t* data, size_t offset) {
  zoobmsg::Hello hello;
  zoobmsg::Hello::unpack(dataLen, data, offset, hello);
}

void Server::handleMsgJoin (size_t dataLen, const uint8_t* data, size_t offset) {
  zoobmsg::Join join;
  zoobmsg::Join::unpack(dataLen, data, offset, join);
}

void Server::handleMsgPlayerCommand (size_t dataLen, const uint8_t* data, size_t offset) {
  zoobmsg::PlayerCommands commands;
  zoobmsg::PlayerCommands::unpack(dataLen, data, offset, commands);
}

void Server::handleDisconnect (const uint64_t& peerID) {
  
}



