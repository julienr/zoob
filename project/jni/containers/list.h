#ifndef LIST_H_
#define LIST_H_

#include <stdlib.h>
#include "utlist.h"
#include "def.h"

#define LIST_FOREACH(type,listname,itername) for (list<type>::iterator itername = listname.begin(); itername.hasNext(); itername++)
#define LIST_FOREACH_CONST(type,listname,itername) for (list<type>::const_iterator itername = listname.begin(); itername.hasNext(); itername++)

//Double linked list
//This is a wrapper around utlist.h
template <class T>
class list {
  friend class iterator;
  private:
    /** Private element structure */
    struct _Element {
      _Element(T d) : data(d), next(NULL), prev(NULL) {}
      T data;
      _Element *next, *prev;
    };
  public:
    /** Simple iterator */
    class iterator {
      friend class list;
      public:
        iterator (_Element* head)
          : current(head){}

        T operator * () {
          return current->data;
        }

        void operator ++ (int) { //postfix ++
          current = current->next;
        }

        bool hasNext () const {
          return (current != NULL);
        }
      private:
        _Element* current;
    };

    class const_iterator {
      friend class list;
      public:
        const_iterator (_Element* head)
          : current(head) {}

        const T& operator * () {
          return current->data;
        }

        void operator ++ (int) {
          current = current->next;
        }

        bool hasNext () const {
          return (current != NULL);
        }
      private:
        _Element* current;
    };
  public:
    list () : head(NULL), _size(0) {}
    ~list() {
      for (_Element* e=head; e; ) {
        _Element* next = e->next;
        delete e;
        e = next;
      }
    }

    bool contains (const T& elem) const {
      _Element* el;
      LL_FOREACH(head, el) {
        if (el->data == elem)
          return true;
      }
      return false;
    }

    void clear () {
      for (_Element* e=head; e; ) {
         _Element* next = e->next;
         DL_DELETE(head, e);
         delete e;
         e = next;
       }
      _size = 0;
    }

    void prepend (T d) {
      _Element* add = new _Element(d);
      DL_PREPEND(head, add);
      _size++;
    }

    void append (T d) {
      _Element* add = new _Element(d);
      DL_APPEND(head, add);
      _size++;
    }

    T firstElement () const {
      ASSERT(head != NULL);
      return head->data;
    }

    bool empty () const {
      return head == NULL;
    }

    void removeFirst () {
      ASSERT(head != NULL);
      _Element* del = head;
      DL_DELETE(head, del);
      delete del;
      _size--;
    }

    /** Remove element "pointer" by the iterator i
     * Invalidates i
     * Returns an iterator to i's next element
     */
    iterator remove (iterator& i) {
      _Element* del = i.current;
      ASSERT(del != NULL);
      _Element* next = del->next;
      DL_DELETE(head, del);
      delete del;
      _size--;
      return iterator(next);
    }

    //Remove all elements == d, returns num removed
    size_t remove (T d) {
      size_t count = 0;
      if (!head)
        return 0;

      for (_Element* e=head; e; ) {
        _Element* next = e->next;
        if (e->data == d) {
          DL_DELETE(head, e);
          delete e;
          _size--;
        }
        e = next;
      }
      return count;
    }

    size_t size () const {
      return _size; 
    }

    iterator begin () {
      return iterator(head);
    }

    const_iterator begin () const {
      return const_iterator(head);
    }
  private:
    list(const list& other) { ASSERT(false); }
    _Element* head;
    size_t _size;
};

#endif /* LIST_H_ */
