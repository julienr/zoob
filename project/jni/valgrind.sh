#!/bin/sh
valgrind --smc-check=all --leak-check=full --suppressions=opengl.supp ./app ../bin/Zoob-NDK.apk
