# -*- Makefile -*-

#target: dependencies
#	action

#list each of the target that you built
all: test

test: test.o cad2d.o
	gcc test.o cad2d.o -o test -l m

test.o: test.c
	gcc test.c -c

cad2d.o: cad2d.c
	gcc cad2d.c -c
#gcc -ansi -pedantic-errors test.c -c


clean:
	rm -f *.o test *.eps *.gtucad