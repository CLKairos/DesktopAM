#pragma once
#include "portaudio.h"
#include <alsa/asoundlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include <cstdlib>

namespace DesktopAM {

	class AudioMixer {
	public:
		AudioMixer();
		void init();

		static void term();
		static void setVolume(const std::string& name, float value);

	private:
		struct AudioChannel {
			std::string name;
			float volume = 1.0f;
			bool muted   = false;
		};

		static PaStream* sharedStream;

		static int audioCallback(
			const void* input, void* output,
			unsigned long frameCount,
			const PaStreamCallbackTimeInfo* timeInfo,
			PaStreamCallbackFlags statusFlags,
			void* userData
		);

		static AudioChannel game;
		static AudioChannel chat;
		static AudioChannel media;
		static AudioChannel system;
	};

}