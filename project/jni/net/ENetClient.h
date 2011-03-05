#ifndef _ENETCLIENT_H
#define _ENETCLIENT_H

#include "Client.h"
#include "enet/enet.h"

class ENetClient : public Client {
  public:
    //@param changeCb is a callback to be called when the level changes
    //@param serverAddr is the address of the server to connec to
    //@param timeout is the time (in seconds) after which we'll consider a connection failed if we got no response
    ENetClient(LevelChangedCb changeCb, const char* serverAddr, double timeout=1.0) : Client(changeCb), serverPeer(NULL), client(NULL), serverAddr(strdup(serverAddr)), connected(false), timeout(timeout)  {}
    bool start ();
    void think (double elapsedS);
    void stop ();
    bool hasTimedOut () const;
    void sendMsgHello (const zoobmsg::Hello& msg);
    void sendMsgPlayerCommands (const zoobmsg::PlayerCommands& msg);
    void sendMsgWantSpawn (const zoobmsg::WantSpawn& msg);
  private:
    ENetPeer* serverPeer;
    ENetHost* client;
    const char* serverAddr;

    bool connected; //true when we got a response to our connection attempt
    double connectionAttemptStarted; //the time at which the last connection attempt started
    double timeout; //time after which we'll consider connection has failed if we don't have any response
};

#endif
