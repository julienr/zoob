#ifndef APP_H_
#define APP_H_

//downcall (Java to C++) functions
//performs all non-gl initialisation
void nativeInit (const char* apkPath);

void nativeInitGL (int level, int difficulty);
void nativeQuit ();
void nativeResize (int w, int h);
void nativeRender ();
void nativeMenu();
void nativePause ();
bool isInMenu ();

void touchEventDown (float x, float y);
void touchEventMove (float x, float y);
void touchEventUp (float x, float y);
void touchEventOther (float x, float y);

void toggleGodMode ();

//upcall (C++ to Java) functions
void saveProgress (int level);
void saveDifficulty (int diff);
void buyFull ();

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

#endif
