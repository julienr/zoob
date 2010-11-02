#ifndef _NETCONTROLLER_H
#define	_NETCONTROLLER_H

#include "def.h"

#define NUM_CHANNELS 2
#define SERVER_PORT 1234

#define MAX_CLIENTS 5

#define PROTOCOL_VERSION 1

//Base class for both Client and Server.
//It expose an API to query game status and send local player commands
class NetController {
  public:

};

#endif	/* _NETCONTROLLER_H */

