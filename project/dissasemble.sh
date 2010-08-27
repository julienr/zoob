#!/bin/bash
#dissasemble libzoob and open vim on the ASM file
NDK_BIN=~/programs/android-ndk-r4/build/prebuilt/linux-x86/arm-eabi-4.4.0/bin
$NDK_BIN/arm-eabi-objdump -d libs/armeabi/libzoob.so > /tmp/libzoob.S
vi /tmp/libzoob.S
