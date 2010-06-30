#ifndef INPUTMANAGER_H_
#define INPUTMANAGER_H_

#include "logic/Game.h"
#include "view/GameManager.h"

// manage inputs and sends events to game
class InputManager {
  private:
    static InputManager* instance;

  public:
    static void registerInstance (InputManager* i) {
      ASSERT(!instance);
      instance = i;
    }

    static InputManager* getInstance () {
      ASSERT(instance);
      return instance;
    }

  public:
    //This can be used to draw input-method specific stuff on the screen (such as a virtual gamepad)
    virtual void draw () {}
    virtual void think (double elapsedS) {}
    virtual void reset () {}

};

#endif /* INPUTMANAGER_H_ */
