//@vertex

uniform vec2 sourceSize;

const vec2[] cTexCoords = vec2[](
    vec2(0.0, 1.0), vec2(1.0, 1.0), vec2(1.0, 0.0),
    vec2(0.0, 1.0), vec2(1.0, 0.0), vec2(0.0, 0.0)
);

out vec2 texCoords;

//Ha! there...asasasas

void main(){

    gl_Position = vec4(cTexCoords[gl_VertexID] * 2.0 - 1.0, 0.0, 1.0);

    texCoords = cTexCoords[gl_VertexID] * sourceSize;
}

//@fragment

uniform sampler2D sourceTexture;

in vec2 texCoords;

out vec4 fragColor;

void main(){

    vec4 color = texture(sourceTexture, texCoords);

    // NOTE: Should be able to do this via an sRGB framebuffer...
    //
    fragColor = vec4(pow(color.rgb, vec3(1.0 / 2.2)), color.a);
}
