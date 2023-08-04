#ifndef MSED_MAIN_H
#define MSED_MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "tinywav.h"

// Backend includes
#include "backends/msed_standard.h"

#define WAVIFY(time, freq, amp) (sin((float)time * (float)freq) * (float)amp)
#define PM_MODE 0
#define FM_MODE 1
#define AM_MODE 2

extern int cycles_per_bit;
extern int enc_dec_mode;
extern float tolerance;
extern uint8_t modulation_mode;

extern FILE *from;
extern FILE *to;
extern TinyWav tw;

#define BACKEND_STRUCT(index, name, f_init, f_encode, f_decode) [index] = (struct backend_functions) { name, f_init, f_encode, f_decode },
struct backend_functions {
	const char *name;
	int (*functions_init)();
	void (*encode_function)();
	size_t (*decode_function)(uint8_t **);
};

enum {
	BACKEND_STD,
	BACKEND_COUNT,
};

static struct backend_functions backends[BACKEND_COUNT] = {
	BACKEND_STRUCT(BACKEND_STD, "std", msed_backend_std_init, msed_backend_std_encode, msed_backend_std_decode)
};

#endif