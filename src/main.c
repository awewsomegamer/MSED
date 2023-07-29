#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

uint8_t mode = 0; // 0: Encode 1: Decode
FILE *from = NULL;
FILE *to = NULL;

int enc_dec_mode = 0;
const char *enc_dec_modes[] = {
	"std"
};

// Wrapper functions which call different functions based
// on enc_dec_mode. This way data can be encoded / decoded
// by calling one function and not having to worry about how.
// All we get back is data. "Middle End"
uint8_t *encode() {

	return NULL;
}

uint8_t *decode() {

	return NULL;
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
		// Do decode stuff
		buffer = decode();

		goto save_generated;
	}

	// Do encode stuff
	buffer = encode();

	save_generated:




	return 0;
}