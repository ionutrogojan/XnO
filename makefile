SOURCE=main.c
PROGRAM=main
LIBS=-lm -lraylib

default: $(SOURCE)
	gcc $(SOURCE) $(LIBS) -o $(PROGRAM)