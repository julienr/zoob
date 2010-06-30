#ifndef SDLINPUTMANAGER_H_
#define SDLINPUTMANAGER_H_

#include "InputManager.h"

class SDLInputManager : public InputManager {
  public:
    SDLInputManager ();

    void draw ();

    //UP : 1 = up, 0 = none, -1 = down
    //LEFT :1 = left, 0 = none, -1 = right
    void setMove (int up, int left);

};

#endif /* SDLINPUTMANAGER_H_ */
