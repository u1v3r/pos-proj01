# Makefile pre proj01

PNAME=proj01
CC=gcc
CFLAGS=-Wall -ansi -g -O $(PNAME).c -o $(PNAME)


build:
	$(CC) $(CFLAGS)
clean:
	rm -f $(PNAME)
