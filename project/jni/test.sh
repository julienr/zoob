#!/bin/bash
testClasses=(containers/SetTest.h containers/ListTest.h)

GCC_OPTS="-DPLATFORM_SDL -fno-rtti -fno-exceptions  -I. -I/usr/include/GL/"

for class in ${testClasses[@]}
do
  echo "-- Running $class"
  cxxtestgen.py --error-printer -o runner.cpp tests/$class
  g++ $GCC_OPTS -o runner runner.cpp
  ./runner
done

rm -f runner
rm -f runner.cpp
