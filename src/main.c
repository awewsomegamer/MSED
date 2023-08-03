#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "include/main.h"
#include "include/tinywav.h"

uint8_t mode = 0; // 0: Encode 1: Decode
FILE *from = NULL;
FILE *to = NULL;
TinyWav tw;

int cycles_per_bit = 1;
int enc_dec_mode = 0;
float tolerance = 0;
uint8_t fm_mode = 0;

void (*encode_functions[BACKEND_COUNT])() = {
	[BACKEND_STD] = backend_std_encode,
};

size_t (*decode_functions[BACKEND_COUNT])(uint8_t **) = {
	[BACKEND_STD] = backend_std_decode,
};

void (*functions_init[BACKEND_COUNT])() = { 
	[BACKEND_STD] = backend_std_init,
};

// encoder.out path/to/data.bin path/to/data.wav ; Encode .bin data to .wav in standard encoder / decoder mode using pulse modulation
// encoder.out path/to/data.wav path/to/data.bin ; Decode .wav data to .bin in standard encoder / decoder mode using pulse modulation
// encoder.out <from> <to> [options]
//			       ^
//			       to
int main(int argc, char **argv) {
	if (argc < 3) {
		if (strcmp(argv[1], "options") == 0) {
			printf("    encoder.out a b [options]\n");
			printf("\ta - File whose data to encode / decode\n");
			printf("\t    If file 'a' is a .bin then the program is encoding to 'b', if it is a .wav then it is decoding to 'b'.\n");
			printf("\tb - File to encode / decode data to\n");
			printf("\t-mode <name> - The name of the backend to use (hint: execute command encoder.out modes)\n");
			printf("\t-fm - Enable frequency modulated mode\n");
			printf("\t-tolerance <integer> - Plus or minus <integer> sample value tolerance\n"); // Express this in plainer english
			return 0;
		} else if (strcmp(argv[1], "modes") == 0) {
			for (int i = 0; i < BACKEND_COUNT; i++)
				printf("%s\n", enc_dec_modes[i]);

			return 0;
		}

		printf("Use case (only use .bin or .wav files):\nencoder.out path/to/data.bin path/to/data.wav ; Encode .bin data to .wav in standard encoder / decoder mode using pulse modulation\nencoder.out path/to/data.wav path/to/data.bin ; Decode .wav data to .bin in standard encoder / decoder mode using pulse modulation\nencoder.out <from> <to> [options]\nType encoder.out options to see a list of options.\n");
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
		// Idea: Wouldn't it be neat to have multiple channels of data?
		tinywav_open_write(&tw, 1, SAMPLE_RATE, TW_FLOAT32, TW_INLINE, argv[2]);
	else
		tinywav_open_read(&tw, argv[1], TW_INLINE);

	// More options have been specified
	if (argc >= 3) {
		for (int i = 3; i < argc; i++) {
			if (strcmp(argv[i], "-mode") == 0) {
				for (int j = 0; j < sizeof(enc_dec_modes) / sizeof(enc_dec_modes[0]); j++) {
					if (strcmp(argv[i + 1], enc_dec_modes[j]) == 0) {
						enc_dec_mode = i;
						break;
					}
				}
			} else if (strcmp(argv[i], "-fm") == 0) {
				fm_mode = 1;
			} else if (strcmp(argv[i], "-cpb") == 0) {
				cycles_per_bit = atoi(argv[i + 1]);
			} else if (strcmp(argv[i], "-tolerance") == 0) {
				tolerance = atof(argv[i + 1]);
			} 
		}

	}

	(*functions_init[enc_dec_mode])();

	if (mode == 1) {
		uint8_t *buffer = NULL;
		size_t size = (*decode_functions[enc_dec_mode])(&buffer);

		fwrite(buffer, 1, size, to);
		tinywav_close_read(&tw);

		return 0;
	}

	(*encode_functions[enc_dec_mode])();
	tinywav_close_write(&tw);

	return 0;
}