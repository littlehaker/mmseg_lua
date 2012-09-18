cp ../src/*.o .
cp ../src/*.lo .
cp ../src/*.h .
rm mmseg_main.o
g++ mmseg.cpp *.o -o mmseg.so -llua5.1 -shared -I/usr/include/lua5.1 -fPIC
