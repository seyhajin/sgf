//@vertex

const vec2[] cTexCoords = vec2[](
vec2(0.0, 1.0), vec2(1.0, 1.0), vec2(1.0, 0.0),
vec2(0.0, 1.0), vec2(1.0, 0.0), vec2(0.0, 0.0)
);

out vec2 texCoords;

void main(){
    texCoords = cTexCoords[gl_VertexID];
    gl_Position = vec4(texCoords * 2.0 - 1.0, 0.0, 1.0);
}

//@fragment

uniform sampler2D sourceTexture;

in vec2 texCoords;

out vec4 fragColor;

void main(){

    vec4 color = texture(sourceTexture, texCoords);

    // Look into doing this with an sRGB framebuffer. I tried but it looks way different...
    //
    fragColor = vec4(pow(color.rgb, vec3(1.0 / 2.2)), 1.0);
}
