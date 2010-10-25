#ifndef _SERVER_H
#define	_SERVER_H

#include "def.h"
#include "NetController.h"

class Server : public NetController {
  public:
    static Server* getInstance() {
      if (!instance)
        instance = new Server();
      return instance;
    }

    void start ();
  private:
    static Server* instance;
    
    pthread_t threadID;
};

#endif	/* _SERVER_H */

