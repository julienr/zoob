#ifndef BINARYHEAP_H_
#define BINARYHEAP_H_

#include "vector.h"

/**
 * A binary heap that store the LOWEST item (with regards to Compare) at the top
 *
 * T is the type of the elements to store
 * Compare is a class that takes two T as argument and returns true
 * if the first is less than the second. This class typically is a function pointer or a functor
 */
template<class T, class Compare>
class binaryheap {
  public:
    binaryheap (size_t startingSize, const Compare& comp = Compare())
      : array(startingSize), comp(comp) {
    }


    void insert (const T& t) {
      array.append(t);
      size_t newItemPos = array.length()-1;
      //swap with parents until it is where it should
      //or it reached the top
      while (newItemPos != 0) {
        const size_t parentPos = newItemPos/2;
        if (!comp(array[newItemPos], array[parentPos]))
          break;
        swap(parentPos, newItemPos);
        newItemPos = parentPos;
      }
    }

    //Looks for an item t such as t==item
    //Returns a pointer to the contained item, NULL if no such item is found
    T* find (const T& item) {
      for (size_t i=0; i<array.length(); i++)
        if (array[i] == item)
          return &array[i];
      return NULL;
    }

    size_t size () const {
      return array.length();
    }

    void clear () {
      array.clear();
    }

    //returns root element without removing it
    const T& pickRoot () const {
      ASSERT(array.length() != 0);
      return array[0];
    }

    //returns and remove root element
    T removeRoot () {
      ASSERT(array.length() != 0);
      T val = array[0];
      if (array.length() == 0)
        return val;
      array[0] = array[array.length()-1];
      array.removeLast();
      _maxHeapify(0);
      return val;
    }

  protected:
    void swap (size_t i1, size_t i2) {
      T tmp = array[i1];
      array[i1] = array[i2];
      array[i2] = tmp;
    }

    //recusrively swap idx with the smallest of its children until
    //it satisfies the heap property
    void _maxHeapify (size_t idx) {
      const size_t left = 2*idx;
      const size_t right = 2*idx+1;
      size_t smallest = idx;
      if (left < array.length() && comp(array[left],array[smallest]))
        smallest = left;
      if (right < array.length() && comp(array[right],array[smallest]))
        smallest = right;
      if (smallest != idx) {
        swap(idx, smallest);
        _maxHeapify(smallest);
      }
    }

    vector<T> array;
    Compare comp;
};


#endif /* BINARYHEAP_H_ */
