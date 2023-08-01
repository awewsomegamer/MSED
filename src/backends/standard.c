#include "../include/backends/standard.h"

float backend_std_encode(uint8_t data) {
	return data ? 1600 : 800;
}

uint8_t backend_std_decode(float data) {
	return (data == 800) ? 0 : 1;
}
