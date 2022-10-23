#include "texturestreamer.h"

#include <fetch/fetch.hh>
#include <imgui/imgui.hh>
#include <stb/stb.hh>

#include <thread>

#if USE_TURBOJPEG
#include "turbojpeg/turbojpeg.h"
#endif

namespace sgf {

#if XDEBUG
#define CDEBUG debug
#else
#define CDEBUG                                                                                                         \
	if (false) debug
#endif

TextureStreamer::TextureStreamer(float tileSize, float tileResolution, uint atlasSize, Type type, bool useArrayTexture)
	: m_tileSize(tileSize), m_tileResolution(tileResolution), m_atlasSize(int(atlasSize)), m_type(type),
	  m_useArrayTexture(useArrayTexture) {

	if (!(m_atlasSize & 1)) panic("Atlas size mustc be even");

	auto format = (m_type == Type::aerial) ? TextureFormat::srgb24 : TextureFormat::r32f;

	m_tileTextureSize = int(m_tileSize / m_tileResolution);

	if (m_useArrayTexture) {

		m_textureSize = m_tileTextureSize;
		m_textureLayers = m_atlasSize * m_atlasSize;

		TextureFlags flags = TextureFlags::mipmap | TextureFlags::clampST;

		m_textureResource = m_arrayTexture =
			graphicsDevice()->createArrayTexture(m_textureSize, m_textureSize, m_textureLayers, format, flags, nullptr);

	} else {

		m_textureSize = m_tileTextureSize * m_atlasSize;
		m_textureLayers = 0;

		TextureFlags flags = TextureFlags::mipmap;

		m_textureResource = m_texture =
			graphicsDevice()->createTexture(m_textureSize, m_textureSize, format, flags, nullptr);
	}

	// Initialize streamer
	uint d = bytesPerPixel(format);
	m_mipLevels = 1;
	for (uint sz = m_tileTextureSize >> 1; sz; sz >>= 1) m_mipmaps[m_mipLevels++] = (uint8_t*)malloc(sz * sz * d);

	// Clear textures;

	auto p = (uint8_t*)malloc(m_tileTextureSize * m_tileTextureSize * 3);
}

TextureStreamer::Tile& TextureStreamer::tileAt(int x, int y) {
	y = m_atlasSize - floorMod(y, m_atlasSize) - 1;
	x = floorMod(x, m_atlasSize);
	return m_tiles[y * m_atlasSize + x];
}

void TextureStreamer::beginStreaming(String serverUrl, CVec2i serverOrigin, CVec3f eyePos) {
	assert(m_state == State::init);
	m_state = State::ready;

	m_serverUrl = std::move(serverUrl);
	m_serverOrigin = serverOrigin;

	m_tiles.clear();
	m_tiles.resize(m_atlasSize * m_atlasSize);

	for (int j = 0; j < m_atlasSize; ++j) {
		for (int i = 0; i < m_atlasSize; ++i) { tileError(Vec2i(i, j)); }
	}

	updateEye(eyePos);
}

void TextureStreamer::endStreaming() {
	m_ending = true;
}

bool TextureStreamer::ended() {
	if (m_ending && m_state != State::loading) m_state = State::ended;
	return m_state == State::ended;
}

void TextureStreamer::updateEye(CVec3f eyePos) {
	if (m_state != State::ready) return;

	m_eyePos = Vec2f(eyePos.x, eyePos.z);
	//	m_eyeTile = Vec2i(std::round(m_eyePos.x / m_tileSize), std::round(m_eyePos.y / m_tileSize));
	m_eyeTile = Vec2i(int(std::floor(m_eyePos.x / m_tileSize)), int(std::floor(m_eyePos.y / m_tileSize)));

	// if (m_state == State::ready)
	loadNextTile();
}

void TextureStreamer::loadNextTile() {
	assert(m_state == State::ready);

	auto updateTile = [this](int x, int y) -> bool {
		Vec2i wc = {x + m_eyeTile.x, y + m_eyeTile.y};

		auto& tile = tileAt(wc.x, wc.y);

		if (tile.coords == wc) return false;

		tile.coords = wc;

		loadTile(tile);

		return true;
	};

	// Eye tile first!
	if (updateTile(0, 0)) return;

	int hw = m_atlasSize / 2;

	for (int r = 1; r <= hw; ++r) {
		for (int i = -r; i < r; ++i) {
			if (updateTile(i, -r)) return;
			if (updateTile(r, i)) return;
			if (updateTile(-i, r)) return;
			if (updateTile(-r, -i)) return;
		}
	}
}

void TextureStreamer::tileError(CVec2i coords) {
	switch (m_type) {
	case Type::aerial:
		m_mipmaps[0] = static_cast<uint8_t*>(std::malloc(m_tileTextureSize * m_tileTextureSize * 3));
		std::memset(m_mipmaps[0], 0xcc, m_tileTextureSize * m_tileTextureSize * 3);
		generateAerialMipmaps();
		break;
	case Type::topo:
		m_mipmaps[0] = static_cast<uint8_t*>(std::malloc(m_tileTextureSize * m_tileTextureSize * 4));
		std::memset(m_mipmaps[0], 0, m_tileTextureSize * m_tileTextureSize * 4);
		generateTopoMipmaps();
		break;
	}
	if (m_useArrayTexture) {
		updateArrayTexture(coords);
	} else {
		updateAtlasTexture(coords);
	}
	free(m_mipmaps[0]);
}

void TextureStreamer::loadTile(Tile& tile) {

	auto url = m_serverUrl + toString(tile.coords.x + m_serverOrigin.x) + "/" +
			   toString(tile.coords.y + m_serverOrigin.y) + (m_type == Type::aerial ? ".jpg" : ".raw");

	tile.state = Tile::State::fetching;
	m_state = State::loading;

	CDEBUG() << "### Loading" << url;

	fetch(url) | [this, &tile, url](CFetchResponse r) mutable {
		if (m_ending) {
			m_state = State::ended;
			return;
		}
		if (r.httpStatus != 200) {
			debug() << "!!! Failed to fetch" << url;
			tile.state = Tile::State::missing;
			tileError(tile.coords);
			m_state = State::ready;
			loadNextTile();
			return;
		}

		Promise<bool> promise;

		std::thread([this, &tile, data = r.text, promise]() mutable {
			//
			// Very careful! we're not on mainThread any more in here!
			//
			switch (m_type) {
			case Type::aerial:
				tile.state = Tile::State::decompressing;
				CDEBUG() << "### Decompressing aerial";
				if (!decompressAerialData(data) || m_ending) break;
				tile.state = Tile::State::mipmapping;
				CDEBUG() << "### Mipmapping aerial";
				generateAerialMipmaps();
				break;
			case Type::topo:
				tile.state = Tile::State::decompressing;
				CDEBUG() << "### Decompressing topo";
				if (!decompressTopoData(data) || m_ending) break;
				tile.state = Tile::State::mipmapping;
				CDEBUG() << "### Mipmapping topo";
				generateTopoMipmaps();
				break;
			}
			promise.resolveAsync(m_mipmaps[0] != nullptr);
		}).detach();

		promise | [this, &tile](bool okay) {
			if (m_ending) {
				m_state = State::ended;
				return;
			}
			if (m_mipmaps[0]) {
				tile.state = Tile::State::updating;
				CDEBUG() << "### Updating" << (m_type == Type::aerial ? "aerial" : "topo");
				if (m_useArrayTexture) {
					updateArrayTexture(tile.coords);
				} else {
					updateAtlasTexture(tile.coords);
				}
				free(m_mipmaps[0]);
				m_mipmaps[0] = nullptr;
				tile.state = Tile::State::loaded;
			} else {
				tile.state = Tile::State::corrupt;
				tileError(tile.coords);
			}
			m_state = State::ready;
		};
	};
}

bool TextureStreamer::decompressAerialData(CString jpegFile) {

	auto jpegData = (const uint8_t*)jpegFile.c_str();
	auto jpegSize = jpegFile.size();

#if USE_TURBOJPEG

	static tjhandle tjInstance = tjInitDecompress();
	if (!tjInstance) panic("tjInitDecompress FAILED");

	int pixelFormat = TJPF_RGB;
	int flags = 0;

	int width, height, subsamp, colorspace;
	if (tjDecompressHeader3(tjInstance, jpegData, jpegSize, &width, &height, &subsamp, &colorspace) < 0) {
		debug() << "!!! Turbojpeg jtDecompressHeader3 failed");
		return nullptr;
	}
	if (width != m_tileTextureSize || height != m_tileTextureSize) {
		debug() << "!!! Invalid aerial texture jpeg size";
		return nullptr;
	}

	uint8_t* dst = static_cast<uint8_t*>(malloc(width * height * tjPixelSize[pixelFormat]));

	if (tjDecompress2(tjInstance, jpegData, jpegSize, imgData, width, 0, height, pixelFormat, flags) < 0) {
		debug() << "!!! Turbojpeg tjDecompress2 failed");
		free(dst);
		return nullptr;
	}

	return dst;

#else

	int width, height, chans;
	uint8_t* dst = stbi_load_from_memory(jpegData, int(jpegSize), &width, &height, &chans, 0);

	if (!dst) {
		debug() << "!!! STB stbi_load_from_memory failed";
		return false;
	}

	if (width != m_tileTextureSize || height != m_tileTextureSize || chans != 3) {
		debug() << "!!! Invalid aerial texture jpeg size";
		free(dst);
		return false;
	}
	m_mipmaps[0] = dst;
	return true;
#endif
}

bool TextureStreamer::decompressTopoData(CString topoFile) {

	if (topoFile.size() != (m_tileTextureSize + 1) * (m_tileTextureSize + 1) * 4) {
		debug() << "!!! Raw tile data invalid length" << topoFile.size();
		return false;
	}

	auto src = (const int16_t*)topoFile.c_str();
	auto dst = (float*)malloc(m_tileTextureSize * m_tileTextureSize * sizeof(float));

	constexpr int seaLevel = 186 * 16;
	constexpr int topoLevel = 0x4000;

	for (int y = 0; y < m_tileTextureSize; ++y) {
		auto srcp = src + y * (m_tileTextureSize + 1) * 2;
		auto dstp = dst + y * m_tileTextureSize;
		for (int x = 0; x < m_tileTextureSize; ++x) {

			int h = *srcp++;
			if (h) h -= seaLevel;

			int d = *srcp++ & 0x7fff;
			d = (d == 0x7fff) ? 0 : d - 0x1000;

			*dstp++ = float(h + d);
		}
	}
	m_mipmaps[0] = (uint8_t*)dst;
	return true;
}

void TextureStreamer::generateAerialMipmaps() {

	for (uint mip = 0; mip < m_mipLevels - 1; ++mip) {

		const uint8_t* srcp = m_mipmaps[mip];
		uint8_t* dstp = m_mipmaps[mip + 1];

		uint sz = m_tileTextureSize >> (mip + 1);
		uint sw = sz * 6;

		for (uint y = 0; y < sz; ++y) {
			for (uint x = 0; x < sz; ++x) {
				uint r = srcp[0] + srcp[3] + srcp[sw] + srcp[sw + 3];
				uint g = srcp[1] + srcp[4] + srcp[sw + 1] + srcp[sw + 4];
				uint b = srcp[2] + srcp[5] + srcp[sw + 2] + srcp[sw + 5];
				dstp[0] = r >> 2;
				dstp[1] = g >> 2;
				dstp[2] = b >> 2;
				srcp += 6;
				dstp += 3;
			}
			srcp += sw;
		}
	}
}

void TextureStreamer::generateTopoMipmaps() {

	for (uint mip = 0; mip < m_mipLevels - 1; ++mip) {

		auto srcp = (const float*)m_mipmaps[mip];
		auto dstp = (float*)m_mipmaps[mip + 1];

		uint sz = m_tileTextureSize >> (mip + 1);
		uint sw = sz * 2;

		for (uint y = 0; y < sz; ++y) {
			for (uint x = 0; x < sz; ++x) {
				float i = srcp[0] + srcp[1] + srcp[sw] + srcp[sw + 1];
				*dstp++ = i * .25f;
				srcp += 2;
			}
			srcp += sw;
		}
	}
}

void TextureStreamer::updateArrayTexture(CVec2i tileCoords) {

	int ix = floorMod(tileCoords.x, m_atlasSize);
	int iy = floorMod(tileCoords.y, m_atlasSize);
	iy = m_atlasSize - iy - 1;
	int iz = iy * m_atlasSize + ix;

	int sz = m_tileTextureSize;
	for (uint mip = 0; mip < m_mipLevels; ++mip) {
		m_arrayTexture->updateData(mip, 0, 0, iz, sz, sz, 1, m_mipmaps[mip]);
		sz >>= 1;
	}
}

void TextureStreamer::updateAtlasTexture(CVec2i tileCoords) {

	int sz = m_tileTextureSize;
	int ix = floorMod(tileCoords.x, m_atlasSize);
	int iy = floorMod(tileCoords.y, m_atlasSize);
	iy = (m_atlasSize - iy - 1) * sz;
	ix = ix * sz;

	for (uint mip = 0; mip < m_mipLevels; ++mip) {
		m_texture->updateData(mip, ix, iy, sz, sz, m_mipmaps[mip]);
		ix >>= 1;
		iy >>= 1;
		sz >>= 1;
	}
}

void TextureStreamer::emitDebug() {
	ImGui::Begin(m_type == Type::aerial ? "Aerial megatexture" : "Topo megatexture");

	ImGuiEx::Debug() << "EyeTile" << m_eyeTile;
	ImGuiEx::Debug() << "EyePos" << m_eyePos;

	auto sz = ImVec2(32, 32);

	int hw = m_atlasSize / 2;

	float vis = float(hw - 1) * m_tileSize;

	for (int j = hw; j >= -hw; --j) {

		bool sameLine = false;

		for (int i = -hw; i <= hw; ++i) {

			char chr = '?';
			ImVec4 color(.25f, .25f, .25f, 1.0f);

			int x = m_eyeTile.x + i;
			int y = m_eyeTile.y + j;
			auto& tile = tileAt(x, y);

			switch (tile.state) {
			case Tile::State::init:
				chr = '-';
				break;
			case Tile::State::fetching:
				chr = 'F';
				break;
			case Tile::State::decompressing:
				chr = 'D';
				break;
			case Tile::State::mipmapping:
				chr = 'M';
				break;
			case Tile::State::updating:
				chr = 'U';
				break;
			case Tile::State::loaded:
				if (x == m_eyeTile.x && y == m_eyeTile.y) {
					chr = '@';
				} else {
					chr = ' ';
				}
				break;
			case Tile::State::missing:
				chr = 'X';
				break;
			case Tile::State::corrupt:
				chr = '*';
				break;
			}

			int ex = (i > 0) ? x : (x + 1);
			float xd = std::abs(float(ex) * m_tileSize - m_eyePos.x);
			int ey = (j > 0) ? y : (y + 1);
			float yd = std::abs(float(ey) * m_tileSize - m_eyePos.y);
			float d = std::max(xd, yd);

			float c = .2f;
			if (std::abs(d) < vis) c = .5f;

			if (tile.coords == Vec2i(x, y)) {
				color = {0, c, 0, 1};
			} else {
				color = {c, 0, 0, 1};
			}

			char label[] = {chr, 0};

			if (sameLine) ImGui::SameLine();
			sameLine = true;

			ImGui::PushStyleColor(ImGuiCol_Button, color);

			ImGui::Button(label, {16, 16});

			ImGui::PopStyleColor(1);
		}
	}
	ImGui::End();
}

} // namespace sgf
