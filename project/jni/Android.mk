LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := zoob

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../libzip/jni/ $(LOCAL_PATH)/../../../libpng/jni/
#LOCAL_STATIC_LIBRARIES := libzip libpng

#-Wno-psabi to remove warning about GCC 4.4 va_list warning
LOCAL_CFLAGS := -Wall -DANDROID_NDK -Wno-psabi

LOCAL_DEFAULT_CPP_EXTENSION := cpp 

LOCAL_SRC_FILES := \
		app.cpp \
    app-android.cpp \
    lib/Vector2.cpp \
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
    view/menu/Font.cpp \
    view/menu/MenuItem.cpp \
    view/menu/Menu.cpp \
    view/menu/MainMenu.cpp \
    view/menu/LostMenu.cpp \
    view/menu/WonMenu.cpp \
    view/menu/EndMenu.cpp \
    view/menu/PausedMenu.cpp \
    view/menu/TutorialMenu.cpp \
    view/menu/BuyFullMenu.cpp \
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
    input/PlayerFormControl.cpp

LIBPNG_PATH := /home/julien/programs/android-ndk-r3/out/apps/libpng/armeabi/
LIBZIP_PATH := /home/julien/programs/android-ndk-r3/out/apps/libzip/armeabi
LOCAL_LDLIBS := -lGLESv1_CM -ldl -llog -lz -L$(LIBPNG_PATH) -lpng -L$(LIBZIP_PATH) -lzip

include $(BUILD_SHARED_LIBRARY)
