#include "std.glsl"

//@vertex

#include "camera.glsl"

// geometry data
layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec4 aColor;

out vec4 color;

void main(){
    gl_Position = camera.viewProjMatrix * aPosition;
    color = aColor;
}

//@fragment

in vec4 color;
out vec4 fragColor;

void main(){
    fragColor = color;
}

