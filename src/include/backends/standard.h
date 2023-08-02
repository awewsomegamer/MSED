#ifndef STANDARD_H
#define STANDARD_H

#include <stdint.h>
#include <stddef.h>

void backend_std_init();
float backend_std_encode(uint8_t data);
uint8_t backend_std_decode(float *data, uint8_t *buffer, size_t *size, int *bit_ptr);

#endif