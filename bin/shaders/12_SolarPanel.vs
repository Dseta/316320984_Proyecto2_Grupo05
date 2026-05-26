#version 330 core

layout (location = 0) in vec3  aPos;
layout (location = 1) in vec3  aNormal;
layout (location = 2) in vec2  aTexCoords;
layout (location = 3) in vec3  tangent;
layout (location = 4) in vec3  bitangent;

out vec2 TexCoords;
out vec3 Normal_worldspace;
out vec3 Position_worldspace;
out vec3 EyeDirection_cameraspace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 PosL = vec4(aPos, 1.0f);
    gl_Position = projection * view * model * PosL;

    TexCoords = aTexCoords;

    Position_worldspace = (model * PosL).xyz;
    Normal_worldspace = normalize(mat3(model) * aNormal);

    vec3 vertexPosition_cameraspace = (view * model * PosL).xyz;
    EyeDirection_cameraspace = vec3(0.0) - vertexPosition_cameraspace;
}
