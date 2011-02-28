#ifndef APP_H_
#define APP_H_

#include "zoobdef.h"
#include "lib/Vector2.h"


//We work with a forced 1.5 aspect ratio for the game area => [15,10] viewport
#define VIEWPORT_WIDTH 15
#define VIEWPORT_HEIGHT 10

// The screen is divided as follow :
// - the biggest area is the window
// - then comes the viewport that contains EVERYTHING that we ever render. it is
//   COMPLETELY FORBIDDEN to render outside the viewport since it won't be visible on some devices
//   depending on the screen ratio
// - then comes the game area where the actual game (level, tanks) are rendered
// |----------------------------------|
// |      |-----------------|         |
// |      |    |-----| life |         |
// |      |    | game| pad  |         |
// |      |    |-----|      |         |
// |      |-----------------|         |
// |                                  |
// |----------------------------------|

//This function should give access to the underlying AppInterface implementation. 
//If it is called and no AppInterface has been allocated yet, it should allocate it.
//It has the same semantic as a Singleton getInstance ()
class AppInterface;
AppInterface* getApp();

class InputManager;
class FileManager;
class Level;
class GameView;

// AppInterface exposes methods that allow bidirectionnal communication between Game (C++) 
// and GUI (Android Java, QT C++, etc...)
// It exposes both downcalls (GUI -> Game) and upcalls (Game -> GUI)
class AppInterface {
  public:
    //Create a new instance for the given serie
    //You MUST call initGL after this constructor as soon as OpenGL context is available
    AppInterface (FileManager* fm, const char* serieJSON);
    virtual ~AppInterface (); 

    //BEGIN downcalls 

    //Performs all OpenGL-related initialisation. Can also be used to register state 
    //(levelLimit, difficulty) changes
    //Not that this method might be called multiple times and it should handle it fine.
    //
    //For example, on Android, after the application resume from a pause, all the OpenGL context
    //should be reinitialised. Therefore, this function is called again
    //  - 'levelLimit' specify the level limit (the player can choose levels in the range [0, level limit])
    //  - 'difficulty' specify the difficulty
    //  - 'useGamepad' if true, a virtual gamePad will be displayed
    //  - 'useTrackball' if true, a cursor controlled by the trackball will be displayed
    void initGL (int levelLimit, int difficulty, bool useGamepad, bool useTrackball);

    //Resizes the game viewport
    void resize (int newW, int newH);


    //Perform logic simulation
    void simulate ();

    //Draws the game 
    void render ();

    //Pauses game simulation
    void pause ();

    //Stops the current game
    void stopGame ();

    //Starts a multiplayer server. Returns false on error
    bool startServer ();

    //Starts a multiplayer client that will connect to the given server. Returns false on error
    bool startClient (const char* serverAddr);

    //Start a new game at a given level
    void startGame (int level);

    //(Cheat) Toggle god mode
    void toggleGodMode ();

    //Controls what debug information is shown
    enum eDebug {
      DEBUG_VISIBILITY=1,
      DEBUG_SHADOWS=1<<1,
      DEBUG_AI=1<<2,
      DEBUG_COLLISIONS=1<<3,
      DEBUG_OVERLAYS=1<<4
    };
    void enableDebug (eDebug what);
    void disableDebug (eDebug what);


    //END downcalls
    
    //BEGIN upcalls

    //Saves the player progress in the current serie. 'level' indicate the _next_ level
    //the player will be able to play
    virtual void saveProgress (int level) = 0;

    //WARNING: this has to be kept in sync with java code
    enum eMenu {
      MENU_MAIN=0,
      MENU_WON,
      MENU_LOST,
      MENU_END,
      MENU_ERROR,
      MENU_LAST,
    };

    //Asks the GUI to display the given Menu. Also pass the current level along
    virtual void showMenu (eMenu id, int currentLevel) = 0;

    //END upcalls
    
    //This function should allocate a new InputManager and return it. It shouldn't 
    //register the new input manager.
    //It is guaranted that this function will be called at most once (at the
    //very beginning of the app livecycle. It is Called just after openGL initialisation
    //so the input manager can allocate openGL resources
    virtual InputManager* createInputManager (bool useGamepad, bool useTrackball) = 0;

    //Utility functions to transform window coordinates to our internal coordinate system
    //transforms x/y screen coordinate to game coordinate
    float XSG (const float x) const;
    float YSG (const float y) const;

    //transforms x/y screen coordinate to game coordinate without applying a translation of half a sprite size
    float XSG_NOTRANSX(const float x) const;
    float YSG_NOTRANSY(const float y) const;

    //transforms x/y game coordinate to screen coordinate
    float XGS (const float x) const;
    float YGS (const float y) const;

    float getXScreenToGame () const { return xScreenToGame; }
    float getYScreenToGame () const { return yScreenToGame; }

    float getTransX() const { return transX; }
    float getTransY() const { return transY; }

    //"Callbacks" for game events
    void onStartGame ();
    void onGameOver ();
    void onGameWon ();
    void onGameUnPaused ();

    //immediatly start playing a new level
    void startLevel(const char* levelJSON);
    
    //State change
    void toPlayingState ();
    void toWonState ();
    void toLostState ();
    void toEndState ();
    void toPauseState ();

  private:
    //Center game area in viewport. Updates transX, transY
    void _centerGameInViewport ();

    //Calculates viewportMargin and viewportScreenDim so we keep a 1.5 aspect ratio
    //for the game area. sW, sH are screen size (in screen coordinates)
    void _forceRatio (float sW, float sH);

    //Stop the current game and goes to STATE_NONE
    void _cleanupGame ();

    //Since initGL will be called on app creation AND each time the opengl 
    //context is recreated, this indicate if we're at app creation (false) or
    //just in GL context recreation (true) 
    bool initialized;

    //ratio between the screen width/height in gamme coordinate and screen width/height in screen coordinate
    //=> to convert x (screen coords) to game coords, simply do x*xScreenToGame
    //Note that with desktop OpenGL, we could achieve the same by retrieving projection/viewport
    //matrix, but OpenGL ES doesn't support that, plus we are in 2D, so it's pretty easy
    float xScreenToGame;
    float yScreenToGame;

    //The margin on (x,y) (screen coords) on each side of the game area, for rendering
    float transX;
    float transY;

    //viewport margin relative to the screen (in screen coords). This should be used only for input
    //since viewport placement is handled by opengl glViewport
    Vector2 viewportMargin;

    //viewport dimensions in screen coordinates
    Vector2 viewportScreenDim;

    int windowWidth;
    int windowHeight;

    int debugFlags;

    Level* lvl;
    GameView* gameView;
};

#endif
