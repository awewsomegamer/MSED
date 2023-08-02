#include "../include/backends/standard.h"
#include "../include/main.h"

void backend_std_init() {
	if (cycles_per_bit == 0)
		cycles_per_bit = 1;
}

float backend_std_encode(uint8_t data) {
	return data ? 1600 : 800;
}

uint8_t backend_std_decode(float data) {
	return (data == 800) ? 0 : 1;
}
