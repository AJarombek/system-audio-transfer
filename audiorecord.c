// Author: Andrew Jarombek
// Date: 4/17/2017
// Take an audio recording
// Sources: http://www.linuxdevcenter.com/pub/a/linux/2007/08/02/an-introduction-to-linux-audio.html

#include <uistd.h>
#include <string.h>

int fd_out;
int sample_rate = 48000;

int main(int argc, char **argv) 
{
	// The default device
	char *name_out = "/dev/dsp";

	// Give the open_audio_function the pathname and a flag
	// O_WRONLY says that the file will be opened for both writing and reading
	fd_out = open_audio_device(name_out, O_WRONLY);

	while(1)
		write_sinewave();

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
}