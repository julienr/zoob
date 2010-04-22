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
    Sprite gameOver;
    Sprite smiley;
    Sprite retry;
};


#endif /* WONMENU_H_ */
