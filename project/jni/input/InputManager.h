#ifndef INPUTMANAGER_H_
#define INPUTMANAGER_H_

#include "logic/Game.h"
#include "view/GameManager.h"

// manage inputs and sends events to game
class InputManager {
  public:
    InputManager (GameManager* gameManager)
      : gameManager(gameManager) {
    }
    //This can be used to draw input-method specific stuff on the screen (such as a virtual gamepad)
    virtual void draw () {}

    void setGame(Game* game) { this->game = game; }

  protected:
    inline
    Game* getGame () { return game; }

    inline
    GameManager* getGameManager () { return gameManager; }
  private:
    Game* game;
    GameManager* const gameManager;
};

#endif /* INPUTMANAGER_H_ */
