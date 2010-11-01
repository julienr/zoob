#include "ENetClient.h"
#include <pthread.h>
#include <unistd.h>

static ENetPeer* serverPeer = NULL;

static ENetPeer* connectToServer (ENetHost* client) {
  ENetAddress address;
  enet_address_set_host(&address, "localhost");
  address.port = SERVER_PORT;

  ENetPeer* peer = enet_host_connect(client, &address, NUM_CHANNELS, 0);
  if (peer == NULL) {
    return NULL;
  }

  /* Wait up to 5 seconds for the connection attempt to succeed. */
  ENetEvent event;
  if (enet_host_service(client, & event, 5000) > 0 &&
          event.type == ENET_EVENT_TYPE_CONNECT) {
    LOGE("Connection to server suceeded");
    return peer;
  } else {
    /* Either the 5 seconds are up or a disconnect event was */
    /* received. Reset the peer in the event the 5 seconds   */
    /* had run out without any significant event.            */
    enet_peer_reset(peer);

    LOGE("Connection to server failed");
    return NULL;
  }

}

static void* clientThread (void* args) {
 if (enet_initialize() != 0) {
    LOGE("An error occured while initializing ENET");
    //TODO: call error method
    return NULL;
  }

  //Create client
  ENetHost* client;
  client = enet_host_create(NULL, 1, NUM_CHANNELS, 0, 0);
  if (!client) {
    LOGE("An error occured while creating ENET client");
    //TODO: call error method
    return NULL;
  }

  serverPeer = connectToServer(client);
  if (!serverPeer) {
    //TODO: call error method
    return NULL;
  }

  //Send hello packet
  zoobmsg::Hello hello;
  hello.nickname.numBytes = 5;
  hello.nickname.bytes = new char[5];
  strncpy(hello.nickname.bytes, "test", 5);
  Client::getInstance()->sendMsgHello(hello);

  ENetEvent event;
  while (true) {
    while (enet_host_service(client, &event, 1000) > 0) {
      switch (event.type) {
        case ENET_EVENT_TYPE_CONNECT: break;  //shouldn't happen
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
            Client::getInstance()->cbName(event.packet->dataLength, event.packet->data, 1); \
            break;

          switch (msgType) {
            MSG_CALLBACK(Version, handleMsgVersion)
            MSG_CALLBACK(Kicked, handleMsgKicked)
            MSG_CALLBACK(Welcome, handleMsgWelcome)
          }
          enet_packet_destroy(event.packet);
          break;
        }
        case ENET_EVENT_TYPE_DISCONNECT: {
          LOGE("%s disconnected", (char*)event.peer->data);
          event.peer->data = NULL;
          break;
        }
        case ENET_EVENT_TYPE_NONE: break; //shouldn't happen
      }
    }
  }
  enet_host_destroy(client);
  enet_deinitialize();
  return NULL;
}

void ENetClient::start () {
  pthread_create(&threadID, NULL, clientThread, NULL);
}

#define SEND_MESSAGE(flags, msgType, channel) \
  const size_t size = msgType::packedSize(msg)+1; \
  ENetPacket* packet = enet_packet_create(NULL, size, flags); \
  packet->data[0] = msgType::messageID; \
  size_t offset = 1; \
  msgType::pack(size, packet->data, offset, msg); \
  enet_peer_send (serverPeer, 0, packet)

void ENetClient::sendMsgHello(const zoobmsg::Hello& msg) {
  LOGI("[sendMsgHello] msg size : %lu", zoobmsg::Hello::packedSize(msg));
  SEND_MESSAGE(ENET_PACKET_FLAG_RELIABLE, zoobmsg::Hello, 0);
}
