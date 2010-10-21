#ifndef _SET_H
#define _SET_H

#include "def.h"
#include "uthash.h"

template <class T>
class set {
  private:
    struct _Entry {
      T key;
      UT_hash_handle hh;
    };

    _Entry* entries;

  public:
    class iterator {
      public:
        friend class set;
        iterator (_Entry* head)
          : current(head) {}

        T operator * () {
          return current->key;
        }

        void operator ++ (int) { //postfix ++
          current = current->hh.next;
        }

        bool hasNext () const {
          return (current != NULL);
        }
      private:
        _Entry* current;
    };

    set () : entries(NULL) {} 

    void insert (const T& what) {
      if (contains(what))
        return;
      _Entry* e = new _Entry();
      e->key = what;
      HASH_ADD_KEYPTR(hh, entries, &e->key, sizeof(T), e);
    }

    bool contains (const T& what) {
      _Entry e;
      e.key = what;
      _Entry* p = NULL;
      HASH_FIND(hh, entries, &e.key, sizeof(T), p);
      return p != NULL;
    }

    iterator iterate () {
      return iterator(entries);
    }
};

#endif
