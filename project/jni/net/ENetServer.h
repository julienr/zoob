#ifndef _ENETSERVER_H
#define	_ENETSERVER_H

#include "Server.h"
#include "enet/enet.h"
#include "containers/map.h"

class ENetServer : public Server {
  public:
    void start ();
  protected:
    void sendMsgWelcome (const uint64_t& peerID, const zoobmsg::Welcome& msg);

  private:
    pthread_t threadID;
};

#endif	/* _ENETSERVER_H */

