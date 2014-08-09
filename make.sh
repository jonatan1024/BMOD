#!/bin/sh

# to compile 32bit bullet libs on 64bit linux use:
# cmake . -G "Unix Makefiles" -DCMAKE_CXX_FLAGS=-m32 -DBUILD_DEMOS=OFF
# make

g++ -static-libstdc++ \
-I ../hlsdk/common/ -I ../hlsdk/dlls/ -I ../hlsdk/engine \
-I ../bullet/src/ -I ../metamod/ -I ../amxmodx/ \
-L ../bullet/src/BulletCollision/ \
-L ../bullet/src/BulletDynamics/ \
-L ../bullet/src/LinearMath/ \
-l BulletCollision -l BulletDynamics -l LinearMath \
call.cpp forwards.cpp main.cpp model.cpp natives.cpp object.cpp \
sdk/amxxmodule.cpp \
-o amxmodx_modules/bmod_amxx_i386.so -shared -m32 \
-Wno-write-strings -Wno-attributes -D stricmp=strcasecmp -fpermissive -fPIC
