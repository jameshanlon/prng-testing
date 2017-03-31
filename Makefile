CC=gcc
CFLAGS=-std=c99 -O3 -Wall -Werror -pedantic -Wconversion -lm \
        -I TestU01/include \
        TestU01/lib/libtestu01.a \
        TestU01/lib/libmylib.a \
        TestU01/lib/libprobdist.a

all: crush-std crush-rev

crush-std: crush.c
	$(CC) $(CFLAGS) $< -o $@

crush-rev: crush.c
	$(CC) $(CFLAGS) $< -o $@ -DREVERSE
