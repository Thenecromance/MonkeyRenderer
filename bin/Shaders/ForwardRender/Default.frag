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
// so how to merge these part into the Light.hpp and also don't need to modified anything....
// maybe SIZE_OF() can working on this?
layout ( binding = 3) restrict readonly buffer PointLights {
    PointLight in_PointLights[];
};

layout ( binding = 4) restrict readonly buffer DirectionalLights {
    DirectionalLight in_DirectionalLights[];
};
layout (binding = 5) restrict readonly buffer SpotLights {
    SpotLight in_SpotLights[];
};

layout (location =0 ) in PerVertex vtx;

layout (location = 0) out vec4 out_FragColor;

layout (binding = 0) uniform sampler2D texture0;





vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos,
                         vec3 viewDir);

void main()
{
    // vec4 color = texture(texture0, vtx.uv);
    // out_FragColor = color;
    vec3 normal = normalize(vtx.normal);
    vec3 viewDir = normalize(cameraPos.xyz - vtx.worldPos);


    for(int i =0 ; i< point_light_count; i++)
    {
        out_FragColor.rgb += CalculatePointLight(in_PointLights[i], normal, vtx.worldPos, viewDir);
    }
};

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos,
                         vec3 viewDir) {
  vec3 lightDir = normalize(light.position - fragPos);
  // diffuse shading
  float diff = max(dot(normal, lightDir), 0.0);
  // specular shading
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
  // attenuation
  float distance = length(light.position - fragPos);
  float attenuation =
      1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance));
  // combine results
  vec3 ambient = vec3(0.2f) * light.color * vec3(texture(texture0,vtx.uv)) ;
  vec3 diffuse = diff * light.color* vec3(texture(texture0,vtx.uv));
  vec3 specular = spec * light.color* vec3(texture(texture0,vtx.uv));
  ambient *= attenuation;
  diffuse *= attenuation;
  specular *= attenuation;
  return (ambient + diffuse + specular);
}

