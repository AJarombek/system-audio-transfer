// Author: Andrew Jarombek
// Date: 4/17/2017
// Take an audio recording
// Sources: http://www.linuxdevcenter.com/pub/a/linux/2007/08/02/an-introduction-to-linux-audio.html
// 			http://manuals.opensound.com/developer/sample_programs.html

#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <linux/soundcard.h>
#include <pthread.h>

int fd_audio;
int sample_rate = 48000;
int buffer_size;

int open_audio_device(char *name, int flag);
void process_input(void);
void * start_thread(void *buffer);

int main(int argc, char **argv) 
{
	// The default device
	char *name_out = "/dev/dsp";

	// Give the open_audio_function the pathname and a flag
	// O_WRONLY says that the file will be opened for both writing and reading
	fd_audio = open_audio_device(name_out, O_WRONLY);

	// Infinite audio input processing
	while (1)
		process_input();

	exit(0);
}

// Open the audio device
int open_audio_device(char *name, int flag)
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

	if (tmp != 1) {
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

// The start_thread function must have the signiture void * st(void *args)
// Play back the audio
void * start_thread(void *buffer)
{
	// Convert back to a short pointer
	short *buf = (short *) buffer;

	// Use the write() system call
	// write the audio bytes
	if (write(fd_audio, buf, buffer_size) != buffer_size) {
		perror("Audio write()");
		exit(-1);
	}

	pthread_exit(0);
}

// Process the audio input and start up a new thread to play it back
void process_input(void)
{
	// 64 KB Buffer
	short buffer[65536];
	int l;

	// A new thread
	pthread_t th;

	buffer_size = sizeof(buffer);

	// Use the read() system call
	// read a block of audio bytes
	if ((l = read(fd_audio, buffer, buffer_size)) == -1) {
		perror("Audio read()");
		exit(-1);
	}

	// Create a new thread and call the start_thread function
	pthread_create(&th, NULL, start_thread, (void *) buffer);
}

