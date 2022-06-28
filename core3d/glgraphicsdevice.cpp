#include "glgraphicsdevice.h"

#include "gltypes.h"

#include <glfw/glfw.hh>

namespace sgf {

namespace {

struct IdMap {

	uint next = 0;
	StringMap<uint> map;

	uint get(CString name) {
		auto it = map.find(name);
		if (it != map.end()) return it->second;
		map.insert(it, std::make_pair(name, ++next));
		return next;
	}
};

IdMap uniformBufferIds;
IdMap uniformIds;
IdMap textureIds;

GLuint glNullVertexArray;

} // namespace

// ***** GLUniformBuffer *****

void GLUniformBuffer::updateData(uint offset, uint size, const void* data) {
	assert(offset + size <= this->size);

	glAssert();
	glBindBuffer(GL_UNIFORM_BUFFER, glBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
	glAssert();
}

// ***** GLVertexBuffer *****

void GLVertexBuffer::updateData(uint firstVertex, uint numVertices, const void* data) {
	assert(firstVertex + numVertices <= length);

	glAssert();
	glBindBuffer(GL_ARRAY_BUFFER, glBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, firstVertex * bytesPerVertex, numVertices * bytesPerVertex, data);
	glAssert();
}

// ***** GLIndexBuffer *****

void GLIndexBuffer::updateData(uint firstIndex, uint numIndices, const void* data) {
	assert(firstIndex + numIndices <= length);

	glAssert();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glBuffer);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, firstIndex * bytesPerIndex, numIndices * bytesPerIndex, data);
	glAssert();
}

// ***** GLGraphicsContext *****

void GLGraphicsContext::setUniformBuffer(CString name, UniformBuffer* uniformBuffer) {
	uint id = uniformBufferIds.get(name);
	if (uniformBuffer == m_uniformBuffers[id]) return;
	m_uniformBuffers[id] = static_cast<GLUniformBuffer*>(uniformBuffer);
	m_dirty |= Dirty::uniformBlockBindings;
}

void GLGraphicsContext::setTextureUniform(CString name, Texture* texture) {
	uint id = textureIds.get(name);
	if (texture == m_textures[id]) return;
	m_textures[id] = static_cast<GLTexture*>(texture);
	m_dirty |= Dirty::textureBindings;
}

void GLGraphicsContext::setSimpleUniform(CString name, CAny any) {
	uint id = uniformIds.get(name);
	if (m_uniforms[id] == any) return;
	m_uniforms[id] = any;
	m_dirty |= Dirty::uniformBindings;
}

void GLGraphicsContext::setVertexBuffer(VertexBuffer* vertexBuffer) {
	if (vertexBuffer == m_vertexBuffer) return;
	m_vertexBuffer = static_cast<GLVertexBuffer*>(vertexBuffer);
	m_dirty |= Dirty::vertexBuffer;
}

void GLGraphicsContext::setIndexBuffer(IndexBuffer* indexBuffer) {
	if (indexBuffer == m_indexBuffer) return;
	m_indexBuffer = static_cast<GLIndexBuffer*>(indexBuffer);
	m_dirty |= Dirty::indexBuffer;
}

void GLGraphicsContext::setFrameBuffer(FrameBuffer* frameBuffer) {
	if (frameBuffer == m_frameBuffer) return;
	m_frameBuffer = static_cast<GLFrameBuffer*>(frameBuffer);
	m_dirty |= Dirty::frameBuffer;
}

void GLGraphicsContext::setShader(Shader* shader) {
	if (shader == m_shader) return;
	m_shader = static_cast<GLShader*>(shader);
	m_dirty |= Dirty::shader;
}

void GLGraphicsContext::setDepthMode(DepthMode mode) {
	if (mode == m_depthMode) return;
	m_depthMode = mode;
	m_dirty |= Dirty::depthMode;
}

void GLGraphicsContext::setBlendMode(BlendMode mode) {
	if (mode == m_blendMode) return;
	m_blendMode = mode;
	m_dirty |= Dirty::blendMode;
}

void GLGraphicsContext::setCullMode(CullMode mode) {
	if (mode == m_cullMode) return;
	m_cullMode = mode;
	m_dirty |= Dirty::cullMode;
}

void GLGraphicsContext::setViewport(CRecti viewport) {
	if (viewport == m_viewport) return;
	m_viewport = viewport;
	m_dirty |= Dirty::viewport;
}

void GLGraphicsContext::validate() {
	glAssert();

	if (bool(m_dirty & Dirty::frameBuffer)) {
		glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer ? m_frameBuffer->glFramebuffer : 0);
		glAssert();
	}

	if (bool(m_dirty & Dirty::viewport)) {
		glViewport(m_viewport.x(), m_viewport.y(), m_viewport.width(), m_viewport.height());
		glAssert();
	}

	if (bool(m_dirty & Dirty::vertexBuffer)) {
		glBindVertexArray(m_vertexBuffer ? m_vertexBuffer->glVertexArray : glNullVertexArray);
		m_dirty |= Dirty::indexBuffer;
		glAssert();
	}

	if (bool(m_dirty & Dirty::indexBuffer)) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer ? m_indexBuffer->glBuffer : 0);
		glAssert();
	}

	if (bool(m_dirty & Dirty::shader)) {
		glUseProgram(m_shader ? m_shader->glProgram : 0);
		m_dirty |= Dirty::bindings;
		glAssert();
	}

	if (bool(m_dirty & Dirty::uniformBlockBindings) && m_shader) {
		for (uint id : m_shader->uniformBlocks) {
			auto buffer = m_uniformBuffers[id].value();
			assert(buffer);
			glBindBufferBase(GL_UNIFORM_BUFFER, id, buffer->glBuffer);
		}
		glAssert();
	}

	if (bool(m_dirty & Dirty::textureBindings) && m_shader) {
		GLint texUnit = GL_TEXTURE0;
		for (uint id : m_shader->textures) {
			auto texture = m_textures[id].value();
			assert(texture);
			glActiveTexture(texUnit++);
			glBindTexture(GL_TEXTURE_2D, texture->glTexture);
		}
		glAssert();
	}

	if (bool(m_dirty & Dirty::uniformBindings) && m_shader) {
		for (auto& uniform : m_shader->uniforms) {
			assert(m_uniforms[uniform.id].exists());
			bindGLUniform(uniform.glType, uniform.glLocation, m_uniforms[uniform.id]);
		}
		glAssert();
	}

	if (bool(m_dirty & Dirty::depthMode)) {
		switch (m_depthMode) {
		case DepthMode::disable:
			glDisable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);
			break;
		case DepthMode::enable:
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LEQUAL);
			glDepthMask(GL_TRUE);
			break;
		case DepthMode::compare:
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LEQUAL);
			glDepthMask(GL_FALSE);
			break;
		}
		glAssert();
	}

	if (bool(m_dirty & Dirty::blendMode)) {
		switch (m_blendMode) {
		case BlendMode::disable:
			glDisable(GL_BLEND);
			break;
		case BlendMode::alpha:
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
			break;
		case BlendMode::additive:
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ONE); //_MINUS_SRC_ALPHA);
			break;
		case BlendMode::multiply:
			glEnable(GL_BLEND);
			glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);
			break;
		}
		glAssert();
	}

	if (bool(m_dirty & Dirty::cullMode)) {
		switch (m_cullMode) {
		case CullMode::disable:
			glDisable(GL_CULL_FACE);
			break;
		case CullMode::front:
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			break;
		case CullMode::back:
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);
			break;
		case CullMode::all:
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT_AND_BACK);
			break;
		}
		glAssert();
	}

	m_dirty = Dirty::none;
}

void GLGraphicsContext::clear(CVec4f color) {

	// TODO: We only really need to validate framebuffer and viewport?
	validate();

	glEnable(GL_SCISSOR_TEST);
	glScissor(m_viewport.x(), m_viewport.y(), m_viewport.width(), m_viewport.height());

	glClearColor(color.x, color.y, color.z, color.w);

	if (m_depthMode != DepthMode::enable) glDepthMask(GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (m_depthMode != DepthMode::enable) glDepthMask(GL_FALSE);

	glDisable(GL_SCISSOR_TEST);

	glAssert();
}

void GLGraphicsContext::drawIndexedGeometry(uint order, uint firstVertex, uint numVertices, uint numInstances) {
	static constexpr GLenum modes[] = {GL_NONE, GL_POINTS, GL_LINES, GL_TRIANGLES};
	static constexpr GLenum types[] = {GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, GL_UNSIGNED_INT};
	static constexpr uint size[] = {1, 2, 4};

	assert(order > 0 && order < 4);

	auto type = types[int(m_indexBuffer->format)];
	auto ptr = reinterpret_cast<void*>(bytesPerIndex(m_indexBuffer->format) * firstVertex);

	validate();

	if (numInstances > 1) {
		glDrawElementsInstanced(modes[order], int(numVertices), type, ptr, int(numInstances));
	} else {
		glDrawElements(modes[order], int(numVertices), type, ptr);
	}

	glAssert();
}

void GLGraphicsContext::drawGeometry(uint order, uint firstVertex, uint numVertices, uint numInstances) {
	static constexpr GLenum modes[] = {GL_NONE, GL_POINTS, GL_LINES, GL_TRIANGLES};

	assert(order > 0 && order < 4);

	validate();

	if (numInstances > 1) {
		glDrawArraysInstanced(modes[order], int(firstVertex), int(numVertices), int(numInstances));
	} else {
		glDrawArrays(modes[order], int(firstVertex), int(numVertices));
	}

	glAssert();
}

// ***** GLGraphicsDevice *****

GLGraphicsDevice::GLGraphicsDevice() {
	glGenVertexArrays(1, &glNullVertexArray);
	glBindVertexArray(glNullVertexArray);
}

Texture* GLGraphicsDevice::createTexture(uint width, uint height, TextureFormat format, TextureFlags flags,
										 const void* data) {

	glAssert();

	GLenum glTarget = GL_TEXTURE_2D;

	GLuint glTexture;
	glGenTextures(1, &glTexture);
	glBindTexture(glTarget, glTexture);

	float aniso = 0.0f;
	if (glfwExtensionSupported("GL_EXT_texture_filter_anisotropic")) {
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
	}
	//	if (GLEW_EXT_texture_filter_anisotropic) { glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso); }

	if (bool(flags & TextureFlags::mipmap)) {
		glTexParameteri(glTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(glTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		if (aniso != 0) glTexParameterf(glTarget, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);
	} else if (bool(flags & TextureFlags::linear)) {
		glTexParameteri(glTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(glTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	} else {
		glTexParameteri(glTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(glTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
	if (bool(flags & TextureFlags::clampS)) {
		glTexParameteri(glTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	} else {
		glTexParameteri(glTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
	}
	if (bool(flags & TextureFlags::clampT)) {
		glTexParameteri(glTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	} else {
		glTexParameteri(glTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	setGLTextureData(width, height, format, flags, data);

	glBindTexture(glTarget, 0);

	glAssert();

	return new GLTexture(this, width, height, format, flags, glTexture);
}

UniformBuffer* GLGraphicsDevice::createUniformBuffer(uint size, const void* data) {

	glAssert();

	GLuint glBuffer;
	glGenBuffers(1, &glBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, glBuffer);
	glBufferData(GL_UNIFORM_BUFFER, size, data, data ? GL_STATIC_DRAW : GL_STREAM_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glAssert();

	return new GLUniformBuffer(this, size, glBuffer);
}

VertexBuffer* GLGraphicsDevice::createVertexBuffer(uint length, VertexFormat format, const void* data) {

	glAssert();

	GLenum glAttribTypes[] = {GL_NONE,	GL_FLOAT, GL_FLOAT,			GL_FLOAT,
							  GL_FLOAT, GL_BYTE,  GL_UNSIGNED_BYTE, GL_UNSIGNED_BYTE};

	uint stride = bytesPerVertex(format);

	GLuint glBuffer;
	glGenBuffers(1, &glBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, glBuffer);
	glBufferData(GL_ARRAY_BUFFER, length * stride, data, data ? GL_STATIC_DRAW : GL_STREAM_DRAW);

	GLuint glVertexArray;
	glGenVertexArrays(1, &glVertexArray);
	glBindVertexArray(glVertexArray);

	int index = 0;
	const char* ptr = nullptr;
	for (auto attrib : format) {
		auto size = channelsPerAttrib(attrib);
		auto type = glAttribTypes[int(attrib)];

		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, int(size), type, false, int(stride), ptr);

		ptr += bytesPerAttrib(attrib);
		++index;
	}
	glBindVertexArray(glNullVertexArray);

	glAssert();

	return new GLVertexBuffer(this, length, format, glVertexArray, glBuffer);
}

IndexBuffer* GLGraphicsDevice::createIndexBuffer(uint length, IndexFormat format, const void* data) {

	glAssert();

	GLuint glBuffer;
	glGenBuffers(1, &glBuffer);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, length * bytesPerIndex(format), data, data ? GL_STATIC_DRAW : GL_STREAM_DRAW);

	glAssert();

	return new GLIndexBuffer(this, length, format, glBuffer);
}

FrameBuffer* GLGraphicsDevice::createFrameBuffer(Texture* colorTexture, Texture* depthTexture) {

	glAssert();

	auto glColorTexture = static_cast<GLTexture*>(colorTexture);
	auto glDepthTexture = static_cast<GLTexture*>(depthTexture);

	GLuint glFramebuffer;
	glGenFramebuffers(1, &glFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, glFramebuffer);
	if (glColorTexture) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, glColorTexture->glTexture, 0);
	}
	if (glDepthTexture) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, glDepthTexture->glTexture, 0);
	}

	auto r = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	const char* err = nullptr;

#define CASE(X)                                                                                                        \
	case X:                                                                                                            \
		err = #X;                                                                                                      \
		break;

	switch (r) {
	case GL_FRAMEBUFFER_COMPLETE:
		break;
		CASE(GL_FRAMEBUFFER_UNDEFINED);
		CASE(GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT);
		CASE(GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT);
		CASE(GL_FRAMEBUFFER_UNSUPPORTED);
		CASE(GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE);
#ifndef USE_OPENGLES
		CASE(GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER);
		CASE(GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER);
		CASE(GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS);
#endif
	default:
		err = "UNKNOWN ERROR";
		break;
	}

	if (err) panic(debug() << "glCheckFramebufferStatus failed:" << err);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glAssert();

	return new GLFrameBuffer(this, colorTexture, depthTexture, glFramebuffer);
}

Shader* GLGraphicsDevice::createShader(CString shaderSrc) {

	glAssert();

	String header;
#ifdef OS_EMSCRIPTEN
	header = "#version 300 es\n";
#else
	header = "#version 450\n";
#endif
	header += "precision mediump float;\n";
	header += "precision highp int;\n";

	String source = header + shaderSrc;

	auto createShader = [](GLenum type, const char* source) -> GLuint {
		while (*source && *source <= ' ') ++source;

		GLuint shader = glCreateShader(type);
		if (!shader) panic("glCreateShader failed");

		glShaderSource(shader, 1, &source, nullptr);
		glCompileShader(shader);

		// Check compile status
		GLint status = GL_FALSE;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
		if (status != GL_TRUE) {
			GLint length = 1;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
			char* log = new char[length];
			glGetShaderInfoLog(shader, length, nullptr, log);
			log[length - 1] = 0;
			debug() << "Compile shader failed:";
			auto lines = splitString(source, "\n");
			for (size_t i = 0; i < lines.size(); ++i) { debug() << i + 1 << " : " << lines[i]; }
			debug() << log;
			delete[] log;
			panic("OOPS");
		}

		return shader;
	};

	size_t i0 = source.find("\n//@vertex");
	verify(i0 != String::npos);

	size_t i1 = source.find("\n//@fragment", i0);
	verify(i1 != String::npos);

	String vsource = source.substr(0, i1);
	String fsource = source.substr(0, i0) + source.substr(i1);

	GLuint vshader = createShader(GL_VERTEX_SHADER, vsource.c_str());
	GLuint fshader = createShader(GL_FRAGMENT_SHADER, fsource.c_str());

	GLuint program = glCreateProgram();
	if (!program) panic("glCreateProgram failed");

	glAttachShader(program, vshader);
	glAttachShader(program, fshader);

	glLinkProgram(program);

	glDeleteShader(vshader);
	glDeleteShader(fshader);

	// Check link status
	GLint status = GL_FALSE;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status != GL_TRUE) {
		GLint length = 10;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
		char* log = new char[length];
		glGetProgramInfoLog(program, length, nullptr, log);
		log[length - 1] = 0;
		debug() << "Link shader program failed:" << length;
		debug() << log;
		panic("OOPS");
	}

	glUseProgram(program);

	int n;
	glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCKS, &n);
	Vector<uint> uniformBlocks;
	for (int i = 0; i < n; ++i) {

		char name[256] = "";
		glGetActiveUniformBlockName(program, i, sizeof(name), nullptr, name);
		GLuint glIndex = glGetUniformBlockIndex(program, name);
		uint id = uniformBufferIds.get(name);

		glUniformBlockBinding(program, glIndex, id);
		uniformBlocks.push_back(id);
	}

	glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &n);
	Vector<GLUniform> uniforms;
	Vector<uint> textures;
	for (int i = 0; i < n; ++i) {

		int blk = 0;
		glGetActiveUniformsiv(program, 1, (GLuint*)&i, GL_UNIFORM_BLOCK_INDEX, &blk);
		if (blk != -1) continue;

		GLint glSize = 0;
		GLenum glType = 0;
		char name[256] = {};
		glGetActiveUniform(program, i, sizeof(name), nullptr, &glSize, &glType, name);
		GLint glLocation = glGetUniformLocation(program, name);

		switch (glType) {
		case GL_FLOAT:
		case GL_FLOAT_VEC2:
		case GL_FLOAT_VEC3:
		case GL_FLOAT_VEC4:
			uniforms.push_back(GLUniform{name, uniformIds.get(name), glSize, glType, glLocation});
			break;
		case GL_SAMPLER_2D:
		case GL_SAMPLER_3D:
		case GL_SAMPLER_CUBE:
			glUniform1i(glLocation, GLint(textures.size()));
			textures.push_back(textureIds.get(name));
			break;
		default:
			panic("OOPS");
		}
	}

	glUseProgram(0);

	glAssert();

	return new GLShader(this, source, program, uniformBlocks, uniforms, textures);
}

GraphicsContext* GLGraphicsDevice::createGraphicsContext() {

	glAssert();

	return new GLGraphicsContext(this);
}

} // namespace sgf
