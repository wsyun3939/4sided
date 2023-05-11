CFLAGS=-O3 -Wall -g
a:main.o 
	g++ main.o -o a
main.o:instance.h config.h bb.cpp
bb.o:instance.h config.h