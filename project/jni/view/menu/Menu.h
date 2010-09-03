#ifndef MENU_H_
#define MENU_H_

#include "def.h"
#include "lib/Vector2.h"
#include "MenuItem.h"
#include "containers/vector.h"

class GameManager;

//For how long (in ms), does the user need to touch a button for it to repeat its action
#define INITIAL_TOUCH_REPEAT_TIME 1000
//When we have started to repeat one item action, how often do we repeat it ? (the user must still have the item touched)
#define TOUCH_REPEAT_TIME 100

/* base class for a menu screen */
class Menu {
  public:
    Menu (GameManager* gm)
      : gameManager(gm),
        menuItems(4),
        touchedItem(-1),
        isRepeating(false) {
    }

    virtual ~Menu ();

    void think ();
    virtual void draw () = 0;

    void handleTouchDown (const Vector2& p);
    void handleTouchUp (const Vector2& p);

    bool inside (const Vector2& p);
  protected:
    GameManager* gameManager;

    //Called when a menuItems is touched
    virtual void actionPerformed (short UNUSED(touchedItem)) {};
    //Called when a a click happens OUTSIDE a menuItem
    virtual void actionPerformed () {};

    void drawItems();

    inline
    void addItem (MenuItem* item) { menuItems.append(item); }

    inline
    short getTouchedItem() const { return touchedItem; };

    inline
    size_t numItems () { return menuItems.length(); }

    inline
    MenuItem* getItem (size_t i) { return menuItems[i]; }

  private:
    vector<MenuItem*> menuItems;
    short touchedItem; //id of touched item, -1 if nothing has been touched

    //only valid if touchedItem != -1. Indicate when the last touched item was pressed
    uint64_t touchTime;
    bool isRepeating; //set to true when we start repeating the current touched item
};

#endif /* MENU_H_ */
