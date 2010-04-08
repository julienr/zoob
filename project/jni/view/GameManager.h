/**
 * Gamemanager manage the highlevel game states, like "playing", "menu", etc...
 */

#ifndef GAMEMANAGER_H_
#define GAMEMANAGER_H_

#include "def.h"
#include "containers/vector.h"
#include "menu/MenuItem.h"

enum eAppState {
  STATE_PLAYING=0,
  STATE_MENU
};

class GameManager;
//Type for a function that will be called when a new game has to be started
typedef void (*startGameCallback_t) (GameManager* menu);

class GameManager {
  public:
    GameManager (startGameCallback_t cb)
      : newGameCB(cb), state(STATE_MENU), currentLevel(0), menuItems(2), touchedItem(-1) {
      //Immediatly start with the first level
      newGameCB(this);
      menuItems.add(new MenuItem("assets/sprites/retry.png", "assets/sprites/retry_hover.png"));
      menuItems.add(new MenuItem("assets/sprites/nextlvl.png", "assets/sprites/nextlvl_hover.png"));
    }

    ~GameManager () {
      for (size_t i=0; i<menuItems.length(); i++)
        delete menuItems[i];
    }

    eAppState getState () { return state; }

    size_t getCurrentLevel () {
      return currentLevel;
    }

    void menuMode () {
      state = STATE_MENU;
    }

    void drawMenu ();

    void handleTouchDown (const Vector2& p);
    void handleTouchUp (const Vector2& p);

    void resize (float sW, float sH);

    inline bool inGame () { return state == STATE_PLAYING; }


  private:
    void _actionRetry ();
    void _actionNextLvl ();

    const startGameCallback_t newGameCB;
    eAppState state;
    size_t currentLevel;

    float screenWidth, screenHeight;

    vector<MenuItem*> menuItems;

    int touchedItem; //idx of touched item, -1 if nothing has been touched
};

#endif /* GAMEMANAGER_H_ */
