#ifndef _ENETSERVER_H
#define	_ENETSERVER_H

#include "Server.h"

class ENetServer : public Server {
  public:
    void start ();
  private:
    uint64_t toUID (ENetPeer* peer) {
      return (peer->address.host << 32) | peer->address.port;
    }

    void toAddress (uint64_t uid, ENetAddress* addr) {
      addr->host = (uid >> 32)&0xffffffff;
      addr->port = (uid)&0xffff;
    }

    pthread_t threadID;
};

#endif	/* _ENETSERVER_H */

