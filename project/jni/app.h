#ifndef APP_H_
#define APP_H_

void nativeInit (const char* apkPath);
void nativeResize (int w, int h);
void nativeRender ();
void nativePause ();
void touchEventDown (float x, float y);
void touchEventMove (float x, float y);
void touchEventUp (float x, float y);
void touchEventOther (float x, float y);

#endif
