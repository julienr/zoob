#ifndef _NETCONTROLLER_H
#define	_NETCONTROLLER_H

#include "zoobdef.h"
#include "logic/PlayerCommand.h"
#include "logic/NetworkedGame.h"


#define NUM_CHANNELS 2
#define SERVER_PORT 1234

#define MAX_CLIENTS 5

#define PROTOCOL_VERSION 1

//Base class for both Client and Server.
//It expose an API to query game status and send local player commands
class NetController {
  public:
    static bool isNetworkedGame () {
      return instance != NULL;
    }
    static NetController* getInstance () {
      if (!instance)
        LOGE("NetController::getInstance() before registerInstance()");
      return instance;
    }

    static void registerInstance (NetController* i) {
      if (instance)
        LOGE("NetController::registerInstance : existing instance");
      instance = i;
    }

    virtual ~NetController () {}

    //Update game with the latest state available from the network. Might do nothing
    //if we haven't received any state update.
    virtual void update(NetworkedGame* game) = 0;

    //Sends a PlayerCommand to be send to the server.
    virtual void sendPlayerCommand (uint16_t tankID, const PlayerCommand& cmd) = 0;

    virtual void sendExplosion (const ExplosionInfo& info) = 0;

    virtual void stop () = 0;
    virtual bool start () = 0;
    virtual void think (double elapsedS) = 0;

    //Should assign a new ID to the given entity
    virtual void assignID (Entity* e) = 0;

    //true if this netcontroller is client, false if it is server
    virtual bool isClient () = 0;

    //Called when the local player wants to spawn in the game
    virtual void wantSpawn () = 0;

    //If this returns true, this means this netcontroller's connection has timed out 
    //and it should be destroyed. The app should display an error message to the user
    virtual bool hasTimedOut () const = 0;

  private:
    static NetController* instance;
};

#endif	/* _NETCONTROLLER_H */

