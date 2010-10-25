#include "Server.h"
#include <pthread.h>
#include <unistd.h>
#include "enet/enet.h"


#define MAX_CLIENTS 5

Server* Server::instance = NULL;

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
  enet_host_destroy(server);
  enet_deinitialize();
  return NULL;
}

void Server::start () {
  pthread_create(&threadID, NULL, serverThread, NULL);
}

