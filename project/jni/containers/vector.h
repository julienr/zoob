#ifndef VECTOR_H_
#define VECTOR_H_

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#define ASSERT(x) assert(x)

template <class T>
class Vector {
  public:
    Vector (size_t initialCapacity)
      : used(0), capacity(initialCapacity), capacityIncr(initialCapacity) {
      data = (T*)malloc(sizeof(T)*capacity);
      ASSERT(data);
    }

    ~Vector () {
      free(data);
    }

    void add (const T& elem) {
      if (used >= capacity) {
        capacity += capacityIncr;
        data = (T*)realloc(data, sizeof(T)*capacity);
        ASSERT(data);
      }
      data[used++] = elem;
    }

    size_t length () {
      return used;
    }

    const T& get (size_t i) {
      ASSERT(i < used);
      return data[i];
    }
  private:
    T* data;
    size_t used;
    size_t capacity;
    const size_t capacityIncr; //By how much a realloc will increase the capacity
};

#endif /* VECTOR_H_ */
