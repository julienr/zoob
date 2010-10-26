#!/bin/sh
PROTOCC=~/programs/protobuf/bin/protoc
$PROTOCC --cpp_out=jni/net/protobuf/ zoob.proto
