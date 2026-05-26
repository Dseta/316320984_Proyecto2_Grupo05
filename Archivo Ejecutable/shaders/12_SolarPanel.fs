#version 330 core

in vec2 TexCoords;
in vec3 Normal_worldspace;
in vec3 Position_worldspace;
in vec3 EyeDirection_cameraspace;

out vec4 FragColor;

// Texturas
uniform sampler2D texture_diffuse1;
uniform samplerCube cubetex;

// Cámara y Luces
uniform vec3 cameraPosition;
uniform vec3 lightPos; 
uniform vec3 lightColor;

// Variables de Fresnel 
uniform float _Bias;
uniform float _Scale;
uniform float _Power;

void main()
{
    // 1. TEXTURA BASE
    vec4 panelColor = texture(texture_diffuse1, TexCoords);
    if (panelColor.a < 0.01) {
        discard;
    }
    
    vec3 N = normalize(Normal_worldspace);
    
    // 2. ILUMINACIÓN (Volumen 3D)
    vec3 ambient = 0.3 * panelColor.rgb;
    
    vec3 L = normalize(lightPos - Position_worldspace);
    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = diff * lightColor * panelColor.rgb;

    vec3 baseColor = ambient + diffuse; 

    // 3. CÁLCULO DE REFLEXIÓN FRESNEL
    vec3 V = normalize(cameraPosition - Position_worldspace);
    vec3 R = reflect(-V, N);
    
    float fresnel = _Bias + _Scale * pow(max(1.0 - abs(dot(V, N)), 0.0), _Power);
    
    // 4. MUESTREO DEL ENTORNO
    vec4 reflectedColor = texture(cubetex, R);
    
    // 5. COMBINACIÓN FINAL
    vec3 finalColor = mix(baseColor, reflectedColor.rgb, clamp(fresnel, 0.0, 1.0));
    
    FragColor = vec4(finalColor, 1.0);
}