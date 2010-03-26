#include "app-android.h"
#include <GLES/gl.h>
#include "def.h"
#include "view/TextureManager.h"
#include "logic/Entity.h"
#include "view/TankView.h"
#include "logic/Tank.h"
#include "logic/Level.h"
#include "view/LevelView.h"
#include "view/GameView.h"
#include "logic/Game.h"
#include "view/GLW.h"

zip* APKArchive;

static void printGLString(const char *name, GLenum s) {
    const char *v = (const char *) glGetString(s);
    LOGI("GL %s = %s\n", name, v);
}

static void checkGlError(const char* op) {
    for (GLint error = glGetError(); error; error = glGetError()) {
        LOGE("after %s() glError (0x%x)\n", op, error);
    }
}

static void loadAPK (const char* apkPath) {
  LOGI("Loading APK %s", apkPath);
  APKArchive = zip_open(apkPath, 0, NULL);
  if (APKArchive == NULL) {
    LOGE("Error loading APK");
    return;
  }

  //Just for debug, print APK contents
  int numFiles = zip_get_num_files(APKArchive);
  for (int i=0; i<numFiles; i++) {
    const char* name = zip_get_name(APKArchive, i, 0);
    if (name == NULL) {
      LOGE("Error reading zip file name at index %i : %s", i, zip_strerror(APKArchive));
      return;
    }
    LOGI("File %i : %s\n", i, name);
  }
}

#include "levels/LevelsData.h"
Level* lvl;
Game* game;
GameView* gameView;

JNIEXPORT void JNICALL Java_net_fhtagn_moob_MoobRenderer_nativeInit
  (JNIEnv * env, jclass cls, jstring apkPath) {
  const char* str;
  jboolean isCopy;
  str = env->GetStringUTFChars(apkPath, &isCopy);
  loadAPK(str);

  lvl = loadEmpty();
  //lvl = loadLevel1();
  //lvl = loadColTest();
  game = new Game(lvl);
  gameView = new GameView(*game);

  printGLString("Version", GL_VERSION);
  printGLString("Vendor", GL_VENDOR);
  printGLString("Renderer", GL_RENDERER);
  printGLString("Extensions", GL_EXTENSIONS);

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glEnable(GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_TEXTURE_2D);
  glClearColor(0,0,0,0);
  glColor4f(1,1,1,1);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
}

/** OpenGL ES doesn't necessarily support retrieving current projection/viewport matrix
 * => We do it by hand cause we now we're in 2d, so that's pretty easy
 * To convert x (screen coords) to game coords, simply do x*xScreenToGame
 */
float xScreenToGame;
float yScreenToGame;

//0.5 is because sprites are square centered on their position
#define XSG(x) (x*xScreenToGame-0.5)
#define YSG(x) (x*xScreenToGame-0.5)

JNIEXPORT void JNICALL Java_net_fhtagn_moob_MoobRenderer_nativeResize
  (JNIEnv *, jclass, jint w, jint h) {
  LOGI("nativeResize (%i,%i)", w, h);
  if(h == 0)
    h = 1;
  glViewport(0, 0, w, h);
  checkGlError("glViewport");
  glMatrixMode(GL_PROJECTION);
  const float ratio=w/(float)h;
  glLoadIdentity();
  const float gameAreaW = 15;
  const float gameAreaH = 15/ratio;
  glOrthof(0, gameAreaW, gameAreaH, 0, -1, 1);

  xScreenToGame = gameAreaW/w;
  yScreenToGame = gameAreaH/h;

  checkGlError("glViewport");
  glMatrixMode(GL_MODELVIEW);
}

JNIEXPORT void JNICALL Java_net_fhtagn_moob_MoobRenderer_nativeRender
  (JNIEnv *, jclass) {
  glClear(GL_COLOR_BUFFER_BIT);
  glLoadIdentity();

  //artificial translation so we see everything (since sprites are renderer on -0.5,0.5)
  GLW::translate(0.5f, 0.5f, 0);

  game->update();
  gameView->draw();
  gameView->debugDraw();
}

JNIEXPORT void JNICALL Java_net_fhtagn_moob_MoobGLSurface_nativePause
  (JNIEnv *, jclass) {
  LOGE("Pause");
}

JNIEXPORT void JNICALL Java_net_fhtagn_moob_MoobGLSurface_touchEventDown
  (JNIEnv *, jclass, jfloat x, jfloat y) {
  const Vector2 p(XSG(x), YSG(y));
  if (gameView->getTankView().inside(p))
    game->startMovingTank(p);
  else
    game->setCursorPosition(p);
}

JNIEXPORT void JNICALL Java_net_fhtagn_moob_MoobGLSurface_touchEventMove
  (JNIEnv *, jclass, jfloat x, jfloat y) {
  const Vector2 p(XSG(x), YSG(y));
  if (game->isMovingTank())
    game->setTankMoveTouchPoint(p);
}

JNIEXPORT void JNICALL Java_net_fhtagn_moob_MoobGLSurface_touchEventUp
  (JNIEnv *, jclass, jfloat x, jfloat y) {
  game->stopMovingTank();
}

JNIEXPORT void JNICALL Java_net_fhtagn_moob_MoobGLSurface_touchEventOther
  (JNIEnv *, jclass, jfloat x, jfloat y) {
  game->stopMovingTank();
}

