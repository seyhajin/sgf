//@vertex

//@include "camera.glsl"

// geometry data
layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec4 aTangent;
layout(location = 3) in vec2 aTexCoords0;
layout(location = 4) in vec2 aTexCoords1;
layout(location = 5) in vec4 aColor;
layout(location = 6) in vec4 aMorph;

// instance data
layout(location = 8) in mat4 iMatrix;
layout(location = 12) in vec4 iColor;
layout(location = 13) in float iMorph;

out vec3 viewPos;
out vec3 viewNormal;
out vec2 texCoords0;
out vec4 modelColor;

void main() {

    vec4 worldPos = iMatrix * aPosition;
    vec3 worldNormal = mat3(iMatrix) * aNormal;

    viewPos = (camera.viewMatrix * worldPos).xyz;
    viewNormal = mat3(camera.viewMatrix) * worldNormal;

    texCoords0 = aTexCoords0;
    modelColor = iColor * aColor;

    gl_Position = camera.projMatrix * vec4(viewPos, 1.0);
}

//@fragment

//@include "scene.glsl"
//@include "material.glsl"

in vec3 viewPos;
in vec3 viewNormal;
in vec2 texCoords0;
in vec4 modelColor;

out vec4 fragColor;

// TODO: Move me to material...
vec4 evalLighting(vec3 viewPos, vec3 viewNormal, vec2 texCoords, vec4 color) {

    vec4 baseColor = texture(baseColorTexture, texCoords) * material.baseColorFactor;

    vec3 diffuse = evalDiffuseLighting(viewPos, viewNormal) * baseColor.rgb * color.rgb;

    vec3 emissive = texture(emissiveTexture, texCoords).rgb * material.emissiveFactor;

    return vec4(diffuse + emissive, baseColor.a);
}

void main() {

    fragColor = evalLighting(viewPos, viewNormal, texCoords0, modelColor);
}
