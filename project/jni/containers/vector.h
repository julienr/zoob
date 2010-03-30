#ifndef VECTOR_H_
#define VECTOR_H_

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#define ASSERT(x) assert(x)

template <class T>
class vector {
  public:
    vector (size_t initialCapacity)
      : used(0), capacity(initialCapacity), capacityIncr(initialCapacity) {
      data = (T*)malloc(sizeof(T)*capacity);
      ASSERT(data);
    }

    ~vector () {
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

    size_t length () const {
      return used;
    }

    const T& operator [] (size_t i) const {
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
