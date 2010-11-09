#ifndef _NETWORKEDGAME_H
#define _NETWORKEDGAME_H

#include "logic/Game.h"
#include "net/Messages.h"
#include "containers/map.h"

class NetworkedGame : public Game {
  public:
    void applyGameState (const zoobmsg::GameState* state);

    void applyCommands (Tank* tank, const PlayerCommand& cmd);

    void update(const double elapsedS);

    static void create (game_callback_t overCallback,
                        game_callback_t wonCallback,
                        Level* level) {
      registerInstance(new NetworkedGame(overCallback, wonCallback, level));
    }

  protected:
    virtual void addRocket (Rocket* r);
    virtual list<Rocket*>::iterator deleteRocket (const list<Rocket*>::iterator& i);
    virtual void addTank (Tank* t);
    virtual list<Tank*>::iterator deleteTank (const list<Tank*>::iterator& i);
    virtual void addBomb (Bomb* b);
    virtual list<Bomb*>::iterator deleteBomb (const list<Bomb*>::iterator& i);

  private:
    NetworkedGame (game_callback_t overCallback,
                   game_callback_t wonCallback,
                   Level* level) 
      : Game(overCallback, wonCallback, level) {}

    map<uint16_t, Tank*> tanksByID;
    map<uint16_t, Rocket*> rocketsByID;
    map<uint16_t, Bomb*> bombsByID;
};

#endif
