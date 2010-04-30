#ifndef APP_H_
#define APP_H_

//performs all non-gl initialisation
void nativeInit (const char* apkPath);

void nativeInitGL ();
void nativeQuit ();
void nativeResize (int w, int h);
void nativeRender ();
void nativeMenu();
void nativePause ();

void touchEventDown (float x, float y);
void touchEventMove (float x, float y);
void touchEventUp (float x, float y);
void touchEventOther (float x, float y);

//Utility functions to transform window coordinates to our internal coordinate system
float XSG (const float x);
float YSG (const float y);
float XSG_NOTRANSX(const float x);
float YSG_NOTRANSY(const float y);

extern float transX;
extern float transY;

#endif
