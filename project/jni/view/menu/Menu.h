#ifndef MENU_H_
#define MENU_H_

#include "def.h"
#include "lib/Vector2.h"
#include "MenuItem.h"
#include "containers/vector.h"

class GameManager;

/* base class for a menu screen */
class Menu {
  public:
    Menu (GameManager* gm)
      : gameManager(gm),
        menuItems(4),
        touchedItem(-1) {
    }

    virtual ~Menu ();

    virtual void draw () = 0;

    void handleTouchDown (const Vector2& p);
    void handleTouchUp (const Vector2& p);
  protected:
    GameManager* gameManager;

    //Called when a menuItems is touched
    virtual void actionPerformed (short touchedItem) {};
    //Called when a a click happens OUTSIDE a menuItem
    virtual void actionPerformed () {};

    void drawItems();

    inline
    void addItem (MenuItem* item) { menuItems.add(item); }

    inline
    short getTouchedItem() const { return touchedItem; };

    inline
    size_t numItems () { return menuItems.length(); }

    inline
    MenuItem* getItem (size_t i) { return menuItems[i]; }

  private:
    vector<MenuItem*> menuItems;
    short touchedItem; //id of touched item, -1 if nothing has been touched
};

#endif /* MENU_H_ */
