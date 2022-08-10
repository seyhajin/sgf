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

IdMap bufferIds;
IdMap textureIds;
IdMap uniformIds;

GLuint glNullVertexArray;

} // namespace

// ***** GLGraphicsBuffer *****

void GLGraphicsBuffer::updateData(uint loffset, uint lsize, const void* data) {
	assert(loffset + lsize <= this->size);

	glAssert();

	glBindBuffer(glTarget, glBuffer);
	glBufferSubData(glTarget, loffset, lsize, data);

	glAssert();
}

void* GLGraphicsBuffer::lockData(uint loffset, uint lsize) {
	assert(!loffset && size && !m_locked);

	void* p = nullptr;

	glAssert();

#ifndef OS_EMSCRIPTEN
	glBindBuffer(glTarget, glBuffer);
	p = glMapBufferRange(glTarget, loffset, lsize, GL_MAP_WRITE_BIT);
	m_mapped = (p != nullptr);
#endif

	if (!m_mapped) {
		if (lsize > m_lockedData.size()) m_lockedData.resize(lsize);
		p = m_lockedData.data();
	}

	m_lockedOffset = loffset;
	m_lockedSize = lsize;
	m_locked = true;

	glAssert();

	return p;
}

void GLGraphicsBuffer::unlockData() {
	assert(m_locked);

	glAssert();

#ifndef OS_EMSCRIPTEN
	if (m_mapped) glUnmapBuffer(glTarget);
#endif

	if (!m_mapped) updateData(m_lockedOffset, m_lockedSize, m_lockedData.data());

	m_mapped = false;
	m_locked = false;

	glAssert();
}

// ***** GLGraphicsContext *****

void GLGraphicsContext::setUniformBuffer(CString name, GraphicsBuffer* graphicsBuffer) {
	assert(graphicsBuffer->type == BufferType::uniform);
	uint id = bufferIds.get(name);
	if (graphicsBuffer == m_uniformBuffers[id]) return;
	m_uniformBuffers[id] = static_cast<GLGraphicsBuffer*>(graphicsBuffer);
	m_dirty |= Dirty::uniformBlockBindings;
}

void GLGraphicsContext::setTexture(CString name, Texture* texture) {
	uint id = textureIds.get(name);
	if (texture == m_textures[id]) return;
	m_textures[id] = static_cast<GLTexture*>(texture);
	m_dirty |= Dirty::textureBindings;
}

void GLGraphicsContext::setUniform(CString name, CAny any) {
	uint id = uniformIds.get(name);
	if (any == m_uniforms[id]) return;
	m_uniforms[id] = any;
	m_dirty |= Dirty::uniformBindings;
}

void GLGraphicsContext::setVertexState(VertexState* state) {
	if (state == m_vertexState) return;
	m_vertexState = static_cast<GLVertexState*>(state);
	m_dirty |= Dirty::vertexState;
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

	if (bool(m_dirty & Dirty::vertexState)) {
		glBindVertexArray(m_vertexState ? m_vertexState->glVertexArray : glNullVertexArray);
		glAssert();
	}

	if (bool(m_dirty & Dirty::shader)) {
		glUseProgram(m_shader ? m_shader->glProgram : 0);
		m_dirty |= Dirty::uniformBlockBindings | Dirty::textureBindings | Dirty::uniformBindings;
		glAssert();
	}

	if (bool(m_dirty & Dirty::uniformBlockBindings) && m_shader) {
		for (uint id : m_shader->uniformBlocks) {
			auto buffer = m_uniformBuffers[id].value();
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

void GLGraphicsContext::drawIndexedGeometry(uint order, uint firstIndex, uint numIndices, uint numInstances) {

	static constexpr GLenum modes[] = {GL_NONE, GL_POINTS, GL_LINES, GL_TRIANGLES};
	static constexpr GLenum types[] = {GL_NONE, GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, GL_UNSIGNED_INT};
	static constexpr uint size[] = {1, 2, 4};

	assert(order > 0 && order < 4);

	auto indexFormat = m_vertexState->layout.indexFormat;
	assert(indexFormat != IndexFormat::none);

	auto type = types[uint(indexFormat)];
	auto ptr = reinterpret_cast<void*>(static_cast<size_t>(bytesPerIndex(indexFormat)) * firstIndex);

	validate();

	if (numInstances > 1) {
		glDrawElementsInstanced(modes[order], int(numIndices), type, ptr, int(numInstances));
	} else {
		glDrawElements(modes[order], int(numIndices), type, ptr);
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

GLGraphicsDevice::GLGraphicsDevice(GLWindow* window) : GraphicsDevice(window) {

#ifndef USE_OPENGLES
#if 0
auto debugFunc = [](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message,
const void* userParam) { debug() << message; };
glDebugMessageCallback(debugFunc, nullptr);
glEnable(GL_DEBUG_OUTPUT);
#endif
#endif

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

	if (checkGLExtension("GL_EXT_texture_filter_anisotropic")) {
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
	}

	if (bool(flags & TextureFlags::mipmap)) {
		glTexParameteri(glTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(glTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		if (aniso) glTexParameterf(glTarget, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);
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

GraphicsBuffer* GLGraphicsDevice::createGraphicsBuffer(BufferType type, uint size, const void* data) {
	GLenum glTarget;
	switch (type) {
	case BufferType::uniform:
		glTarget = GL_UNIFORM_BUFFER;
		break;
	case BufferType::vertex:
		glTarget = GL_ARRAY_BUFFER;
		break;
	case BufferType::index:
		glTarget = GL_ELEMENT_ARRAY_BUFFER;
		break;
	}
	GLuint glBuffer;
	glGenBuffers(1, &glBuffer);
	glBindBuffer(glTarget, glBuffer);
	glBufferData(glTarget, size, data, data ? GL_STATIC_DRAW : GL_STREAM_DRAW);

	return new GLGraphicsBuffer(this, type, size, glTarget, glBuffer);
}

VertexState* GLGraphicsDevice::createVertexState(CVector<GraphicsBuffer*> vertexBuffers, GraphicsBuffer* indexBuffer,
												 VertexLayout layout) {
	glAssert();

	GLuint glVertexArray;
	glGenVertexArrays(1, &glVertexArray);
	glBindVertexArray(glVertexArray);

	for (auto& attrib : layout.attribLayouts) {

		if (attrib.format == AttribFormat::none) continue;

		auto location = attrib.location;

		auto offset = static_cast<const char*>(nullptr) + attrib.offset;
		auto vbuffer = static_cast<GLGraphicsBuffer*>(vertexBuffers[attrib.buffer]);
		assert(vbuffer->type == BufferType::vertex);

		glEnableVertexAttribArray(location);
		glBindBuffer(GL_ARRAY_BUFFER, vbuffer->glBuffer);
		glVertexAttribPointer(location, int(channelsPerAttrib(attrib.format)), glAttribTypes[uint(attrib.format)],
							  false, attrib.pitch, offset);
		glVertexAttribDivisor(location, attrib.divisor);
	}

	if (indexBuffer) {
		assert(indexBuffer->type == BufferType::index);
		auto ibuffer = static_cast<GLGraphicsBuffer*>(indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuffer->glBuffer);
		constexpr GLenum glTypes[] = {GL_NONE, GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, GL_UNSIGNED_INT};
	}

	Vector<SharedPtr<GraphicsBuffer>> sharedBuffers;
	for (auto vbuffer : vertexBuffers) sharedBuffers.push_back(vbuffer);

	glAssert();

	return new GLVertexState(this, std::move(sharedBuffers), indexBuffer, std::move(layout), glVertexArray);
}

FrameBuffer* GLGraphicsDevice::createFrameBuffer(Texture* colorTexture, Texture* depthTexture) {

	glAssert();

	assert(colorTexture || depthTexture);

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

	if (err) panic(String("glCheckFramebufferStatus failed:") + err);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glAssert();

	uint width = colorTexture ? colorTexture->width : depthTexture->width;
	uint height = colorTexture ? colorTexture->height : depthTexture->height;

	auto fbuffer = new GLFrameBuffer(this, colorTexture, depthTexture, width, height, glFramebuffer);

	fbuffer->deleted.connect([glFramebuffer] { glDeleteFramebuffers(1, &glFramebuffer); });

	return fbuffer;
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
			auto lines = stringSplit(source, "\n");
			for (size_t i = 0; i < lines.size(); ++i) { debug() << i + 1 << " : " << lines[i]; }
			debug() << log;
			delete[] log;
			panic("SHADER ERROR");
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
		uint id = bufferIds.get(name);

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

Texture* GLGraphicsDevice::wrapGLTexture(uint width, uint height, TextureFormat format, TextureFlags flags,
										 GLuint texture) {

	glAssert();

	return new GLTexture(this, width, height, format, flags | TextureFlags::unmanaged, texture);
}

} // namespace sgf
