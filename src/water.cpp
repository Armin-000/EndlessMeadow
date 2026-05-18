#include "../include/water.hpp"

const char* waterVertexShader = R"(
#version 330

in vec3 vertexPosition;
in vec2 vertexTexCoord;

uniform mat4 mvp;
uniform float time;

out vec2 fragTexCoord;
out float waveHeight;

void main()
{
    vec3 pos = vertexPosition;

    float wave1 = sin(pos.x * 0.030 + time * 0.85) * 0.18;
    float wave2 = cos(pos.z * 0.042 + time * 0.65) * 0.12;
    float wave3 = sin((pos.x + pos.z) * 0.018 + time * 0.45) * 0.20;

    float wave = wave1 + wave2 + wave3;

    pos.y += wave;

    waveHeight = wave;
    fragTexCoord = vertexTexCoord;

    gl_Position = mvp * vec4(pos, 1.0);
}
)";

const char* waterFragmentShader = R"(
#version 330

in vec2 fragTexCoord;
in float waveHeight;

uniform float time;

out vec4 finalColor;

void main()
{
    vec3 deepWater = vec3(0.015, 0.115, 0.155);
    vec3 midWater = vec3(0.020, 0.250, 0.310);
    vec3 lightWater = vec3(0.200, 0.520, 0.600);

    float shimmer =
        sin((fragTexCoord.x * 95.0 + fragTexCoord.y * 55.0) + time * 1.8) * 0.5 + 0.5;

    float softWave = smoothstep(-0.25, 0.35, waveHeight);
    float highlight = smoothstep(0.82, 1.0, shimmer) * 0.22;

    vec3 color = mix(deepWater, midWater, softWave);
    color = mix(color, lightWater, highlight);

    finalColor = vec4(color, 1.0);
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