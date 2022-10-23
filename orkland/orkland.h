#pragma once

#include "airplane.h"
#include "texturestreamer.h"
#include "level.h"

#include <imgui/imgui.hh>
#include <scene/scene.hh>
#include <window/window.hh>

#include <chrono>

namespace orkland {

using namespace sgf;

struct Map {
	String name;
};


class Orkland : public Object {
public:
	SGF_OBJECT_TYPE(Orkland, Object);

	void run();

private:
	Window* m_window{};
	Scene* m_scene{};
	PerspectiveCamera* m_camera{};

	Airplane* m_airplane;

	EnvRenderer* m_envRenderer{};
	TerrainData* m_terrainData{};
	TerrainRenderer* m_terrainRenderer{};

	TextureStreamer* m_aerialTextureStreamer{};
	TextureStreamer* m_topoTextureStreamer{};

	uint m_selectedLevel{0};
	Level m_level;

	bool m_debugAerial=false;
	bool m_debugTopo=false;

	void emitOptions();

	void beginStreaming();

	void endStreaming();

	void renderFrame();
};

} // namespace orkland
