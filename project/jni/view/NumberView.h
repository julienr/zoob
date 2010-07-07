#ifndef NUMBERVIEW_H_
#define NUMBERVIEW_H_

#include "view/Sprite.h"

class NumberView {
  public:
    static void destroy () {
      delete instance;
    }

    static NumberView* getInstance () {
      if (!instance)
        instance = new NumberView();
      return instance;
    }

    /* if multiple digits need to be rendered, they will all have size 'size'
     * and be centered in 'pos'
     */
    void drawInt (int val, const Vector2& pos, const Vector2& size);

  private:
    static NumberView* instance;

    NumberView ();
    ~NumberView ();
    Sprite* numbers[10];
};

#endif /* NUMBERVIEW_H_ */
