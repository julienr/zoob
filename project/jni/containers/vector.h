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

    void removeLast () {
      ASSERT(used > 0);
      used--;
    }

    size_t length () const {
      return used;
    }

    bool contains (const T& elem) const {
      for (size_t i=0; i<used; i++)
        if (data[i] == elem)
          return true;
      return false;
    }

    void clear () {
      //reinitialize to initial size
      data = (T*)realloc(data, sizeof(T)*capacityIncr);
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
    T* data;
    size_t used;
    size_t capacity;
    const size_t capacityIncr; //By how much a realloc will increase the capacity
};

#endif /* VECTOR_H_ */
