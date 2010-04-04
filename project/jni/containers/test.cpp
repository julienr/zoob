//Containers test suite

#include "vector.h"
#include "list.h"
#include <stdlib.h>
#include <stdio.h>

int main (int argc, char** argv) {
  printf("--Vector\n");
  vector<int> v(2);
  v.add(1);
  v.add(2);
  v.add(3);
  v.add(4);
  v.add(5);

  for (int i=0; i<v.length(); i++)
    printf("%i\n", v[i]);

  printf("--List\n");
  list<int> l;
  l.prepend(1);
  l.prepend(2);
  l.prepend(3);
  l.prepend(4);

  for (list<int>::iterator i=l.begin(); i.hasNext(); i++) {
    printf("%i\n", *i);
  }

  list<int>::iterator three = l.begin();
  printf("removing %i\n", *three);
  three = l.remove(three);
  printf("removing : %i\n", *three);
  l.remove(three);
  l.prepend(5);
  l.append(3);
  for (list<int>::iterator i=l.begin(); i.hasNext(); i++) {
    printf("%i\n", *i);
  }
  return 0;
}
