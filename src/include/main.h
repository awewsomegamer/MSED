#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "backends/standard.h"

enum {
	BACKEND_STD,
	BACKEND_COUNT,
};

static const char *enc_dec_modes[BACKEND_COUNT] = {
	[BACKEND_STD] = "std"
};

#endif