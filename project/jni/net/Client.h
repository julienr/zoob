#ifndef _CLIENT_H
#define	_CLIENT_H

#include "Messages.h"
#include "def.h"
#include "NetController.h"
#include "containers/blockingqueue.h"

class Client : public NetController {
  public:
    static Client* getInstance() {
      if (!instance)
        LOGE("Client::getInstance() before registerInstance()");
      return instance;
    }

    static void registerInstance (Client* c) {
      instance = c;
    }


    virtual void start () = 0;

    void update(NetworkedGame& game);
    void sendPlayerCommand (uint16_t localPlayerID, const PlayerCommand& cmd);

    void handleMsgVersion (size_t dataLen, const uint8_t* data, size_t offset);
    void handleMsgKicked (size_t dataLen, const uint8_t* data, size_t offset);
    void handleMsgWelcome (size_t dataLen, const uint8_t* data, size_t offset);
    void handleGameState (size_t dataLen, const uint8_t* data, size_t offset);

    virtual void sendMsgHello(const zoobmsg::Hello& msg) = 0;
    virtual void sendMsgPlayerCommands (const zoobmsg::PlayerCommands& msg) = 0;


  private:
    static Client* instance;
    pthread_t threadID;
    blockingqueue<zoobmsg::GameState*> gameStates;
};

#endif	/* _CLIENT_H */

