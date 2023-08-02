#include "../include/backends/standard.h"
#include "../include/main.h"

#define ZERO 800.0f
#define ONE 1600.0f

void backend_std_init() {
	if (cycles_per_bit == 0)
		cycles_per_bit = 1;
}

float backend_std_encode(uint8_t data) {
	return data ? ONE : ZERO;
}

uint8_t backend_std_decode(float *data, uint8_t *buffer, size_t *size, int *bit_ptr) {
	if (fm_mode) {
		// Decode from FM
		return 0;
	}

	// Decode from PM
	return (data[0] == PM_WAVIFY(ZERO)) ? 0 : 1;
}
