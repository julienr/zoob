#ifndef _ENETSERVER_H
#define	_ENETSERVER_H

#include "Server.h"
#include "enet/enet.h"
#include "containers/map.h"

class ENetServer : public Server {
  public:
    void start ();

    void update(NetworkedGame& game);
    void sendPlayerCommand (uint16_t localPlayerID, const PlayerCommand& cmd);

  protected:
    void sendMsgWelcome (const uint64_t& peerID, const zoobmsg::Welcome& msg);
    void sendMsgVersion (const uint64_t& peerID, const zoobmsg::Version& msg);

  private:
    pthread_t threadID;
};

#endif	/* _ENETSERVER_H */

