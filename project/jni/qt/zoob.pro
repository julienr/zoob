TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += . ../ /usr/include/GL/ /home/julien/android/libenet/jni/ /home/julien/android/libfont/jni/ /usr/include/freetype2/
QT += opengl
LIBS +=  -L/usr/local/lib -lm -lpng -lzip -ljansson -L/home/julien/android/libenet/bin/linux -lenet  -L/home/julien/android/libfont/jni -lfont -lfreetype
DEFINES += PLATFORM_SDL #ZOOB_DBG_FPS
#Android doesn't support exceptions, so zoob isn't using them
QMAKE_CXXFLAGS += -Wall -fno-rtti -fno-exceptions
#PROFILING
QMAKE_CXXFLAGS_DEBUG += -g -DDEBUG 
QMAKE_LFLAGS_DEBUG += -g

#FPS debug
#debug {
#  DEFINES += ZOOB_DBG_FPS
#}

#use -spec linux-clang or linux-g++ to switch
#QMAKE_CXX = colorgcc
#QMAKE_CXX = clang++

# Input
#qt specific first
HEADERS += GameScreen.h MainWindow.h MenuScreen.h InterMenu.h DebugAction.h
SOURCES += main.cpp MainWindow.cpp MenuScreen.cpp GameScreen.cpp InterMenu.cpp DebugAction.cpp
#zoob (copied from Android.mk, but changed assignation to = instead of :=)
LOCAL_SRC_FILES = \
		app.cpp \
    lib/TimerManager.cpp \
    lib/Polygon.cpp \
    lib/Line.cpp \
    lib/FileManager.cpp \
    net/Server.cpp \
    net/Client.cpp \
    net/ENetServer.cpp \
    net/ENetClient.cpp \
    net/Messages.cpp \
    net/NetController.cpp \
    logic/Viewable.cpp \
    logic/Level.cpp \
    logic/Game.cpp \
    logic/NetworkedGame.cpp \
    logic/Tank.cpp \
    logic/ProgressionManager.cpp \
    logic/PlayerTank.cpp \
    logic/ShadowPolygon.cpp \
    levels/LevelsData.cpp \
    view/textures/TextureManager.cpp \
    view/textures/TextureGroup.cpp \
    view/textures/Texture.cpp \
    view/Sprite.cpp \
    view/Animation.cpp \
    view/primitives/Square.cpp \
    view/primitives/Circle.cpp \
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
    ai/algorithms/PathFinder.cpp \
    logic/VisibilityGrid.cpp \
    input/AndroidInputManager.cpp \
    input/PlayerFormControl.cpp \
    input/InputManager.cpp

for(file,LOCAL_SRC_FILES) {
  SOURCES += ../$$file
}

