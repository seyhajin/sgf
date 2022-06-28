//
// Created by marksibly on 20/06/22.
//

#include "shaderloader.h"

namespace sgf {

namespace {

size_t findLine(CString str, CString substr, size_t pos = 0) {
	for (;;) {
		pos = str.find(substr, pos);
		if (pos == String::npos || pos == 0 || str[pos - 1] == '\n') return pos;
		pos += substr.size();
	}
}

void loadFrags(CString path, String frags[]) {

	String source = loadString(path);
	auto i0 = findLine(source, "//@vertex");

	if (i0 == String::npos) {
		frags[0] = frags[1] = source;
		return;
	}

	String common = source.substr(0, i0);

	i0 = source.find('\n', i0);
	if (i0 == String::npos) panic("OOPS");

	auto i1 = findLine(source, "//@fragment", i0);
	if (i1 == String::npos) panic("OOPS");

	frags[0] = common + source.substr(i0, i1 - i0);

	i1 = source.find('\n', i1);
	if (i1 == String::npos) panic("OOPS");

	frags[1] = common + source.substr(i1);
}

} // namespace

Shader* loadShader(CString path) {

	String idir;
	auto i = path.rfind('/');
	if (i != String::npos) idir = path.substr(0, i + 1);

	String frags[2];
	loadFrags(path, frags);

	for (uint frag = 0; frag < 2; ++frag) {

		String& source = frags[frag];
		StringMap<bool> inced;

		for (size_t pos = 0;;) {

			pos = findLine(source, "#include", pos);
			if (pos == String::npos) break;

			auto sol = pos;
			auto eol = source.find('\n', sol);
			verify(eol != String::npos);

			auto i0 = source.find('\"', sol);
			verify(i0 != String::npos);
			auto i1 = source.find('\"', ++i0);
			verify(i1 != String::npos && i1 < eol);

			auto ipath = source.substr(i0, i1 - i0);

			String r;
			if (inced.find(ipath) == inced.end()) {
				String ifrags[2];
				loadFrags(idir + ipath, ifrags);
				r = ifrags[frag];
				inced[ipath] = true;
			}

			source = source.replace(sol, eol - sol, r);
		}
	}

	auto source = "//@vertex\n" + frags[0] + "//@fragment\n" + frags[1];

	return graphicsDevice()->createShader(source);
}

} // namespace sgf
