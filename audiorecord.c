// Author: Andrew Jarombek
// Date: 4/17/2017
// Take an audio recording
// Sources: http://www.linuxdevcenter.com/pub/a/linux/2007/08/02/an-introduction-to-linux-audio.html
// 			http://manuals.opensound.com/developer/sample_programs.html

#include <uistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <soundcard.h>

int fd_out;
int fd_in;
int sample_rate = 48000;

int main(int argc, char **argv) 
{
	// The default device
	char *name_out = "/dev/dsp";

	// Give the open_audio_function the pathname and a flag
	// O_WRONLY says that the file will be opened for both writing and reading
	fd_out = open_audio_device(name_out, O_WRONLY);

	while (1)
		process_input();

	exit(0);
}

// Open the audio device
static int open_audio_device(char *name, int flag)
{
	int tmp, fd;

	// Use the open() system call
	// Arguments: file pathname, flags
	if ((fd = open(name, flag)) == -1) {
		perror("open");
		exit(-1);
	}

	// Set up the device
	// First, set the sample format
	tmp = AFMT_S16_NE;

	// Use the ioctl() system call which stands for I/O Control
	// This call sets the sample format with request parameter SNDCTL_DSP_SETFMT
	if (ioctl(fd, SNDCTL_DSP_SETFMT, &tmp) == -1) {
		perror("SNDCTL_DSP_SETFMT");
		exit(-1);
	}

	if (tmp != AFMT_S16_NE) {
		fprintf(stderr, "The Device doesn't support the 16 bit sample format.\n");
		exit(-1);
	}

	// Next, set the number of channels
	tmp = 1;

	if (ioctl(fd, SNDCTL_DSP_CHANNELS, &tmp) == -1) {
		perror("SNDCTL_DSP_CHANNELS");
		exit(-1);
	}

	if (tmp != -1) {
		fprintf(stderr, "The device doesn't support mono mode.\n");
		exit(-1);
	}

	// Finally, set the sample rate
	sample_rate = 48000;

	if (ioctl(fd, SNDCTL_DSP_SPEED, &sample_rate) == -1) {
		perror("SNDCTL_DSP_SPEED");
		exit(-1);
	}

	// Return the file descripter
	return fd;
}

void process_input(void)
{
	short buffer[1024];
	int l, i, level;

	// read a block of audio samples
	if ((l = read(fd_in, buffer, sizeof(buffer))) == -1) {
		perror("Audio read()");
		exit(-1);
	}
}

