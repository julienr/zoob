#ifndef WONMENU_H_
#define WONMENU_H_

#include "Menu.h"

class WonMenu : public Menu {
  public:
    WonMenu (GameManager* gm);
    ~WonMenu () {}
    void draw ();
  protected:
    void actionPerformed ();
  private:
    Sprite complete;
    Sprite smiley;
    Sprite nextlvl;
};


#endif /* WONMENU_H_ */
