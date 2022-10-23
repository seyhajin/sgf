uniform sampler2D envSkyTexture;

struct EnvParams {
    vec4 fogColor;
    float fogRange;
    float fogPower;
};

layout (std140) uniform envParams {
    EnvParams env;
};
