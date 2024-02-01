CC = gcc
CFLAGS = -Wall -Wextra -Werror -Wpedantic -g $(shell pkg-config --cflags gmp)
LFLAGS = $(shell pkg-config --libs gmp)

.PHONY: all clean format

all: keygen encrypt decrypt

keygen: keygen.o randstate.o numtheory.o rsa.o
	$(CC) -o keygen keygen.o randstate.o numtheory.o rsa.o $(LFLAGS) 

encrypt: encrypt.o randstate.o numtheory.o rsa.o
	$(CC) -o encrypt encrypt.o randstate.o numtheory.o rsa.o $(LFLAGS) 


decrypt: decrypt.o randstate.o numtheory.o rsa.o
	$(CC) -o decrypt decrypt.o randstate.o numtheory.o rsa.o $(LFLAGS) 

decrypt.o: decrypt.c randstate.c numtheory.c rsa.c
	$(CC) $(CFLAGS) -c decrypt.c randstate.c numtheory.c rsa.c

encrypt.o: encrypt.c randstate.c numtheory.c rsa.c
	$(CC) $(CFLAGS) -c encrypt.c randstate.c numtheory.c rsa.c

keygen.o: keygen.c randstate.c numtheory.c rsa.c
	$(CC) $(CFLAGS) -c keygen.c randstate.c numtheory.c rsa.c

rsa.o: rsa.c
	$(CC) $(CFLAGS) -c rsa.c

randstate.o: randstate.c
	$(CC) $(CFLAGS) -c randstate.c

format:
	clang-format -i -style=file *.[ch]

clean:
	rm -f test keygen encrypt decrypt *.o *.priv *.pub

debug: all



