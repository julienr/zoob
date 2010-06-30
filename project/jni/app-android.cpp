#include "app-android.h"
#include "app.h"
#include "def.h"
#include "input/AndroidInputManager.h"

/** JNI UPCALL STUFF **/
static JNIEnv* jniEnv = NULL;

static jclass zoobRendererClass;
static jobject zoobRenderer;
static jmethodID java_saveProgress;
static jmethodID java_saveDifficulty;
static jmethodID java_buyFull;

void saveProgress (int level) {
  LOGE("saveProgress : %i", level);
  jniEnv->CallVoidMethod(zoobRenderer, java_saveProgress, level);
}

void saveDifficulty (int diff) {
  LOGE("saveDifficulty : %i", diff);
  jniEnv->CallVoidMethod(zoobRenderer, java_saveDifficulty, diff);
}

void buyFull () {
  LOGE("buyFull");
  jniEnv->CallVoidMethod(zoobRenderer, java_buyFull);
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
  zoobRendererClass = jniEnv->FindClass("net/fhtagn/zoobgame/ZoobRenderer");
  if (zoobRendererClass == NULL)
    LOGE("Unable to find net.fhtagn.Zoob Java class from JNI");

  if (!jniEnv->IsInstanceOf(zoobRenderer, zoobRendererClass))
    LOGE("zoob is not an instance of ZoobRenderer");

  JNI_GET_METHOD(java_saveProgress, "saveProgress", "(I)V");
  JNI_GET_METHOD(java_saveDifficulty, "saveDifficulty", "(I)V");
  JNI_GET_METHOD(java_buyFull, "buyFullVersion", "()V");
}


/** Input Manager **/
static AndroidInputManager* inputManager = NULL;
InputManager* createInputManager () {
  inputManager = new AndroidInputManager;
  return inputManager;
}

/**
 * VERY IMPORTANT: the JNIEnv obtained here is thread specific (render-thread specific in our case).
 * We SHOULDN'T use it to make upcall to stuff outside the rendering thread
 */
JNIEXPORT void JNICALL Java_net_fhtagn_zoobgame_ZoobRenderer_nativeInit
  (JNIEnv * env, jclass cls, jstring apkPath, jobject zoob) {
  ASSERT(jniEnv == NULL);
  init_for_upcall(env, zoob);
  const char* str;
  jboolean isCopy;
  str = env->GetStringUTFChars(apkPath, &isCopy);
  nativeInit(str);
}

JNIEXPORT void JNICALL Java_net_fhtagn_zoobgame_ZoobRenderer_nativeInitGL
  (JNIEnv *, jclass, int level, int difficulty) {
  nativeInitGL(level, difficulty);
}


JNIEXPORT void JNICALL Java_net_fhtagn_zoobgame_ZoobRenderer_nativeResize
  (JNIEnv *, jclass, jint w, jint h) {
  nativeResize(w,h);
}

JNIEXPORT void JNICALL Java_net_fhtagn_zoobgame_ZoobRenderer_nativeRender
  (JNIEnv *, jclass) {
  nativeRender();
}

JNIEXPORT void JNICALL Java_net_fhtagn_zoobgame_ZoobRenderer_nativeMenu
  (JNIEnv *, jclass) {
  nativeMenu();
}

JNIEXPORT jboolean JNICALL Java_net_fhtagn_zoobgame_Zoob_isInMenu
  (JNIEnv *, jclass) {
  return isInMenu();
}

JNIEXPORT void JNICALL Java_net_fhtagn_zoobgame_ZoobRenderer_nativePause
  (JNIEnv *, jclass) {
  nativePause();
}

JNIEXPORT void JNICALL Java_net_fhtagn_zoobgame_ZoobRenderer_touchEventDown
  (JNIEnv *, jclass, jfloat x, jfloat y) {
  inputManager->touchEventDown(x,y);
}

JNIEXPORT void JNICALL Java_net_fhtagn_zoobgame_ZoobRenderer_touchEventMove
  (JNIEnv *, jclass, jfloat x, jfloat y) {
  inputManager->touchEventMove(x,y);
}

JNIEXPORT void JNICALL Java_net_fhtagn_zoobgame_ZoobRenderer_touchEventUp
  (JNIEnv *, jclass, jfloat x, jfloat y) {
  inputManager->touchEventUp(x,y);
}

JNIEXPORT void JNICALL Java_net_fhtagn_zoobgame_ZoobRenderer_touchEventOther
  (JNIEnv *, jclass, jfloat x, jfloat y) {
  inputManager->touchEventOther(x,y);
}

JNIEXPORT void JNICALL Java_net_fhtagn_zoobgame_ZoobRenderer_trackballMove
  (JNIEnv *, jclass, jfloat rx, jfloat ry) {
  inputManager->trackballMove(rx, ry);
}

JNIEXPORT void JNICALL Java_net_fhtagn_zoobgame_ZoobRenderer_trackballClick
  (JNIEnv *, jclass, jfloat rx, jfloat ry) {
  inputManager->trackballClick(rx, ry);
}
