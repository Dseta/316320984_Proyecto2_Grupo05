#version 330 core
layout (location = 0) in vec3  aPos;
layout (location = 1) in vec3  aNormal;
layout (location = 2) in vec2  aTexCoords;
layout (location = 3) in vec3  tangent;
layout (location = 4) in vec3  bitangent;

out vec2 TexCoords;
out vec3 EyeDirection_cameraspace;
out vec3 LightDirection_cameraspace;
out mat3 TBN; // ¡La matriz traductora!

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 lightPosition;

void main()
{
    vec4 PosL = vec4(aPos, 1.0f);
    gl_Position = projection * view * model * PosL;

    // MULTIPLICADOR DE TILING: 
    // Cambia este 15.0 por un número mayor si quieres las piedras más pequeñas
    TexCoords = aTexCoords * 15.0;  

    vec3 vertexPosition_cameraspace = ( view * model * PosL).xyz;
    EyeDirection_cameraspace = vec3(0,0,0) - vertexPosition_cameraspace;
    
    vec3 LightPosition_cameraspace = ( view * vec4(lightPosition,1)).xyz;
    LightDirection_cameraspace = LightPosition_cameraspace + EyeDirection_cameraspace;

    // CONSTRUCCIÓN DE LA MATRIZ TBN
    vec3 T = normalize(vec3(view * model * vec4(tangent,   0.0)));
    vec3 B = normalize(vec3(view * model * vec4(bitangent, 0.0)));
    vec3 N = normalize(vec3(view * model * vec4(aNormal,   0.0)));
    TBN = mat3(T, B, N);
}