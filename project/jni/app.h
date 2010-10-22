#ifndef APP_H_
#define APP_H_

//downcall (Java to C++) functions
//performs all non-gl initialisation
void nativeInit (const char* apkPath, const char* serieJSON);
void nativeLoadSerie (const char* serieJSON);

void nativeInitGL (int level, int difficulty, int useGamepad, int useTrackball);
void nativeQuit ();
void nativeResize (int w, int h);
void nativeRender ();
void nativePause ();
void nativeStopGame ();

void startGame (int level);

void toggleGodMode ();

//upcall (C++ to Java) functions
void saveProgress (int level);

//WARNING: this has to be kept in sync with java code
enum eMenu {
    MENU_MAIN=0,
    MENU_WON,
    MENU_LOST,
    MENU_END,
    MENU_ERROR,
    MENU_LAST,
};
void showMenu (int id, int currentLevel);

//Utility functions to transform window coordinates to our internal coordinate system
float XSG (const float x);
float YSG (const float y);
float XSG_NOTRANSX(const float x);
float YSG_NOTRANSY(const float y);

//game to screen
float XGS (const float x);
float YGS (const float x);

extern float xScreenToGame;
extern float yScreenToGame;

extern float transX;
extern float transY;

//This function MUST be defined by the <platform>-app file and it must
//allocate a new InputManager and return it. It shouldn't REGISTER the
//new input manager
//It is GUARANTEED that this function will be called at most once (at the
//very beginning of the app livecycle. Called just after openGL initialisation
//so the input manager can allocate openGL resources
class InputManager;
extern InputManager* createInputManager (int useGamepad, int useTrackball);

//DEBUG functions
enum eDebug {
  DEBUG_VISIBILITY=1,
  DEBUG_SHADOWS=1<<1,
  DEBUG_AI=1<<2,
  DEBUG_COLLISIONS=1<<3,
  DEBUG_OVERLAYS=1<<4
};
void enableDebug (eDebug what);
void disableDebug (eDebug what);

#endif
