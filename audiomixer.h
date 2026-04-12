#pragma once
#include "portaudio.h"
#include <alsa/asoundlib.h>
#include <iostream>

namespace DesktopAM {
	class AudioMixer
	{
	public:
		AudioMixer();
		void init();
		static void term() {
			Pa_Terminate();
		}
	private:
		struct AudioChannel {
			std::string name;        // "game", "chat", "media"
			float volume;            // 0.0 to 1.0
			bool muted;
			PaStream* stream;
		};
	};
}