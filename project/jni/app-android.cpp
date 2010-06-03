#include "app-android.h"
#include "app.h"
#include "def.h"

static JNIEnv* jniEnv = NULL;

static jclass zoobClass;
static jobject zoobAppObj;
static jmethodID java_saveProgress;
static jmethodID java_saveDifficulty;

void saveProgress (int level) {
  jniEnv->CallVoidMethod(zoobAppObj, java_saveProgress, level);
}

void saveDifficulty (int diff) {
  jniEnv->CallVoidMethod(zoobAppObj, java_saveDifficulty, diff);
}

#define JNI_GET_METHOD(var,name,type) \
  var = jniEnv->GetMethodID(zoobClass,name,type); \
    if (var == NULL) \
      LOGE("Unable to get method id for "name" from JNI");


//performs necessary initialization for upcalls (c->java) to work
static void init_for_upcall (JNIEnv* env, jobject zoob) {
  jniEnv = env;
  zoobAppObj = zoob;
  zoobClass = jniEnv->FindClass("net.fhtagn.zoob.ZoobApplication");
  if (zoobClass == NULL)
    LOGE("Unable to find net.fhtagn.Zoob Java class from JNI");

  if (!jniEnv->IsInstanceOf(zoobAppObj, zoobClass))
    LOGE("Zoob app not instance of ZoobApplication");

  JNI_GET_METHOD(java_saveProgress, "saveProgress", "(I)V");
  JNI_GET_METHOD(java_saveDifficulty, "saveDifficulty", "(I)V");
}

JNIEXPORT void JNICALL Java_net_fhtagn_zoob_ZoobRenderer_nativeInit
  (JNIEnv * env, jclass cls, jstring apkPath, jobject zoob) {
  ASSERT(jniEnv == NULL);
  init_for_upcall(env, zoob);
  const char* str;
  jboolean isCopy;
  str = env->GetStringUTFChars(apkPath, &isCopy);
  nativeInit(str);
}

JNIEXPORT void JNICALL Java_net_fhtagn_zoob_ZoobRenderer_nativeInitGL
  (JNIEnv *, jclass, int level, int difficulty) {
    nativeInitGL(level, difficulty);
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
