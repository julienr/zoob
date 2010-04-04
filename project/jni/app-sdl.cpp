#include <SDL/SDL.h>
#include "def.h"
#include "app.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

int main (int argc, char** argv) {
  if (argc < 2) {
    LOGE("Usage : <apk path>");
    return -1;
  }
  SDL_Init(SDL_INIT_VIDEO);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_Surface* screen = SDL_SetVideoMode (SCREEN_WIDTH, SCREEN_HEIGHT, 16, SDL_OPENGL);

  nativeInit (argv[1]);
  nativeResize(SCREEN_WIDTH,SCREEN_HEIGHT);

  bool leftBtnClicked = false;
  bool done = false;
  while (!done) {
    //Event handling
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_MOUSEBUTTONDOWN:
          if (event.button.button == SDL_BUTTON_LEFT) {
            touchEventDown(event.button.x, event.button.y);
            leftBtnClicked = true;
          }
          break;
        case SDL_MOUSEBUTTONUP:
          if (event.button.button == SDL_BUTTON_LEFT) {
            touchEventUp(event.button.x, event.button.y);
            leftBtnClicked = false;
          }
          break;
        case SDL_MOUSEMOTION:
          if (leftBtnClicked)
            touchEventMove(event.motion.x, event.motion.y);
          break;
        case SDL_QUIT:
          done = true;
          break;
        case SDL_KEYDOWN:
          if (event.key.keysym.sym == SDLK_ESCAPE)
            done = true;
          break;
      }
    }
    nativeRender();
    SDL_GL_SwapBuffers();
  }
}

