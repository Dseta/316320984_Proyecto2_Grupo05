#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform vec3 glowColor; // Color de la lámpara (ej. amarillo o blanco)
uniform float intensity; // Qué tanto brilla (puedes ponerle 2.0 o más)

void main()
{
    vec4 texColor = texture(texture_diffuse1, TexCoords);
    
    // Si la textura es transparente, la descartamos
    if(texColor.a < 0.1) discard;

    // El color final es la textura combinada con el color del foco y su brillo
    FragColor = vec4(texColor.rgb * glowColor * intensity, 1.0);
}