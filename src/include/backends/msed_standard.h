#ifndef MSED_STANDARD_BACKEND_H
#define MSED_STANDARD_BACKEND_H

#include <stdint.h>
#include <stddef.h>
#include "../tinywav.h"

int msed_backend_std_init();
void msed_backend_std_encode();
size_t msed_backend_std_decode(uint8_t **buffer);

#endif