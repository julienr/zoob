#include "app-android.h"
#include "app.h"

JNIEXPORT void JNICALL Java_net_fhtagn_moob_MoobRenderer_nativeInit
  (JNIEnv * env, jclass cls, jstring apkPath) {
  const char* str;
  jboolean isCopy;
  str = env->GetStringUTFChars(apkPath, &isCopy);
  nativeInit(str);
}

JNIEXPORT void JNICALL Java_net_fhtagn_moob_MoobRenderer_nativeResize
  (JNIEnv *, jclass, jint w, jint h) {
  nativeResize(w,h);
}

JNIEXPORT void JNICALL Java_net_fhtagn_moob_MoobRenderer_nativeRender
  (JNIEnv *, jclass) {
  nativeRender();
}

JNIEXPORT void JNICALL Java_net_fhtagn_moob_MoobGLSurface_nativeMenu
  (JNIEnv *, jclass) {
  nativeMenu();
}

JNIEXPORT void JNICALL Java_net_fhtagn_moob_MoobGLSurface_touchEventDown
  (JNIEnv *, jclass, jfloat x, jfloat y) {
  touchEventDown(x,y);
}

JNIEXPORT void JNICALL Java_net_fhtagn_moob_MoobGLSurface_touchEventMove
  (JNIEnv *, jclass, jfloat x, jfloat y) {
  touchEventMove(x,y);
}

JNIEXPORT void JNICALL Java_net_fhtagn_moob_MoobGLSurface_touchEventUp
  (JNIEnv *, jclass, jfloat x, jfloat y) {
  touchEventUp(x,y);
}

JNIEXPORT void JNICALL Java_net_fhtagn_moob_MoobGLSurface_touchEventOther
  (JNIEnv *, jclass, jfloat x, jfloat y) {
  touchEventOther(x,y);
}
