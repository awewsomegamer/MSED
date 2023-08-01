#ifndef STANDARD_H
#define STANDARD_H

#include <stdint.h>

void backend_std_init();
float backend_std_encode(uint8_t data);
uint8_t backend_std_decode(float data);

#endif