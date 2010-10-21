#include <cxxtest/TestSuite.h>
#include <containers/set.h>

class SetTests : public CxxTest::TestSuite
{
  public:
    void testInsert () {
      set<int> s;
      s.insert(2);
      TS_ASSERT(s.contains(2));
    }
};
