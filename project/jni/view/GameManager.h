/**
 * Gamemanager manage the highlevel game states, like "playing", "menu", etc...
 */

#ifndef GAMEMANAGER_H_
#define GAMEMANAGER_H_

#include "def.h"
#include "containers/vector.h"
#include "menu/MenuItem.h"
#include "menu/Font.h"
#include "levels/LevelsData.h"

enum eAppState {
  STATE_PLAYING=0,
  STATE_MENU
};

class GameManager;
//Type for a function that will be called when a new game has to be started
typedef void (*startGameCallback_t) (GameManager* menu);

#define MENU_ITEM_START 0
#define MENU_ITEM_OPTIONS 1
#define MENU_ITEM_NEXT 2
#define MENU_ITEM_PREV 3

class GameManager {
  public:
    GameManager (startGameCallback_t cb)
      : newGameCB(cb), state(STATE_MENU), currentLevel(0), menuItems(4), touchedItem(-1),
        logo("assets/sprites/logo.png") {
      //Immediatly start with the first level
      newGameCB(this);

      //FIXME: delegate all this stuff to a "menudisplay" class and only handle the states here
      menuItems.add(new MenuItem("assets/sprites/menuitems/start.png",
                                 "assets/sprites/menuitems/start_h.png",
                                 MENU_ITEM_START));
      menuItems.add(new MenuItem("assets/sprites/menuitems/options.png",
                                  "assets/sprites/menuitems/options_h.png",
                                  MENU_ITEM_OPTIONS));
      menuItems.add(new MenuItem("assets/sprites/menuitems/next.png",
                                  "assets/sprites/menuitems/next_h.png",
                                  MENU_ITEM_NEXT));
      menuItems.add(new MenuItem("assets/sprites/menuitems/prev.png",
                                  "assets/sprites/menuitems/prev_h.png",
                                  MENU_ITEM_PREV));

      numbers[0] = new Sprite("assets/sprites/menuitems/0.png");
      numbers[1] = new Sprite("assets/sprites/menuitems/1.png");
      numbers[2] = new Sprite("assets/sprites/menuitems/2.png");
      numbers[3] = new Sprite("assets/sprites/menuitems/3.png");
      //font = new Font("assets/fonts/modenine.png");
    }

    ~GameManager () {
      for (size_t i=0; i<menuItems.length(); i++)
        delete menuItems[i];
      for (int i=0; i<3; i++)
        delete numbers[i];
    }

    eAppState getState () { return state; }

    size_t getCurrentLevel () {
      return currentLevel;
    }

    inline
    bool isAtLastLevel () {
      return currentLevel == numLevels-1;
    }

    inline
    bool isAtFirstLevel () {
      return currentLevel == 0;
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
    void _actionStart ();
    void _actionOptions ();
    void _actionNext ();
    void _actionPrev ();

    const startGameCallback_t newGameCB;
    eAppState state;
    size_t currentLevel;
    float screenWidth, screenHeight;

    vector<MenuItem*> menuItems;
    short touchedItem; //id of touched item, -1 if nothing has been touched
    Sprite* numbers[4];
    Sprite logo;
    Vector2 logoPos, logoSize;
    Vector2 numberPos, numberSize;

    //Font* font;
};

#endif /* GAMEMANAGER_H_ */
