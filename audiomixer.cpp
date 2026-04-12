#include "audiomixer.h"
#include <alsa/asoundlib.h>

static void alsaSilencer(const char*, int, const char*, int, const char*, ...) {}

DesktopAM::AudioMixer::AudioMixer() {
	// Constructor implementation (if needed)
}

void DesktopAM::AudioMixer::init() {
	snd_lib_error_set_handler(alsaSilencer);

	PaError err = Pa_Initialize();
	if (err != paNoError) {
		std::cout << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
	}
}