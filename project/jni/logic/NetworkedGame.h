#ifndef _NETWORKEDGAME_H
#define _NETWORKEDGAME_H

#include "logic/Game.h"
#include "net/Messages.h"
#include "containers/map.h"

class NetworkedGame : public Game {
  public:
    void applyGameState (const zoobmsg::GameState* state);
  protected:
    virtual void addRocket (Rocket* r);
    virtual list<Rocket*>::iterator deleteRocket (const list<Rocket*>::iterator& i);
    virtual void addTank (Tank* t);
    virtual list<Tank*>::iterator deleteTank (const list<Tank*>::iterator& i);
    virtual void addBomb (Bomb* b);
    virtual list<Bomb*>::iterator deleteBomb (const list<Bomb*>::iterator& i);

  private:
    map<uint16_t, Tank*> tanksByID;
    map<uint16_t, Rocket*> rocketsByID;
    map<uint16_t, Bomb*> bombsByID;
};

#endif
