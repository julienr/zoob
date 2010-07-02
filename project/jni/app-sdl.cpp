#include <SDL/SDL.h>
#include "def.h"
#include "app.h"
#include "input/SDLInputManager.h"
#include "input/AndroidInputManager.h"

#define SCREEN_WIDTH 480//480
#define SCREEN_HEIGHT 320//279

void saveProgress (int level) {
  LOGE("SDL saveProgress : level %i", level);
}

void saveDifficulty (int diff) {
  LOGE("SDL saveDifficulty : diff %i", diff);
}

void buyFull () {
  LOGE("Buy full");
}

//The SDL app can use the android input manager too, for debugging purposes
enum eInputManager {
    INPUT_ANDROID,
    INPUT_SDL
};

eInputManager inputType = INPUT_SDL;

union {
    SDLInputManager* sdl;
    AndroidInputManager* android;
} inputManager;

InputManager* createInputManager (int inputMethod) {
  if (inputType == INPUT_SDL) {
    inputManager.sdl = new SDLInputManager();
    return inputManager.sdl;
  } else {
    inputManager.android = new AndroidInputManager((eInputMode)inputMethod);
    return inputManager.android;
  }
}

static bool handleAndroidInputEvents ();
static bool handleSDLInputEvents ();

int cursorW, cursorH;

int main (int argc, char** argv) {
  if (argc < 2) {
    LOGE("Usage : <apk path>");
    return -1;
  }

  inputType = INPUT_ANDROID;

  int level = 0;
  if (argc >= 3)
    level = atoi(argv[2]);
  int difficulty = 0;
  if (argc >= 4)
    difficulty = atoi(argv[3]);

  SDL_Init(SDL_INIT_VIDEO);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  /*SDL_Surface* screen =*/ SDL_SetVideoMode (SCREEN_WIDTH, SCREEN_HEIGHT, 16, SDL_OPENGL);

  nativeInit (argv[1]);
  nativeInitGL(level, difficulty, 0);
  nativeResize(SCREEN_WIDTH,SCREEN_HEIGHT);

  bool done = false;

  SDL_Cursor* cursor = SDL_GetCursor();
  cursorW = cursor->area.w;
  cursorH = cursor->area.h;
  LOGE("Cursor w,h : %i,%i", cursorW, cursorH);

  while (!done) {
    //Event handling
    if (inputType == INPUT_ANDROID)
      done = handleAndroidInputEvents();
    else
      done = handleSDLInputEvents();
    nativeRender();
    SDL_GL_SwapBuffers();
  }
  nativeQuit();
  SDL_Quit();
}

#define MOUSE_EVENT_POS event.button.x+cursorW, event.button.y+cursorH

static bool handleAndroidInputEvents () {
  static bool leftBtnClicked = false;
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_MOUSEBUTTONDOWN:
        if (event.button.button == SDL_BUTTON_LEFT) {
          inputManager.android->touchEventDown(MOUSE_EVENT_POS);
          leftBtnClicked = true;
        }
        break;
      case SDL_MOUSEBUTTONUP:
        if (event.button.button == SDL_BUTTON_LEFT) {
          inputManager.android->touchEventUp(MOUSE_EVENT_POS);
          leftBtnClicked = false;
        }
        break;
      case SDL_MOUSEMOTION:
        if (leftBtnClicked)
          inputManager.android->touchEventMove(MOUSE_EVENT_POS);
        break;
      case SDL_QUIT:
        return true;
        break;
      case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {
          case SDLK_ESCAPE: return true;
          case SDLK_m: nativeMenu(); break;
          case SDLK_p: nativePause(); break;
          case SDLK_g: toggleGodMode(); break;
          default: break;
        }
        break;
    }
  }
  return false;
}

struct {
    int up; //1 = up, 0 = none, -1 = down
    int left; //1 = left, 0 = none, -1 = right

    void reset () {
      up = left = 0;
    }
} keyState;

static bool handleSDLInputEvents () {
  static bool leftBtnClicked = false;
  keyState.reset();

  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_MOUSEBUTTONDOWN:
        if (event.button.button == SDL_BUTTON_LEFT) {
          //touchEventDown(MOUSE_EVENT_POS);
          leftBtnClicked = true;
        }
        break;
      case SDL_MOUSEBUTTONUP:
        if (event.button.button == SDL_BUTTON_LEFT) {
          //touchEventUp(MOUSE_EVENT_POS);
          leftBtnClicked = false;
        }
        break;
      case SDL_MOUSEMOTION:
        if (leftBtnClicked) {
          //touchEventMove(MOUSE_EVENT_POS);
        }
        break;
      case SDL_QUIT:
        return true;
        break;
      case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {
          case SDLK_ESCAPE: return true;
          case SDLK_m: nativeMenu(); break;
          case SDLK_p: nativePause(); break;
          case SDLK_g: toggleGodMode(); break;
          //keyState
          case SDLK_w: keyState.up++; break;
          case SDLK_s: keyState.up--; break;
          case SDLK_d: keyState.left--; break;
          case SDLK_a: keyState.left++; break;
          default: break;
        }
        break;
    }
  }

  inputManager.sdl->setMove(keyState.up, keyState.left);
  return false;
}
