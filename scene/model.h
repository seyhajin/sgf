#pragma once

#include "mesh.h"

namespace sgf {

extern const VertexAttribs defaultModelAttribs;

class Scene;
class ModelRenderer;

class Model : public Shared {
public:
	SGF_OBJECT_TYPE(Model, Shared);

	struct Surface {
		SharedPtr<Material> const material;
		uint const firstIndex;
		uint const numIndices;

		Surface(Material* material, uint firstIndex, uint numIndices)
			: material(material), firstIndex(firstIndex), numIndices(numIndices) {
		}
	};

	SharedPtr<GraphicsBuffer> const vertexBuffer;
	SharedPtr<GraphicsBuffer> const indexBuffer;
	SharedPtr<GraphicsBuffer> const outlineBuffer;
	Vector<Surface> const opaqueSurfaces;
	Vector<Surface> const blendedSurfaces;
	VertexLayout const vertexLayout;

	mutable Map<Scene*, ModelRenderer*> m_renderers;

	Model(GraphicsBuffer* vertexBuffer, GraphicsBuffer* indexBuffer, Vector<Surface> opaqueSurfaces,
		  Vector<Surface> blendedSurfaces, GraphicsBuffer* outlineBuffer, VertexLayout vertexLayout)
		: vertexBuffer(vertexBuffer), indexBuffer(indexBuffer), outlineBuffer(outlineBuffer),
		  opaqueSurfaces(std::move(opaqueSurfaces)), blendedSurfaces(std::move(blendedSurfaces)),
		  vertexLayout(std::move(vertexLayout)) {
	}

	bool hasOpaqueSurfaces() const {
		return !opaqueSurfaces.empty();
	}

	bool hasBlendedSurfaces() const {
		return !blendedSurfaces.empty();
	}

	ModelRenderer* getRenderer(Scene* scene);
};

Model* createModel(const Mesh* mesh, CVertexAttribs attribFormats = defaultModelAttribs);

} // namespace sgf
