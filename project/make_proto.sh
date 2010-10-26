#!/bin/sh
PROTOCC=~/programs/protobuf/bin/protoc
$PROTOCC --cpp_out=jni/net/protobuf/ zoob.proto

#Now, the protobuf extension_set.h header is using the STL. We don't have the STL on android.
#But since we don't need protobuf extension, we can just remove this header from the generated code
HEADER=jni/net/protobuf/zoob.pb.h
sed -i -e 's/\(#include <google\/protobuf\/extension_set.h>\)/\/\/\1/g' $HEADER 
