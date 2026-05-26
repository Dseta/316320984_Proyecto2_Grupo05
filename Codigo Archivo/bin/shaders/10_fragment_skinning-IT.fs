#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 ex_N; 
in vec3 EyeDirection_cameraspace;

uniform sampler2D texture_diffuse1;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// NUEVAS VARIABLES PARA CONECTAR EL SOL
uniform vec3 lightPos;
uniform vec3 lightColor;

void main()
{    
    // 1. Componente Ambiental (Se debilita con la luz del sol)
    // Multiplicamos por 0.3 para que las sombras no sean brillantes
    vec4 MaterialAmbientColor = vec4(lightColor * 0.3, 1.0);

    // Dirección de la luz basada en el sol de tu C++
    vec3 LightPosition_cameraspace = ( view * vec4(lightPos, 1.0f)).xyz;
    vec3 LightDirection_cameraspace = LightPosition_cameraspace + EyeDirection_cameraspace;
    vec3 l = normalize( LightDirection_cameraspace );

    // 2. Componente Difusa (Usa el color que envía el C++)
    float intensity = clamp(dot(ex_N, l), 0.0, 1.0);
    vec4 MaterialDiffuseColor = intensity * vec4(lightColor, 1.0);

    // 3. Componente Especular (Los reflejos metálicos también bajan de noche)
    vec4 LightPower = vec4(lightColor, 1.0f);
    
    vec3 Normal_cameraspace = ( view * model * vec4(ex_N,0)).xyz;
    vec3 n = normalize( Normal_cameraspace );
    
    vec3 R = reflect(-l,n);
    vec3 E = normalize(EyeDirection_cameraspace);
    
    float cosAlpha = clamp( dot( E,R ), 0.0, 1.0 );
    vec4 MaterialSpecularColor =  vec4(1.0, 1.0, 1.0, 1.0) * LightPower * pow(cosAlpha, 5.0);
    
    vec4 texel = texture(texture_diffuse1, TexCoords);

    // Luz base mínima: Para que en la noche absoluta se vean un poquito (luz de estrellas/Tierra)
    vec4 luzBaseNocturna = vec4(0.02, 0.02, 0.03, 1.0) * texel;

    FragColor = texel * (MaterialAmbientColor + MaterialDiffuseColor + MaterialSpecularColor) + luzBaseNocturna;
}