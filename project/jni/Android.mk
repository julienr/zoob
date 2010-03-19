LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := moob

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../libzip/jni/ $(LOCAL_PATH)/../../../libpng/jni/
#LOCAL_STATIC_LIBRARIES := libzip libpng

#-Wno-psabi to remove warning about GCC 4.4 va_list warning
LOCAL_CFLAGS := -DANDROID_NDK -Wno-psabi

LOCAL_DEFAULT_CPP_EXTENSION := cpp 

LOCAL_SRC_FILES := \
    app-android.cpp \
    lib/Vector2.cpp \
    logic/BoundingBox.cpp \
    logic/CollisionManager.cpp \
    logic/Viewable.cpp \
    logic/Level.cpp \
    logic/Game.cpp \
    levels/LevelsData.cpp \
    view/TextureManager.cpp \
    view/Sprite.cpp \
    view/Square.cpp \
    view/LevelView.cpp \
    view/GameView.cpp

LIBPNG_PATH := /home/silver/programs/android-ndk-r3/out/apps/libpng/armeabi/
LIBZIP_PATH := /home/silver/programs/android-ndk-r3/out/apps/libzip/armeabi
LOCAL_LDLIBS := -lGLESv1_CM -ldl -llog -lz -L$(LIBPNG_PATH) -lpng -L$(LIBZIP_PATH) -lzip

include $(BUILD_SHARED_LIBRARY)
