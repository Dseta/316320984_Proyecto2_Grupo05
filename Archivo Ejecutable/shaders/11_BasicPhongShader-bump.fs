#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;
in mat3 TBN; // Recibimos la matriz

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;

uniform vec4 LightColor;
uniform vec4 LightPower;
uniform int  alphaIndex;
uniform float distance;

uniform vec4 MaterialAmbientColor;
uniform vec4 MaterialDiffuseColor;
uniform vec4 MaterialSpecularColor;
uniform float transparency;

void main()
{    
    // 1. Obtener el color del mapa de normales
    vec3 normTex = texture(texture_normal1, TexCoords).xyz;

    normTex.xy *= 4.0;
    
    // 2. Desempaquetar de [0, 1] a [-1, 1]
    normTex = normTex * 2.0 - 1.0; 
    
    // 3. Traducir al espacio correcto usando la matriz TBN
    vec3 n = normalize(TBN * normTex);
    
    vec3 l = normalize( LightDirection_cameraspace );
    vec3 E = normalize(EyeDirection_cameraspace);

    // Cálculo de componente ambiental
    vec4 K_a = MaterialAmbientColor * LightColor;

    // Cálculo de la componente difusa
    float cosTheta = clamp( dot( n,l ), 0.0, 1.0 );
    vec4 K_d = MaterialDiffuseColor * LightColor * cosTheta;

    // Cálculo de componente especular
    vec3 R = reflect(-l,n);
    float cosAlpha = clamp( dot(E,R), 0.0, 1.0 );
    vec4 K_s = MaterialSpecularColor * LightColor * pow(cosAlpha, alphaIndex);

    // Cálculo de iluminación
    // Nota: Agregué un +1.0 a la distancia para evitar que la luz explote si te acercas mucho
    float distSq = (distance * distance) + 1.0; 
    
    vec4 ex_color = K_a  * LightPower / distSq +
                    K_d  * LightPower / distSq +
                    K_s  * LightPower / distSq;
                    
    ex_color.a = transparency;

    vec4 texel = texture(texture_diffuse1, TexCoords);

    FragColor = texel * ex_color;
}