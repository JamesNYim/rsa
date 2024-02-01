# Assignment 5: Cryptography (README.md)

## Short description:
	This program will be able to generate public and private keys. It will also be able to encrypt a message in a file. It will also be able to said message.

## Build
	Type "make" on the command line with the Makefile provided in the directory this will make keygen, encrypt, and decrypt
	Type "make keygen" to only make keygen
	Type "make encrypt" to only make encrypt
	Type "make decrypt" to only make decrypt

## Running
	./keygen -(function options)
	./encrypt -(function options)
	./decrypt -(function options)

## Cleaning
	Type "clean" on the command line, using Makefile provided

## Errors:
	Currently checking for signatures does not work in encrypt.c (this is on line 88)

## Error Handling:
	If there is an error. Check if input files for the respective program is present. The program will not work of the input files are not present.












