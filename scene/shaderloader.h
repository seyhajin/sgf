#pragma once

#include "renderparams.h"

#include <core3d/core3d.hh>

namespace sgf {

Shader* loadShader(CString path);

class ShaderLoader {
public:
	explicit ShaderLoader(CString path) {
		m_loadFunc = [path] { return loadShader(path); };
	}

	Shader* open() {
		if (!m_shader) m_shader = m_loadFunc();
		return m_shader;
	}

private:
	using LoadFunc = Function<Shader*()>;

	SharedPtr<Shader> m_shader;
	LoadFunc m_loadFunc;
};

} // namespace sgf
