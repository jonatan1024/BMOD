#!/bin/sh

# to compile 32bit bullet libs on 64bit linux... eh... good luck! :\

g++ -m32 -static-libstdc++ -Wno-write-strings -Wno-attributes -D stricmp=strcasecmp -fpermissive \
-I ../hlsdk/common/ -I ../hlsdk/dlls/ -I ../hlsdk/engine -I ../bullet/src/ -I ../metamod/ -I ../amxmodx/ \
-L ../bullet/src/.libs/ \
-Wl,--start-group \
call.cpp forwards.cpp main.cpp model.cpp natives.cpp object.cpp sdk/amxxmodule.cpp euler.cpp debug.cpp \
-Wl,--end-group \
-l BulletCollision -l BulletDynamics -l LinearMath \
-shared -o amxmodx_modules/bmod_amxx_i386.so
