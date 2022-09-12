#include <geom/geom.hh>

using namespace sgf;

Vec3f rndEuler() {
	return {rnd(-pi, pi), rnd(-pi, pi), rnd(-pi, pi)};
}

bool approxEqual(CMat3f x, CMat3f y, float epsilon) {
	float e = 1e-6f;
	const float* px = &x.i.x;
	const float* py = &y.i.x;
	for (uint i = 0; i < 9; ++i) {
		if (std::abs(px[i] - py[i])<= epsilon) continue;
		debug() << "### OOPS:";
		debug() << "###" << x;
		debug() << "###" << y;
		return false;
	}
	return true;
}

int testEulerQuatMatConversions() {

	int errs = 0;

	for (int i = 0; i < 10000; ++i) {

		auto e = rndEuler();

		Mat3f m0 = Mat3f::rotation(e);				      // euler -> mat

		Mat3f m1 = Mat3f(Quatf::rotation(e));		  // euler -> quat -> mat
		if (!approxEqual(m0, m1, 1e-6f)) ++errs;

		m1 = Mat3f(Quatf(m0));						  // mat-> quat -> mat
		if (!approxEqual(m0, m1, 1e-6f)) ++errs;

		m1 = Mat3f::rotation(m0.rotation());		      // mat -> euler -> mat
		if (!approxEqual(m0, m1, 1e-6f)) ++errs;

		m1 = Mat3f::rotation((Quatf(m0).rotation()));  // mat -> quat -> euler -> mat
		if (!approxEqual(m0, m1, 1e-6f)) ++errs;

		m1 = Mat3f(Quatf::rotation(m0.rotation()));	  // mat -> euler -> quat -> mat
		if (!approxEqual(m0, m1, 1e-6f)) ++errs;
	}
	return errs;
}

int testQuatOps() {
	int errs = 0;

	for (int i = 0; i < 10000; ++i) {

		// Test '*'
		Quatf q0=Quatf::rotation(rndEuler());
		Quatf q1=Quatf::rotation(rndEuler());

		Mat3f m0=Mat3f(q0) * Mat3f(q1);
		Mat3f m1=Mat3f(q0*q1);

		if (approxEqual(m0, m1, 1e-6)) continue;

		debug() << "### OOPS:\n" << q0 << q1 << "\n" << m0 << "\n" << m1;
		++errs;
	}

	return errs;
}

int main() {

	testEulerQuatMatConversions();

#if 0
	// euler -> quat broken!
	// euler -> mat -> quat OK!

	auto e = rndEuler();

	auto q0 = Quatf::rotation(e);	// euler -> quat
	auto q1 = Quatf(Mat3f::rotation(e));		// euler -> mat -> quat

	debug() << "###"<<q0<<q0.length();
	debug() << "###"<<q1<<q1.length();
	debug() << "###"<< q0.dot(q1);
#endif
}
