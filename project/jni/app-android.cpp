#include "app-android.h"
#include "app.h"
#include "zoobdef.h"
#include "input/AndroidInputManager.h"
#include "lib/FileManager.h"

//for JNI upcalls
static JNIEnv* jniEnv = NULL;

static jclass zoobRendererClass;
static jobject zoobRenderer;
static jmethodID java_saveProgress;
static jmethodID java_showMenu;

class AppAndroid : public AppInterface {
  public:
    AppAndroid (const char* apkPath, const char* serie)
      : AppInterface(new APKFileManager(apkPath), serie) {}

    virtual InputManager* createInputManager (bool useGamepad, bool useTrackball) {
      inputManager = new AndroidInputManager(useGamepad, useTrackball);
      return inputManager;
    }

    virtual void saveProgress (int level) {
      LOGE("saveProgress : %i", level);
      jniEnv->CallVoidMethod(zoobRenderer, java_saveProgress, level);
    }

    void showMenu (eMenu id, int currentLevel) {
      LOGE("showMenu : %i, currentLevel : %i", id, currentLevel);
      jniEnv->CallVoidMethod(zoobRenderer, java_showMenu, (int)id, currentLevel);
    }

    AndroidInputManager* getInputManager () const {
      return inputManager;
    }

  private:
    AndroidInputManager* inputManager;
};

static AppAndroid* appInterface = NULL;

AppInterface* getApp () {
  return appInterface;
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

/**
 * VERY IMPORTANT: the JNIEnv obtained here is thread specific (render-thread specific in our case).
 * We SHOULDN'T use it to make upcall to stuff outside the rendering thread
 */
JNIEXPORT void JNICALL Java_net_fhtagn_zoobgame_ZoobRenderer_nativeInit
  (JNIEnv * env, jclass cls, jstring apk, jobject zoob, jstring levelSerie) {
  ASSERT(jniEnv == NULL);
  init_for_upcall(env, zoob);
  const char* apkPath = env->GetStringUTFChars(apk, NULL);
  const char* serie;
  serie = env->GetStringUTFChars(levelSerie, NULL);
  appInterface = new AppAndroid(apkPath, serie);
//  env->ReleaseStringUTFChars(apkPath, str);
}

JNIEXPORT void JNICALL Java_net_fhtagn_zoobgame_ZoobRenderer_nativeStartGame
  (JNIEnv *, jclass, int lvl) {
  
  appInterface->startGame(lvl);
}

JNIEXPORT void JNICALL Java_net_fhtagn_zoobgame_ZoobRenderer_nativeInitGL
  (JNIEnv *, jclass, int difficulty, int useGamepad, int useTrackball) {
  appInterface->initGL(difficulty, useGamepad, useTrackball);
}


JNIEXPORT void JNICALL Java_net_fhtagn_zoobgame_ZoobRenderer_nativeResize
  (JNIEnv *, jclass, jint w, jint h) {
  appInterface->resize(w,h);
}

JNIEXPORT void JNICALL Java_net_fhtagn_zoobgame_ZoobRenderer_nativeRender
  (JNIEnv *, jclass) {
  appInterface->simulate();
  appInterface->render();
}

JNIEXPORT void JNICALL Java_net_fhtagn_zoobgame_ZoobRenderer_nativePause
  (JNIEnv *, jclass) {
  appInterface->pause();
}

JNIEXPORT void JNICALL Java_net_fhtagn_zoobgame_ZoobRenderer_nativeStopGame
  (JNIEnv *, jclass) {
  appInterface->stopGame();
}

JNIEXPORT void JNICALL Java_net_fhtagn_zoobgame_ZoobRenderer_touchEventDown
  (JNIEnv *, jclass, jfloat x, jfloat y) {
  appInterface->getInputManager()->touchEventDown(x,y);
}

JNIEXPORT void JNICALL Java_net_fhtagn_zoobgame_ZoobRenderer_touchEventMove
  (JNIEnv *, jclass, jfloat x, jfloat y) {
  appInterface->getInputManager()->touchEventMove(x,y);
}

JNIEXPORT void JNICALL Java_net_fhtagn_zoobgame_ZoobRenderer_touchEventUp
  (JNIEnv *, jclass, jfloat x, jfloat y) {
  appInterface->getInputManager()->touchEventUp(x,y);
}

JNIEXPORT void JNICALL Java_net_fhtagn_zoobgame_ZoobRenderer_touchEventOther
  (JNIEnv *, jclass, jfloat x, jfloat y) {
  appInterface->getInputManager()->touchEventOther(x,y);
}

JNIEXPORT void JNICALL Java_net_fhtagn_zoobgame_ZoobRenderer_trackballMove
  (JNIEnv *, jclass, jfloat rx, jfloat ry) {
  appInterface->getInputManager()->trackballMove(rx, ry);
}

JNIEXPORT void JNICALL Java_net_fhtagn_zoobgame_ZoobRenderer_trackballClick
  (JNIEnv *, jclass, jfloat rx, jfloat ry) {
  appInterface->getInputManager()->trackballClick(rx, ry);
}

JNIEXPORT void JNICALL Java_net_fhtagn_zoobgame_ZoobRenderer_touchEventSecondaryDown
  (JNIEnv *, jclass, jfloat x, jfloat y) {
  appInterface->getInputManager()->touchEventSecondaryDown(x, y);
}

JNIEXPORT void JNICALL Java_net_fhtagn_zoobgame_ZoobRenderer_touchEventSecondaryUp
  (JNIEnv *, jclass, jfloat x, jfloat y) {
  appInterface->getInputManager()->touchEventSecondaryUp(x, y);
}

JNIEXPORT void JNICALL Java_net_fhtagn_zoobgame_ZoobRenderer_touchEventSecondaryMove
  (JNIEnv *, jclass, jfloat x, jfloat y) {
  appInterface->getInputManager()->touchEventSecondaryMove(x, y);
}
