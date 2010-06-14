#!/bin/sh
valgrind --smc-check=all --leak-check=full --undef-value-errors=yes --suppressions=opengl.supp ./app ../bin/Zoob-NDK.apk
