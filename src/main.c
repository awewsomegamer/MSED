#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "include/main.h"
#include "include/tinywav.h"
#include <math.h>

#define SAMPLE_RATE 48000

uint8_t mode = 0; // 0: Encode 1: Decode
FILE *from = NULL;
FILE *to = NULL;
TinyWav tw;

int enc_dec_mode = 0;

float (*encode_functions[BACKEND_COUNT])(uint8_t) = {
	[BACKEND_STD] = backend_std_encode,
};

uint8_t (*decode_functions[BACKEND_COUNT])(float) = {
	[BACKEND_STD] = backend_std_decode,
};

// Wrapper functions which call different functions based
// on enc_dec_mode. This way data can be encoded / decoded
// by calling one function and not having to worry about how.
// All we get back is data. "Middle End"
void encode() {
	// Get the size of the file to be encoded
	fseek(from, 0, SEEK_END);
	size_t file_size = ftell(from);
	fseek(from, 0, SEEK_SET);

	// Read in the data from the file to be encoded
	uint8_t *data = malloc(file_size);
	fread(data, 1, file_size, from);

	float wiggle = 0;
	for (size_t i = 0; i < file_size; i++) {
		for (int j = 0; j < 8; j++) {
			float freq = (*encode_functions[enc_dec_mode])((data[i] >> j) & 1);
			float *samples = (float *)malloc(sizeof(float) * freq);

			for (int s = 0; s < freq; s++) {
				samples[s] = sin(freq * wiggle) / 10;
				wiggle += 0.1;
			}

			tinywav_write_f(&tw, samples, freq);
			memset(samples, 0, freq);
			free(samples);
		}
		
	}
}

void decode() {

}

// encoder.out path/to/data.bin path/to/data.wav ; Encode .bin data to .wav in standard encoder / decoder mode
// encoder.out path/to/data.wav path/to/data.bin ; Decode .wav data to .bin in standard encoder / decoder mode
// encoder.out <from> <to> [encoder / decoder mode]
//			       ^
//			       to
int main(int argc, char **argv) {
	if (argc < 3) {
		printf("Use case (only use .bin or .wav files):\nencoder.out <from> <to> [encoder / decoder mode]\nencoder.out path/to/data.bin path/to/data.wav ; Encode .bin data to .wav in standard encoder / decoder mode\nencoder.out path/to/data.wav path/to/data.bin ; Decode .wav data to .bin in standard encoder / decoder mode\n");
		return 1;
	}

	// Initialize
	from = fopen(argv[1], "r");
	to = fopen(argv[2], "w+");
	mode = *(argv[2] + strlen(argv[2]) - 1) == 'n';

	// Ensure we have opened the "from" file
	if (from == NULL) {
		printf("Unable to open file \"%s\"\n", argv[1]);
		return 1;
	}

	// Ensure we have opened the "to" file
	if (to == NULL) {
		printf("Unable to open file \"%s\"\n", argv[2]);
		return 1;
	}

	if (mode == 0)
		tinywav_open_write(&tw, 1, SAMPLE_RATE, TW_FLOAT32, TW_INLINE, argv[2]);
	else
		tinywav_open_read(&tw, argv[1], TW_SPLIT);

	// enc_dec_mode was specified
	if (argc > 3) {
		for (int i = 0; i < sizeof(enc_dec_modes) / sizeof(enc_dec_modes[0]); i++) {
			if (strcmp(argv[3], enc_dec_modes[i]) == 0) {
				enc_dec_mode = i;
				break;
			}
		}
	}

	uint8_t *buffer;
	if (mode == 1) {
		decode();
		tinywav_close_read(&tw);

		return 0;
	}

	encode();
	tinywav_close_write(&tw);

	return 0;
}