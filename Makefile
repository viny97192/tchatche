CC=gcc
CFLAGS=-W -Wall -ansi -pedantic

all:tchatche tchatche_serveur

tchatche:tchatche.o conversions.o
	$(CC) -o tchatche tchatche.o conversions.o -pthread

tchatche_serveur:tchatche_serveur.o conversions.o
	$(CC) -o tchatche_serveur tchatche_serveur.o conversions.o

conversions.o:conversions.c 
	$(CC) -o conversions.o -c conversions.c

tchatche.o:tchatche.c
	$(CC) -o tchatche.o -c tchatche.c 

tchatche_serveur.o:tchatche_serveur.c
	$(CC) -o tchatche_serveur.o -c tchatche_serveur.c

.PHONY:clean exec

clean:
	rm -rf *.o

exec:
	rm -rf tchatche tchatche_serveur

fifo:
	rm fifo*
