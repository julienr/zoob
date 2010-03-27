//Containers test suite

#include "vector.h"
#include "list.h"
#include <stdlib.h>
#include <stdio.h>

int main (int argc, char** argv) {
  printf("--Vector\n");
  Vector<int> v(2);
  v.add(1);
  v.add(2);
  v.add(3);
  v.add(4);
  v.add(5);

  for (int i=0; i<v.length(); i++)
    printf("%i\n", v.get(i));

  printf("--List\n");
  List<int> l;
  l.prepend(1);
  l.prepend(2);
  l.prepend(3);
  l.prepend(4);

  for (List<int>::Iterator i=l.iterator(); i.hasNext(); i++) {
    printf("%i\n", *i);
  }

  List<int>::Iterator three = l.iterator();
  three++;
  printf("Removing : %i\n", *three);
  l.remove(three);
  l.prepend(5);
  for (List<int>::Iterator i=l.iterator(); i.hasNext(); i++) {
    printf("%i\n", *i);
  }
  return 0;
}
