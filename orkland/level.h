#pragma once

#include <geom/geom.hh>

namespace sgf {

struct Level {
	uint index;
	String name;

	String aerialServer;
	Recti aerialBounds;
	float aerialResolution{1};

	String topoServer;
	Recti topoBounds;
	float topoResolution{1};

	Recti bounds;
	Vec2i origin;
	uint atlasSize{0};
};

CVector<Level> levels();

void scrapeTiles(int levelIndex, int x, int y, int type, CString outDir);

}
