#include "app-android.h"
#include "app.h"
#include "def.h"

static JNIEnv* jniEnv = NULL;

static jclass zoobClass;
static jmethodID java_saveProgress;

void saveProgress (int level) {
  LOGE("NDK saveProgress, level %i", level);
  jniEnv->CallStaticVoidMethod(zoobClass, java_saveProgress, level);
}

//performs necessary initialization for upcalls (c->java) to work
static void init_for_upcall (JNIEnv* env) {
  jniEnv = env;
  zoobClass = jniEnv->FindClass("net.fhtagn.zoob/Zoob");
  if (zoobClass == NULL)
    LOGE("Unable to find net.fhtagn.Zoob Java class from JNI");

  java_saveProgress = jniEnv->GetStaticMethodID(zoobClass, "saveProgress", "(I)V");
  if (java_saveProgress == NULL)
    LOGE("Unable to get method id for saveProgress from JNI");
}

JNIEXPORT void JNICALL Java_net_fhtagn_zoob_ZoobRenderer_nativeInit
  (JNIEnv * env, jclass cls, jstring apkPath) {
  ASSERT(jniEnv == NULL);
  init_for_upcall(env);
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
