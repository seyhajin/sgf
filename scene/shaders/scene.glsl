#include "light.glsl"
#include "camera.glsl"

struct SceneParams {
    vec4 clearColor;
    vec4 ambientLightColor;
    vec4 directionalLightColor;
    int numLights;
    LightParams lights[maxLights];
    float renderTime;
    int debugFlags;
};

layout (std140) uniform sceneParams {
    SceneParams scene;
};

vec4 applySimpleLighting(vec3 viewPos, vec3 viewNormal, vec4 diffuseColor, vec3 emissiveColor) {

    // ambient diffuse
    vec3 diffuse = scene.ambientLightColor;

    // plus directional diffuse
    diffuse += scene.directionalLightColor * max(dot(viewNormal, camera.directionalLightVector), 0.0);

    for (int i = 0; i < numLights; ++i) {

        vec3 lvec = camera.lightPositions[i] - viewPos;
        float ld = length(lvec);
        lvec /= ld;

        float r = scene.lights[i].radius;
        float d = max(ld - r, 0.0);

        float denom = d / r + 1.0;
        float atten = scene.lights[i].intensity / (denom * denom);

        // plus per light diffuse
        diffuse += scene.lights[i].color * max(dot(viewNormal, lvec), 0.0) * atten;
    }

    return vec4(diffuse * diffuseColor.rgb + emissiveColor, diffuseColor.a);
}
