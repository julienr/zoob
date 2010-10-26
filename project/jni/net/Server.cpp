#include "Server.h"

Server* Server::instance = NULL;

void Server::handleConnect (uint64_t peerID) {

}

void Server::handleMessage(const ::google::protobuf::MessageLite& msg) {
  LOGE("Receive unhandled message of type %s", msg.GetTypeName().data());
}

void Server::handleMessage(const zoobmsg::Hello& msg) {

}

void Server::handleMessage(const zoobmsg::Join& msg) {

}

void Server::handleMessage(const zoobmsg::PlayerCommand& msg) {

}

void Server::handleDisconnect (uint64_t peerID) {
  
}


