#include "../include/backends/standard.h"
#include "../include/main.h"

void backend_std_init() {
	samples_per_bit = 240;
}

float backend_std_encode(uint8_t data) {
	return data ? 1600 : 800;
}

uint8_t backend_std_decode(float data) {
	return (data == 800) ? 0 : 1;
}
