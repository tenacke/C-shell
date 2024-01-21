myshell: main.o alias.o builtins.o parse.o print.o shell.o strlib.o process.o types.h
	gcc -g -o myshell main.o alias.o builtins.o parse.o print.o shell.o strlib.o process.o

main.o: main.c main.h
	gcc -g -c main.c -o main.o

alias.o: alias.c alias.h
	gcc -g -c alias.c -o alias.o

builtins.o: builtins.c builtins.h 
	gcc -g -c builtins.c -o builtins.o

parse.o: parse.c parse.h
	gcc -g -c parse.c -o parse.o

print.o: print.c print.h
	gcc -g -c print.c -o print.o

shell.o: shell.c shell.h
	gcc -g -c shell.c -o shell.o

process.o: process.c process.h
	gcc -g -c process.c -o process.o

strlib.o: strlib.c strlib.h
	gcc -g -c strlib.c -o strlib.o

clean:	
	rm -f *.o myshell