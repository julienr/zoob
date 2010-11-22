#ifndef _CLIENT_H
#define	_CLIENT_H

#include "Messages.h"
#include "def.h"
#include "NetController.h"
#include "containers/blockingqueue.h"

class Client : public NetController {
  public:
    Client () 
      : lastSpawn(NULL),
        lastWelcome(NULL) {
      pthread_mutex_init(&mutex, NULL);
    }

    ~Client () {
      pthread_mutex_destroy(&mutex);
    }

    virtual bool start () = 0;
    virtual void stop () = 0;
    virtual void think (double elapsedS) = 0;

    void update(NetworkedGame* game);
    void sendPlayerCommand (uint16_t localPlayerID, const PlayerCommand& cmd);
    void assignID (Entity* e) {
      //do nothing, id will be assigned by server
    }

    void wantSpawn ();

    //Returns NULL if now new level message was received since last queried.
    //Returns the JSON description for the level otherwise, alongside with the
    //new playerID and the serverState
    char* hasNewLevel (uint16_t* playerID, ServerState* serverState);
    bool hasSpawned (Vector2& position, uint16_t& id);

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
    pthread_t threadID;
    blockingqueue<zoobmsg::GameState*> gameStates;

    //To synchronize game and network threads
    pthread_mutex_t mutex;

    //Store the last spawn message received. It will be queried by the game thread
    zoobmsg::Spawn* lastSpawn;

    //We always store the last welcome message received. It will be queried by the game thread using
    //hasNewLevel.
    zoobmsg::Welcome* lastWelcome;
};

#endif	/* _CLIENT_H */

