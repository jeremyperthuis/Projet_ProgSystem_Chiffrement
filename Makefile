
all: prog

prog: director.o main.o
	gcc -o prog director.o main.o

director.o: director.c
	gcc -o director.o -c director.c

main.o: main.c director.h
	gcc -o main.o -c main.c

clean:
	rm -f *.o

mrproper: clean
	rm -f $prog