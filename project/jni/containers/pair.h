#ifndef PAIR_H_
#define PAIR_H_

template <class U, class V>
class pair {
  public:
    pair () {}
    pair (const pair<U,V>& other)
      : first(other.first), second(other.second) {}
    pair (const U& u, const V& v)
      : first(u), second(v) {}
    U first;
    V second;

    bool operator == (const pair<U,V>& other) {
      return other.first == first && other.second == second;
    }
};

#endif
