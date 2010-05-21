#include "NumberView.h"

NumberView* NumberView::instance = NULL;

NumberView::NumberView () {
  numbers[0] = new Sprite("assets/sprites/menuitems/digits/0.png");
  numbers[1] = new Sprite("assets/sprites/menuitems/digits/1.png");
  numbers[2] = new Sprite("assets/sprites/menuitems/digits/2.png");
  numbers[3] = new Sprite("assets/sprites/menuitems/digits/3.png");
  numbers[4] = new Sprite("assets/sprites/menuitems/digits/4.png");
  numbers[5] = new Sprite("assets/sprites/menuitems/digits/5.png");
  numbers[6] = new Sprite("assets/sprites/menuitems/digits/6.png");
  numbers[7] = new Sprite("assets/sprites/menuitems/digits/7.png");
  numbers[8] = new Sprite("assets/sprites/menuitems/digits/8.png");
  numbers[9] = new Sprite("assets/sprites/menuitems/digits/9.png");
}

NumberView::~NumberView () {
  for (int i=0; i<10; i++)
    delete numbers[i];
}

void NumberView::drawInt (int val, const Vector2& pos, const Vector2& size) {
  int numDigits = 1 + val/10;
  //numWidth is the visible width of the number (since most of the texture is transparent. 0.35f is
  //a hardcoded empirical constante
  float digitWidth = 0.35f*size.x;
  //pos is center, startX is rightmost digit position
  float x = pos.x+(numDigits*digitWidth)/2.0f;
  //LOGE("numDigits : %i, digitWidth %f => x %f", numDigits, digitWidth, x);
  //draw digits, right to left
  do {
    const int digit = val%10;
    //LOGE("digit : %i", digit);
    numbers[digit]->draw(Vector2(x, pos.y), size);
    x -= digitWidth;
    val /= 10;
  } while (val != 0);
}
