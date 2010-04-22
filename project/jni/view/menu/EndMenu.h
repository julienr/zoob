#ifndef ENDMENU_H_
#define ENDMENU_H_

#include "Menu.h"

#define NUM_STARS 3

class EndMenu : public Menu {
  public:
    EndMenu (GameManager* gm);
    ~EndMenu () {
      for (int i=0; i<NUM_STARS; i++)
        delete stars[i];
    }
    void draw ();
  protected:
    void actionPerformed ();
  private:
    Sprite complete;
    Sprite starSprite;
    Sprite thx;

    uint64_t lastUpdate;

    struct _Star {
      _Star (const Vector2& p, const Vector2& s, float r, float rS)
        : pos(p), size(s), rot(r), rotSpeed(rS) {}
      Vector2 pos;
      Vector2 size;
      float rot;
      float rotSpeed;
    };
    _Star* stars[NUM_STARS];
};


#endif /* ENDMENU_H_ */
