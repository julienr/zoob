#ifndef DEF_H_
#define DEF_H_

#include <android/log.h>
#include <zip.h>
#include <GLES/gl.h>

extern zip* APKArchive;

#define STRINGIFY(x) #x
#define LOG_TAG    __FILE__ ":" STRINGIFY(__LINE__)
#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

//int to fixed point
#define iX(x) (x<<16)
//float ti fixed point
#define fX(x) ((int)(x * (1  << 16)))

#endif /* DEF_H_ */
