//@include "light.glsl"

struct CameraParams {
    mat4 projMatrix;
    mat4 invProjMatrix;
    mat4 cameraMatrix;
    mat4 viewMatrix;
    mat4 viewProjMatrix;
    vec4 directionalLightVector;
    vec4 lightPositions[maxLights];
    float clipNear;
    float clipFar;
};

layout (std140) uniform cameraParams {
    CameraParams camera;
};

// Convert depth from texture to view space coords.
float depthToViewZ(float depth){

    //return camera.clipNear * camera.clipFar / ((camera.clipNear - camera.clipFar) * depth + camera.clipFar);

    //return Projection._43 / (zw - Projection._33);
    return camera.projMatrix[3][2] / (depth - camera.projMatrix[2][2]);
}
