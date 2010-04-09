#include <SDL/SDL.h>
#include "def.h"
#include "app.h"

#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 279

int main (int argc, char** argv) {
  if (argc < 2) {
    LOGE("Usage : <apk path>");
    return -1;
  }
  SDL_Init(SDL_INIT_VIDEO);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  /*SDL_Surface* screen =*/ SDL_SetVideoMode (SCREEN_WIDTH, SCREEN_HEIGHT, 16, SDL_OPENGL);

  nativeInit (argv[1]);
  nativeResize(SCREEN_WIDTH,SCREEN_HEIGHT);

  bool leftBtnClicked = false;
  bool done = false;

  SDL_Cursor* cursor = SDL_GetCursor();
  int cursorW = cursor->area.w;
  int cursorH = cursor->area.h;
  LOGE("Cursor w,h : %i,%i", cursorW, cursorH);

#define MOUSE_EVENT_POS event.button.x+cursorW, event.button.y+cursorH

  while (!done) {
    //Event handling
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_MOUSEBUTTONDOWN:
          if (event.button.button == SDL_BUTTON_LEFT) {
            touchEventDown(MOUSE_EVENT_POS);
            leftBtnClicked = true;
          }
          break;
        case SDL_MOUSEBUTTONUP:
          if (event.button.button == SDL_BUTTON_LEFT) {
            touchEventUp(MOUSE_EVENT_POS);
            leftBtnClicked = false;
          }
          break;
        case SDL_MOUSEMOTION:
          if (leftBtnClicked)
            touchEventMove(MOUSE_EVENT_POS);
          break;
        case SDL_QUIT:
          done = true;
          break;
        case SDL_KEYDOWN:
          if (event.key.keysym.sym == SDLK_ESCAPE)
            done = true;
          if (event.key.keysym.sym == SDLK_m)
            nativeMenu();
          break;
      }
    }
    nativeRender();
    SDL_GL_SwapBuffers();
  }
  nativeQuit();
  SDL_Quit();
}

