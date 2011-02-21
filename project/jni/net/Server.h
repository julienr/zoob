#ifndef _SERVER_H
#define	_SERVER_H

#include "zoobdef.h"
#include "Messages.h"
#include "NetController.h"

/**
 * This class must be subclassed to implement the packet transport handling
 * For example, we'll have one subclass for libenet, one for bluetooth and so on
 *
 * Remark on peerID :
 * To provide a transport-agnostic identification of a peer, we use a uint64_t
 * as a unique identifier for a peer. This id is created and interpreted by the
 * underlying transport implementation.
 *
 */
class Server : public NetController {
  public:
    Server () : playerIDGen(1), entityIDGen(1), state(WARM_UP), lastGameStateBroadcast(Utils::getCurrentTimeMillis()) {}

    /**
     * This method should be implemented by the transport-specific subclass
     * It will most likely start a new thread that will somehow listen for network events.
     * The handleXXX callbacks should be called from this thread.
     */
    virtual bool start () = 0;
    virtual void stop () = 0;
    virtual void think (double elapsedS) = 0;
 
    void update(NetworkedGame* game);
    void sendPlayerCommand (uint16_t localPlayerID, const PlayerCommand& cmd);

    void sendExplosion (const ExplosionInfo& info) {
      explosions.append(info);
    }

    void assignID (Entity* e) {
      e->setID(entityIDGen++);
    }

    //already handled by local game simulation
    void wantSpawn ();

    ServerState getState () {
      return state;
    }

    void setState (ServerState newState) {
      state = newState;
    }

  
    //These are callbacks for the various events that can happen on server-side.
    //These are called in the server thread
    void handleConnect (const uint16_t peerID);

    //Handles for various messages
    void handleMsgHello (const uint16_t peerID, size_t dataLen, const uint8_t* data, size_t offset);
    void handleMsgWantSpawn (const uint16_t peerID, size_t dataLen, const uint8_t* data, size_t offset);
    void handleMsgPlayerCommand (const uint16_t peerID, size_t dataLen, const uint8_t* data, size_t offset);
    
    void handleDisconnect (const uint16_t peerID);


    //Implemented by subclass for efficiency (avoid buffer copies)
    virtual void sendMsgWelcome (const uint16_t peerID, const zoobmsg::Welcome& msg) = 0;
    virtual void sendMsgVersion (const uint16_t peerID, const zoobmsg::Version& msg) = 0;
    virtual void sendMsgGameState (const uint16_t peerID, const zoobmsg::GameState& msg) = 0;
    virtual void sendMsgSpawn (const uint16_t peerID, const zoobmsg::Spawn& msg) = 0;

    bool isClient () {
      return false;
    }

  private:
    //Try to spawn all the tanks that are waiting for it
    void _doSpawns (NetworkedGame* game);
    uint16_t playerIDGen;
    uint16_t entityIDGen;
    ServerState state;

    //contains all the playerID that requested a spawn but haven't gotten it yet
    list<uint16_t> spawnQueue;

    set<uint16_t> connectedClients;

    //list of explosions to be sent to clients on next gamestate
    list<ExplosionInfo> explosions;

    uint64_t lastGameStateBroadcast;
};

#endif	/* _SERVER_H */

