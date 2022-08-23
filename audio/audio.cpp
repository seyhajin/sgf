#include "audio.h"

#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

namespace sgf {

namespace {
constexpr float zsc = -1;
}

AudioDevice* AudioDevice::g_audioDevice;

Sound::~Sound() {
	// Doesn't exist in emscripten?!? !!!!
	alDeleteBuffers(1, &m_alBuffer);
}

Sound* loadSound(CString path) {

	drwav wav;
	assert(drwav_init_file(&wav, path.c_str(), nullptr));

	size_t size = wav.totalPCMFrameCount * wav.channels * sizeof(drwav_int16);

	drwav_int16* data = (drwav_int16*)malloc(size);

	drwav_read_pcm_frames_s16(&wav, wav.totalPCMFrameCount, data);

	drwav_uninit(&wav);

	ALuint alBuffer;

	alGenBuffers(1, &alBuffer);
	auto format = (wav.channels == 2) ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;

	alBufferData(alBuffer, format, data, size, wav.sampleRate);
	free(data);

	return new Sound(alBuffer);
}

const AudioDevice::Channel* AudioDevice::allocChannel() {

	for (auto& channel : m_channels) {
		ALint state;
		alGetSourcei(channel.alSource, AL_SOURCE_STATE, &state);
		if (state == AL_PLAYING || state == AL_PAUSED) continue;
		channel.id += maxChannels;
		return &channel;
	}
	return nullptr;
}

const AudioDevice::Channel* AudioDevice::validChannel(int channelId) const {

	Channel* channel = &m_channels[channelId & (maxChannels - 1)];
	return channel->id == channelId ? channel : nullptr;
}

int AudioDevice::playSound(Sound* sound, float volume) {

	auto channel = allocChannel();
	if (!channel) return 0;

	alSourcei(channel->alSource, AL_BUFFER, sound->alBuffer());
	// alSourcei(channel->alSource, AL_SOURCE_RELATIVE, AL_TRUE);
	// alSourcef(channel->alSource, AL_MAX_DISTANCE, 1000.0f);
	alSourcef(channel->alSource, AL_ROLLOFF_FACTOR, .25f);
	alSourcef(channel->alSource, AL_GAIN, volume);

	alSourcePlay(channel->alSource);

	return channel->id;
}

bool AudioDevice::channelPlaying(int channelId) const {
	return validChannel(channelId) != nullptr;
}

void AudioDevice::setChannelVolume(int channelId, float volume) {

	if (auto channel = validChannel(channelId)) alSourcef(channel->alSource, AL_GAIN, volume);
}

void AudioDevice::setChannelPosition(int channelId, CVec3f pos) {

	if (auto channel = validChannel(channelId)) alSource3f(channel->alSource, AL_POSITION, pos.x, pos.y, pos.z * zsc);
}

void AudioDevice::setChannelDirection(int channelId, CVec3f dir) {

	if (auto channel = validChannel(channelId)) alSource3f(channel->alSource, AL_DIRECTION, dir.x, dir.y, dir.z * zsc);
}

void AudioDevice::setChannelVelocity(int channelId, CVec3f vel) {

#ifndef __EMSCRIPTEN__
	if (auto channel = validChannel(channelId)) alSource3f(channel->alSource, AL_VELOCITY, vel.x, vel.y, vel.z * zsc);
#endif
}

void AudioDevice::setListenerPosition(CVec3f pos) {
	alListener3f(AL_POSITION, pos.x, pos.y, pos.z * zsc);
}

void AudioDevice::setListenerOrientation(CMat3f rot) {

	Mat3f m = Mat3f::scale({1, 1, zsc}) * rot;

	float vals[] = {m.k.x, m.k.y, m.k.z, m.j.x, m.j.y, m.j.z};

	alListenerfv(AL_ORIENTATION, vals);
}

void AudioDevice::setListenerVelocity(CVec3f vel) {
	alListener3f(AL_VELOCITY, vel.x, vel.y, vel.z * zsc);
}

AudioDevice::AudioDevice() {
	assert(!g_audioDevice);
	g_audioDevice = this;

	m_alDevice = alcOpenDevice(nullptr);
	assert(m_alDevice != nullptr);

	m_alContext = alcCreateContext(m_alDevice, nullptr);
	assert(m_alContext != nullptr);

	assert(alcMakeContextCurrent(m_alContext));

	ALuint sources[maxChannels];
	alGenSources(maxChannels, sources);

	for (int i = 0; i < maxChannels; ++i) { m_channels[i] = {i | maxChannels, sources[i]}; }
}

void createAudioDevice() {
	new AudioDevice();
}

} // namespace sgf
