#pragma once

#include <scene/model.h>

namespace sgf {

Mesh* loadMesh(CString assetPath);

#if 0
Model* loadModel(CString assetPath);

class ModelAsset;
using CModelAsset = const ModelAsset&;

class ModelAsset {
public:
	ModelAsset() : m_loader([]() -> Model* { return nullptr; }) {
	}

	ModelAsset(Model* model) : m_model(model) {
		assert(model);
	}

	ModelAsset(CString assetPath) : m_loader([assetPath]() -> Model* { return loadModel(assetPath); }) {
	}

	ModelAsset(CModelAsset) = default;
	ModelAsset(ModelAsset&&) = default;
	ModelAsset& operator=(CModelAsset) = default;
	ModelAsset& operator=(ModelAsset&&) = default;

	bool operator==(CModelAsset that) const {
		return m_loader == that.m_loader;
	}

	bool operator!=(CModelAsset that) const {
		return m_loader != that.m_loader;
	}

	bool operator<(CModelAsset that) const {
		return m_loader < that.m_loader;
	}

	Model* open() const {
		if (!m_model) m_model = m_loader();
		return m_model;
	}

private:
	mutable SharedPtr<Model> m_model;
	Function<Model*()> m_loader;
};
#endif

} // namespace sgf
