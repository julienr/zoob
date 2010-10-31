#include "Client.h"
#include "enet/enet.h"

Client* Client::instance = NULL;


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
