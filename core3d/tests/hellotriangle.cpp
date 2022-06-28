
#include "glwindow/glwindow.hh"

#include <core3d/core3d.hh>

using namespace sgf;

constexpr auto shaderSource = R"(

//@vertex

layout(location=0) in vec2 aPosition;
layout(location=1) in vec4 aColor;

out vec4 color;

void main() {

	gl_Position = vec4(aPosition, 0.0, 1.0);
	color=aColor;
}

//@fragment

in vec4 color;

struct ShaderParams {
	vec4 clearColor;
};

layout (std140) uniform shaderParams {
	ShaderParams params;
};

out vec4 fragColor;

void main() {

	fragColor = color * params.clearColor;
}

)";

struct ShaderParams {
	Vec4f clearColor;
};

struct Vertex {
	Vec2f position;
	Vec4f color;
};

VertexFormat vertexFormat{AttribFormat::float2, AttribFormat::float4};

int main() {

	assert(bytesPerVertex(vertexFormat) == sizeof(Vertex));

	uint width = 1280;
	uint height = 720;

	auto window = new GLWindow("Skirmish 2022!", width, height);

#ifndef USE_OPENGLES
	auto debugFunc = [](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message,
						const void* userParam) { debug() << "OpenGL Debug:" << message; };
	glDebugMessageCallback(debugFunc, nullptr);
	glEnable(GL_DEBUG_OUTPUT);
#endif

	auto device = new GLGraphicsDevice();

	auto context = device->createGraphicsContext();

	Vertex vertices[] = {{{0, 1}, {1, 0, 0, 1}}, {{1, -1}, {0, 1, 0, 1}}, {{-1, -1}, {0, 0, 1, 1}}};
	auto vbuffer = device->createVertexBuffer(3, {AttribFormat::float2, AttribFormat::float4}, vertices);

	ShaderParams shaderParams{1};
	auto ubuffer = device->createUniformBuffer(sizeof(shaderParams), &shaderParams);

	auto shader = device->createShader(shaderSource);

	context->setVertexBuffer(vbuffer);
	context->setUniformBuffer("shaderParams", ubuffer);
	context->setSimpleUniform("color", Any(Vec4f(1, .5f, .25, 1)));
	context->setShader(shader);

	window->run([device, context, shader] {
		context->clear(Vec4f(0, 0, 1, 1));

		context->drawGeometry(3, 0, 3, 1);
	});
}
