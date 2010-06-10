#ifndef INPUTMANAGER_H_
#define INPUTMANAGER_H_

#include "logic/Game.h"
#include "view/GameManager.h"

// manage inputs and sends events to game
class InputManager {
  public:
    //This can be used to draw input-method specific stuff on the screen (such as a virtual gamepad)
    virtual void draw () {}
  protected:
    Game* getGame () { return Game::getInstance(); }

};

#endif /* INPUTMANAGER_H_ */
