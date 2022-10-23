#pragma once

#include <scene/scene.hh>

namespace sgf {

class TextureStreamer : public Object {
public:
	SGF_OBJECT_TYPE(TextureStreamer, Object)

	enum struct Type { aerial, topo };

	TextureStreamer(float tileSize, float tileResolution, uint atlasSize, Type type, bool useArrayTexture);
//	~TextureStreamer() override;

	float tileSize() const {
		return m_tileSize;
	}

	float tileResolution() const {
		return m_tileResolution;
	}

	uint atlasSize() const {
		return m_atlasSize;
	}

	Type type() const {
		return m_type;
	}

	TextureResource* textureResource() const {
		return m_textureResource.value();
	}

	void beginStreaming(String serverUrl, CVec2i serverOrigin, CVec3f eyePos);

	void updateEye(CVec3f eyePos);

	void endStreaming();

	bool ended();

	void emitDebug();

private:
	// ***** Texture *****

	float m_tileSize;
	float m_tileResolution;
	int m_atlasSize;
	Type m_type;
	bool m_useArrayTexture;

	int m_tileTextureSize;
	int m_textureSize;
	int m_textureLayers;

	SharedPtr<TextureResource> m_textureResource;
	ArrayTexture* m_arrayTexture;
	Texture* m_texture;

	// ***** Streamer *****
	enum struct State { init, ready, loading, ended };

	struct Tile {
		enum struct State { init, fetching, decompressing, mipmapping, updating, loaded, missing, corrupt };
		Vec2i coords{-0x7fffffff, 0x7fffffff};
		State state{State::init};
	};

	State m_state{State::init};
	String m_serverUrl;
	Vec2i m_serverOrigin;
	Vec2f m_eyePos;
	Vec2i m_eyeTile;
	Vector<Tile> m_tiles;
	uint8_t* m_mipmaps[16]{};
	uint m_mipLevels;
	bool m_ending{false};

	Tile& tileAt(int x, int y);

	int64_t bytesFetched{};
	double fetchTime{};

	void loadNextTile();
	void loadTile(Tile& tile);

	bool decompressAerialData(CString jpegData);
	bool decompressTopoData(CString jpegData);

	void generateAerialMipmaps();
	void generateTopoMipmaps();

	void updateArrayTexture(CVec2i tileCoords);
	void updateAtlasTexture(CVec2i tileCoords);

	void tileError(CVec2i tileCoords);
};

} // namespace sgf
