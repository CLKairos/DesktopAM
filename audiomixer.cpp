#include "audiomixer.h"

#define SAMPLE_RATE 44100
#define FRAMES_PER_BUFFER 256

namespace DesktopAM {
    AudioMixer::AudioChannel AudioMixer::game   = {"game",   1.0f, false};
    AudioMixer::AudioChannel AudioMixer::chat   = {"chat",   1.0f, false};
    AudioMixer::AudioChannel AudioMixer::media  = {"media",  1.0f, false};
    AudioMixer::AudioChannel AudioMixer::system = {"system", 1.0f, false};
}

static void alsaSilencer(const char*, int, const char*, int, const char*, ...) {}

PaStream* DesktopAM::AudioMixer::sharedStream = nullptr;

// Simple buffers (replace with ring buffer later)
static std::vector<float> gameBuf, chatBuf, mediaBuf, systemBuf;
static std::mutex gameMtx, chatMtx, mediaMtx, systemMtx;

static float popSample(std::vector<float>& buf, std::mutex& mtx) {
    std::lock_guard<std::mutex> lock(mtx);
    if (buf.empty()) return 0.f;

    float s = buf.front();
    buf.erase(buf.begin());
    return s;
}

int DesktopAM::AudioMixer::audioCallback(
    const void*, void* output,
    unsigned long frameCount,
    const PaStreamCallbackTimeInfo*,
    PaStreamCallbackFlags,
    void*)
{
    float* out = static_cast<float*>(output);

    for (unsigned long i = 0; i < frameCount; i++) {
        float mixed = 0.f;
        int active = 0;

        if (!game.muted) {
            mixed += popSample(gameBuf, gameMtx) * game.volume;
            active++;
        }
        if (!chat.muted) {
            mixed += popSample(chatBuf, chatMtx) * chat.volume;
            active++;
        }
        if (!media.muted) {
            mixed += popSample(mediaBuf, mediaMtx) * media.volume;
            active++;
        }
        if (!system.muted) {
            mixed += popSample(systemBuf, systemMtx) * system.volume;
            active++;
        }

        if (active > 0) mixed /= active;

        if (mixed > 1.f) mixed = 1.f;
        if (mixed < -1.f) mixed = -1.f;

        *out++ = mixed;
        *out++ = mixed;
    }

    return paContinue;
}

void DesktopAM::AudioMixer::init() {
    snd_lib_error_set_handler(alsaSilencer);

    // Store module IDs so we can unload properly
    std::system("pactl load-module module-null-sink sink_name=game_sink sink_properties=device.description=Game > /tmp/game_sink_id");
    std::system("pactl load-module module-null-sink sink_name=chat_sink sink_properties=device.description=Chat > /tmp/chat_sink_id");
    std::system("pactl load-module module-null-sink sink_name=media_sink sink_properties=device.description=Media > /tmp/media_sink_id");
    std::system("pactl load-module module-null-sink sink_name=system_sink sink_properties=device.description=System > /tmp/system_sink_id");

    Pa_Initialize();

    Pa_OpenDefaultStream(
        &sharedStream,
        0,
        2,
        paFloat32,
        SAMPLE_RATE,
        FRAMES_PER_BUFFER,
        audioCallback,
        nullptr
    );

    Pa_StartStream(sharedStream);
}

void DesktopAM::AudioMixer::term() {
    Pa_StopStream(sharedStream);
    Pa_CloseStream(sharedStream);
    Pa_Terminate();

    // unload each module safely
    std::system("pactl unload-module $(cat /tmp/game_sink_id)");
    std::system("pactl unload-module $(cat /tmp/chat_sink_id)");
    std::system("pactl unload-module $(cat /tmp/media_sink_id)");
    std::system("pactl unload-module $(cat /tmp/system_sink_id)");
}

DesktopAM::AudioMixer::AudioMixer() {}

void DesktopAM::AudioMixer::setVolume(const std::string& name, float value) {
    if      (name == game.name)   game.volume   = value;
    else if (name == chat.name)   chat.volume   = value;
    else if (name == media.name)  media.volume  = value;
    else if (name == system.name) system.volume = value;
    else std::cout << "Invalid audio name: " << name << std::endl;
}