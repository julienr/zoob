#ifndef VECTOR_H_
#define VECTOR_H_

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "def.h"

template <class T>
class vector {
  public:
    vector (size_t initialCapacity)
      : used(0), capacity(initialCapacity), capacityIncr(initialCapacity) {
      data = (T*)malloc(sizeof(T)*capacity);
      ASSERT(data);
    }

    //COPY elements from an existing array
    vector (size_t len, T* arr)
      : used(len), capacity(len), capacityIncr(len) {
      data = (T*)malloc(sizeof(T)*capacity);
      memcpy(data, arr, sizeof(T)*len);
      ASSERT(data);
    }

    ~vector () {
      free(data);
    }

    void append (T elem) {
      if (used >= capacity) {
        capacity += capacityIncr;
        data = (T*)realloc(data, sizeof(T)*capacity);
        ASSERT(data);
      }
      data[used++] = elem;
    }

    T removeLast () {
      ASSERT(!empty());
      used--;
      return data[used];
    }

    T last () {
      ASSERT(!empty());
      return data[used-1];
    }

    size_t length () const {
      return used;
    }

    size_t size() const {
      return used;
    }

    bool empty () const {
      return used == 0;
    }

    bool contains (const T& elem) const {
      for (size_t i=0; i<used; i++)
        if (data[i] == elem)
          return true;
      return false;
    }

    void clear () {
      if (used >= 5*capacityIncr) {
        //reinitialize to initial size if it has grown a lot
        data = (T*)realloc(data, sizeof(T)*capacityIncr);
        capacity = capacityIncr;
        ASSERT(data);
      }
      used = 0;
    }

    T get (size_t i) const {
      return (*this)[i];
    }

    T& operator [] (size_t i) const {
      ASSERT(i < used);
      return data[i];
    }
  private:
    vector (const vector<T>& other):capacityIncr(0) { ASSERT(false); }
    size_t used;
    size_t capacity;
    const size_t capacityIncr; //By how much a realloc will increase the capacity
    T* data;
};

#endif /* VECTOR_H_ */
