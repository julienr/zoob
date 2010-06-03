#ifndef BUYFULLMENU_H_
#define BUYFULLMENU_H_

#include "Menu.h"

class BuyFullMenu : public Menu {
  public:
    BuyFullMenu (GameManager* gm);
    ~BuyFullMenu () {}
    void draw ();
  protected:
    void actionPerformed (short touchedItem);
  private:
    void _actionYes();
    void _actionNo();
    Sprite text;
};

#endif /* BUYFULLMENU_H_ */
