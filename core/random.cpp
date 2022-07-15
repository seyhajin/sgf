#include "random.h"

namespace sgf {

namespace {

struct ranctx {
	uint a = 3;
	uint b = 11;
	uint c = 23;
	uint d = 37;
};

ranctx r;

#define rot(x, k) (((x) << (k)) | ((x) >> (32 - (k))))
uint rndval(ranctx* x) {
	uint e = x->a - rot(x->b, 27);
	x->a = x->b ^ rot(x->c, 17);
	x->b = x->c + x->d;
	x->c = x->d + e;
	x->d = e + x->a;
	return x->d;
}

void rndinit(ranctx* x, uint seed) {
	uint i;
	x->a = 0xf1ea5eed, x->b = x->c = x->d = seed;
	for (i = 0; i < 20; ++i) { (void)rndval(x); }
}

} // namespace

void seedRnd(uint seed) {
	rndinit(&r, seed);
}

void seedRnd() {
	uint tm = time(nullptr);
	seedRnd(tm);
}

float rnd() {
	return float(rndval(&r) & 0xffffff) / float(0x1000000);
}

float rnd(float max) {
	return rnd() * max;
}

float rnd(float min, float max) {
	return rnd(max - min) + min;
}

}
