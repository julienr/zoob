#ifndef _ENETCLIENT_H
#define _ENETCLIENT_H

#include "Client.h"
#include "enet/enet.h"

class ENetClient : public Client {
  public:
    ENetClient() : serverPeer(NULL), client(NULL) {}
    bool start ();
    void think (double elapsedS);
    void stop ();
    void sendMsgHello (const zoobmsg::Hello& msg);
    void sendMsgPlayerCommands (const zoobmsg::PlayerCommands& msg);
    void sendMsgWantSpawn (const zoobmsg::WantSpawn& msg);
  private:
    ENetPeer* serverPeer;
    ENetHost* client;
};

#endif
