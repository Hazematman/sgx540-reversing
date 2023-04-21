#!/bin/bash
gcc -std=gnu99 -ldl -shared -Wall -Wextra ldpreload.c -o intercept.so -fPIC -pthread
