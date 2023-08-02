#include "../include/backends/standard.h"
#include "../include/main.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define ZERO 800.0f
#define ONE 1600.0f
#define LEADER SAMPLE_RATE

int encoded_one_sc = 0;
int encoded_zero_sc = 0;
float *encoded_one_exp = NULL;
float *encoded_zero_exp = NULL;
int sample_wave_size = 0;

void backend_std_init() {
	// Measure sample count to represent one cycle
	if (fm_mode) {
		if (cycles_per_bit > 1)
			printf("Note: Encoding data using multiple cycles is not yet supported in FM mode. The number of cycles per bit will be set to 1.\n");
		cycles_per_bit = 1;


		int transients = 0;	
		float time = 0;
		
		encoded_one_exp = (float *)malloc(SAMPLE_RATE * sizeof(float));

		float last = FM_WAVIFY(time, ONE);
		while (transients < 3) {
			if (last <= 0 && FM_WAVIFY(time, ONE) >= 0)
				transients++;

			last = FM_WAVIFY(time, ONE);

			encoded_one_exp[encoded_one_sc++] = FM_WAVIFY(time, ONE);

			time += TIME_INC;
		}
		
		// Increment once more, index -> size
		encoded_one_sc++;


		time = 0;
		transients = 0;

		encoded_zero_exp = (float *)malloc(SAMPLE_RATE * sizeof(float));

		last = FM_WAVIFY(time, ZERO);
		while (transients < 3) {
			if (last <= 0 && FM_WAVIFY(time, ZERO) >= 0)
				transients++;

			last = FM_WAVIFY(time, ZERO);

			encoded_zero_exp[encoded_zero_sc++] = FM_WAVIFY(time, ONE);

			time += TIME_INC;
		}

		// Increment once more, index -> size
		encoded_zero_sc++;

		// Get a sample of the waveform of a one or a zero
		// Get minimum of either sample count
		sample_wave_size = (encoded_zero_sc < encoded_one_sc) ? encoded_zero_sc : encoded_one_sc;

		// Reformat buffers to only show minimum amount
		encoded_one_exp = realloc(encoded_one_exp, sample_wave_size);
		encoded_zero_exp = realloc(encoded_zero_exp, sample_wave_size);

		assert(encoded_one_sc != 0);
		assert(encoded_zero_sc != 0);
		assert(encoded_one_exp != NULL);
		assert(encoded_zero_exp != NULL);
		assert(sample_wave_size != 0);
	}
}

void backend_std_encode() {
	// Get the size of the file to be encoded
	fseek(from, 0, SEEK_END);
	size_t file_size = ftell(from);
	fseek(from, 0, SEEK_SET);

	// Read in the data from the file to be encoded
	uint8_t *data = malloc(file_size);
	fread(data, 1, file_size, from);

	float *samples = (float *)malloc(LEADER * sizeof(float));
	memset(samples, 0, LEADER * sizeof(float));
	tinywav_write_f(&tw, samples, LEADER);
	free(samples);

	if (fm_mode) {
		for (int i = 0 ; i < file_size; i++) {
			for (int j = 0; j < 8; j++) {
				uint8_t value = (data[i] >> j) & 1;
				int s = value ? encoded_one_sc : encoded_zero_sc;
				samples = (float *)malloc(s * sizeof(float));

				float time = 0;
				for (int si = 0; si < s; si++) {
					samples[si] = FM_WAVIFY(time, (value ? ONE : ZERO));
					time += TIME_INC;
				}

				tinywav_write_f(&tw, samples, s);

				free(samples);
			}
		}

		return;
	}
	
	
	samples = (float *)malloc(cycles_per_bit * sizeof(float));

	for (int i = 0 ; i < file_size; i++) {
		for (int j = 0; j < 8; j++) {
			uint8_t value = (data[i] >> j) & 1;

			for (int si = 0; si < cycles_per_bit; si++)
				samples[si] = PM_WAVIFY((value ? ONE : ZERO));
			
			tinywav_write_f(&tw, samples, cycles_per_bit);
		}
	}

	free(samples);
}

size_t backend_std_decode(uint8_t *buffer) {
	buffer = malloc(1);
	size_t buffer_size = 0;

	float *samples = (float *)malloc(LEADER * sizeof(float));
	tinywav_read_f(&tw, samples, LEADER);
	free(samples);

	if (fm_mode) {
		samples = (float *)malloc(sample_wave_size * sizeof(float));
		int bit_ptr;

		while (tinywav_read_f(&tw, samples, sample_wave_size) != 0) {
			int one_matches = 0;
			int zero_matches = 0;

			for (int i = 0; i < sample_wave_size; i++) {
				int min = samples[i] - tolerance;
				int max = samples[i] + tolerance;

				if (encoded_one_exp[i] <= max && encoded_one_exp[i] >= min)
					one_matches++;
				else
					zero_matches++;
			}

			uint8_t value = 0;

			if (one_matches > zero_matches) {
				// It is probably a one

				// Position reader at next cycle
				if (encoded_one_sc - sample_wave_size > 0) {
					int i = encoded_one_sc;
					for (; (encoded_one_sc - sample_wave_size) > sample_wave_size; i -= sample_wave_size)
						tinywav_read_f(&tw, samples, sample_wave_size);

					tinywav_read_f(&tw, samples, i);
				}
				
				value = 1;
			}
			
			buffer[buffer_size] |= (value << bit_ptr);

			if (bit_ptr == 8) {
				bit_ptr = 0;
				buffer = realloc(buffer, ++buffer_size + 1);
			}
		}

		free(samples);

		return buffer_size;
	}

	samples = (float *)malloc(cycles_per_bit * sizeof(float));
	uint8_t bit_ptr = 0;
	
	// Segmentation fault
	while (tinywav_read_f(&tw, samples, cycles_per_bit) != 0) {

		*(buffer + buffer_size) |= ((PM_WAVIFY(ONE) == samples[0]) << bit_ptr);
		bit_ptr++;

		if (bit_ptr == 8) {
			bit_ptr = 0;
			buffer = realloc(buffer, ++buffer_size + 1);
		}
	}

	free(samples);

	// Decode from PM
	return buffer_size;
}
