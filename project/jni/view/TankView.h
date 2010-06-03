#ifndef TANKVIEW_H_
#define TANKVIEW_H_

#include "logic/Tank.h"
#include "Sprite.h"
#include "GLW.h"

class TankView {
  public:
    TankView (const Tank* t) :
      tank(t),
      shieldSprite("assets/sprites/shield_1.png", TEX_GROUP_GAME) {
      tankSprite[0] = new Sprite("assets/sprites/tank1.png", TEX_GROUP_GAME);
      tankSprite[1] = new Sprite("assets/sprites/tank1_damaged_1.png", TEX_GROUP_GAME);
      tankSprite[2] = new Sprite("assets/sprites/tank1_damaged_2.png", TEX_GROUP_GAME);
      bounceTankSprite[0] = new Sprite("assets/sprites/tank_bounce.png", TEX_GROUP_GAME);
      bounceTankSprite[1] = new Sprite("assets/sprites/tank_bounce_damaged_1.png", TEX_GROUP_GAME);
      bounceTankSprite[2] = new Sprite("assets/sprites/tank_bounce_damaged_1.png", TEX_GROUP_GAME);
    }

    ~TankView () {
      for (int i=0; i<3; i++) {
        delete tankSprite[i];
        delete bounceTankSprite[i];
      }
    }

     // Return the display color for the given tank (depends on tanktype)
    static eColor getColor (eTankType type) {
      switch(type) {
        case TANK_PLAYER: return GREEN;
        case BOSS_SIMPLE:
        case TANK_SIMPLE: return RED;
        case BOSS_BOUNCE:
        case TANK_BOUNCE: return ORANGE;
        case TANK_STATIC: return GREY;
        case BOSS_BURST:
        case TANK_BURST: return VIOLET;
        case BOSS_SHIELD:
        case TANK_SHIELD: return YELLOW;
        default: LOGE("Unhandled tank type : %i", type); return WHITE;
      }
    }

    virtual void draw () {
      if (!tank->isAlive())
        return;
      eTankType type = tank->getTankType();
      const int state = abs(tank->getMaxLives()-tank->getLivesLeft());
      GLW::color(getColor(type));
      switch(type) {
        case TANK_PLAYER:
        case TANK_STATIC:
        case TANK_SIMPLE:
        case TANK_BURST:
        case BOSS_SIMPLE:
        case BOSS_BURST:
          tankSprite[state]->draw(*tank, tank->getRotation(), getTankScale());
          break;
        case TANK_SHIELD:
        case BOSS_SHIELD:
          tankSprite[state]->draw(*tank, tank->getRotation(), getTankScale());
          shieldSprite.draw(*tank, 0, 1.1f);
          break;
        case TANK_BOUNCE:
        case BOSS_BOUNCE:
          bounceTankSprite[state]->draw(*tank, tank->getRotation(), getTankScale());
          break;
      }
      GLW::colorWhite();
    }

    virtual float getTankScale () { return 1.0f; }

    Vector2 getCenter () {
      return tankSprite[0]->getCenter(*tank);
    }

    bool touchInside (const Vector2& p) const {
      return tankSprite[0]->touchInside(*tank, p);
    }
  protected:
    Sprite* getTankSprite () { return tankSprite[0]; }
    Sprite* getBounceTankSprite () { return bounceTankSprite[0]; }
  private:
    const Tank* tank;
    Sprite* tankSprite[3]; //0=undamaged, 1=damaged_1, 2=damaged_2
    Sprite* bounceTankSprite[3];//0=undamaged, 1_damaged_1, 2=damaged2
    Sprite shieldSprite;

};

#endif /* TANKVIEW_H_ */
