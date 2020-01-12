#ifndef MAIN_H			//Statement to be sure this header file cannot be called more than once
#define MAIN_H


#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "game.h"
#include "hardware.h"

// Function prototypes (Only non-static functions are defined in the header files)
// Functions that cannot be included from the library properly
int usleep(unsigned int usec);


#endif //MAIN_H