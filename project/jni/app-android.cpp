#include "app-android.h"
#include "app.h"
#include "zoobdef.h"
#include "input/AndroidInputManager.h"
#include "lib/FileManager.h"

/** JNI UPCALL STUFF **/
static JNIEnv* jniEnv = NULL;

static jclass zoobRendererClass;
static jobject zoobRenderer;
static jmethodID java_saveProgress;
static jmethodID java_showMenu;

void saveProgress (int level) {
  LOGE("saveProgress : %i", level);
  jniEnv->CallVoidMethod(zoobRenderer, java_saveProgress, level);
}

void showMenu (int id, int currentLevel) {
  LOGE("showMenu : %i, currentLevel : %i", id, currentLevel);
  jniEnv->CallVoidMethod(zoobRenderer, java_showMenu, id, currentLevel);
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
  JNI_GET_METHOD(java_showMenu, "showMenu", "(II)V");
}

static const char* apkPath = NULL;

FileManager* createFileManager () {
  if (!apkPath) {
    LOGE("createFileManager called before jni init");
    abort();
  }
  return new APKFileManager(apkPath); 
}

/** Input Manager **/
static AndroidInputManager* inputManager = NULL;
InputManager* createInputManager (int useGamepad, int useTrackball) {
  inputManager = new AndroidInputManager(useGamepad!=0, useTrackball!=0);
  return inputManager;
}

/**
 * VERY IMPORTANT: the JNIEnv obtained here is thread specific (render-thread specific in our case).
 * We SHOULDN'T use it to make upcall to stuff outside the rendering thread
 */
JNIEXPORT void JNICALL Java_net_fhtagn_zoobgame_ZoobRenderer_nativeInit
  (JNIEnv * env, jclass cls, jstring apk, jobject zoob, jstring levelSerie) {
  ASSERT(jniEnv == NULL);
  init_for_upcall(env, zoob);
  apkPath = env->GetStringUTFChars(apk, NULL);
  const char* serie;
  serie = env->GetStringUTFChars(levelSerie, NULL);
  nativeInit(serie);
//  env->ReleaseStringUTFChars(apkPath, str);
}

JNIEXPORT void JNICALL Java_net_fhtagn_zoobgame_ZoobRenderer_nativeStartGame
  (JNIEnv *, jclass, int lvl) {
  startGame(lvl);
}

JNIEXPORT void JNICALL Java_net_fhtagn_zoobgame_ZoobRenderer_nativeInitGL
  (JNIEnv *, jclass, int level, int difficulty, int useGamepad, int useTrackball) {
  nativeInitGL(level, difficulty, useGamepad, useTrackball);
}


JNIEXPORT void JNICALL Java_net_fhtagn_zoobgame_ZoobRenderer_nativeResize
  (JNIEnv *, jclass, jint w, jint h) {
  nativeResize(w,h);
}

JNIEXPORT void JNICALL Java_net_fhtagn_zoobgame_ZoobRenderer_nativeRender
  (JNIEnv *, jclass) {
  nativeRender();
}

JNIEXPORT void JNICALL Java_net_fhtagn_zoobgame_ZoobRenderer_nativePause
  (JNIEnv *, jclass) {
  nativePause();
}

JNIEXPORT void JNICALL Java_net_fhtagn_zoobgame_ZoobRenderer_nativeStopGame
  (JNIEnv *, jclass) {
  nativeStopGame();
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

JNIEXPORT void JNICALL Java_net_fhtagn_zoobgame_ZoobRenderer_touchEventSecondaryDown
  (JNIEnv *, jclass, jfloat x, jfloat y) {
  inputManager->touchEventSecondaryDown(x, y);
}

JNIEXPORT void JNICALL Java_net_fhtagn_zoobgame_ZoobRenderer_touchEventSecondaryUp
  (JNIEnv *, jclass, jfloat x, jfloat y) {
  inputManager->touchEventSecondaryUp(x, y);
}

JNIEXPORT void JNICALL Java_net_fhtagn_zoobgame_ZoobRenderer_touchEventSecondaryMove
  (JNIEnv *, jclass, jfloat x, jfloat y) {
  inputManager->touchEventSecondaryMove(x, y);
}
