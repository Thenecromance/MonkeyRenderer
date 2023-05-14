//
#version 460 core
#include <CommonHeader.h>

layout (std140, binding = 0) uniform PerFrameData
{
    mat4 view;
    mat4 proj;
    vec4 cameraPos;
};


layout (location = 0) in PerVertex vtx;

layout (location = 0) out vec4 out_FragColor;

layout (binding = 0) uniform sampler2D texture0;

void main()
{
    vec4 color = texture(texture0, vtx.uv);
    out_FragColor = color;
};
