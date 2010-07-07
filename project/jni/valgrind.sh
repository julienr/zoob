#!/bin/sh
valgrind --smc-check=all --leak-check=full --undef-value-errors=yes --suppressions=opengl.supp --show-reachable=yes ./app ../bin/Zoob.apk 5
