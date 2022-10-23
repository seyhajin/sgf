//@vertex

//@include "camera.glsl"
//@include "terrain.glsl"

const vec4 offs[13] = vec4[13](
vec4(0.0), vec4(-1.0, 0.0, 1.0, 0.0), vec4(0.0),
vec4(0.0, -1.0, 0.0, 1.0), vec4(0.0), vec4(0.0, -1.0, 0.0, 1.0),
vec4(0.0), vec4(-1.0, 0.0, 1.0, 0.0), vec4(0.0),
vec4(-.5, .5f, .5, -.5), vec4(-.5, -.5, .5, .5),
vec4(-.5, -.5, .5, .5), vec4(-.5, .5, .5, -.5)
);

const vec2 lods[] = vec2[](
vec2(0.0, 0.0), vec2(0.0, 1.0),
vec2(1.0, 0.0), vec2(1.0, 1.0),
vec2(2.0, 0.0), vec2(2.0, 0.0), vec2(2.0, 0.0), vec2(2.0, 1.0),
vec2(3.0, 0.0), vec2(3.0, 0.0), vec2(3.0, 0.0), vec2(3.0, 0.0), vec2(3.0, 0.0), vec2(3.0, 0.0), vec2(3.0, 0.0), vec2(3.0, 1.0),
vec2(4.0, 0.0), vec2(4.0, 0.0), vec2(4.0, 0.0), vec2(4.0, 0.0), vec2(4.0, 0.0), vec2(4.0, 0.0), vec2(4.0, 0.0), vec2(4.0, 0.0),
vec2(4.0, 0.0), vec2(4.0, 0.0), vec2(4.0, 0.0), vec2(4.0, 0.0), vec2(4.0, 0.0), vec2(4.0, 0.0), vec2(4.0, 0.0), vec2(4.0, 1.0),
vec2(5.0, 0.0), vec2(5.0, 0.0), vec2(5.0, 0.0), vec2(5.0, 0.0), vec2(5.0, 0.0), vec2(5.0, 0.0), vec2(5.0, 0.0), vec2(5.0, 0.0),
vec2(5.0, 0.0), vec2(5.0, 0.0), vec2(5.0, 0.0), vec2(5.0, 0.0), vec2(5.0, 0.0), vec2(5.0, 0.0), vec2(5.0, 0.0), vec2(5.0, 0.0),
vec2(5.0, 0.0), vec2(5.0, 0.0), vec2(5.0, 0.0), vec2(5.0, 0.0), vec2(5.0, 0.0), vec2(5.0, 0.0), vec2(5.0, 0.0), vec2(5.0, 0.0),
vec2(5.0, 0.0), vec2(5.0, 0.0), vec2(5.0, 0.0), vec2(5.0, 0.0), vec2(5.0, 0.0), vec2(5.0, 0.0), vec2(5.0, 0.0), vec2(5.0, 1.0),
vec2(6.0, 0.0)
);

layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec4 aColor;

out vec3 worldPos;
out vec3 viewPos;
flat out vec4 color;
out float lod;

vec2 negy(vec2 v) {
    return vec2(v.x, -v.y);
}

void main() {

    // ***** Compute worldPos *****

    float tileSize = float(terrain.quadsPerTile);

    vec3 cameraPos = camera.cameraMatrix[3].xyz;

    vec3 modelPos = round(vec3(cameraPos.x, 0.0, cameraPos.z) / tileSize) * tileSize;

    worldPos = modelPos + aPosition.xyz;

    // ***** Compute lodNear, lodFar and tween *****

    vec3 tPos = worldPos - cameraPos;

    float d = max(abs(tPos.x), abs(tPos.z)) / tileSize - 0.5;

    int i = min(int(floor(d)) + 1, 64);

    float lodNear= lods[i].s;
    float tween = lods[i].t * fract(d);

    if (lodNear == aPosition.y + 1.0 && tween == 0.0) {
        lodNear = aPosition.y;
        tween = 1.0;
    } else if (lodNear != aPosition.y) {
        color = vec4(1.0, 0.0, 1.0, 1.0);
        return;
    }
    float lodFar = lodNear + 1.0;

    lod=lodNear + tween;

    // ***** Compute height and update worldPos  *****

    vec4 off = offs[gl_VertexID % 13] * exp2(lodNear);

    vec2 texCoords = worldPos.xz;
    vec2 texScale = terrain.topoTextureScale;

    float heightNear = textureLod(terrainTopoTexture, texCoords * texScale, lodNear).r;
    float height0 = textureLod(terrainTopoTexture, (texCoords + off.xy) * texScale, lodFar).r;
    float height1 = textureLod(terrainTopoTexture, (texCoords + off.zw) * texScale, lodFar).r;

    float heightFar = mix(height0, height1, .5);

    float height = mix(heightNear, heightFar, tween) / 32.0f;

    //    vec2 texCoords = worldPos.xz * terrain.topoTextureScale;
    //    float height = textureLod(terrainTopoTexture, texCoords, 0.0).r / 64.0f;
    //    height = 0.0;

    worldPos.y = height;

    // ***** Compute viewPos *****

    viewPos = (camera.viewMatrix * vec4(worldPos, 1.0)).xyz;

    color = vec4(0.0);

    gl_Position = camera.projMatrix * vec4(viewPos, 1.0);
}

//@fragment

//@include "camera.glsl"
//@include "terrain.glsl"

in vec3 worldPos;
in vec3 viewPos;
flat in vec4 color;
in float lod;

out vec4 fragColor;

void main() {

    //vec2 v = abs(worldPos.xz - camera.cameraMatrix[3].xz);
    //float d=max(v.x, v.y);

    float d = length(worldPos.xz - camera.cameraMatrix[3].xz);

    if (d >= 512.0 * float(terrain.tileMapSize / 2u - 1u)){
        discard;
        return;
    }

    vec2 tc = worldPos.xz * terrain.aerialTextureScale;
    vec2 fc = floor(tc);
    vec2 ic = mod(fc, float(terrain.tileMapSize));
    float layer = ic.y * float(terrain.tileMapSize) + ic.x;

    fragColor = textureLod(terrainAerialTexture, vec3(tc-fc, layer), lod);

    fragColor = vec4(mix(fragColor.rgb, color.rgb, color.a), 1.0);
}
