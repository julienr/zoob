#ifndef _ENETSERVER_H
#define	_ENETSERVER_H

#include "Server.h"
#include "enet/enet.h"
#include "containers/map.h"

class ENetServer : public Server {
  public:
    void start ();
 
  protected:
    void sendMsgWelcome (const uint16_t peerID, const zoobmsg::Welcome& msg);
    void sendMsgVersion (const uint16_t peerID, const zoobmsg::Version& msg);
    void sendMsgGameState (const uint16_t peerID, const zoobmsg::GameState& msg);

  private:
    pthread_t threadID;
};

#endif	/* _ENETSERVER_H */

