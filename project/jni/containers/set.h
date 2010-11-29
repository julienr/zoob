#ifndef _SET_H
#define _SET_H

#include "def.h"
#include "uthash.h"

#define SET_FOREACH(type,setname,itername) for (set<type>::iterator itername=setname.begin(); itername.hasNext(); itername++)
#define SET_FOREACH_CONST(type,setname,itername) for (set<type>::const_iterator itername=setname.begin(); itername.hasNext(); itername++)

template <class T>
class set {
  private:
    struct _Entry {
      T key;
      UT_hash_handle hh;
    };

  public:
    class iterator {
      public:
        friend class set;
        iterator (_Entry* head)
          : current(head) {}

        T operator * () const {
          return current->key;
        }

        void operator ++ (int) { //postfix ++
          current = (_Entry*)current->hh.next;
        }

        bool hasNext () const {
          return (current != NULL);
        }
      private:
        _Entry* current;
    };

    class const_iterator {
      public:
        friend class set;
        const_iterator (_Entry* head)
          : current(head) {}

        const T& operator * () const {
          return current->key;
        }

        void operator ++ (int) { //postfix ++
          current = (_Entry*)current->hh.next;
        }

        bool hasNext () const {
          return (current != NULL);
        }
      private:
        _Entry* current;
    };


    set () : entries(NULL), _size(0) {}

    explicit set (const set<T>& o)
      : entries(NULL), _size(0) {
      for (set<T>::const_iterator i = o.begin(); i.hasNext(); i++) {
        insert(*i);
      }
    }

    void insert (const T& what) {
      if (contains(what))
        return;
      _Entry* e = new _Entry();
      e->key = what;
      HASH_ADD_KEYPTR(hh, entries, &e->key, sizeof(T), e);
      _size++;
    }

    bool contains (const T& what) {
      _Entry* p = NULL;
      HASH_FIND(hh, entries, &what, sizeof(T), p);
      return p != NULL;
    }

    void remove (const T& what) {
      _Entry* p = NULL;
      HASH_FIND(hh, entries, &what, sizeof(T), p);
      if (p != NULL) {
        HASH_DELETE(hh, entries, p);
        _size--;
      }
    }

    size_t size() const {
      return _size;
    }

    iterator begin () {
      return iterator(entries);
    }

    const_iterator begin () const {
      return const_iterator(entries);
    }

  private:
    _Entry* entries;
    size_t _size;
};

#endif
