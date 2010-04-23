#ifndef PAUSEDMENU_H_
#define PAUSEDMENU_H_

#include "Menu.h"

class PausedMenu : public Menu {
  public:
    PausedMenu (GameManager* gm);
    ~PausedMenu () {}
    void draw ();
  protected:
    void actionPerformed ();
  private:
    Sprite paused;
    Sprite spriteContinue;
};


#endif /* PAUSEDMENU_H_ */
