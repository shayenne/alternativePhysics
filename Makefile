LDFLAGS= -lm
CFLAGS= -Wall -pedantic --ansi
OFLAGS= -std=c99 -O3 -openmp

all: 	alterPhysics
	@echo Programação Paralela e Distribuída - 2 Semestre/2016

alterPhysics: ep.c
	gcc -o alterPhysics ep.c $(CFLAGS) $(OFLAGS) $(LDFLAGS) 
