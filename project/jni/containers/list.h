#ifndef LIST_H_
#define LIST_H_

#include "utlist.h"

//Double linked list
//This is a wrapper around utlist.h
template <class T>
class List {
  friend class Iterator;
  private:
    /** Private element structure */
    struct _Element {
      _Element(const T& d) : data(d), next(NULL), prev(NULL) {}
      T data;
      _Element *next, *prev;
    };
  public:
    /** Simple iterator */
    class Iterator {
      friend class List;
      public:
        Iterator (_Element* head)
          : current(head){}

        const T& operator * () const {
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
  public:
    List () : head(NULL) {}
    ~List() {
      for (_Element* e=head; e; ) {
        _Element* next = e->next;
        delete e;
        e = next;
      }
    }

    void prepend (const T& d) {
      _Element* add = new _Element(d);
      DL_PREPEND(head, add);
    }

    void append (const T& d) {
      _Element* add = new _Element(d);
      DL_APPEND(head, add);
    }

    /** Remove element "pointer" by the iterator
     * Will _automatically_ advance the iterator to its next element
     */
    void remove (const Iterator& i) {
      _Element* del = i.current;
      DL_DELETE(head, del);
      delete del;
    }

    Iterator iterator () {
      return Iterator(head);
    }
  private:
    _Element* head;
};

#endif /* LIST_H_ */
