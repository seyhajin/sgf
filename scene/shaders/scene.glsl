#include "light.glsl"
#include "camera.glsl"

struct SceneParams {
    vec4 clearColor;
    vec4 ambientLightColor;
    vec4 directionalLightVector;
    vec4 directionalLightColor;
    int numLights;
    LightParams lights[maxLights];
    float renderTime;
    int debugFlags;
};

layout (std140) uniform sceneParams {
    SceneParams scene;
};

vec3 evalDiffuseLighting(vec3 viewPos, vec3 viewNormal) {

    // ambient diffuse
    vec3 diffuse = scene.ambientLightColor.rgb;

    // plus directional diffuse
    diffuse += scene.directionalLightColor.rgb * max(dot(viewNormal, camera.directionalLightVector.xyz), 0.0);

    for (int i = 0; i < scene.numLights; ++i) {

        vec3 lv = camera.lightPositions[i].xyz - viewPos;
        float ld = length(lv);
        lv /= ld;

        float r = scene.lights[i].radius;
        float d = max(ld - r, 0.0);

        float denom = d / r + 1.0;
        float atten = scene.lights[i].color.a / (denom * denom);

        // plus per light diffuse
        diffuse += scene.lights[i].color.rgb * max(dot(viewNormal, lv), 0.0) * atten;
    }

    return diffuse;
}

