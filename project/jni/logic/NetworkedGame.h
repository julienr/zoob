#ifndef _NETWORKEDGAME_H
#define _NETWORKEDGAME_H

#include "logic/Game.h"
#include "net/Messages.h"
#include "containers/map.h"

class NetworkedGame : public Game {
  public:
    void applyGameState (const zoobmsg::GameState& state);

    void applyCommands (uint16_t id, const PlayerCommand& cmd);
    void applyCommands (Tank* tank, const PlayerCommand& cmd);

    void update(const double elapsedS);

    static void create (game_callback_t overCallback,
                        game_callback_t wonCallback,
                        Level* level) {
      LOGI("[NetworkedGame] create");
      registerInstance(new NetworkedGame(overCallback, wonCallback, level));
    }

    virtual void touch (Entity* e1, Entity* e2, const Vector2& colPoint);
    virtual void multiTouch (Entity* source, const list<Entity*>& touched, const Vector2& colPoint);

    virtual const char* getPlayerName (uint16_t tankID) {
      if(tanksToName.contains(tankID))
        return tanksToName.get(tankID);
      else
        return NULL;
    }

  protected:
    void addRocket (Rocket* r);
    list<Rocket*>::iterator deleteRocket (const list<Rocket*>::iterator& i);
    void addTank (Tank* t);
    list<Tank*>::iterator deleteTank (const list<Tank*>::iterator& i);
    void addBomb (Bomb* b);
    list<Bomb*>::iterator deleteBomb (const list<Bomb*>::iterator& i);

    void explode (const ExplosionInfo& info);

    bool isGameOver () const;
    bool isGameWon (int numAlives) const;

    void spawnTanks (const Level* level, Vector2& playerStartPosition);
    void spawnPlayer ();

  private:
    NetworkedGame (game_callback_t overCallback,
                   game_callback_t wonCallback,
                   Level* level) 
      : Game(overCallback, wonCallback, level) {}

    Entity* getEntityByID (uint16_t id);

    map<uint16_t, char*> tanksToName;

    map<uint16_t, Tank*> tanksByID;
    map<uint16_t, Rocket*> rocketsByID;
    map<uint16_t, Bomb*> bombsByID;
};

#endif
