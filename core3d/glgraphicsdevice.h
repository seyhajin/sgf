#pragma once

#include "graphicsdevice.h"

#include "gltypes.h"

namespace sgf {

class GLGraphicsDevice;

// ***** GLFrameBuffer *****

class GLFrameBuffer : public FrameBuffer {
public:
	SGF_OBJECT_TYPE(GLFrameBuffer, FrameBuffer)

	GLuint const glFramebuffer;

	~GLFrameBuffer() override {
		glDeleteFramebuffers(1, &glFramebuffer);
	}

protected:
	friend class GLGraphicsDevice;

	GLFrameBuffer(GraphicsDevice* device, Texture* colorTexture, Texture* depthTexture, GLuint glFramebuffer)
		: FrameBuffer(device, colorTexture, depthTexture), glFramebuffer(glFramebuffer) {
	}
};

// ***** GLGraphicsBuffer *****

class GLGraphicsBuffer : public GraphicsBuffer {
public:
	SGF_OBJECT_TYPE(GLGraphicsBuffer, GraphicsBuffer)

	GLenum const glTarget;
	GLuint const glBuffer;

	void updateData(uint offset, uint size, const void* data) override;

	void* lockData(uint offset, uint size) override;
	void unlockData() override;

protected:
	friend class GLGraphicsDevice;

	GLGraphicsBuffer(GraphicsDevice* device, BufferType type, uint size, GLenum glTarget, GLuint glBuffer)
		: GraphicsBuffer(device, type, size), glTarget(glTarget), glBuffer(glBuffer) {
	}

private:
	Vector<uchar> m_lockedData;
	uint m_lockedOffset = 0;
	uint m_lockedSize = 0;
	bool m_mapped = false;
	bool m_locked = false;
};

// ***** GLVertexState *****

class GLVertexState : public VertexState {
public:
	SGF_OBJECT_TYPE(GLVertexState, VertexState)

	GLuint const glVertexArray;

protected:
	friend class GLGraphicsDevice;

	GLVertexState(GraphicsDevice* device, Vector<SharedPtr<GraphicsBuffer>> vertexBuffers, GraphicsBuffer* indexBuffer,
				  VertexLayout layout, GLuint glVertexArray)
		: VertexState(device, std::move(vertexBuffers), indexBuffer, std::move(layout)), glVertexArray(glVertexArray) {
	}
};

// ***** GLTexture *****

class GLTexture : public Texture {
public:
	SGF_OBJECT_TYPE(GLTexture, Texture)

	GLuint const glTexture;

	~GLTexture() override {
		if(!bool(flags & TextureFlags::unmanaged))glDeleteTextures(1, &glTexture);
	}

protected:
	friend class GLGraphicsDevice;

	GLTexture(GraphicsDevice* device, uint width, uint height, TextureFormat format, TextureFlags flags,
			  GLuint glTexture)
		: Texture(device, width, height, format, flags), glTexture(glTexture) {
	}
};

// ***** GLShader *****

class GLShader : public Shader {
public:
	SGF_OBJECT_TYPE(GLShader, Shader)

	GLuint const glProgram;
	Vector<uint> const uniformBlocks;
	Vector<GLUniform> const uniforms;
	Vector<uint> const textures;

	~GLShader() override {
		glDeleteProgram(glProgram);
	}

protected:
	friend class GLGraphicsDevice;

	GLShader(GraphicsDevice* device, String source, GLuint glProgram, Vector<uint> uniformBlocks,
			 Vector<GLUniform> uniforms, Vector<uint> textures)
		: Shader(device, std::move(source)), glProgram(glProgram), uniformBlocks(std::move(uniformBlocks)),
		  uniforms(std::move(uniforms)), textures(std::move(textures)) {
	}
};

// ***** GLGraphicsContext *****

class GLGraphicsContext : public GraphicsContext {
public:
	SGF_OBJECT_TYPE(GLGraphicsContext, GraphicsContext)

	void setFrameBuffer(FrameBuffer* frameBuffer) override;
	void setViewport(CRecti viewport) override;
	void setDepthMode(DepthMode mode) override;
	void setBlendMode(BlendMode mode) override;
	void setCullMode(CullMode mode) override;

	void setVertexState(VertexState* vertexState) override;

	void setUniformBuffer(CString name, GraphicsBuffer* buffer) override;
	void setTexture(CString name, Texture* texture) override;
	void setUniform(CString name, CAny any) override;

	void setShader(Shader* shader) override;

	void clear(CVec4f color) override;
	void drawIndexedGeometry(uint order, uint firstVertex, uint numVertices, uint numInstances) override;
	void drawGeometry(uint order, uint firstVertex, uint numVertices, uint numInstances) override;

protected:
	friend class GLGraphicsDevice;

	explicit GLGraphicsContext(GraphicsDevice* device) : GraphicsContext(device) {
	}

private:
	enum struct Dirty {
		// clang-format off
		none = 					0x0000,
		frameBuffer = 			0x0001,
		viewport =				0x0002,
		depthMode = 			0x0004,
		blendMode = 			0x0008,
		cullMode = 				0x0010,
		vertexState =			0x0020,
		shader = 				0x0040,
		uniformBlockBindings =	0x0080,
		textureBindings = 		0x0100,
		uniformBindings =		0x0200,
		all = 					0x03ff,
		// clang-format on
	};

	Dirty m_dirty = Dirty::all;

	SharedPtr<GLFrameBuffer> m_frameBuffer;
	Recti m_viewport{{0, 0}, {640, 480}};
	DepthMode m_depthMode = DepthMode::disable;
	BlendMode m_blendMode = BlendMode::disable;
	CullMode m_cullMode = CullMode::disable;
	SharedPtr<GLVertexState> m_vertexState;
	SharedPtr<GLShader> m_shader;

	SharedPtr<GLGraphicsBuffer> m_uniformBuffers[32]{};
	SharedPtr<GLTexture> m_textures[64]{};
	Any m_uniforms[64]{};

	void validate();
};

// ***** GCGraphicsDevice *****

class GLGraphicsDevice : public GraphicsDevice {
public:
	SGF_OBJECT_TYPE(GLGraphicsDevice, GraphicsDevice)

	GLGraphicsDevice();

	FrameBuffer* createFrameBuffer(Texture* colorTexture, Texture* depthTexture) override;
	GraphicsBuffer* createGraphicsBuffer(BufferType type, uint size, const void* data) override;
	VertexState* createVertexState(CVector<GraphicsBuffer*> vertexBuffers, GraphicsBuffer* indexBuffer,
								   VertexLayout layout) override;
	Texture* createTexture(uint width, uint height, TextureFormat format, TextureFlags flags,
						   const void* data) override;
	Shader* createShader(CString source) override;
	GraphicsContext* createGraphicsContext() override;

	Texture* wrapGLTexture(uint width, uint height, TextureFormat format, TextureFlags flags,
						   GLuint texture);
};

} // namespace sgf
