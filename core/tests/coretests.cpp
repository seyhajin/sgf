#include <core/core.hh>

using namespace sgf;

int main() {

	for (int x = -99; x <= 99; ++x) {
		int z = floorMod(x, 9);
		debug() << "###" << x << "->" << z;
		assert(z >= 0 && z < 9);
	}
}
