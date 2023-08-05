# Modulated Signal Encoder / Decoder (MSED)

Simple modulated signal encoder / decoder command line program with swappable backends.

## Command Line Usage

```bash
$ msed a b [options]
```
Files: <br>
```
    encoder.out a b [options]
    Files:
	a - File whose data to encode / decode
	    If file 'a' is a .bin then the program is encoding to 'b', if it is a .wav 
	    then it is decoding to 'b'.
	b - File to encode / decode data to
    Options:
	-mode <name> - The name of the backend to use (hint: execute command msed 
						       backends)
	-pm - Change to pulse modulated mode (default)
	-fm - Change to frequency modulated mode
	-am - Change to amplitude modulated mode
	-cpb <integer> - Specify the number of cycles per bit of data (default: 1)
	-tolerance <integer> - Plus or minus <integer> sample value tolerance (default: 0)
```

## Building from source

Ensure that you have <a href="https://archlinux.org/packages/core/x86_64/gcc/">`gcc`</a> and <a href="https://archlinux.org/packages/core/x86_64/make/">`make`</a>.

Execute the following command:
```bash
make
```
This will create a file called `msed` which is the program.


## Creating a new backend

There are three functions which a backend must implement:

* The initializer function
* The encoder function
* The decoder function

### Initializtion

The main program expects that the initialization function returns the sample rate the backend would like for audio files. Besides that, the initialization function is helpful for pre-computing waveforms and times.

### Encoding

The encoding function doesn't return any value, it simply writes its data to the opened output `.wav` file. The wave file can be written to using the Tinywav library. The Tinywav context variable, tw, is a global variable that is accessible when "main.h" is included.

### Decoding

The decode function is expected to write to a given `uint8_t **` buffer and to return the size of the buffer.

See `src/include/backends/standard.h` and `src/backends/standard.c` for an implementation above specification.

## Contributing

Please ensure that your code contains comments not too frequently, don't describe every line, and not too infrequently, don't describe only every function.

Please also aim to create simple code, not a big bowl of spaghetti.
