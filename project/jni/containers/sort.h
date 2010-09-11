#ifndef SORT_H_
#define SORT_H_

template <class T>
struct Compare {
  //Returns -1 if t1<t2
  //        0 i f1 == t2
  //        1 if t1 > t2
  virtual int operator () (const T& t1, const T& t2) const = 0;
};

//sort the given array with smallest element at index 0
template <class T>
void sort (T* arr, const int len, const Compare<T>& cmp) {
  _sort(arr, 0, len, cmp);
}

#define SWAP(i,j) do { _tmp=arr[i]; arr[i]=arr[j]; arr[j]=_tmp; } while(0);

//sort arr[from,to[
template <class T>
static void _sort (T* arr, const int from, const int to, const Compare<T>& cmp) {
  T _tmp; //used by SWAP
  if (to-from == 0)
    return;

  //use the median of first, mid and last elements as pivot
  //also reorder first, mid and last elements
  const int mid = (from+to)/2;
  if (cmp(arr[mid], arr[from]) == -1) SWAP(from, mid);
  if (cmp(arr[to-1], arr[from]) == -1) SWAP(from, to-1);
  if (cmp(arr[to-1], arr[mid]) == -1) SWAP(to-1, mid);
  const T pivotVal = arr[mid];
  if (to-from < 3) //we're done
    return;
  //put pivot just before 'to', which has the greatest of the 3 elements
  SWAP(mid, to-2);

  //move elements based on pivot
  int pivotIdx = to-2;
  for (int i=to-3; i>=from; i--) {
    if (i != pivotIdx && (cmp(arr[i],pivotVal) == 1)) {
      SWAP(i, pivotIdx-1);
      SWAP(pivotIdx-1, pivotIdx);
      pivotIdx--;
    }
  }
  _sort(arr, from, pivotIdx, cmp);
  _sort(arr, pivotIdx+1, to, cmp);
}


#endif /* SORT_H_ */
