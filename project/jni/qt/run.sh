#!/bin/sh
PROTOBUF_LIB=/home/julien/programs/protobuf/lib/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PROTOBUF_LIB ./zoob ../../../levels/original.json ../../bin/Zoob-debug.apk
