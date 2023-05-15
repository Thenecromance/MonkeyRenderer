//
#version 460 core
#include <CommonHeader.h>
#include <Light.hpp>
layout (std140, binding = 0) uniform PerFrameData
{
    mat4 view;
    mat4 proj;
    vec4 cameraPos;
};
layout (std430, binding = 3) restrict readonly buffer PointLights {
    PointLight in_PointLights[];
};

layout (std430, binding = 4) restrict readonly buffer DirectionalLights {
    DirectionalLight in_DirectionalLights[];
};
layout (std430, binding = 5) restrict readonly buffer SpotLights {
    SpotLight in_SpotLights[];
};

layout (location = 0) in PerVertex vtx;

layout (location = 0) out vec4 out_FragColor;

layout (binding = 0) uniform sampler2D texture0;

void main()
{
    vec4 color = texture(texture0, vtx.uv);
    out_FragColor = color;
};
