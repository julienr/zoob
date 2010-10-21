#include <cxxtest/TestSuite.h>
#include <containers/set.h>

class SetTests : public CxxTest::TestSuite
{
  public:
    //Test set insertion
    void testInsert () {
      set<int> s;
      for (int i=0; i<5; i++)
        s.insert(i);

      for (int i=0; i<5; i++)
        TS_ASSERT(s.contains(i));
    }

    //Test set removal
    void testRemove () {
      set<int> s;
      for (int i=0; i<5; i++)
        s.insert(i);

      s.remove(2);
      s.remove(4);

      TS_ASSERT(s.contains(0));
      TS_ASSERT(s.contains(1));
      TS_ASSERT(s.contains(3));
    }

    //Insert the same value multiple times
    //and check that there are no duplicates
    void testNoDuplicates () {
      set<int> s;
      for (int i=0; i<5; i++)
        s.insert(i);
      s.insert(1);
      s.insert(2);

      TS_ASSERT(s.size() == 5);
    }

    void testIterator () {
      bool visited[5];
      set<int> s;
      for (int i=0; i<5; i++) {
        visited[i] = false;
        s.insert(i);
      }
      s.insert(1);
      s.insert(2);

      for (set<int>::iterator i = s.begin(); i.hasNext(); i++) {
        const int v = *i;
        TS_ASSERT(!visited[v]);
        visited[v] = true;
      }

      //check that we have visited everything
      for (int i=0; i<5; i++) {
        TS_ASSERT(visited[i]);
      }
    }
};
