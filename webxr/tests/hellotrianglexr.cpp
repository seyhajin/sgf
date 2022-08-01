
#include <core3d/core3d.hh>
#include <glwindow/glwindow.h>
#include <webxr/webxr.hh>

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

Mat4f viewProjMatrix;

void drawTriangle(CAffineMat4f modelMatrix) {

	ShaderParams params;
	params.mvpMatrix = viewProjMatrix * modelMatrix;

	ubuffer->updateData(0, sizeof(ShaderParams), &params);

	gc->drawGeometry(3, 0, 3, 1);
}

void renderFrame(double millis, XRFrame* frame) {

	frame->session->requestFrame(renderFrame);

	auto viewerPose = frame->getViewerPose();
	if (!viewerPose) return;

	window->beginFrame();

	auto handPoses = frame->getHandPoses();

	auto modelMatrix = AffineMat4f::translation({0, 0, .5f});

	// Set framebuffer
	gc->setFrameBuffer(frame->session->frameBuffer());

	for (uint eye = 0; eye < 2; ++eye) {

		auto& view = viewerPose->views[eye];

		auto p = view.projectionMatrix;

		auto v = view.transform.inverse();

		viewProjMatrix = p * v;

		auto uiMatrix = viewProjMatrix * AffineMat4f::position({0,0,.2f});

		// Set viewport
		gc->setViewport(view.viewport);

		gc->setDepthMode(DepthMode::enable);

		gc->clear({0, 0, 0, 1});

		drawTriangle(modelMatrix);

		for(uint hand = 0;hand<2;++hand) {

			auto handMatrix = handPoses[hand].transform * AffineMat4f::rotation({pi * .5f, pi, 0}) *
							  AffineMat4f::scale({.3f, .3f, .3f});

			drawTriangle(handMatrix);
		}

		gc->setDepthMode(DepthMode::compare);
	}

	window->endFrame();
}

int main() {

	window = new GLWindow("Skirmish 2022!", 640, 480);

	device = new GLGraphicsDevice();
	gc = graphicsDevice()->createGraphicsContext();

	VertexLayout vertexLayout{
		{{AttribFormat::float2, 0, 0, 0, sizeof(Vertex)}, {AttribFormat::float4, 0, 1, 8, sizeof(Vertex)}}};
	float sz=.1f;
	Vertex vertices[] = {{{0, sz}, {1, 0, 0, 1}}, {{sz, -sz}, {0, 1, 0, 1}}, {{-sz, -sz}, {0, 0, 1, 1}}};
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

#else
	new OpenXRSystem(window);

	xrSystem()->isSessionSupported() | [](bool supported) {
		if (supported) {
			debug() << "Requesting session";
			xrSystem()->requestSession() | [](XRSession* session) { //
				debug() << "Session created!";
				session->requestFrame(renderFrame);
			};
		}
	};

#endif

	runAppEventLoop();
}
