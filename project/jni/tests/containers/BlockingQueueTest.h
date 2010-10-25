#include <cxxtest/TestSuite.h>
#include <containers/blockingqueue.h>


#define MAX_SLEEP 50 //in milliseconds
#define NUM_WRITES 100

//uncomment following line to make process sleep between read/writes
//#define SLEEP usleep((rand()%MAX_SLEEP)*1000)
#define SLEEP

//comment two following line for verbose out
#undef LOGI
#define LOGI(...)

void* reader(void* data) {
  blockingqueue<int>* queue = static_cast<blockingqueue<int>* >(data);

  int currentVal = -1;
  int val;
  while (queue->pop(&val)) {
    LOGI("reading %i", val);
    //When we have multiple reader, we cannot assert that val==currentVal,
    //but we should read in increasing manner
    TS_ASSERT(val > currentVal);
    currentVal = val;
    SLEEP;
  }
  pthread_exit(NULL);
}

void* writer(void* data) {
  blockingqueue<int>* queue = static_cast<blockingqueue<int>* >(data);

  for (int i=0; i<NUM_WRITES; i++) {
    LOGI("writing %i", i);
    queue->append(i);
    SLEEP;
  }
  LOGI("finished writing");
  queue->finishWriting();
  pthread_exit(NULL);
}

class ListTests : public CxxTest::TestSuite
{
  public:
    void setUp () {
      srand(time(NULL));
    }

    //Test single writer, single reader
    void testSingleReader () {
      LOGI("\n--Single reader");
      const int numThreads = 2;
      blockingqueue<int> q;
      pthread_t threads[numThreads];
      pthread_attr_t attr;

      pthread_attr_init(&attr);
      pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

      pthread_create(&threads[0], &attr, writer, &q);
      pthread_create(&threads[1], &attr, reader, &q);

      for (int i=0; i<numThreads; i++) {
        pthread_join(threads[i], NULL);
      }
    }

    //Test single writer, single reader
    void testMultipleReader () {
      LOGI("\n--Multiple reader");
      const int numReader = 5;
      blockingqueue<int> q;
      pthread_t threads[numReader+1];
      pthread_attr_t attr;

      pthread_attr_init(&attr);
      pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

      pthread_create(&threads[0], &attr, writer, &q);
      for (int i=0; i<numReader;i++)
        pthread_create(&threads[i+1], &attr, reader, &q);

      for (int i=0; i<numReader+1; i++) {
        pthread_join(threads[i], NULL);
      }
    }


};


