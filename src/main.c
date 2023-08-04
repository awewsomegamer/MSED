#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "include/backends/msed_standard.h"
#include "include/main.h"
#include "include/tinywav.h"

uint8_t mode = 0; // 0: Encode 1: Decode
FILE *from = NULL;
FILE *to = NULL;
TinyWav tw;

int cycles_per_bit = 1;
int enc_dec_mode = 0;
float tolerance = 0;
uint8_t modulation_mode = PM_MODE;

// encoder.out path/to/data.bin path/to/data.wav ; Encode .bin data to .wav in standard encoder / decoder mode using pulse modulation
// encoder.out path/to/data.wav path/to/data.bin ; Decode .wav data to .bin in standard encoder / decoder mode using pulse modulation
// encoder.out <from> <to> [options]
//			       ^
//			       to
int main(int argc, char **argv) {
	// Command line help
	if (argc == 1) {
		printf("Use case (only use .bin or .wav files):\nencoder.out path/to/data.bin path/to/data.wav ; Encode .bin data to .wav in standard encoder / decoder mode using pulse modulation\nencoder.out path/to/data.wav path/to/data.bin ; Decode .wav data to .bin in standard encoder / decoder mode using pulse modulation\nencoder.out <from> <to> [options]\nType encoder.out options to see a list of options.\n");
		return 1;
	}

	if (argc < 3) {
		if (strcmp(argv[1], "options") == 0) {
			printf("    encoder.out a b [options]\n");
			printf("    Files:\n");
			printf("\ta - File whose data to encode / decode\n");
			printf("\t    If file 'a' is a .bin then the program is encoding to 'b', if it is a .wav then it is decoding to 'b'.\n");
			printf("\tb - File to encode / decode data to\n");
			printf("    Options:\n");
			printf("\t-mode <name> - The name of the backend to use (hint: execute command msed backends)\n");
			printf("\t-pm - Change to pulse modulated mode (default)\n");
			printf("\t-fm - Change to frequency modulated mode\n");
			printf("\t-am - Change to amplitude modulated mode\n");
			printf("\t-cpb <integer> - Specify the number of cycles per bit of data (default: 1)\n");
			printf("\t-tolerance <integer> - Plus or minus <integer> sample value tolerance (default: 0)\n"); // Express this in plainer english

			return 0;
		} else if (strcmp(argv[1], "backends") == 0) {
			for (int i = 0; i < BACKEND_COUNT; i++)
				printf("%s\n", backends[i].name);

			return 0;
		}
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

	// More options have been specified
	if (argc >= 3) {
		for (int i = 3; i < argc; i++) {
			if (strcmp(argv[i], "-mode") == 0) {
				for (int j = 0; j < BACKEND_COUNT; j++) {
					if (strcmp(argv[i + 1], backends[i].name) == 0) {
						enc_dec_mode = i;
						break;
					}
				}
			} else if (strcmp(argv[i], "-fm") == 0) {
				modulation_mode = FM_MODE;
			} else if (strcmp(argv[i], "-am") == 0) {
				modulation_mode = AM_MODE;
			} else if (strcmp(argv[i], "-cpb") == 0) {
				cycles_per_bit = atoi(argv[i + 1]);
				if (cycles_per_bit == 0) {
					printf("Cycles per bit cannot be 0!\n");
					return 1;
				}
			} else if (strcmp(argv[i], "-tolerance") == 0) {
				tolerance = atof(argv[i + 1]);
			} 
		}
	}

	int sample_rate = (*backends[enc_dec_mode].functions_init)();

	if (mode == 0)
		// Idea: Wouldn't it be neat to have multiple channels of data?
		tinywav_open_write(&tw, 1, sample_rate, TW_FLOAT32, TW_INLINE, argv[2]);
	else
		tinywav_open_read(&tw, argv[1], TW_INLINE);

	if (mode == 1) {
		// Decode
		uint8_t *buffer = NULL;
		size_t size = (*backends[enc_dec_mode].decode_function)(&buffer);

		fwrite(buffer, 1, size, to);
		tinywav_close_read(&tw);

		return 0;
	}

	// Encode
	(*backends[enc_dec_mode].encode_function)();
	tinywav_close_write(&tw);

	return 0;
}