#ifndef _NETCONTROLLER_H
#define	_NETCONTROLLER_H

#include "def.h"
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
    //MUST be called from the game thread
    virtual void update(NetworkedGame* game) = 0;

    //Enqueue a PlayerCommand to be send to the server. 
    //MUST be called from the game thread
    virtual void sendPlayerCommand (uint16_t localPlayerID, const PlayerCommand& cmd) = 0;

    virtual void start () = 0;

    //Should assign a new ID to the given entity
    virtual void assignID (Entity* e) = 0;

    //true if this netcontroller is client, false if it is server
    virtual bool isClient () = 0;

    //Check if this netcontroller has received a command to load a new level.
    //Returns NULL if no such command was received, otherwise returns a NEWLY allocated string containing the 
    //JSON level which should be freed with free
    //playerID and serverState are set only if the function doesn't return NULL
    virtual char* hasNewLevel (uint16_t* playerID, ServerState* serverState) = 0;
  private:
    static NetController* instance;
};

#endif	/* _NETCONTROLLER_H */

