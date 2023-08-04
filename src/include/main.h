#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "backends/standard.h"
#include "tinywav.h"

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

enum {
	BACKEND_STD,
	BACKEND_COUNT,
};

static const char *enc_dec_modes[BACKEND_COUNT] = {
	[BACKEND_STD] = "std"
};

#endif