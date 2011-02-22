LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := zoob

ANDROID_LIBS := $(LOCAL_PATH)/../../../

LOCAL_C_INCLUDES := $(ANDROID_LIBS)/libzip/jni/ \
									  $(ANDROID_LIBS)/libpng/jni/ \
										$(ANDROID_LIBS)/jansson/jni/ \
										$(ANDROID_LIBS)/libfont/jni/ \
										$(ANDROID_LIBS)/libenet/jni/ \
										$(ANDROID_LIBS)/libfreetype/jni/include/

#LOCAL_STATIC_LIBRARIES := libzip libpng

#-Wno-psabi to remove warning about GCC 4.4 va_list warning
LOCAL_CFLAGS := -Wall -DANDROID_NDK -Wno-psabi -Wno-unused

LOCAL_DEFAULT_CPP_EXTENSION := .cpp 

LOCAL_SRC_FILES := \
    app-android.cpp \
    app.cpp \
    lib/Polygon.cpp \
    lib/Line.cpp \
    lib/TimerManager.cpp \
		lib/FileManager.cpp \
    net/Server.cpp \
    net/ENetServer.cpp \
    net/Client.cpp \
		net/ENetClient.cpp \
		net/NetController.cpp \
		net/Messages.cpp \
    logic/Viewable.cpp \
    logic/Level.cpp \
    logic/Game.cpp \
    logic/NetworkedGame.cpp \
    logic/Tank.cpp \
    logic/ProgressionManager.cpp \
    logic/PlayerTank.cpp \
    logic/ShadowPolygon.cpp \
    levels/LevelManager.cpp \
    view/textures/TextureManager.cpp \
    view/textures/TextureGroup.cpp \
    view/textures/Texture.cpp \
    view/Sprite.cpp \
    view/primitives/Square.cpp \
    view/primitives/Circle.cpp \
    view/LevelView.cpp \
    view/Animation.cpp \
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
    ai/algorithms/PathFinder.cpp \
    logic/VisibilityGrid.cpp \
    input/AndroidInputManager.cpp \
    input/PlayerFormControl.cpp \
    input/InputManager.cpp

LIBPNG_PATH := /home/julien/android/libpng/obj/local/armeabi/
LIBZIP_PATH := /home/julien/android/libzip/obj/local/armeabi/
LIBJANSSON_PATH := /home/julien/android/jansson/obj/local/armeabi/
LIBENET_PATH := /home/julien/android/libenet/obj/local/armeabi/
LIBFREETYPE_PATH := /home/julien/android/libfreetype/obj/local/armeabi/
LIBFONT_PATH := /home/julien/android/libfont/obj/local/armeabi/

LOCAL_LDLIBS := -lGLESv1_CM -ldl -llog -lz -L$(LIBPNG_PATH) -lpng -L$(LIBZIP_PATH) -lzip -L$(LIBJANSSON_PATH) -ljansson  -L$(LIBENET_PATH) -lenet -L$(LIBFONT_PATH) -lfont -L$(LIBFREETYPE_PATH) -lfreetype

#To generate a map file in case of segfault
#see : http://stackoverflow.com/questions/2314273/get-function-names-from-call-stack
#LOCAL_LDFLAGS := -Wl,-Map=app.map
include $(BUILD_SHARED_LIBRARY)
