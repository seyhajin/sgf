#pragma once

#include "types.h"

namespace sgf {

class Window;
class GraphicsDevice;

// ***** GraphicsResource ******

class GraphicsResource : public Shared {
public:
	SGF_OBJECT_TYPE(GraphicsResource, Shared)

	GraphicsDevice* const graphicsDevice;

protected:
	GraphicsResource(GraphicsDevice* device) : graphicsDevice(device) {
	}
};

// ***** TextureResource *****

class TextureResource : public GraphicsResource {
public:
	SGF_OBJECT_TYPE(TextureResource, GraphicsResource)

protected:
	TextureResource(GraphicsDevice* device) : GraphicsResource(device) {
	}
};

// ***** Texture *****

class Texture : public TextureResource {
public:
	SGF_OBJECT_TYPE(Texture, TextureResource)

	uint const width;
	uint const height;
	TextureFormat const format;
	TextureFlags const flags;

	virtual void updateData(uint mipLevel,uint x, uint y, uint width, uint height, const void* data) = 0;

protected:
	Texture(GraphicsDevice* device, uint width, uint height, TextureFormat format, TextureFlags flags)
		: TextureResource(device), width(width), height(height), format(format), flags(flags) {
	}
};

// ***** ArrayTexture *****

class ArrayTexture : public TextureResource {
public:
	SGF_OBJECT_TYPE(ArrayTexture, TextureResource)

	uint const width;
	uint const height;
	uint const depth;
	TextureFormat const format;
	TextureFlags const flags;

	virtual void updateData(uint mipLevel, uint x, uint y, uint z, uint width, uint height, uint depth, const void* data) = 0;

protected:
	ArrayTexture(GraphicsDevice* device, uint width, uint height, uint depth, TextureFormat format, TextureFlags flags)
		: TextureResource(device), width(width), height(height), depth(depth), format(format), flags(flags) {
	}
};

// ***** GraphicsBuffer *****

class GraphicsBuffer : public GraphicsResource {
public:
	SGF_OBJECT_TYPE(GraphicsBuffer, GraphicsResource)

	BufferType type;
	uint const size;

	virtual void updateData(uint offset, uint size, const void* data) = 0;

	virtual void* lockData(uint offset, uint size) = 0;
	virtual void unlockData() = 0;

protected:
	GraphicsBuffer(GraphicsDevice* device, BufferType type, uint size)
		: GraphicsResource(device), type(type), size(size) {
	}
};

// ***** VertexState *****

class VertexState : public GraphicsResource {
public:
	SGF_OBJECT_TYPE(VertexState, GraphicsResource)

	Vector<SharedPtr<GraphicsBuffer>> const vertexBuffers;
	SharedPtr<GraphicsBuffer> const indexBuffer;
	VertexLayout const layout;

protected:
	VertexState(GraphicsDevice* device, Vector<SharedPtr<GraphicsBuffer>> vertexBuffers, GraphicsBuffer* indexBuffer,
				VertexLayout layout)
		: GraphicsResource(device), vertexBuffers(std::move(vertexBuffers)), indexBuffer(indexBuffer),
		  layout(std::move(layout)) {
	}
};

// ***** FrameBuffer *****

class FrameBuffer : public GraphicsResource {
public:
	SGF_OBJECT_TYPE(FrameBuffer, GraphicsResource)

	SharedPtr<Texture> const colorTexture;
	SharedPtr<Texture> const depthTexture;
	uint const width;
	uint const height;

protected:
	FrameBuffer(GraphicsDevice* device, Texture* colorTexture, Texture* depthTexture, uint width, uint height)
		: GraphicsResource(device), colorTexture(colorTexture), depthTexture(depthTexture), width(width),
		  height(height) {
	}
};

// ***** Shader *****

class Shader : public GraphicsResource {
public:
	SGF_OBJECT_TYPE(Shader, GraphicsResource)

	String const source;

protected:
	Shader(GraphicsDevice* device, String source) : GraphicsResource(device), source(std::move(source)) {
	}
};

// ***** GraphicsContext *****

class GraphicsContext : public GraphicsResource {
public:
	SGF_OBJECT_TYPE(GraphicsContext, GraphicsResource)

	virtual void setFrameBuffer(FrameBuffer* frameBuffer) = 0;
	virtual void setViewport(CRecti viewport) = 0;
	virtual void setDepthMode(DepthMode mode) = 0;
	virtual void setBlendMode(BlendMode mode) = 0;
	virtual void setCullMode(CullMode mode) = 0;

	virtual void setVertexState(VertexState* vertexState) = 0;

	virtual void setUniformBuffer(CString name, GraphicsBuffer* buffer) = 0;
	virtual void setTexture(CString name, TextureResource* texture) = 0;
	virtual void setUniform(CString name, CAny any) = 0;
	virtual void setShader(Shader* shader) = 0;

	virtual void clear(CVec4f color) = 0;
	virtual void drawIndexedGeometry(uint order, uint firstIndex, uint numIndices, uint numInstances) = 0;
	virtual void drawGeometry(uint order, uint firstVertex, uint numVertices, uint numInstances) = 0;

protected:
	explicit GraphicsContext(GraphicsDevice* device) : GraphicsResource(device) {
	}
};

// ***** GraphicsDevice *****

class GraphicsDevice : public Object {
public:
	SGF_OBJECT_TYPE(GraphicsDevice, Object);

	Window* const window;

	virtual Texture* createTexture(uint width, uint height, TextureFormat format, TextureFlags flags,
								   const void* data) = 0;
	virtual ArrayTexture* createArrayTexture(uint width, uint height, uint depth, TextureFormat format, TextureFlags flags,
								   const void* data) = 0;
	virtual GraphicsBuffer* createGraphicsBuffer(BufferType type, uint size, const void* data) = 0;
	virtual FrameBuffer* createFrameBuffer(Texture* colorTexture, Texture* depthTexture) = 0;
	virtual VertexState* createVertexState(CVector<GraphicsBuffer*> vertexBuffers, GraphicsBuffer* indexBuffer,
										   VertexLayout layout) = 0;
	virtual Shader* createShader(CString source) = 0;
	virtual GraphicsContext* createGraphicsContext() = 0;

protected:
	GraphicsDevice(Window* window) : window(window) {
	}

private:
	static inline GraphicsDevice* g_graphicsDevice;

	friend GraphicsDevice* createGraphicsDevice(Window* window);

	friend GraphicsDevice* graphicsDevice();
};

GraphicsDevice* createGraphicsDevice(Window* window);

inline GraphicsDevice* graphicsDevice() {
	return GraphicsDevice::g_graphicsDevice;
}

} // namespace sgf
