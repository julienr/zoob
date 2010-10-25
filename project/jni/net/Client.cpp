#include "Client.h"
#include <pthread.h>
#include <unistd.h>
#include "enet/enet.h"

Client* Client::instance = NULL;

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

  ENetPeer* peer = connectToServer(client);
  if (!peer) {
    //TODO: call error method
    return NULL;
  }

  //Send a single packet
  ENetPacket * packet = enet_packet_create ("hello",
                                            strlen ("hello") + 1,
                                            ENET_PACKET_FLAG_RELIABLE);
  enet_peer_send(peer, 0, packet);
  enet_host_flush(client);


  ENetEvent event;
  while (true) {
    while (enet_host_service(client, &event, 1000) > 0) {
      switch (event.type) {
        case ENET_EVENT_TYPE_CONNECT: {
          LOGE("New client connected from %x:%u", event.peer->address.host,
                  event.peer->address.port);
          event.peer->data = (void*)"Client information";
          break;
        }
         case ENET_EVENT_TYPE_RECEIVE: {
          LOGE("Packet of length %u with content %s received from %s on channel %u",
                  event.packet->dataLength,
                  event.packet->data,
                  event.peer->data,
                  event.channelID);
          enet_packet_destroy(event.packet);
          break;
        }
        case ENET_EVENT_TYPE_DISCONNECT: {
          LOGE("%s disconnected", event.peer->data);
          event.peer->data = NULL;
        }
      }
    }
  }
  enet_host_destroy(client);
  enet_deinitialize();
  return NULL;
}

void Client::start () {
  pthread_create(&threadID, NULL, clientThread, NULL);
}
