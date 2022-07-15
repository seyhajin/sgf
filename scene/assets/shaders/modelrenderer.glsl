#define FLAT

//@vertex

#include "camera.glsl"

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
FLAT out vec3 viewNormal;
out vec2 texCoords0;
out vec4 color;

void main(){

    vec4 worldPos = iMatrix * aPosition;
    vec4 worldNormal = iMatrix * vec4(aNormal, 0.0);

    viewPos = (camera.viewMatrix * worldPos).xyz;
    viewNormal = (camera.viewMatrix * worldNormal).xyz;

    texCoords0 = aTexCoords0;
    color = iColor * aColor;

    gl_Position = camera.projMatrix * vec4(viewPos, 1.0);
}

//@fragment

in vec3 viewPos;
FLAT in vec3 viewNormal;
in vec2 texCoords0;
in vec4 color;

out vec4 fragColor;

void main(){

    fragColor = color * -viewNormal.z;
}
