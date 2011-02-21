#ifndef GAME_H_
#define GAME_H_

#include "zoobdef.h"
#include "logic/Tank.h"
#include "logic/EnemyTank.h"
#include "logic/PlayerTank.h"
#include "logic/Rocket.h"
#include "logic/Level.h"
#include "levels/LevelManager.h"
#include "logic/physics/CollisionManager.h"
#include "lib/Utils.h"
#include "containers/vector.h"
#include "logic/ShadowPolygon.h"
#include "logic/VisibilityGrid.h"
#include "ai/algorithms/PathFinder.h"
#include "lib/Color.h"
#include "logic/PlayerCommand.h"
#include "iview/ExplosionInfo.h"

#include "iview/IGameView.h"

//Each boss level start with an intro. This is the duration of the intro
#define BOSS_INTRO_TIME 3

enum eGameState {
  GAME_RUNNING,
  GAME_PAUSED
};

typedef void (*game_callback_t) ();

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
  protected:
    Game (game_callback_t overCallback,
          game_callback_t wonCallback,
          Level* level);
    virtual ~Game ();
    
    static void registerInstance(Game* game) {
      LOGI("[Game::registerInstace] instance=0x%p", game);
      instance = game;
      game->_construct();
    }
  public:
    static Game* getInstance() {
      return instance;
    }

    //overCallback : the function to call when game is over
    //wonCallback : the function to call when game is won
    static void create (game_callback_t overCallback,
                        game_callback_t wonCallback,
                        Level* level) {
      LOGI("[Game] create");
      registerInstance(new Game(overCallback, wonCallback, level));
    }

    static void destroy () {
      delete instance;
      instance = NULL;
    }
    
    void attach (IGameView* view);

    void unpause () {
      gameState = GAME_RUNNING;
    }

    void pause () {
      gameState = GAME_PAUSED;
    }

    bool isPaused () {
      return gameState == GAME_PAUSED;
    }

    //Try to spawn a tank of size entityRadius. The newTank() function is called
    //to allocate the tank
    //returns true if a spawn position was found and the tank spawned. tank contains the newly allocated tank
    //returns false otherwise
    bool findSpawnPosition (float entityRadius, Vector2& position);
    bool spawnTank (float entityRadius, Tank* (*newTank) (void), Tank*& tank);

    virtual void applyCommands (uint16_t /*tankID*/, const PlayerCommand& /*cmd*/) {
      LOGE("[Game::applyCommands (uint16_t id)] By ID not implemented");
    }
    virtual void applyCommands (Tank* tank, const PlayerCommand& cmd);

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

    list<Rocket*>::const_iterator getRockets () const {
      return rockets.begin();
    }
    
    list<Bomb*>::const_iterator getMines () const {
      return bombs.begin();
    }

    const vector<ShadowPolygon*>& getPlayerShadows () const {
      return playerShadows;
    }

    //Accumulator is used for physic simulation
    //See nativeRender's physic simulation loop
    void accumulate (double elapsedS) {
      accumulator += elapsedS;
    }

    void useAccumulatedTime (double dt) {
      accumulator -= dt;
    }

    void resetAccumulator () {
      accumulator = 0;
    }

    double getAccumulator () {
      return accumulator;
    }

    //Touch occurs between e1 and e2
    virtual void touch (Entity* e1, Entity* e2, const Vector2& colPoint);

    //Touch all the entities in the given list
    virtual void multiTouch (Entity* source, const list<Entity*>& touched, const Vector2& colPoint);

    virtual void update(const double elapsedS);

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

    void playerSpawned (PlayerTank* tank);

    FireRatePolicy* newPlayerFirePolicy () {
      return new IntervalFirePolicy(ProgressionManager::getInstance()->getPlayerFireInterval());
    }

    float getPlayerRocketSpeed () {
      return ProgressionManager::getInstance()->getPlayerRocketsSpeed();
    }

    //Return player's name for a given tank. Might return NULL if name is not available
    //(like for local player or AI tanks)
    virtual const char* getPlayerName (uint16_t tankID) {
      return NULL;
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

    virtual void explode (const ExplosionInfo& info);

    virtual bool isGameOver () const;
    virtual bool isGameWon (int numAlives) const;

    //Spawn tanks based on the level description. Might be overloaded (for example
    // by client NetworkedGame to spawn nothing). Might also modify playerStartPosition
    virtual void spawnTanks (const Level* level, Vector2& playerStartPosition);

    //Request to spawn a player tank. Should call playerSpawned when the spawn is ready
    virtual void spawnPlayer ();

    //Send the tank position. It is ASSUMED that the position has been checked
    //for safety and no collision check is therefore performed.
    //The new position is also directly saved as the last safe position
    void setAuthoritativeTankPosition (Tank* tank, const Vector2& newPos);
    void setAuthoritativeRocketPosition (Rocket* rocket, const Vector2& newPos);
    void setAuthoritativeBombPosition (Bomb* bomb, const Vector2& newPos);

    //Returns a set containing all rockets/tanks/bombs id
    void getRocketsID (set<uint16_t>& ids) const;
    void getTanksID (set<uint16_t>& ids) const;
    void getBombsID (set<uint16_t>& ids) const;

  private:
    //Game is using some kind of two-phases construction. SpawnTanks is called
    //during construction and relies on addRocket/addTank which can be overriden.
    //But since the Game class will be created before subclass (NetworkedGame), 
    //Game's addRocket/Tank will be called. Therefore, this _construct method is
    //called automatically in registerInstance
    void _construct ();

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

    bool constructed;
    double accumulator;

    CollisionManager colManager;
    PlayerTank* playerTank;
    list<Tank*> tanks;
    list<Rocket*> rockets;
    list<Bomb*> bombs;
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

    IGameView* attachedView;

    //Only used in single-player
    Vector2 playerStartPosition;

#ifdef DEBUG
    void dbg_clear ();
    list<CellOverlay> dbg_overlays;
    list<DebugPath*> dbg_paths;
    list<DebugPolygon> dbg_polys;
#endif
};

#endif /* GAME_H_ */
