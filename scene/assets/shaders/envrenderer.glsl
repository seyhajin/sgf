//@vertex

const vec2[6] cTexCoords = vec2[6](
    vec2(0.0, 1.0), vec2(1.0, 1.0), vec2(1.0, 0.0),
    vec2(0.0, 1.0), vec2(1.0, 0.0), vec2(0.0, 0.0)
);

out vec2 texCoords;

void main(){
    texCoords = cTexCoords[gl_VertexID];
    gl_Position = vec4(texCoords * 2.0 - 1.0, 0.0, 1.0);
}

//@fragment

//@include "std.glsl"
//@include "scene.glsl"
//@include "camera.glsl"
//@include "env.glsl"

uniform sampler2D sourceTexture;
uniform sampler2D depthTexture;

in vec2 texCoords;

out vec4 fragColor;

void main() {

    float depth = texture(depthTexture, texCoords).r;

    float viewZ = depthToViewZ(depth);

    vec4 tv = camera.invProjMatrix * vec4(texCoords * 2.0 - 1.0, 0.0, 1.0);

    vec3 viewVec = tv.xyz / tv.w;

    vec3 viewPos = viewVec * (viewZ / viewVec.z);

    float alpha = min(length(viewPos) / env.fogRange, 1.0);

    if(alpha >= 1.0) {

        vec3 worldVec = mat3(camera.cameraMatrix) * viewVec;

        float pitch = -atan(worldVec.y, sqrt(worldVec.x * worldVec.x + worldVec.z * worldVec.z));

        float yaw = atan(worldVec.x, worldVec.z);

        vec3 texCoords = vec3(yaw / twopi + 0.5, pitch / pi + 0.5, 0.0);

        fragColor = texture(envSkyTexture, texCoords.st);

    }else{
        vec4 color = texture(sourceTexture, texCoords);

        fragColor = mix(color, env.fogColor, pow(alpha, env.fogPower));
    }
}
