#!/bin/bash
#testClasses=(containers/SetTest.h containers/ListTest.h containers/BlockingQueueTest.h)
testClasses=(net/MessagesTest.h)

GCC_OPTS="-Wall -DPLATFORM_SDL -fno-rtti -fno-exceptions  -I. -I/usr/include/GL/ -lpthread"

ADD_SOURCES="net/msg.cpp"

rm -f runner

for class in ${testClasses[@]}
do
  echo "-- Running $class"
  cxxtestgen.py --error-printer -o runner.cpp tests/$class
  g++ $GCC_OPTS -o runner runner.cpp $ADD_SOURCES
  ./runner
done

rm -f runner
rm -f runner.cpp
