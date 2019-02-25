export CC=i686-linux-android-clang
export CXX=i686-linux-android-clang++

PWD=`pwd`

$CXX mprop.cpp -o mprop -static  -l$PWD/x86_M/libc.a -l$PWD/x86_M/libdl.a
