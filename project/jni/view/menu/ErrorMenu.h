#ifndef ERRORMENU_H_
#define ERRORMENU_H_

#include "Menu.h"

class ErrorMenu : public Menu {
  public:
    ErrorMenu (GameManager* gm);
    ~ErrorMenu ();

    void draw ();
  protected:
    void actionPerformed();
  private:
    Sprite error;
    Sprite continueSprite;
};

#endif /* ERRORMENU_H_ */
