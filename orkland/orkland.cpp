#include "orkland.h"

namespace orkland {

using namespace sgf;

/**
 * https://slate.com/technology/2009/01/how-far-away-is-the-horizon.html
 * http://www.ringbell.co.uk/info/hdist.htm
 *
 * Hight point in NZ is Aoraki / Mount Cook @ 3724m (dist = 218km)
 *
 * Everest is 8849m (dist 336km)!
 *
 */
void Orkland::run() {

	m_window = createMainWindow("Orkland", 1280, 720);

	//	m_window->fullScreen = true;

	createGraphicsDevice(m_window);

	m_scene = new Scene(graphicsDevice());
	m_scene->clearColor = Vec4f(.5f, .75, 1.0f, 1);
	m_scene->directionalLightVector = Vec3f(0, -.5f, -1).normalized();
	m_scene->directionalLightColor = Vec3f(1);

	m_camera = new PerspectiveCamera();
	m_camera->fovY = 50.0f;
	m_camera->zNear = .1f;
	m_camera->zFar = 10000.0f;

	m_airplane = new Airplane();
	m_airplane->setPosition({0, 128, -256});
	m_camera->setParent(m_airplane);
	m_airplane->enable();

	m_terrainData = new TerrainData();
	m_terrainRenderer = new TerrainRenderer(m_terrainData);
	m_scene->addRenderer(m_terrainRenderer);

	m_envRenderer = new EnvRenderer();
	m_envRenderer->skyTexture = loadTexture("ClearBlueSky.jpg", TextureFormat::srgb24, TextureFlags::linear);
	m_envRenderer->fogRange = 10000.0f;
	m_scene->addRenderer(m_envRenderer);

	m_selectedLevel = 0;

	beginStreaming();

	ImGuiEx::CreateContext(mainWindow());

	mainWindow()->keyboard()->key(SGF_KEY_1).pressed.connect(this, [this] { m_debugAerial = !m_debugAerial; });
	mainWindow()->keyboard()->key(SGF_KEY_2).pressed.connect(this, [this] { m_debugTopo = !m_debugTopo; });

	mainWindow()->run([this] { renderFrame(); });
}

void Orkland::beginStreaming() {

	m_level = levels()[m_selectedLevel];

	float tileSize = 512;

	float zFar = tileSize * float(m_level.atlasSize / 2u - 1u);

	auto origin = m_level.origin;

	m_airplane->setMatrix(AffineMat4f({},Vec3f(0,100,0)));
	m_airplane->reset();
	m_airplane->update();

	m_camera->zNear = .1f;
	m_camera->zFar = zFar + 4.0f;

	m_envRenderer->fogRange = zFar;
	m_envRenderer->fogColor = Vec4f(.4f, .6f, .8f, 1);
	m_envRenderer->fogPower = 1.6f;

	//	m_terrainData->lodLevels = 6;
	//	m_terrainData->quadsPerTile = 64;
	m_terrainData->lodLevels = 6;
	m_terrainData->quadsPerTile = 64;
	m_terrainData->tileMapSize = m_level.atlasSize;

	debug() << "### Terrain size" << m_terrainData->terrainSize();

	debug() << "### Camera zNear, zFar" << m_camera->zNear << m_camera->zFar;

	m_aerialTextureStreamer =
		new TextureStreamer(tileSize, m_level.aerialResolution, m_level.atlasSize, TextureStreamer::Type::aerial, true);
	m_terrainData->aerialTexture = m_aerialTextureStreamer->textureResource()->cast<ArrayTexture>();
	m_terrainData->aerialTextureScale = Vec2f(1, -1) / tileSize;

	m_topoTextureStreamer =
		new TextureStreamer(tileSize, m_level.topoResolution, m_level.atlasSize, TextureStreamer::Type::topo, false);
	m_terrainData->topoTexture = m_topoTextureStreamer->textureResource()->cast<Texture>();
	m_terrainData->topoTextureScale = Vec2f(1, -1) / (tileSize * float(m_level.atlasSize));

	m_terrainRenderer->validate();

	m_aerialTextureStreamer->beginStreaming(m_level.aerialServer, origin, m_camera->worldPosition());
	m_topoTextureStreamer->beginStreaming(m_level.topoServer, origin, m_camera->worldPosition());
}

void Orkland::endStreaming() {
	m_aerialTextureStreamer->endStreaming();
	m_topoTextureStreamer->endStreaming();
}

void Orkland::emitOptions() {
	ImGui::SetNextWindowSize(ImVec2(160, 80));
	ImGui::Begin("Options");
	if (ImGui::BeginCombo("Area", m_level.name.c_str(), 0)) {
		for (uint i = 0; i < levels().size(); ++i) {
			const bool selected = (m_selectedLevel == i);
			if (ImGui::Selectable(levels()[i].name.c_str(), selected)) m_selectedLevel = i;
			if (selected) ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	ImGui::End();
}

void Orkland::renderFrame() {

	ImGuiEx::NewFrame();

	if (m_selectedLevel != m_level.index) {
		if (m_aerialTextureStreamer->ended() && m_topoTextureStreamer->ended()) {
			m_terrainData->aerialTexture = nullptr;
			m_terrainData->topoTexture = nullptr;
			delete m_aerialTextureStreamer;
			delete m_topoTextureStreamer;
			beginStreaming();
		}
	} else {
		emitOptions();
		if (m_selectedLevel != m_level.index) { endStreaming(); }
	}

	m_scene->update();

	if (m_aerialTextureStreamer) {
		m_aerialTextureStreamer->updateEye(m_camera->worldPosition());
		if (m_debugAerial) m_aerialTextureStreamer->emitDebug();
	}
	if (m_topoTextureStreamer) {
		m_topoTextureStreamer->updateEye(m_camera->worldPosition());
		if (m_debugTopo) m_topoTextureStreamer->emitDebug();
	}

	m_scene->render();

	ImGuiEx::Render();
}

} // namespace orkland

int main() {

	auto orkland = new orkland::Orkland();

	orkland->run();
}
