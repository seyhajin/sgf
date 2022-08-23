#pragma once

#include "mesh.h"

namespace sgf {

extern const VertexAttribs defaultModelAttribs;

class Scene;
class ModelRenderer;

class ModelRenderData : public Shared {
public:
	SGF_OBJECT_TYPE(ModelRenderData, Shared);

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
	Vector<Surface> const opaqueSurfaces;
	Vector<Surface> const blendedSurfaces;
	VertexLayout const vertexLayout;
	SharedPtr<GraphicsBuffer> const outlineBuffer;

	ModelRenderData(GraphicsBuffer* vertexBuffer, GraphicsBuffer* indexBuffer, Vector<Surface> opaqueSurfaces,
		  Vector<Surface> blendedSurfaces, VertexLayout vertexLayout, GraphicsBuffer* outlineBuffer)
		: vertexBuffer(vertexBuffer), indexBuffer(indexBuffer),
		  opaqueSurfaces(std::move(opaqueSurfaces)), blendedSurfaces(std::move(blendedSurfaces)),
		  vertexLayout(std::move(vertexLayout)), outlineBuffer(outlineBuffer) {
	}

	bool hasOpaqueSurfaces() const {
		return !opaqueSurfaces.empty();
	}

	bool hasBlendedSurfaces() const {
		return !blendedSurfaces.empty();
	}

	ModelRenderer* getRenderer(Scene* scene);

private:
	mutable Map<Scene*, ModelRenderer*> m_renderers;
};

ModelRenderData* createModelRenderData(const Mesh* mesh, CVertexAttribs attribFormats = defaultModelAttribs);

} // namespace sgf
