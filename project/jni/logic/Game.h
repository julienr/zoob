#ifndef GAME_H_
#define GAME_H_

#include "def.h"
#include "logic/Tank.h"
#include "logic/EnemyTank.h"
#include "logic/PlayerTank.h"
#include "logic/Rocket.h"
#include "logic/Level.h"
#include "levels/LevelsData.h"
#include "logic/physics/CollisionManager.h"
#include "lib/Utils.h"
#include "containers/vector.h"
#include "logic/ShadowPolygon.h"
#include "logic/VisibilityGrid.h"
#include "ai/algorithms/PathFinder.h"
#include "lib/Color.h"
#include "logic/PlayerCommand.h"

//Each boss level start with an intro. This is the duration of the intro
#define BOSS_INTRO_TIME 3

enum eGameState {
  GAME_RUNNING,
  GAME_PAUSED
};

typedef void (*game_callback_t) ();

struct ExplosionLocation {
    enum eType {
        EXPLOSION_BOOM,
        EXPLOSION_POOF
    };
    ExplosionLocation (const Vector2& p, eType t)
      : position(p), type(t) {}
    Vector2 position;
    eType type;
};

//DEBUG only: draw an overlay of the given color over a cell
struct CellOverlay {
    const int x;
    const int y;
    const eColor color;
    CellOverlay (int x, int y, eColor col)
      : x(x), y(y), color(col) {}
};

//DEBUG only: a path that can be drawn
struct DebugPath {
    const Path* path;
    const eColor color;
    DebugPath (Path* path, eColor col)
      : path(path->copy()), color(col) {}

    ~DebugPath () {
      delete path;
    }
  private:
    DebugPath (const DebugPath& o)
      : path(NULL), color(o.color) {}
};

struct DebugPolygon {
    const Polygon poly;
    const eColor color;
    DebugPolygon (const Polygon& p, eColor col)
      : poly(p), color(col) {}
};

class Game {
  private:
    static Game* instance;
    Game (game_callback_t overCallback,
          game_callback_t wonCallback,
          Level* level);
    ~Game ();
  public:
    static Game* getInstance() {
      return instance;
    }

    //overCallback : the function to call when game is over
    //wonCallback : the function to call when game is won
    static void create (game_callback_t overCallback,
                        game_callback_t wonCallback,
                        Level* level) {
      ASSERT(!instance);
      instance = new Game(overCallback, wonCallback, level);
    }

    static void destroy () {
      delete instance;
      instance = NULL;
    }

    void unpause () {
      gameState = GAME_RUNNING;
    }

    void pause () {
      gameState = GAME_PAUSED;
    }

    bool isPaused () {
      return gameState == GAME_PAUSED;
    }

    void applyCommands (Tank* tank, const PlayerCommand& cmd);

    PlayerTank* getPlayerTank () const {
      return playerTank;
    }

    const list<Tank*>* getTanks () const {
      return &tanks;
    }

    const Level* getLevel () const {
      return level;
    }

    const CollisionManager& getColManager () const {
      return colManager;
    }

    const list<Rocket*>::const_iterator getRockets () const {
      return rockets.begin();
    }
    
    const list<Bomb*>::const_iterator getMines () const {
      return bombs.begin();
    }

    const vector<ShadowPolygon*>& getPlayerShadows () const {
      return playerShadows;
    }

    //Returns the explosions that have to be displayed. The caller should delete the explosions
    //hes has handled
    list<ExplosionLocation>& getExplosions () {
      return explosions;
    }

    void touch (Entity* e1, Entity* e2, const Vector2& colPoint);

    void update(const double elapsedS);

    void toggleGodMode () {
      godMode = !godMode;
      LOGE("god mode : %i", godMode);
    }

    bool hasShadows () const {
      return calculateShadows;
    }

    VisibilityGrid& getPlayerVisibility () {
      return playerVisibility;
    }

    const VisibilityGrid& getPlayerVisibility () const {
      return playerVisibility;
    }

    bool inIntro () const {
      return !introDone;
    }

    double getIntroTimeLeft () const {
      return introTimeLeft;
    }

    PathFinder* getPathFinder () {
      return &pathFinder;
    }

    //DEBUG only: returns a list of grid cells to be drawn in overlay
#ifdef DEBUG
    const list<CellOverlay>& dbg_getCellOverlays () {
      return dbg_overlays;
    }

    void dbg_addCellOverlay (CellOverlay o) {
      dbg_overlays.append(o);
    }


    const list<DebugPath*>& dbg_getDebugPaths () {
      return dbg_paths;
    }

    void dbg_addDebugPath (DebugPath* d) {
      dbg_paths.append(d);
    }

    void dbg_addDebugPolygon (DebugPolygon d) {
      dbg_polys.append(d);
    }

    const list<DebugPolygon>& dbg_getDebugPolygons () {
      return dbg_polys;
    }
#endif

  protected:
    //These are functions that are called when a rocket/mine/tank is
    //added/removed (destroyed) from the game. This allow subclasses
    //to keep track of them.
    //The subclass MUST call the superclass method before proceeding
    virtual void addRocket (Rocket* r);
    virtual list<Rocket*>::iterator deleteRocket (const list<Rocket*>::iterator& i);
    virtual void addTank (Tank* t);
    virtual list<Tank*>::iterator deleteTank (const list<Tank*>::iterator& i);
    virtual void addBomb (Bomb* b);
    virtual list<Bomb*>::iterator deleteBomb (const list<Bomb*>::iterator& i);

  private:
    void _handleTriggers ();
    void _updateRockets (double elapsedS);
    void _updateBombs (double elapsedS);
    int _updateTanks (double elapsedS);
    void _updatePlayer (double elapsedS);

    void _doAI (EnemyTank* tank, double elapsedS);

    void _tankFire (Tank* tank, const Vector2& aimPosition);
    void _tankDropBomb (Tank* tank);
    void _tankActivateShield (Tank* tank) {
      tank->startShield();
    }

    /*void playerFire (const Vector2& cursorPosition);
    void playerDropBomb ();

    void playerActivateShield () {
      playerTank->startShield();
    }*/

    //Move and rotate the tank according to t->getMoveDir and calls slideMove
    void doTankMove (Tank* t, double elapsedS);

    void doFireRocket (Tank* t, const Vector2& dir);

    //translate the given entity of <move>, sliding against collider
    void slideMove (Entity* e, Vector2 move);

    //translate the given rocket, bouncing against wall (and registering the bounce in the rocket)
    void bounceMove (Rocket* r, Vector2 move);

    void _calculatePlayerShadows ();

    CollisionManager colManager;
    PlayerTank* playerTank;
    list<Tank*> tanks;
    list<Rocket*> rockets;
    list<Bomb*> bombs;
    list<ExplosionLocation> explosions;
    Level* level;

    //elapsed time for last calculated frame
    double elapsedS;

    Vector2 tankMoveDir;

    game_callback_t gameOverCallback;
    game_callback_t gameWonCallback;

    eGameState gameState;

    bool godMode;

    //shadows calculated using the player as a light source
    vector<ShadowPolygon*> playerShadows;
    //Shadow calculations might be disabled
    bool calculateShadows;

    VisibilityGrid playerVisibility;

    //for AI
    PathFinder pathFinder;

    double introTimeLeft;
    bool introDone;

#ifdef DEBUG
    void dbg_clear ();
    list<CellOverlay> dbg_overlays;
    list<DebugPath*> dbg_paths;
    list<DebugPolygon> dbg_polys;
#endif
};

#endif /* GAME_H_ */
