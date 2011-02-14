#include "ENetServer.h"
#include <pthread.h>
#include <unistd.h>

void ENetServer::think (double /*elapsedS*/) {
  ENetEvent event;
  while (enet_host_service(server, &event, 0) > 0) {
    switch (event.type) {
      case ENET_EVENT_TYPE_NONE: break;
      case ENET_EVENT_TYPE_CONNECT: {
        const uint16_t peerID = peerIDGen++;
        uidToPeers.insert(peerID, event.peer);
        peerToUids.insert(event.peer, peerID);
        LOGE("New client connected from %x:%u, assigned uid=%i", event.peer->address.host,
                event.peer->address.port, peerID);

        handleConnect(toUID(event.peer));
        break;
      }
      case ENET_EVENT_TYPE_RECEIVE: {
        /*LOGE("Packet of length %lu with content %s received from %s on channel %u",
                event.packet->dataLength,
                (char*)event.packet->data,
                event.peer->data,
                event.channelID);*/
        if (event.packet->dataLength < 1) {
          LOGE("Dropping packet with length %lu", event.packet->dataLength);
          break;
        }
        uint8_t msgType = event.packet->data[0];
#define MSG_CALLBACK(type, cbName) \
        case zoobmsg::type::messageID: \
          static_cast<ENetServer*>(NetController::getInstance())->cbName(toUID(event.peer), event.packet->dataLength, event.packet->data, 1); \
          break;


        switch (msgType) {
          MSG_CALLBACK(Hello, handleMsgHello)
          MSG_CALLBACK(WantSpawn, handleMsgWantSpawn)
          MSG_CALLBACK(PlayerCommands, handleMsgPlayerCommand)
        }
        enet_packet_destroy(event.packet);
        break;
      }
      case ENET_EVENT_TYPE_DISCONNECT: {
        event.peer->data = NULL;
        const uint16_t peerID = peerToUids.get(event.peer);
        uidToPeers.remove(peerID);
        peerToUids.remove(event.peer);
      }
    }
  }
}

bool ENetServer::start () {
    if (enet_initialize() != 0) {
    LOGE("An error occured while initializing ENET");
    //FIXME: call error method
    return false;
  }

  //Create server
  ENetAddress address;

  address.host = ENET_HOST_ANY;
  address.port = SERVER_PORT;

  server = enet_host_create(&address, MAX_CLIENTS, NUM_CHANNELS, 0, 0);
  if (!server) {
    LOGE("An error occured while creating ENET server");
    //FIXME: call error method
    return false;
  }
  return true;
}

void ENetServer::stop () {
  enet_host_destroy(server);
  enet_deinitialize();
}


//To send a message, we first calculate its size. We add one because the first
//byte will store the message id.
#define SEND_MESSAGE(flags, msgType, channel) \
  const size_t size = msgType::packedSize(msg)+1; \
  ENetPacket* packet = enet_packet_create(NULL, size, flags); \
  packet->data[0] = msgType::messageID; \
  size_t offset = 1; \
  msgType::pack(size, packet->data, offset, msg); \
  enet_peer_send (toPeer(peerID), channel, packet)


void ENetServer::sendMsgWelcome (const uint16_t peerID, const zoobmsg::Welcome& msg) {
  SEND_MESSAGE(ENET_PACKET_FLAG_RELIABLE, zoobmsg::Welcome, 0);
}

void ENetServer::sendMsgVersion (const uint16_t peerID, const zoobmsg::Version& msg) {
  SEND_MESSAGE(ENET_PACKET_FLAG_RELIABLE, zoobmsg::Version, 0);
}

void ENetServer::sendMsgGameState (const uint16_t peerID, const zoobmsg::GameState& msg) {
  //LOGI("[sendMgsGameState] msg size : %lu", zoobmsg::GameState::packedSize(msg));
  SEND_MESSAGE(0, zoobmsg::GameState, 1);
}

void ENetServer::sendMsgSpawn (const uint16_t peerID, const zoobmsg::Spawn& msg) {
  SEND_MESSAGE(ENET_PACKET_FLAG_RELIABLE, zoobmsg::Spawn, 0);
}
