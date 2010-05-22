#include "app-android.h"
#include "app.h"

JNIEXPORT void JNICALL Java_net_fhtagn_zoob_ZoobRenderer_nativeInit
  (JNIEnv * env, jclass cls, jstring apkPath) {
  const char* str;
  jboolean isCopy;
  str = env->GetStringUTFChars(apkPath, &isCopy);
  nativeInit(str);
}

JNIEXPORT void JNICALL Java_net_fhtagn_zoob_ZoobRenderer_nativeInitGL
  (JNIEnv *, jclass) {
    nativeInitGL();
}


JNIEXPORT void JNICALL Java_net_fhtagn_zoob_ZoobRenderer_nativeResize
  (JNIEnv *, jclass, jint w, jint h) {
  nativeResize(w,h);
}

JNIEXPORT void JNICALL Java_net_fhtagn_zoob_ZoobRenderer_nativeRender
  (JNIEnv *, jclass) {
  nativeRender();
}

JNIEXPORT void JNICALL Java_net_fhtagn_zoob_ZoobGLSurface_nativeMenu
  (JNIEnv *, jclass) {
  nativeMenu();
}

JNIEXPORT void JNICALL Java_net_fhtagn_zoob_ZoobGLSurface_nativePause
  (JNIEnv *, jclass) {
  nativePause();
}

JNIEXPORT void JNICALL Java_net_fhtagn_zoob_ZoobGLSurface_touchEventDown
  (JNIEnv *, jclass, jfloat x, jfloat y) {
  touchEventDown(x,y);
}

JNIEXPORT void JNICALL Java_net_fhtagn_zoob_ZoobGLSurface_touchEventMove
  (JNIEnv *, jclass, jfloat x, jfloat y) {
  touchEventMove(x,y);
}

JNIEXPORT void JNICALL Java_net_fhtagn_zoob_ZoobGLSurface_touchEventUp
  (JNIEnv *, jclass, jfloat x, jfloat y) {
  touchEventUp(x,y);
}

JNIEXPORT void JNICALL Java_net_fhtagn_zoob_ZoobGLSurface_touchEventOther
  (JNIEnv *, jclass, jfloat x, jfloat y) {
  touchEventOther(x,y);
}
