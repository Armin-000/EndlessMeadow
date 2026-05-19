#include "../include/water.hpp"

const char* waterVertexShader = R"(
#version 330

in vec3 vertexPosition;
in vec2 vertexTexCoord;

uniform mat4 mvp;
uniform float time;

out vec2 fragTexCoord;
out float waveHeight;
out float foamMask;

void main()
{
    vec3 pos = vertexPosition;

    float wave1 = sin(pos.x * 0.020 + time * 0.28) * 0.10;
    float wave2 = cos(pos.z * 0.026 + time * 0.22) * 0.08;
    float wave3 = sin((pos.x + pos.z) * 0.014 + time * 0.18) * 0.09;

    float wave = wave1 + wave2 + wave3;

    waveHeight = wave;
    foamMask = smoothstep(0.18, 0.34, wave);

    fragTexCoord = vertexTexCoord;

    // Water geometry stays flat.
    gl_Position = mvp * vec4(pos, 1.0);
}
)";

const char* waterFragmentShader = R"(
#version 330

in vec2 fragTexCoord;
in float waveHeight;
in float foamMask;

uniform float time;

out vec4 finalColor;

void main()
{
    vec3 deepWater  = vec3(0.010, 0.075, 0.105);
    vec3 midWater   = vec3(0.020, 0.210, 0.270);
    vec3 brightAqua = vec3(0.120, 0.470, 0.540);
    vec3 foamColor  = vec3(0.760, 0.940, 0.900);

    float waveSoft = smoothstep(-0.28, 0.30, waveHeight);

    float shimmer1 =
        sin((fragTexCoord.x * 115.0 + fragTexCoord.y * 65.0) + time * 0.55) * 0.5 + 0.5;

    float shimmer2 =
        cos((fragTexCoord.x * 45.0 - fragTexCoord.y * 85.0) + time * 0.35) * 0.5 + 0.5;

    float shimmer = shimmer1 * shimmer2;

    float highlight = smoothstep(0.76, 1.0, shimmer) * 0.20;

    float slowRipples =
        sin(fragTexCoord.x * 260.0 + time * 0.45) *
        cos(fragTexCoord.y * 220.0 - time * 0.38);

    slowRipples *= 0.018;

    vec3 color = mix(deepWater, midWater, waveSoft);
    color = mix(color, brightAqua, highlight);
    color += slowRipples;

    color = mix(color, foamColor, foamMask * 0.22);

    float edgeFade =
        smoothstep(0.0, 0.18, fragTexCoord.x) *
        smoothstep(0.0, 0.18, fragTexCoord.y) *
        smoothstep(0.0, 0.18, 1.0 - fragTexCoord.x) *
        smoothstep(0.0, 0.18, 1.0 - fragTexCoord.y);

    color = mix(color * 0.78, color, edgeFade);

    finalColor = vec4(color, 0.94);
}
)";

void DrawWaterAroundPlayer(Model waterModel, Vector3 playerPos)
{
    DrawModel(
        waterModel,
        Vector3{playerPos.x, WATER_LEVEL, playerPos.z},
        1.0f,
        WHITE
    );
}