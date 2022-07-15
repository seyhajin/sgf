precision mediump float;
precision highp int;// lowp = -255...+255; mediump = -1023...+1023; highp = -65535...+65535

const float pi = 3.14159265359;
const float twopi = pi * 2.0;

const vec2[] cQuadClipCoords = vec2[](
vec2(-1.0, 1.0), vec2(1.0, 1.0), vec2(1.0, -1.0),
vec2(-1.0, 1.0), vec2(1.0, -1.0), vec2(-1.0, -1.0)
);

const vec2[] cQuadTexCoords = vec2[](
vec2(0.0, 1.0), vec2(1.0, 1.0), vec2(1.0, 0.0),
vec2(0.0, 1.0), vec2(1.0, 0.0), vec2(0.0, 0.0)
);
