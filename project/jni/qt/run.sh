#!/bin/sh
PROTOBUF_LIB=/home/julien/programs/protobuf/lib/
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PROTOBUF_LIB ./zoob ../../../levels/multiplayer.json ../../bin/Zoob-debug.apk
