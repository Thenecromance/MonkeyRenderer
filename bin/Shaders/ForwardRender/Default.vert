//
#version 460 core
#include <CommonHeader.h>
#include <Light.hpp>

layout (std140, binding = 0) uniform PerFrameData
{
    mat4 view;
    mat4 proj;
    vec4 cameraPos;
    int point_light_count;
    int direction_light_count;
    int spot_light_count;
};

layout (std430, binding = 1) restrict readonly buffer Vertices
{
    Vertex in_Vertices[];
};

layout (std430, binding = 2) restrict readonly buffer Matrices
{
    mat4 in_Model[];
};


layout (location = 0) out PerVertex vtx;



vec3 getPosition(int i) {
    return vec3(in_Vertices[i].pos[0], in_Vertices[i].pos[1],
                in_Vertices[i].pos[2]);
}

vec3 getNormal(int i) {
    return vec3(in_Vertices[i].n[0], in_Vertices[i].n[1], in_Vertices[i].n[2]);
}

vec2 getTexCoord(int i) {
    return vec2(in_Vertices[i].tc[0], in_Vertices[i].tc[1]);
}

void main()
{
    mat4 model = in_Model[gl_InstanceID];
    mat4 MVP = proj * view * model;

    vec3 pos = getPosition(gl_VertexID);
    gl_Position = MVP * vec4(pos, 1.0);

    mat3 normalMatrix = mat3(transpose(inverse(model)));

    vtx.uv = getTexCoord(gl_VertexID);
    vtx.normal = getNormal(gl_VertexID) * normalMatrix;
    vtx.worldPos = (model * vec4(pos, 1.0)).xyz;

}
