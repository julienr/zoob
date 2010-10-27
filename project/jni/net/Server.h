#ifndef _SERVER_H
#define	_SERVER_H

#include "def.h"
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
 * Remark on threading :
 * When we refer to a server thread, this is the thread that is used when doing
 * blocking networking i/o. This is as opposed to the game thread that handles
 * the logic and rendering
 */
class Server : public NetController {
  public:
    static Server* getInstance() {
      if (!instance)
        LOGE("Server::getInstance() before registerInstance() called");
      return instance;
    }

    /**
     * This method should be implemented by the transport-specific subclass
     * It will most likely start a new thread that will somehow listen for network events.
     * The handleXXX callbacks should be called from this thread.
     */
    virtual void start () = 0;

protected:
    //These are callbacks for the various events that can happen on server-side.
    //These are called in the server thread
    void handleConnect (const uint64_t& peerID);

    //Handles for various messages
    void handleMsgHello (size_t dataLen, const uint8_t* data, size_t offset);
    void handleMsgJoin (size_t dataLen, const uint8_t* data, size_t offset);
    void handleMsgPlayerCommand (size_t dataLen, const uint8_t* data, size_t offset);
    
    void handleDisconnect (const uint64_t& peerID);

    //Implemented by subclass for efficiency (avoid buffer copies)
    virtual void sendMsgWelcome (const uint64_t& peerID, const zoobmsg::Welcome& msg) = 0;
    
  private:
    void registerInstance (Server* i) {
      instance = i;
    }
    static Server* instance;
};

#endif	/* _SERVER_H */

