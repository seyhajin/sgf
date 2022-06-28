#pragma once

#include <core/core.hh>
#include <geom/geom.hh>

#include <AL/al.h>
#include <AL/alc.h>

namespace sgf {

class Sound : public Shared {
public:
	Sound(ALuint alBuffer) : m_alBuffer(alBuffer) {
	}
	~Sound();

	ALuint alBuffer() const {
		return m_alBuffer;
	}

private:
	ALuint m_alBuffer;
};

Sound* loadSound(CString path);

class AudioDevice {
public:
	static constexpr int maxChannels = 32;

	int cueSound(Sound* sound);
	int playSound(Sound* sound, float volume = 1.0f);

	bool channelPlaying(int channel) const;

	void setChannelPosition(int channel, CVec3f position);
	void setChannelDirection(int channel, CVec3f direction);
	void setChannelVelocity(int channel, CVec3f velocity);

	void setChannelVolume(int channel, float volume);
	void stopChannel(int channel);

	void setListenerPosition(CVec3f position);
	void setListenerOrientation(CMat3f orientation);
	void setListenerVelocity(CVec3f velocity);

private:
	ALCdevice* m_alDevice;
	ALCcontext* m_alContext;

	static AudioDevice* g_audioDevice;

	struct Channel {
		int id;
		ALuint alSource;
	};

	mutable Channel m_channels[maxChannels];

	const Channel* allocChannel();
	const Channel* validChannel(int channel) const;

	ALuint channelSound(int channel);

	friend void createAudioDevice();
	friend AudioDevice* audioDevice();

	AudioDevice();
};

void createAudioDevice();

inline AudioDevice* audioDevice() {
	assert(AudioDevice::g_audioDevice);
	return AudioDevice::g_audioDevice;
}

} // namespace sgf
