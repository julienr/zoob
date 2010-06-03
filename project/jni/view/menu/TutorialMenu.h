#ifndef TUTORIALMENU_H_
#define TUTORIALMENU_H_

#include "Menu.h"

class TutorialMenu : public Menu {
  public:
    TutorialMenu (GameManager* gm);
    ~TutorialMenu () {}
    void draw();
  protected:
    void actionPerformed ();
  private:
    Sprite text;
};

#endif /* TUTORIALMENU_H_ */
