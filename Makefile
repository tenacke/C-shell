myshell: main.o alias.o builtins.o history.o parse.o print.o shell.o strlib.o process.o types.h
	gcc-13 -g -o myshell main.o alias.o builtins.o history.o parse.o print.o shell.o strlib.o process.o

main.o: main.c main.h
	gcc-13 -g -c main.c -o main.o

alias.o: alias.c alias.h
	gcc-13 -g -c alias.c -o alias.o

builtins.o: builtins.c builtins.h 
	gcc-13 -g -c builtins.c -o builtins.o

history.o: history.c history.h
	gcc-13 -g -c history.c -o history.o

parse.o: parse.c parse.h
	gcc-13 -g -c parse.c -o parse.o

print.o: print.c print.h
	gcc-13 -g -c print.c -o print.o

shell.o: shell.c shell.h
	gcc-13 -g -c shell.c -o shell.o

process.o: process.c process.h
	gcc-13 -g -c process.c -o process.o

strlib.o: strlib.c strlib.h
	gcc-13 -g -c strlib.c -o strlib.o

clean:	
	rm -f *.o myshell