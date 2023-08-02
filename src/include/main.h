#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "backends/standard.h"

#define SAMPLE_RATE 48000 // Idea: could be neat if the user was able to specify this
			  // although, I don't know how useful that would be.
#define STD_SAMPLE_COUNT (SAMPLE_RATE / 10)
#define TIME_INC 0.00001
#define FM_WAVIFY(time, freq) (sin((float)time * (float)freq) / 2)
#define PM_WAVIFY(freq) (sin((float)freq) / 2)

extern int cycles_per_bit;
extern int enc_dec_mode;
extern uint8_t fm_mode;

enum {
	BACKEND_STD,
	BACKEND_COUNT,
};

static const char *enc_dec_modes[BACKEND_COUNT] = {
	[BACKEND_STD] = "std"
};

#endif