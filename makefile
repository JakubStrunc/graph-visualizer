EXE=graph.EXE
OBJ=main.o postfixmath.o postscript.o queue.o shuntingyard.o stack.o
OPT=-g -std=c99 -pedantic -Wall -Wextra



$(EXE): $(OBJ)
	gcc $(OBJ) -o $(EXE) $(OPT) -lm -lc -z noexecstack

.c.o:
	gcc -c $^ $(OPT)


rebuild:
	rm -f $(OBJ) $(EXE)
	make
