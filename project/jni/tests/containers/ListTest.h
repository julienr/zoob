#include <cxxtest/TestSuite.h>
#include <containers/list.h>

class ListTests : public CxxTest::TestSuite
{
  public:
    //Test set insertion
    void testInsert () {
      list<int> s;
      for (int i=0; i<5; i++)
        s.append(i);

      for (int i=0; i<5; i++)
        TS_ASSERT(s.contains(i));
    }

    //Test set removal
    void testRemove () {
      list<int> s;
      for (int i=0; i<5; i++)
        s.append(i);

      s.remove(2);
      s.remove(4);

      TS_ASSERT(s.contains(0));
      TS_ASSERT(s.contains(1));
      TS_ASSERT(s.contains(3));
    }
};
