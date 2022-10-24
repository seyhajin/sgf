#include "level.h"

#include "RSJparser.h"
#include <fetch/fetch.hh>

namespace sgf {

namespace {

Vector<Level> g_levels;

void loadLevels() {

	String source = loadString(resolveAssetPath("levels.json"));

	RSJresource rsjFile(source);

	auto& rsjLevels = rsjFile["levels"].as_array();

	for (auto& rsjLevel : rsjLevels) {

		Level level;
		level.name = rsjLevel["name"].as<String>();

		{
			level.aerialServer = rsjLevel["aerialServer"].as<String>();
			level.aerialResolution = float(rsjLevel["aerialResolution"].as<double>(.25f));
			auto& rsjBounds = rsjLevel["aerialBounds"].as_array();
			assert(rsjBounds.size() == 4);
			level.aerialBounds.min.x = rsjBounds[0].as<int>();
			level.aerialBounds.max.x = rsjBounds[1].as<int>() + 1;
			level.aerialBounds.min.y = rsjBounds[2].as<int>();
			level.aerialBounds.max.y = rsjBounds[3].as<int>() + 1;
		}

		{
			level.topoServer = rsjLevel["topoServer"].as<String>();
			level.topoResolution = float(rsjLevel["topoResolution"].as<double>(1.0f));
			auto& rsjBounds = rsjLevel["topoBounds"].as_array();
			assert(rsjBounds.size() == 4);
			level.topoBounds.min.x = rsjBounds[0].as<int>();
			level.topoBounds.max.x = rsjBounds[1].as<int>() + 1;
			level.topoBounds.min.y = rsjBounds[2].as<int>();
			level.topoBounds.max.y = rsjBounds[3].as<int>() + 1;
		}

		level.bounds = level.aerialBounds & level.topoBounds;

		if (rsjLevel["origin"].exists()) {
			auto& rsjOrigin = rsjLevel["origin"].as_array();
			assert(rsjOrigin.size() == 2);
			level.origin.x = rsjOrigin[0].as<int>();
			level.origin.y = rsjOrigin[1].as<int>();
		} else {
			level.origin = level.bounds.center();
		}

		level.atlasSize = uint(rsjLevel["atlasSize"].as<int>());

		level.index = g_levels.size();
		g_levels.push_back(level);
	}
}

} // namespace

CVector<Level> levels() {

	if (g_levels.empty()) loadLevels();

	return g_levels;
}

void scrapeTiles(int levelIndex, int x, int y, int type, CString outDir) {

	type = 1;

	auto& level = levels()[levelIndex];
	auto path =
		toString(x + level.bounds.min.x) + "/" + toString(y + level.bounds.min.y) + (type == 0 ? ".jpg" : ".raw");

	auto url = replace((type == 0 ? level.aerialServer : level.topoServer), "roa.nz", "skid.nz") + path;

	String dir = outDir + level.name + "/";
	if (x == 0 && y == 0 && type == 1) {
		createDir(dir);
		assert(isDir(dir));
	}
	String outPath = dir + replace(path, "/", ",");

	fetch(url) | [levelIndex, x, y, type, outDir, url, outPath](CFetchResponse r) mutable {
		auto& level = levels()[levelIndex];

		if (r.httpStatus != 200) { //
//			debug() << "Error scraping" << url << r.httpStatus;
		} else {
			debug() << "Scraping tile " << url;
			saveString(r.text, outPath);
		}

		if (type == 0) {
			scrapeTiles(levelIndex, x, y, 1, outDir);
			return;
		}

		if (++x >= level.bounds.width()) {
			if (++y >= level.bounds.height()) {
				if (++levelIndex >= levels().size()) {
					debug() << "All done!";
					exit(0);
				}
				y = 0;
			}
			x = 0;
		}
		scrapeTiles(levelIndex, x, y, 0, outDir);
	};
}

} // namespace sgf
