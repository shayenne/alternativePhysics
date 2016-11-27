LDFLAGS= -lm
CFLAGS= -Wall -pedantic --ansi
OFLAGS= -std=c99 -O3

DEPS = ep.h ppmFunctions.h
OBJ = ep.o ppmFunctions.o

%.o:	%.c $(DEPS)
	$(CC) $(CFLAGS)  $(OFLAGS) $(LDFLAGS)  -c -o $@ $< $(OFLAGS) $(LDFLAGS) 

alterPhysics1:	$(OBJ)
	gcc $(CFLAGS) -o $@ $^  $(OFLAGS) $(LDFLAGS) 

all: 	alterPhysics
	@echo Programação Paralela e Distribuída - 2 Semestre/2016

alterPhysics: ep.c ppmFunctions.c ep.h ppmFunctions.h
	gcc -o alterPhysics ep.c $(CFLAGS) $(OFLAGS) $(LDFLAGS) 

clean:
	rm *~ alterPhysics
