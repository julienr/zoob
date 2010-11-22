#ifndef _ENETSERVER_H
#define	_ENETSERVER_H

#include "Server.h"
#include "enet/enet.h"
#include "containers/map.h"

class ENetServer : public Server {
  public:
    ENetServer () : server(NULL), peerIDGen(1) {
    }
    
    bool start ();
    void stop ();
    void think (double elapsedS);
 
  protected:
    void sendMsgWelcome (const uint16_t peerID, const zoobmsg::Welcome& msg);
    void sendMsgVersion (const uint16_t peerID, const zoobmsg::Version& msg);
    void sendMsgGameState (const uint16_t peerID, const zoobmsg::GameState& msg);
    void sendMsgSpawn (const uint16_t peerID, const zoobmsg::Spawn& msg);

  private:
    ENetHost* server;
    //This is a "bimap". Keep them synchronized manually
    map<uint16_t, ENetPeer*> uidToPeers;
    map<ENetPeer*, uint16_t> peerToUids;

    uint16_t toUID (ENetPeer* peer) {
      return peerToUids.get(peer);
    }

    ENetPeer* toPeer (uint16_t uid) {
      return uidToPeers.get(uid);
    }

    uint16_t peerIDGen;
};

#endif	/* _ENETSERVER_H */

