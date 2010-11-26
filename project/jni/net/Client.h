#ifndef _CLIENT_H
#define	_CLIENT_H

#include "Messages.h"
#include "def.h"
#include "NetController.h"
#include "containers/blockingqueue.h"

typedef void (*LevelChangedCb) (const char*);

class Client : public NetController {
  public:
    
    /// Construct a new network client. A callback to handle level-changed events
    /// must be supplied
    Client (LevelChangedCb changeCb) : levelChangedCb(changeCb) {}

    virtual bool start () = 0;
    virtual void stop () = 0;
    virtual void think (double elapsedS) = 0;

    void update(NetworkedGame* game);
    void sendPlayerCommand (uint16_t localPlayerID, const PlayerCommand& cmd);
    void assignID (Entity* e) {
      //do nothing, id will be assigned by server
    }

    void wantSpawn ();

    void handleMsgVersion (size_t dataLen, const uint8_t* data, size_t offset);
    void handleMsgKicked (size_t dataLen, const uint8_t* data, size_t offset);
    void handleMsgWelcome (size_t dataLen, const uint8_t* data, size_t offset);
    void handleMsgGameState (size_t dataLen, const uint8_t* data, size_t offset);
    void handleMsgSpawn (size_t dataLen, const uint8_t* data, size_t offset);

    virtual void sendMsgHello(const zoobmsg::Hello& msg) = 0;
    virtual void sendMsgWantSpawn (const zoobmsg::WantSpawn& msg) = 0;
    virtual void sendMsgPlayerCommands (const zoobmsg::PlayerCommands& msg) = 0;

    bool isClient () {
      return true;
    }
  private:
    LevelChangedCb levelChangedCb;
};

#endif	/* _CLIENT_H */

