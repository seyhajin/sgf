#pragma once

#include "renderparams.h"

#include <core3d/core3d.hh>

namespace sgf {

Shader* loadShader(CString path);

class ShaderAsset;
using CShaderAsset = const ShaderAsset&;

class ShaderAsset {
public:
	ShaderAsset() : m_loader([]() -> Shader* { return nullptr; }) {
	}
	
	ShaderAsset(CString assetPath) : m_loader([assetPath] { return loadShader(assetPath); }) {
	}

	ShaderAsset(CShaderAsset) = default;

	ShaderAsset(ShaderAsset&&) = default;

	ShaderAsset& operator=(CShaderAsset) = default;

	ShaderAsset& operator=(ShaderAsset&&) = default;

	bool operator==(CShaderAsset that) const {
		return m_loader == that.m_loader;
	}

	bool operator!=(CShaderAsset that) const {
		return m_loader != that.m_loader;
	}

	bool operator<(CShaderAsset that) const {
		return m_loader < that.m_loader;
	}

	Shader* open() const {
		if (!m_shader) m_shader = m_loader();
		return m_shader;
	}

private:
	using LoadFunc = Function<Shader*()>;

	mutable SharedPtr<Shader> m_shader;
	Function<Shader*()> m_loader;
};

} // namespace sgf
