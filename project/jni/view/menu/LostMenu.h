#ifndef LOSTMENU_H_
#define LOSTMENU_H_

#include "Menu.h"

class LostMenu : public Menu {
  public:
    LostMenu (GameManager* gm);
    ~LostMenu () {}
    void draw ();
  protected:
    void actionPerformed ();
  private:
    Sprite gameOver;
    Sprite smiley;
    Sprite retry;
};

#endif /* LOSTMENU_H_ */
