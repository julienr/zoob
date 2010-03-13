#include "app-android.h"
#include <GLES/gl.h>
#include "def.h"
#include "view/TextureManager.h"
#include "logic/Entity.h"
#include "view/TankView.h"
#include "logic/Tank.h"

zip* APKArchive;

int square[12] = {
    fX(-0.5), fX(-0.5), 0,
    fX(0.5), fX(-0.5), 0,
    fX(-0.5), fX(0.5), 0,
    fX(0.5), fX(0.5), 0
};

int texCoords[8] = {
    0, fX(1),
    fX(1), fX(1),
    0,0,
    fX(1),0
};

GLuint texture;

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
      LOGE("Error reading zip file name at index %i : %s", zip_strerror(APKArchive));
      return;
    }
    LOGI("File %i : %s\n", i, name);
  }
}

Tank tank;
TankView* tankView;

JNIEXPORT void JNICALL Java_net_fhtagn_moob_MoobRenderer_nativeInit
  (JNIEnv * env, jclass cls, jstring apkPath) {
  const char* str;
  jboolean isCopy;
  str = env->GetStringUTFChars(apkPath, &isCopy);
  loadAPK(str);

  //texture = loadTextureFromPNG("assets/sprites/texture.png", width, height);
  texture = TextureManager::getInstance()->get("assets/sprites/texture.png");

  printGLString("Version", GL_VERSION);
  printGLString("Vendor", GL_VENDOR);
  printGLString("Renderer", GL_RENDERER);
  printGLString("Extensions", GL_EXTENSIONS);

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glEnable(GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_TEXTURE_2D);
  glClearColor(1,0,0,0);
  glColor4f(1,1,1,1);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  tank.setPosition(Vector2(2,2));
  tankView = new TankView(tank);
}

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
  glOrthof(0, 15, 15/ratio, 0, -1, 1);
  checkGlError("glViewport");
  glMatrixMode(GL_MODELVIEW);
}

JNIEXPORT void JNICALL Java_net_fhtagn_moob_MoobRenderer_nativeRender
  (JNIEnv *, jclass) {

  static float pos = 0;
  pos += 0.1f;
  if (pos > 15)
    pos = 0;
  glClear(GL_COLOR_BUFFER_BIT);
  glLoadIdentity();

  glPushMatrix();
  glTranslatef(pos, 5, 0);
  glScalef(5,5,0);
  glVertexPointer(3, GL_FIXED, 0, square);
  glTexCoordPointer(2, GL_FIXED, 0, texCoords);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glPopMatrix();

  tankView->draw();
}

JNIEXPORT void JNICALL Java_net_fhtagn_moob_MoobGLSurface_nativePause
  (JNIEnv *, jclass) {
  LOGE("Pause");
}

