#ifndef UTILITIES_HPP
#define UTILITIES_HPP

// std includes
#include <stdint.h>

// project cpp includes
#define MILLISECONDS_PER_SECOND 1000
#define MICROSECONDS_PER_SECOND 1000000
#define MICROS_TO_MILLISECONDS(us) ((unsigned long)((double)(us) / (double)MILLISECONDS_PER_SECOND));
void printArray(const char *title, volatile uint8_t *data, uint8_t quantity);

#endif // UTILITIES_HPP
