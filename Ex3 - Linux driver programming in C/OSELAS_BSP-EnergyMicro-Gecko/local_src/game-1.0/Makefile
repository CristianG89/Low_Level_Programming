#############################
# MAKEFILE FOR EX3, TDT4258 #
#############################

# Definition of GNU C compiler (named GCC) to generate object .o file
CC = arm-cortexm3-uclinuxeabi-gcc
# Definition of GNU linker to generate .o file. In C, files can be linked with "-gcc" command, calling later "-ld" automatically.
LD = arm-cortexm3-uclinuxeabi-gcc

# Add more flags here if desired, but don't remove the "+="
CFLAGS += -O2 -g -Wall -std=c99 #-Wsign-compare -Wfloat-equal -Wformat-security #-Werror
LDFLAGS +=

game: main.o game.o hardware.o
	${LD} ${LDFLAGS} $^ -o $@ 

%.o: %.c
	${CC} ${CFLAGS} -c $< -o $@

install:

.PHONY: pretty
pretty :
	-indent *.c *.h

.PHONY: clean
clean :
	-rm -rf game *.o