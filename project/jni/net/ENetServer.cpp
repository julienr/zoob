#include "ENetServer.h"
#include <pthread.h>
#include <unistd.h>

map<uint64_t, ENetPeer*> peers;

static uint64_t toUID (ENetPeer* peer) {
  return ((uint64_t)peer->address.host << 32) | (uint64_t)peer->address.port;
}

static ENetPeer* toPeer (uint64_t uid) {
  return peers.get(uid);
}

static void* serverThread (void* args) {
  if (enet_initialize() != 0) {
    LOGE("An error occured while initializing ENET");
    //FIXME: call error method
    return NULL;
  }

  //Create server
  ENetAddress address;
  ENetHost* server;

  address.host = ENET_HOST_ANY;
  address.port = SERVER_PORT;

  server = enet_host_create(&address, MAX_CLIENTS, NUM_CHANNELS, 0, 0);
  if (!server) {
    LOGE("An error occured while creating ENET server");
    //FIXME: call error method
    return NULL;
  }


  ENetEvent event;
  while (true) {
    while (enet_host_service(server, &event, 1000) > 0) {
      switch (event.type) {
        case ENET_EVENT_TYPE_NONE: break;
        case ENET_EVENT_TYPE_CONNECT: {
          LOGE("New client connected from %x:%u", event.peer->address.host,
                  event.peer->address.port);
          event.peer->data = (void*)"Client information";

            //Send a single packet
            ENetPacket * packet = enet_packet_create ("hi from server",
                                                      strlen ("hi from server") + 1,
                                                      ENET_PACKET_FLAG_RELIABLE);
            enet_peer_send(event.peer, 0, packet);
            enet_host_flush(server);
            peers.insert(toUID(event.peer), event.peer);
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
            Server::getInstance()->cbName(toUID(event.peer), event.packet->dataLength, event.packet->data, 1); \
            break;


          switch (msgType) {
            MSG_CALLBACK(Hello, handleMsgHello)
            MSG_CALLBACK(Join, handleMsgJoin)
            MSG_CALLBACK(PlayerCommands, handleMsgPlayerCommand)
          }
          enet_packet_destroy(event.packet);
          break;
        }
        case ENET_EVENT_TYPE_DISCONNECT: {
          LOGE("%s disconnected", (char*)event.peer->data);
          event.peer->data = NULL;
          peers.remove(toUID(event.peer));
        }
      }
    }
  }
  enet_host_destroy(server);
  enet_deinitialize();
  return NULL;
}

void ENetServer::start () {
  pthread_create(&threadID, NULL, serverThread, NULL);
}

void ENetServer::update(NetworkedGame& game) {
  //TODO
}

void ENetServer::sendPlayerCommand (uint16_t localPlayerID, const PlayerCommand& cmd) {
  //TODO
}

//To send a message, we first calculate its size. We add one because the first
//byte will store the message id.
#define SEND_MESSAGE(flags, msgType, channel) \
  const size_t size = msgType::packedSize(msg)+1; \
  ENetPacket* packet = enet_packet_create(NULL, size, flags); \
  packet->data[0] = msgType::messageID; \
  size_t offset = 1; \
  msgType::pack(size, packet->data, offset, msg); \
  enet_peer_send (toPeer(peerID), 0, packet)


void ENetServer::sendMsgWelcome (const uint64_t& peerID, const zoobmsg::Welcome& msg) {
  SEND_MESSAGE(ENET_PACKET_FLAG_RELIABLE, zoobmsg::Welcome, 0);
}

void ENetServer::sendMsgVersion (const uint64_t& peerID, const zoobmsg::Version& msg) {
  SEND_MESSAGE(ENET_PACKET_FLAG_RELIABLE, zoobmsg::Version, 0);
}
