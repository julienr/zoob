#ifndef _ENETCLIENT_H
#define _ENETCLIENT_H

#include "Client.h"
#include "enet/enet.h"

class ENetClient : public Client {
  public:
    void start ();
    void sendMsgHello (const zoobmsg::Hello& msg);
  private:
    pthread_t threadID;
};

#endif
