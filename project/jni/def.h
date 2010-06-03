#ifndef DEF_H_
#define DEF_H_

#include <zip.h>
#include <assert.h>
#include <stdint.h>

extern zip* APKArchive;

#define ASSERT(x) assert(x)

#define STRINGIFY(x) #x
#define LOG_TAG    __FILE__ ":" STRINGIFY(__LINE__)


#ifdef PLATFORM_SDL //SDL standalone build
  #include <gl.h>
  #define LOGI(...) do {printf(__VA_ARGS__);printf("\n");} while(0)
  #define LOGE(...) do {printf(__VA_ARGS__);printf("\n");} while(0)
#else //Android build
  #include <android/log.h>
  #include <GLES/gl.h>
  #define LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
  #define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#endif

//BEGIN opengl-related defs
//Since the desktop version of opengl doesn't support fixed-point,
#ifdef PLATFORM_SDL
  #define MGL_DATATYPE float
  #define MGL_TYPE GL_FLOAT
  #define iX(x) (x)
  #define fX(x) (x)
#else
  #define MGL_DATATYPE int
  #define MGL_TYPE GL_FIXED
  //int to fixed point
  #define iX(x) (x<<16)
  //float to fixed point
  #define fX(x) ((int)(x * (1  << 16)))
#endif

//END

//logical tile size
//FIXME: move to a logic-specific include ?
#define TILE_SIZE 1.0
#define GRID_CELL_SIZE 0.5f
#define TANK_BCIRCLE_R 0.4f
#define BOSS_BCIRCLE_R 0.5f
#define ROCKET_BCIRCLE_R 0.2f
#define BOMB_BCIRCLE_R 0.4f

#define ROCKET_MAX_BOUNCES 3

#define MAX_BOMBS_PER_TANK 3

#define BOMB_LIFETIME 5
#define BOMB_EXPLOSION_RADIUS 1.5f

//texture groups
enum {
    TEX_GROUP_UTILS, //always present
    TEX_GROUP_GAME, //game related textures
    TEX_GROUP_MENU, //main menu
    TEX_GROUP_TUTORIAL,
    TEX_GROUP_GET_FULL_VERSION,
};

#endif /* DEF_H_ */
