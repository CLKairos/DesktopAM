#include "audiomixer.h"

DesktopAM::AudioMixer::AudioMixer() {
	// Constructor implementation (if needed)
}

void DesktopAM::AudioMixer::init() {
	PaError err = Pa_Initialize();
	if (err != paNoError) {
		std::cout << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
	}
}