all: test

test: test.o cad2d.o
	gcc test.o cad2d.o -o test -l m

test.o: test.c
	gcc -ansi -pedantic-errors test.c -c

cad2d.o: cad2d.c
	gcc -ansi -pedantic-errors cad2d.c -c

clean:
	rm -f *.o test *.eps *.gtucad