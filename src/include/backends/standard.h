#ifndef STANDARD_H
#define STANDARD_H

#include <stdint.h>
#include <stddef.h>
#include "../tinywav.h"

int backend_std_init();
void backend_std_encode();
size_t backend_std_decode(uint8_t **buffer);

#endif