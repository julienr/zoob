#include <cxxtest/TestSuite.h>
#include <net/Messages.h>

class MessagesTests : public CxxTest::TestSuite {
public:
  void testSerializeUnserialize() {
    zoobmsg::Hello hello;
    hello.nickname.numBytes = 5;
    hello.nickname.bytes = new char[5];
    strncpy(hello.nickname.bytes, "test", 5);
    

    //LOGE("\nserialized size : %lu", hello.serializedSize());
    const size_t bufferSize = hello.serializedSize();
    TS_ASSERT(bufferSize == 7); //5 characters(5) + 1 array counters(2) = 7
    char* buffer = new char[bufferSize];
    size_t offset = 0;
    hello.serialize(buffer, offset);

    zoobmsg::Hello test2;
    offset = 0;
    TS_ASSERT(test2.unserialize(bufferSize, buffer, offset));
    delete [] buffer;
    TS_ASSERT(test2.nickname.numBytes == hello.nickname.numBytes);
    for (uint16_t i = 0; i < test2.nickname.numBytes; i++)
      TS_ASSERT(test2.nickname.bytes[i] == hello.nickname.bytes[i]);
  }

  void testDecode() {
    zoobmsg::Hello hello;
    hello.nickname.numBytes = 5;
    hello.nickname.bytes = new char[5];
    strncpy(hello.nickname.bytes, "test", 5);


    //LOGE("\nserialized size : %lu", hello.serializedSize());
    const size_t bufferSize = hello.serializedSize() + 1;
    TS_ASSERT(bufferSize == 8); //5 characters(5) + 1 array counters(2) = 7 + 1 byte for message ID
    char* buffer = new char[bufferSize];
    size_t offset = 0;
    zoobmsg::encodeMessage(&hello, buffer, offset);

    offset = 0;
    zoobmsg::Message* msg = zoobmsg::decodeMessage(bufferSize, buffer, offset);
    TS_ASSERT(msg != NULL);
    TS_ASSERT(msg->getIdentifier() == zoobmsg::HELLO);
    //Since we cannot use RTTI, we need to cast to get the right type at runtime
    printMessage((zoobmsg::Hello*)msg);
  }

private:

  static void printMessage(zoobmsg::Hello* msg) {
    //LOGE("message : HELLO");
  }

  static void printMessage(zoobmsg::Message* msg) {
    //LOGE("message : generic message class");
  }
};

