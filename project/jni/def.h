#ifndef DEF_H_
#define DEF_H_

#include <android/log.h>
#include <zip.h>
#include <GLES/gl.h>
#include <assert.h>

extern zip* APKArchive;

#define ASSERT(x) assert(x)

#define STRINGIFY(x) #x
#define LOG_TAG    __FILE__ ":" STRINGIFY(__LINE__)


#ifdef PLATFORM_SDL
#define LOGI(...) do {printf(__VA_ARGS__);printf("\n");} while(0)
#define LOGE(...) do {printf(__VA_ARGS__);printf("\n");} while(0)
#else
#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#endif

//int to fixed point
#define iX(x) (x<<16)
//float to fixed point
#define fX(x) ((int)(x * (1  << 16)))

//logical tile size
#define TILE_SIZE 1.0
#define TANK_BCIRCLE_R 0.4f
#define ROCKET_BCIRCLE_R 0.2f

#endif /* DEF_H_ */
