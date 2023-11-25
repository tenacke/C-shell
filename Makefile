myshell: main.o func.o
	gcc -o myshell main.o func.o

main.o: main.c func.h
	gcc -c main.c -o main.o

func.o: func.c func.h
	gcc -c func.c -o func.o

clean:	
	rm -f *.o myshell