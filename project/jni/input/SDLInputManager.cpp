#include "SDLInputManager.h"
#include "logic/Game.h"
#include "view/GameManager.h"

SDLInputManager::SDLInputManager ()
 : InputManager() {

}

void SDLInputManager::draw () {

}

void SDLInputManager::setMove (int up, int left) {
  if (GameManager::getInstance()->inGame()) {
    LOGE("game instance : %p", Game::getInstance());
    Vector2 move(left, up);
    move.normalize();
    Game::getInstance()->setTankMoveDir(move);
  }
}

