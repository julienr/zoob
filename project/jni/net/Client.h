#ifndef _CLIENT_H
#define	_CLIENT_H

#include "def.h"
#include "NetController.h"

class Client : public NetController {
  public:
    static Client* getInstance() {
      if (!instance)
        instance = new Client();
      return instance;
    }

    void start ();
  private:
    static Client* instance;
    pthread_t threadID;
};

#endif	/* _CLIENT_H */

