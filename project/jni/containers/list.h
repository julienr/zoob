#ifndef LIST_H_
#define LIST_H_

#include "utlist.h"

//Double linked list
//This is a wrapper around utlist.h
template <class T>
class list {
  friend class iterator;
  private:
    /** Private element structure */
    struct _Element {
      _Element(const T& d) : data(d), next(NULL), prev(NULL) {}
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
    list () : head(NULL) {}
    ~list() {
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
    void remove (const iterator& i) {
      _Element* del = i.current;
      DL_DELETE(head, del);
      delete del;
    }

    //Remove all elements == d, returns num removed
    size_t remove (const T& d) {
      size_t count = 0;
      if (!head)
        return 0;

      //Start at head->next because deleting head is a special case
      _Element* tmp;
      for (tmp = head->next; tmp;) {
        _Element* next = tmp->next;
        if (tmp->data == d) {
          if (tmp->prev)
            tmp->prev->next = tmp->next;
          if (tmp->next)
            tmp->next->prev = tmp->prev;
          delete tmp;
          count++;
        }
        tmp = next;
      }

      if (head->data == d) {
        head = head->next;
        delete head;
        count++;
      }

      return count;
    }

    iterator begin () const {
      return iterator(head);
    }
  private:
    _Element* head;
};

#endif /* LIST_H_ */
