#pragma once

#include "graphicsdevice.h"

#include "gltypes.h"

namespace sgf {

class GLGraphicsDevice;

// ***** GLTexture *****

class GLTexture : public Texture {
public:
	GLuint glTexture;

	~GLTexture() override {
		glDeleteTextures(1, &glTexture);
	}

protected:
	friend class GLGraphicsDevice;

	GLTexture(GraphicsDevice* device, uint width, uint height, TextureFormat format, TextureFlags flags,
			  GLuint glTexture)
		: Texture(device, width, height, format, flags), glTexture(glTexture) {
	}
};

// ***** GLUniformBuffer *****

class GLUniformBuffer : public UniformBuffer {
public:
	GLuint glBuffer;

	void updateData(uint offset, uint size, const void* data) override;

protected:
	friend class GLGraphicsDevice;

	GLUniformBuffer(GraphicsDevice* device, uint size, GLuint glBuffer)
		: UniformBuffer(device, size), glBuffer(glBuffer) {
	}
};

// ***** GLVertexBuffer *****

class GLVertexBuffer : public VertexBuffer {
public:
	GLuint const glVertexArray;
	GLuint const glBuffer;

	void updateData(uint firstVertex, uint numVertices, const void* data) override;

	~GLVertexBuffer() override {
		glDeleteBuffers(1, &glBuffer);
	}

protected:
	friend class GLGraphicsDevice;

	GLVertexBuffer(GraphicsDevice* device, uint length, VertexFormat format, GLuint glVertexArray, GLuint glBuffer)
		: VertexBuffer(device, length, std::move(format)), glVertexArray(glVertexArray), glBuffer(glBuffer) {
	}
};

// ***** GLIndexBuffer *****

class GLIndexBuffer : public IndexBuffer {
public:
	GLuint const glBuffer;

	void updateData(uint firstIndex, uint numIndices, const void* data) override;

	~GLIndexBuffer() override {
		glDeleteBuffers(1, &glBuffer);
	}

protected:
	friend class GLGraphicsDevice;

	GLIndexBuffer(GraphicsDevice* device, uint length, IndexFormat format, GLuint glBuffer)
		: IndexBuffer(device, length, format), glBuffer(glBuffer) {
	}
};

// ***** GLFrameBuffer *****

class GLFrameBuffer : public FrameBuffer {
public:
	GLuint glFramebuffer;

	~GLFrameBuffer() override {
		glDeleteFramebuffers(1, &glFramebuffer);
	}

protected:
	friend class GLGraphicsDevice;

	GLFrameBuffer(GraphicsDevice* device, Texture* colorTexture, Texture* depthTexture, GLuint glFramebuffer)
		: FrameBuffer(device, colorTexture, depthTexture), glFramebuffer(glFramebuffer) {
	}
};

// ***** GLShader *****

class GLShader : public Shader {
public:
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
	void setUniformBuffer(CString name, UniformBuffer* buffer) override;
	void setTextureUniform(CString name, Texture* texture) override;
	void setSimpleUniform(CString name, CAny any) override;

	void setVertexBuffer(VertexBuffer* buffer) override;
	void setIndexBuffer(IndexBuffer* buffer) override;
	void setFrameBuffer(FrameBuffer* frameBuffer) override;

	void setShader(Shader* shader) override;

	void setViewport(CRecti viewport) override;
	void setDepthMode(DepthMode mode) override;
	void setBlendMode(BlendMode mode) override;
	void setCullMode(CullMode mode) override;

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
		textures =      		0x0001,
		vertexBuffer =			0x0002,
		indexBuffer = 			0x0004,
		frameBuffer = 			0x0008,
		shader = 				0x0010,
		viewport =				0x0020,
		depthMode = 			0x0040,
		blendMode = 			0x0080,
		cullMode = 				0x0100,
		uniformBlockBindings = 	0x0200,
		uniformBindings =		0x0400,
		textureBindings = 		0x0800,
		all = 					0x0fff,
		bindings = uniformBlockBindings | uniformBindings | textureBindings,
		// clang-format on
	};

	Dirty m_dirty = Dirty::all;

	SharedPtr<GLTexture> m_textures[64]{};
	SharedPtr<GLUniformBuffer> m_uniformBuffers[32]{};
	Any m_uniforms[64]{};

	SharedPtr<GLVertexBuffer> m_vertexBuffer;
	SharedPtr<GLIndexBuffer> m_indexBuffer;
	SharedPtr<GLFrameBuffer> m_frameBuffer;
	SharedPtr<GLShader> m_shader;
	DepthMode m_depthMode = DepthMode::disable;
	BlendMode m_blendMode = BlendMode::disable;
	CullMode m_cullMode = CullMode::disable;
	Recti m_viewport{{0, 0}, {640, 480}};

	void validate();
};

// ***** GCGraphicsDevice *****

class GLGraphicsDevice : public GraphicsDevice {
public:
	GLGraphicsDevice();

	Texture* createTexture(uint width, uint height, TextureFormat format, TextureFlags flags,
						   const void* data) override;
	UniformBuffer* createUniformBuffer(uint size, const void* data) override;
	VertexBuffer* createVertexBuffer(uint length, VertexFormat format, const void* data) override;
	IndexBuffer* createIndexBuffer(uint length, IndexFormat format, const void* data) override;
	FrameBuffer* createFrameBuffer(Texture* colorTexture, Texture* depthTexture) override;
	Shader* createShader(CString source) override;
	GraphicsContext* createGraphicsContext() override;

	// OpenGL extensions
	// Texture* createGLTextureWrapper(uint width, uint height, TextureFormat format, TextureFlags flags, GLuint
	// glTexture);
};

} // namespace sgf
