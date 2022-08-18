
#include <window/window.hh>

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

//clang-format off
VertexLayout vertexLayout{
	{{AttribFormat::float2, 0, 0, 0, sizeof(Vertex)}, {AttribFormat::float4, 0, 1, 8, sizeof(Vertex)}}};
//clang-format on

int main() {

	uint width = 1280;
	uint height = 720;

	auto window = createWindow("Skirmish 2022!", width, height);

	auto device = createGraphicsDevice(window);

	auto context = device->createGraphicsContext();

	Vertex vertices[] = {{{0, 1}, {1, 0, 0, 1}}, {{1, -1}, {0, 1, 0, 1}}, {{-1, -1}, {0, 0, 1, 1}}};
	auto vbuffer = device->createGraphicsBuffer(BufferType::vertex, sizeof(vertices), vertices);

	auto vstate = device->createVertexState({vbuffer}, nullptr, vertexLayout);

	ShaderParams shaderParams{1};
	auto ubuffer = device->createGraphicsBuffer(BufferType::uniform, sizeof(shaderParams), &shaderParams);

	auto shader = device->createShader(shaderSource);

	context->setVertexState(vstate);
	context->setUniformBuffer("shaderParams", ubuffer);
	context->setUniform("color", Any(Vec4f(1, .5f, .25, 1)));
	context->setShader(shader);

	window->run([device, context, shader] {
		context->clear(Vec4f(0, 0, 1, 1));

		context->drawGeometry(3, 0, 3, 1);
	});
}
