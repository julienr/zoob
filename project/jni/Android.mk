LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := moob

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../libzip/jni/ $(LOCAL_PATH)/../../../libpng/jni/
#LOCAL_STATIC_LIBRARIES := libzip libpng

#-Wno-psabi to remove warning about GCC 4.4 va_list warning
LOCAL_CFLAGS := -Wall -DANDROID_NDK -Wno-psabi

LOCAL_DEFAULT_CPP_EXTENSION := cpp 

LOCAL_SRC_FILES := \
		app.cpp \
    app-android.cpp \
    lib/Vector2.cpp \
    logic/Viewable.cpp \
    logic/Level.cpp \
    logic/Game.cpp \
    logic/Tank.cpp \
    logic/GreenTank.cpp \
    levels/LevelsData.cpp \
    view/TextureManager.cpp \
    view/Sprite.cpp \
    view/Square.cpp \
    view/LevelView.cpp \
    view/GameView.cpp \
    view/Explosion.cpp \
    view/GameManager.cpp \
    view/menu/Font.cpp \
    view/menu/MenuItem.cpp \
    view/menu/Menu.cpp \
    view/menu/MainMenu.cpp \
    view/menu/LostMenu.cpp \
    view/menu/WonMenu.cpp \
    view/menu/EndMenu.cpp \
    logic/physics/AABBox.cpp \
    logic/physics/CollisionManager.cpp \
    logic/physics/Grid.cpp \
    ai/shoot/AimPolicy.cpp \
    ai/shoot/RandomPolicy.cpp

LIBPNG_PATH := /home/silver/programs/android-ndk-r3/out/apps/libpng/armeabi/
LIBZIP_PATH := /home/silver/programs/android-ndk-r3/out/apps/libzip/armeabi
LOCAL_LDLIBS := -lGLESv1_CM -ldl -llog -lz -L$(LIBPNG_PATH) -lpng -L$(LIBZIP_PATH) -lzip

include $(BUILD_SHARED_LIBRARY)
