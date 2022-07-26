
#include <core3d/core3d.hh>
#include <corexr/corexr.hh>
#include <glwindow/glwindow.h>

#ifdef OS_EMSCRIPTEN
#include <emscripten.h>
#endif

using namespace sgf;

constexpr auto shaderSource = R"(

//@vertex

struct ShaderParams {
	mat4 mvpMatrix;
};

layout (std140) uniform shaderParams {
	ShaderParams params;
};

layout(location=0) in vec2 aPosition;
layout(location=1) in vec4 aColor;

out vec4 color;

void main() {

	gl_Position = params.mvpMatrix * vec4(aPosition, 0.0, 1.0);
	color=aColor;
}

//@fragment

in vec4 color;

out vec4 fragColor;

void main() {

	fragColor = color;
}

)";

struct ShaderParams {
	Mat4f mvpMatrix;
};

struct Vertex {
	Vec2f position;
	Vec4f color;
};

//clang-format off
//clang-format on

GLWindow* window;
GLGraphicsDevice* device;
SharedPtr<GraphicsContext> gc;
SharedPtr<GraphicsBuffer> vbuffer;
SharedPtr<VertexState> vstate;
SharedPtr<GraphicsBuffer> ubuffer;
SharedPtr<Shader> shader;

void renderFrame(double millis, XRFrame* frame) {

	frame->session->requestFrame(renderFrame);

	auto pose = frame->getViewerPose();
	if (!pose) return;

	window->beginFrame();

	ShaderParams params;

	auto m = AffineMat4f::translation({0, 0, .5f}) * AffineMat4f::scale({.1f,.1f,.1f});

	// Set framebuffer
	gc->setFrameBuffer(frame->session->frameBuffer());

	for (uint eye = 0; eye < 2; ++eye) {

		auto& view = pose->views[eye];

		auto p = view.projectionMatrix;

		auto v = view.transform.inverse();

		params.mvpMatrix = p * v * m;

		ubuffer->updateData(0, sizeof(ShaderParams), &params);

		// Set viewport
		gc->setViewport(view.viewport);

		// Render scene
		gc->clear({0, 0, 0, 1});
		gc->drawGeometry(3, 0, 3, 1);
	}

	window->endFrame();
}

int main() {

	window = new GLWindow("Skirmish 2022!", 640, 480);

	device = new GLGraphicsDevice();
	gc = graphicsDevice()->createGraphicsContext();

	VertexLayout vertexLayout{
		{{AttribFormat::float2, 0, 0, 0, sizeof(Vertex)}, {AttribFormat::float4, 0, 1, 8, sizeof(Vertex)}}};
	Vertex vertices[] = {{{0, 1}, {1, 0, 0, 1}}, {{1, -1}, {0, 1, 0, 1}}, {{-1, -1}, {0, 0, 1, 1}}};
	vbuffer = device->createGraphicsBuffer(BufferType::vertex, sizeof(vertices), vertices);
	vstate = device->createVertexState({vbuffer}, nullptr, vertexLayout);
	ubuffer = device->createGraphicsBuffer(BufferType::uniform, sizeof(ShaderParams), nullptr);
	shader = device->createShader(shaderSource);

	gc->setVertexState(vstate);
	gc->setUniformBuffer("shaderParams", ubuffer);
	gc->setShader(shader);

#ifdef EMSCRIPTEN
	new WebXRSystem();

	xrSystem()->isSessionSupported() | [](bool supported) {
		debug() << ">>> Session supported:" << supported;
		setStartVRButtonEnabled(supported);
	};

	startVRButtonClicked.connect([] {
		debug() << "Requesting session";
		xrSystem()->requestSession() | [](XRSession* session) { //
			debug() << "Session created!";
			session->requestFrame(renderFrame);
		};
	});

	emscripten_exit_with_live_runtime();

#else
	new OpenXRSystem(window);

	debug() << "Requesting session";
	xrSystem()->requestSession() | [](XRSession* session) { //
		debug() << "Session created!";
		session->requestFrame(renderFrame);
	};

	for(;;) waitAppEvents();

#endif

}
