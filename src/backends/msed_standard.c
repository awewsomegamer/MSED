#include "../include/backends/msed_standard.h"
#include "../include/main.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define SAMPLE_RATE 480000
#define STD_SAMPLE_COUNT 1000
#define TIME_INC 0.00001

#define ZERO 800.0f
#define ONE (float)(ZERO * 2)
#define LEADER SAMPLE_RATE
#define AMPLITUDE 1/2

int encoded_one_sc = 0;
int encoded_zero_sc = 0;
float *encoded_one_exp = NULL;
float *encoded_zero_exp = NULL;
int sample_wave_size = 0;

int msed_backend_std_init() {
	// Pre-calculate a 1 and a 0, as well as measuring how many samples it takes to write each one
	if (modulation_mode == FM_MODE) {
		// Generation variables
		int transients = 0;
		float time = 0;
		
		// Allocate the samples buffer for a 1
		encoded_one_exp = (float *)malloc(SAMPLE_RATE * sizeof(float));
		assert(encoded_one_exp != NULL);

		// Fill the buffer with an FM cycle representing a 1
		float last = WAVIFY(time, ONE, AMPLITUDE);
		while (transients < 3) {
			// Count the amount of times we cross 0
			if (last <= 0 && WAVIFY(time, ONE, AMPLITUDE) >= 0)
				transients++;

			last = WAVIFY(time, ONE, AMPLITUDE);

			encoded_one_exp[encoded_one_sc++] = WAVIFY(time, ONE, AMPLITUDE);

			time += TIME_INC;
		}
		
		// Increment once more, index -> size
		encoded_one_sc++;

		// Reset variables used for generation
		time = 0;
		transients = 0;

		// Allocate the samples buffer for a 0
		encoded_zero_exp = (float *)malloc(SAMPLE_RATE * sizeof(float));
		assert(encoded_zero_exp != NULL);

		// Fill the buffer with an FM cycle representing a 0
		last = WAVIFY(time, ZERO, AMPLITUDE);
		while (transients < 3) {
			// Count the amount of times we cross 0
			if (last <= 0 && WAVIFY(time, ZERO, AMPLITUDE) >= 0)
				transients++;

			last = WAVIFY(time, ZERO, AMPLITUDE);

			encoded_zero_exp[encoded_zero_sc++] = WAVIFY(time, ZERO, AMPLITUDE);

			time += TIME_INC;
		}

		// Increment once more, index -> size
		encoded_zero_sc++;

		// Get a sample of the waveform of a one or a zero
		// Get minimum of either sample count
		sample_wave_size = (encoded_zero_sc < encoded_one_sc) ? encoded_zero_sc : encoded_one_sc;

		// Save on memory
		encoded_one_exp = realloc(encoded_one_exp, encoded_one_sc * sizeof(float));
		assert(encoded_one_exp != NULL);
		encoded_zero_exp = realloc(encoded_zero_exp, encoded_zero_sc * sizeof(float));
		assert(encoded_zero_exp != NULL);

		// Asserts
		assert(sample_wave_size != 0);
		assert(encoded_one_sc != 0);
		assert(encoded_zero_sc != 0);
	} else if (modulation_mode == AM_MODE) {
		// Generation variable
		float time = 0;

		// Alocate samples buffer for an encoded one
		encoded_one_exp = (float *)malloc(SAMPLE_RATE * sizeof(float));
		assert(encoded_one_exp != NULL);

		// Fill the buffer with an AM cycle representing a 1
		for (int i = 0; i < SAMPLE_RATE; i++) {
			encoded_one_exp[i] = WAVIFY(time, ONE, AMPLITUDE);
			time += TIME_INC;
			encoded_one_sc++;

			if (i >= STD_SAMPLE_COUNT && (encoded_one_exp[i - 1] <= 0 && encoded_one_exp[i] >= 0)) {
				encoded_one_exp[i] = 0;
				break;
			}
		}

		encoded_one_sc++;

		// Reset generation variable
		time = 0;

		// Allocate samples buffer for an encoded zero
		encoded_zero_exp = (float *)malloc(SAMPLE_RATE * sizeof(float));
		assert(encoded_zero_exp != NULL);
		
		// Fill the buffer with an AM cycle representing a 0
		for (int i = 0; i < SAMPLE_RATE; i++) {
			encoded_zero_exp[i] = WAVIFY(time, ONE, AMPLITUDE / 2);
			time += TIME_INC;
			encoded_zero_sc++;

			if (i >= STD_SAMPLE_COUNT && (encoded_zero_exp[i - 1] <= 0 && encoded_zero_exp[i] >= 0))
				break;
		}

		encoded_zero_sc++;

		sample_wave_size = (encoded_zero_sc < encoded_one_sc) ? encoded_zero_sc : encoded_one_sc;

		// Save on memory
		encoded_one_exp = realloc(encoded_one_exp, encoded_one_sc * sizeof(float));
		assert(encoded_one_exp != NULL);
		encoded_zero_exp = realloc(encoded_zero_exp, encoded_zero_sc * sizeof(float));
		assert(encoded_zero_exp != NULL);

		// Asserts
		assert(sample_wave_size != 0);
		assert(encoded_one_sc != 0);
		assert(encoded_zero_sc != 0);
	}

	return SAMPLE_RATE;
}

void msed_backend_std_encode() {
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
	// Memory manage
	free(samples);

	// FM Encoder / AM Encoder
	if (modulation_mode == FM_MODE || modulation_mode == AM_MODE) {
		// Read every byte
		for (int i = 0 ; i < file_size; i++) {
			// In little endian order, write bits
			for (int j = 0; j < 8; j++) {
				// Get current bit
				uint8_t value = (data[i] >> j) & 1;

				// Write the pre-calculated 1 or 0 sample buffer cycles_per_bit amount of times
				for (int c = 0; c < cycles_per_bit; c++)
					tinywav_write_f(&tw, value ? encoded_one_exp : encoded_zero_exp, value ? encoded_one_sc : encoded_zero_sc);
			}
		}

		return;
	}
	
	// PM Encoder
	// Allocate sample buffer
	samples = (float *)malloc(cycles_per_bit * sizeof(float));

	// Read every byte
	for (int i = 0 ; i < file_size; i++) {
		// In little endian order, write bits
		for (int j = 0; j < 8; j++) {
			// Get current bit
			uint8_t value = (data[i] >> j) & 1;

			// Fill sample buffer
			for (int si = 0; si < cycles_per_bit; si++)
				samples[si] = WAVIFY(1, (value ? ONE : ZERO), AMPLITUDE);
			
			// Write sample buffer
			tinywav_write_f(&tw, samples, cycles_per_bit);
		}
	}

	// Memory manage
	free(samples);
}

size_t msed_backend_std_decode(uint8_t **buffer) {
	// Initialize the data buffer
	*buffer = malloc(1);
	size_t buffer_size = 0;

	// Read in 1 second header
	float *samples = (float *)malloc(LEADER * sizeof(float));
	tinywav_read_f(&tw, samples, LEADER);
	// Memory manage
	free(samples);

	// FM Decoder / AM Decoder
	if (modulation_mode == FM_MODE || modulation_mode == AM_MODE) {
		// Initialize the sample buffer
		samples = (float *)malloc(sample_wave_size * sizeof(float));
		int bit_ptr = 0;
		
		// While we can read samples
		while (tinywav_read_f(&tw, samples, sample_wave_size) != 0) {
			// Compare the current sample  buffer to pre-calculated 1 and 0 buffers
			// and tally the matches between the two (including tolerance)
			int one_matches = 0;
			int zero_matches = 0;

			for (int i = 0; i < sample_wave_size; i++) {
				float min = samples[i] - tolerance;
				float max = samples[i] + tolerance;

				if (encoded_one_exp[i] <= max && encoded_one_exp[i] >= min)
					one_matches++;
				else
					zero_matches++;
			}

			uint8_t value = 0;
			int sc = encoded_zero_sc;

			if (one_matches > zero_matches) {
				// It is probably a one
				sc = encoded_one_sc;
				value = 1;
			}
			
			// Read in the rest of the cycle (if we haven't already done so)
			sc -= sample_wave_size;

			if (sc > 0) {
				for (; sc > sample_wave_size; sc -= sample_wave_size)
					tinywav_read_f(&tw, samples, sample_wave_size);

				tinywav_read_f(&tw, samples, sc);
			}
			
			// Read in the rest of the cycles for this bit (if we have't already done so)
			sc = (value ? encoded_one_sc : encoded_zero_sc);

			float *scratch = (float *)malloc(sc * sizeof(float));
			for (int c = 1; c < cycles_per_bit; c++)
				tinywav_read_f(&tw, scratch, sc);

			// Memory manage
			free(scratch);

			// Write the bit to the current byte of the buffer
			buffer[0][buffer_size] |= (value << bit_ptr++);

			// Have we filled up the byte?
			if (bit_ptr == 8) {
				// Yes, reset the bit pointer and allocate one more byte to the buffer
				bit_ptr = 0;
				*buffer = realloc(*buffer, ++buffer_size + 1);
			}
		}

		// Memory manage
		free(samples);

		return buffer_size + 1;
	}

	// PM Decoder
	// Initialize sample buffer
	samples = (float *)malloc(cycles_per_bit * sizeof(float));
	uint8_t bit_ptr = 0;
	
	// While we can read samples (in this case the number of cycles per bit corresponds nicely
	// to the number of samples we need to read per bit)
	while (tinywav_read_f(&tw, samples, cycles_per_bit) != 0) {
		// Compare the sample to either a 1 or a zero, and place it in the current byte of the buffer
		buffer[0][buffer_size] |= (((float)WAVIFY(1, ONE, AMPLITUDE) == (float)samples[0]) << bit_ptr++);

		// Have we filled up a byte?
		if (bit_ptr == 8) {
			// Yes, reset the bit pointer and allocate one more byte to the buffer
			bit_ptr = 0;
			*buffer = realloc(*buffer, ++buffer_size + 1);
		}
	}

	// Memory manage
	free(samples);

	return buffer_size + 1;
}
