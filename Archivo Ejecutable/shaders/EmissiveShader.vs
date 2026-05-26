#version 330 core

// Solo necesitamos la posición (0) y la textura (2)
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;

// Las matrices clásicas
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Multiplicación estándar para ubicar el modelo en el mundo 3D
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    
    // Pasamos las coordenadas de textura al Fragment Shader (.fs)
    TexCoords = aTexCoords;
}