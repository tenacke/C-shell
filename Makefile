myshell: main.o alias.o builtins.o history.o parse.o print.o shell.o strlib.o types.h
	gcc -o myshell main.o alias.o builtins.o history.o parse.o print.o shell.o strlib.o

main.o: main.c main.h
	gcc -c main.c -o main.o

alias.o: alias.c alias.h
	gcc -c alias.c -o alias.o

builtins.o: builtins.c builtins.h 
	gcc -c builtins.c -o builtins.o

history.o: history.c history.h
	gcc -c history.c -o history.o

parse.o: parse.c parse.h
	gcc -c parse.c -o parse.o

print.o: print.c print.h
	gcc -c print.c -o print.o

shell.o: shell.c shell.h
	gcc -c shell.c -o shell.o

strlib.o: strlib.c strlib.h
	gcc -c strlib.c -o strlib.o

clean:	
	rm -f *.o myshell