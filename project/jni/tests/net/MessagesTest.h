#include <cxxtest/TestSuite.h>
#include <net/Messages.h>

#define ENCODE_DECODE_MSG(msgType, msgName, rcvName, expSize) \
    const size_t bufferSize = msgType::packedSize(msgName); \
    TS_ASSERT(bufferSize == expSize);  \
    uint8_t* buffer = new uint8_t[bufferSize]; \
    size_t offset = 0; \
    msgType::pack(bufferSize, buffer, offset, msgName); \
    msgType rcvName; \
    offset = 0; \
    msgType::unpack(bufferSize, buffer, offset, rcvName); 

#define ASSERT_ARR_EQ(cnt1, arr1, cnt2, arr2) \
  TS_ASSERT(cnt1 == cnt2); \
  for (uint16_t i=0; i<cnt1; i++) \
    TS_ASSERT(arr1[i] == arr2[i]);

#define EEQ(x,y) (fabs(x-y) < 10E-5)

class MessagesTests : public CxxTest::TestSuite {
public:
  void testHello () {
    zoobmsg::Hello hello;
    hello.nickname.numBytes = 5;
    hello.nickname.bytes = new char[5];
    strncpy(hello.nickname.bytes, "test", 5);

    //7 = 5 chars(5) + 1 array counter(2)
    ENCODE_DECODE_MSG(zoobmsg::Hello, hello, hello2, 7);

    ASSERT_ARR_EQ(hello2.nickname.numBytes, hello2.nickname.bytes, 
                  hello.nickname.numBytes, hello.nickname.bytes);
  }

  void testGameState () {
    zoobmsg::GameState gamestate;
    gamestate.numPlayerInfos = 2;
    gamestate.playerInfos = new zoobmsg::PlayerInfo[2];

    //player 1 infos
    zoobmsg::PlayerInfo& pi1 = gamestate.playerInfos[0];
    pi1.playerID = 1;
    pi1.position.x = -4.5;
    pi1.position.y = 6.3;
    pi1.numRocketInfos = 1;
    pi1.rocketInfos = new zoobmsg::RocketInfo[1];
    pi1.rocketInfos[0].position.x = -5;
    pi1.rocketInfos[0].position.y = 5.7;
    pi1.rocketInfos[0].rocketID = 1;
    pi1.rocketInfos[0].velocity.x = -78;
    pi1.rocketInfos[0].velocity.y = 100;
    pi1.numMineInfos = 0;
    pi1.velocity.x = -3;
    pi1.velocity.y = 10.5;

    //player 2 infos
    zoobmsg::PlayerInfo& pi2 = gamestate.playerInfos[1];
    pi2.playerID = 2;
    pi2.position.x = 3;
    pi2.position.y = 4;
    pi2.numRocketInfos = 0;
    pi2.numMineInfos = 0;
    pi2.velocity.x = 0;
    pi2.velocity.y = 0;


    //gs.numPlayerInfos (2)
    //player1info : pid(2), position(8), numRocketInfos(2), numMineInfos(2), velocity(8) = 22
    //player1info.rocketinfo : position(8), velocity(8), rocketID(2)=18
    //
    //player2info = 22
    //total = 64
    ENCODE_DECODE_MSG(zoobmsg::GameState, gamestate, gamestate2, 64);

    zoobmsg::PlayerInfo& r1 = gamestate2.playerInfos[0];
    TS_ASSERT(r1.playerID == 1);
    TS_ASSERT(EEQ(r1.position.x,-4.5));
    TS_ASSERT(EEQ(r1.position.y,6.3));
    TS_ASSERT(r1.numRocketInfos == 1);
    TS_ASSERT(EEQ(r1.rocketInfos[0].position.x,-5));
    TS_ASSERT(EEQ(r1.rocketInfos[0].position.y, 5.7));
    TS_ASSERT(r1.rocketInfos[0].rocketID == 1);
    TS_ASSERT(EEQ(r1.rocketInfos[0].velocity.x, -78));
    TS_ASSERT(EEQ(r1.rocketInfos[0].velocity.y, 100));
    TS_ASSERT(r1.numMineInfos == 0);
    TS_ASSERT(EEQ(r1.velocity.x, -3));
    TS_ASSERT(EEQ(r1.velocity.y,10.5));

    zoobmsg::PlayerInfo& r2 = gamestate2.playerInfos[1];
    TS_ASSERT(r2.playerID == 2);
    TS_ASSERT(EEQ(r2.position.x,3));
    TS_ASSERT(EEQ(r2.position.y,4));
    TS_ASSERT(r2.numRocketInfos == 0);
    TS_ASSERT(r2.numMineInfos == 0);
    TS_ASSERT(EEQ(r2.velocity.x,0));
    TS_ASSERT(EEQ(r2.velocity.y,0));
  }
};

