LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := zoob

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../libzip/jni/ $(LOCAL_PATH)/../../../libpng/jni/ $(LOCAL_PATH)/../../../jansson/jni/

#LOCAL_STATIC_LIBRARIES := libzip libpng

#-Wno-psabi to remove warning about GCC 4.4 va_list warning
LOCAL_CFLAGS := -Wall -DANDROID_NDK -Wno-psabi

LOCAL_DEFAULT_CPP_EXTENSION := cpp 

LOCAL_SRC_FILES := \
		app-android.cpp \
		app.cpp \
    lib/Vector2.cpp \
    lib/Polygon.cpp \
    lib/Line.cpp \
    lib/TimerManager.cpp \
    logic/Viewable.cpp \
    logic/Level.cpp \
    logic/Game.cpp \
    logic/Tank.cpp \
    logic/ProgressionManager.cpp \
    logic/PlayerTank.cpp \
    logic/ShadowPolygon.cpp \
    levels/LevelsData.cpp \
    levels/levelgen.cpp \
    view/textures/TextureManager.cpp \
    view/textures/TextureGroup.cpp \
    view/textures/Texture.cpp \
    view/Sprite.cpp \
    view/Square.cpp \
    view/LevelView.cpp \
    view/GameView.cpp \
    view/Explosion.cpp \
    view/GameManager.cpp \
    view/NumberView.cpp \
    view/menu/MenuItem.cpp \
    view/menu/Menu.cpp \
    view/menu/PausedMenu.cpp \
    view/menu/Button.cpp \
    view/ShadowPolygonView.cpp \
    logic/physics/AABBox.cpp \
    logic/physics/CollisionManager.cpp \
    logic/physics/Grid.cpp \
    logic/Difficulty.cpp \
    ai/shoot/AimPolicy.cpp \
    ai/shoot/RandomPolicy.cpp \
    ai/shoot/AlwaysPolicy.cpp \
    ai/movement/PathPolicy.cpp \
    ai/movement/SmartPolicy.cpp \
    ai/TankAI.cpp \
    ai/algorithms/AStar.cpp \
    logic/VisibilityGrid.cpp \
    input/AndroidInputManager.cpp \
    input/PlayerFormControl.cpp \
    input/InputManager.cpp

LIBPNG_PATH := /home/julien/android/libpng/bin/ndk/local/armeabi/
LIBZIP_PATH := /home/julien/android/libzip/bin/ndk/local/armeabi/
LIBJANSSON_PATH := /home/julien/android/jansson/bin/ndk/local/armeabi/
LOCAL_LDLIBS := -lGLESv1_CM -ldl -llog -lz -L$(LIBPNG_PATH) -lpng -L$(LIBZIP_PATH) -lzip -L$(LIBJANSSON_PATH) -ljansson

#To generate a map file in case of segfault
#see : http://stackoverflow.com/questions/2314273/get-function-names-from-call-stack
#LOCAL_LDFLAGS := -Wl,-Map=app.map
include $(BUILD_SHARED_LIBRARY)
