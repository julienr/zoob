#include "ENetClient.h"
#include <unistd.h>

void ENetClient::think (double /*elapsedS*/) {
  ENetEvent event;
  while (enet_host_service(client, &event, 0) > 0) {
    switch (event.type) {
      case ENET_EVENT_TYPE_CONNECT: {
        LOGI("ENET connected... sending HELLO");
        connected = true;
        zoobmsg::Hello hello;
        hello.nickname.numBytes = 5;
        hello.nickname.bytes = new char[5];
        strncpy(hello.nickname.bytes, "test", 5);
        sendMsgHello(hello);
        break;
      }
      case ENET_EVENT_TYPE_RECEIVE: {
        /*LOGE("Packet of length %lu with content %s received from %s on channel %u",
                event.packet->dataLength,
                (char*)event.packet->data,
                (char*)event.peer->data,
                event.channelID);*/
        if (event.packet->dataLength < 1) {
          LOGE("Dropping packet with length %lu", event.packet->dataLength);
          break;
        }
        uint8_t msgType = event.packet->data[0];
#define MSG_CALLBACK(type, cbName) \
        case zoobmsg::type::messageID: \
          cbName(event.packet->dataLength, event.packet->data, 1); \
          break;

        switch (msgType) {
          MSG_CALLBACK(Version, handleMsgVersion)
          MSG_CALLBACK(Kicked, handleMsgKicked)
          MSG_CALLBACK(Welcome, handleMsgWelcome)
          MSG_CALLBACK(Spawn, handleMsgSpawn)
          MSG_CALLBACK(GameState, handleMsgGameState)
        }
        enet_packet_destroy(event.packet);
        break;
      }
      case ENET_EVENT_TYPE_DISCONNECT: {
        LOGE("%s disconnected", (char*)event.peer->data);
        event.peer->data = NULL;
        break;
      }
      case ENET_EVENT_TYPE_NONE: break;
    }
  }
}

bool ENetClient::hasTimedOut () const {
  if (connected)
    return false;

  const double now = Utils::getCurrentTimeMillis();
  if ((now-connectionAttemptStarted) > timeout)
    return true;
  else
    return false;
}

void ENetClient::stop () {
  LOGI("[ENetClient] client finished");
  enet_host_destroy(client);
  enet_deinitialize();
}

bool ENetClient::start () {
   if (enet_initialize() != 0) {
    LOGE("An error occured while initializing ENET");
    //TODO: call error method
    return false;
  }
  LOGI("ENET initialized");

  //Create client
  client = enet_host_create(NULL, 1, NUM_CHANNELS, 0, 0);
  if (!client) {
    LOGE("An error occured while creating ENET client");
    //TODO: call error method
    return false;
  }
  LOGI("ENET client created");

  //Connect to server
  ENetAddress address = {0,0};
  enet_address_set_host(&address, serverAddr);
  address.port = SERVER_PORT;

  LOGI("Connecting to %s", serverAddr);
  connectionAttemptStarted = Utils::getCurrentTimeMillis();

  serverPeer = enet_host_connect(client, &address, NUM_CHANNELS, 0);
  if (serverPeer == NULL) {
    return false;
  }
  LOGI("Connected, server peer created, waiting for EVENT_TYPE_CONNECT");
  return true;
}

#define SEND_MESSAGE(flags, msgType, channel) \
  const size_t size = msgType::packedSize(msg)+1; \
  ENetPacket* packet = enet_packet_create(NULL, size, flags); \
  packet->data[0] = msgType::messageID; \
  size_t offset = 1; \
  msgType::pack(size, packet->data, offset, msg); \
  enet_peer_send (serverPeer, channel, packet)

void ENetClient::sendMsgHello(const zoobmsg::Hello& msg) {
  //LOGI("[sendMsgHello] msg size : %lu", zoobmsg::Hello::packedSize(msg));
  SEND_MESSAGE(ENET_PACKET_FLAG_RELIABLE, zoobmsg::Hello, 0);
}

void ENetClient::sendMsgPlayerCommands (const zoobmsg::PlayerCommands& msg) {
  //LOGI("[sendMsgPlayerCommands] msg size : %lu", zoobmsg::PlayerCommands::packedSize(msg));
  SEND_MESSAGE(ENET_PACKET_FLAG_RELIABLE, zoobmsg::PlayerCommands, 0);
}

void ENetClient::sendMsgWantSpawn (const zoobmsg::WantSpawn& msg) {
  SEND_MESSAGE(ENET_PACKET_FLAG_RELIABLE, zoobmsg::WantSpawn, 0);
}
