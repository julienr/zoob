#include "app-android.h"
#include "app.h"
#include "def.h"

static JNIEnv* jniEnv = NULL;

static jclass zoobRendererClass;
static jobject zoobRenderer;
static jmethodID java_saveProgress;
static jmethodID java_saveDifficulty;

void saveProgress (int level) {
  LOGE("saveProgress : %i", level);
  jniEnv->CallVoidMethod(zoobRenderer, java_saveProgress, level);
}

void saveDifficulty (int diff) {
  LOGE("saveDifficulty : %i", diff);
  jniEnv->CallVoidMethod(zoobRenderer, java_saveDifficulty, diff);
}

#define JNI_GET_METHOD(var,name,type) \
  var = jniEnv->GetMethodID(zoobRendererClass,name,type); \
    if (var == NULL) \
      LOGE("Unable to get method id for "name" from JNI");


//performs necessary initialization for upcalls (c->java) to work
static void init_for_upcall (JNIEnv* env, jobject zoob) {
  LOGE("got jniEnv=0x%p and zoobRenderer object=0x%p", env, zoob);
  jniEnv = env;
  zoobRenderer = env->NewGlobalRef(zoob);
  //zoobClass = jniEnv->FindClass("net.fhtagn.zoob.ZoobApplication");
  zoobRendererClass = jniEnv->FindClass("net/fhtagn/zoob/ZoobRenderer");
  if (zoobRendererClass == NULL)
    LOGE("Unable to find net.fhtagn.Zoob Java class from JNI");

  if (!jniEnv->IsInstanceOf(zoobRenderer, zoobRendererClass))
    LOGE("zoob is not an instance of ZoobRenderer");

  JNI_GET_METHOD(java_saveProgress, "saveProgress", "(I)V");
  JNI_GET_METHOD(java_saveDifficulty, "saveDifficulty", "(I)V");
}

/**
 * VERY IMPORTANT: the JNIEnv obtained here is thread specific (render-thread specific in our case).
 * We SHOULDN'T use it to make upcall to stuff outside the rendering thread
 */
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

JNIEXPORT void JNICALL Java_net_fhtagn_zoob_ZoobRenderer_nativeMenu
  (JNIEnv *, jclass) {
  nativeMenu();
}

JNIEXPORT jboolean JNICALL Java_net_fhtagn_zoob_Zoob_isInMenu
  (JNIEnv *, jclass) {
  return isInMenu();
}

JNIEXPORT void JNICALL Java_net_fhtagn_zoob_ZoobRenderer_nativePause
  (JNIEnv *, jclass) {
  nativePause();
}

JNIEXPORT void JNICALL Java_net_fhtagn_zoob_ZoobRenderer_touchEventDown
  (JNIEnv *, jclass, jfloat x, jfloat y) {
  touchEventDown(x,y);
}

JNIEXPORT void JNICALL Java_net_fhtagn_zoob_ZoobRenderer_touchEventMove
  (JNIEnv *, jclass, jfloat x, jfloat y) {
  touchEventMove(x,y);
}

JNIEXPORT void JNICALL Java_net_fhtagn_zoob_ZoobRenderer_touchEventUp
  (JNIEnv *, jclass, jfloat x, jfloat y) {
  touchEventUp(x,y);
}

JNIEXPORT void JNICALL Java_net_fhtagn_zoob_ZoobRenderer_touchEventOther
  (JNIEnv *, jclass, jfloat x, jfloat y) {
  touchEventOther(x,y);
}
