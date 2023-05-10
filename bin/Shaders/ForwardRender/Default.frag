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

    vec3 n = normalize(vtx.normal);
    vec3 v = normalize(cameraPos.xyz - vtx.worldPos);
    vec3 reflection = -normalize(reflect(v, n));

    float eta = 1.00 / 1.31; // ice
    vec3 refraction = -normalize(refract(v, n, eta));

    const float R0 = ((1.0 - eta) * (1.0 - eta)) / ((1.0 + eta) * (1.0 + eta));
    const float Rtheta = R0 + (1.0 - R0) * pow((1.0 - dot(-v, n)), 5.0);

    vec4 color = texture(texture0, vtx.uv);
//    out_FragColor = color;
    out_FragColor= vec4(1.0);
};
