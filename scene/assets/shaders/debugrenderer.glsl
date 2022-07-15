#include "std.glsl"

//@vertex

#include "camera.glsl"

// geometry data
layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords0;
layout(location = 3) in vec4 aColor;

out vec3 viewPos;
out vec3 viewNormal;
out vec4 color;

void main(){
    gl_Position = camera.viewProjMatrix * aPosition;

    viewPos = (camera.viewMatrix * aPosition).xyz;
    viewNormal = mat3(camera.viewMatrix) * aNormal;

    color = aColor;
}

//@fragment

#include "scene.glsl"

in vec3 viewPos;
in vec3 viewNormal;
in vec4 color;

out vec4 fragColor;

void main(){

    vec3 lighting = evalDiffuseLighting(viewPos, viewNormal);

    fragColor = vec4(lighting * color.rgb, color.a);
}
