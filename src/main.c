#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

uint8_t mode = 0; // 0: Encode 1: Decode
FILE *from = NULL;
FILE *to = NULL;

// encoder.out path/to/data.bin path/to/data.wav ; Encode .bin data to .wav
// encoder.out path/to/data.wav path/to/data.bin ; Decode .wav data to .bin
//			       ^
//			       to
int main(int argc, char **argv) {
	if (argc < 3) {
		printf("Use case (only use .bin or .wav files):\nencoder.out path/to/data.bin path/to/data.wav ; Encode .bin data to .wav\nencoder.out path/to/data.wav path/to/data.bin ; Decode .wav data to .bin\n");
		return 1;
	}

	from = fopen(argv[1], "r");
	to = fopen(argv[2], "w+");
	mode = *(argv[2] + strlen(argv[2]) - 1) == 'n';

	if (from == NULL) {
		printf("Unable to open file \"%s\"\n", argv[1]);
		return 1;
	}

	if (to == NULL) {
		printf("Unable to open file \"%s\"\n", argv[2]);
		return 1;
	}

	if (mode == 1) {
		// Do decode stuff

		return 0;
	}

	// Do encode stuff


	return 0;
}