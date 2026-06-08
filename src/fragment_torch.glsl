#version 330 core

out vec4 FragColor;

in vec3 vertexPosition;
in vec3 vertexNormal;
in vec2 vertexUV;

uniform vec3 uCameraPosition;
uniform vec3 uMaterialColor;
uniform vec3 uLightPosition;
uniform float uLightIntensity;
uniform float uTime; 
void main() 
{
    // utilisation de phong: ambiant + diffuse + specular
    
    // fbm avec des sinus (pour variation des couleurs)   
    // float fbm = sin(uTime)  + 0.5 * sin(uTime * 2) + 0.25 * sin(uTime * 4);
    vec3 lightColor = vec3(1.0, 1.0, 1.0) * (uLightIntensity);
    
    // Attenuation de la lumière selon Ogre Wiki
    float k_linear = 0.07;
    float k_quadratic = 0.015;
    float k_constant = 1.0;
    float distance = length(uLightPosition - vertexPosition);
    float attenuation = 1.0 / (k_constant + k_linear * distance + k_quadratic * (distance * distance));
    
    // ambiant
    float ambiantValue = 0.1;
    vec3 ambiant = ambiantValue * uMaterialColor * attenuation;  
    
    // diffus
    vec3 normal = normalize(vertexNormal);
    vec3 lightDirection = normalize(uLightPosition - vertexPosition);
    float diffuseValue = max(dot(normal, lightDirection), 0.0);
    vec3 diffuse = diffuseValue * ambiantValue * attenuation * uMaterialColor * lightColor;
    
    //speculaire
    vec3 viewDirection = normalize(uCameraPosition - vertexPosition);
    vec3 reflectDirection = reflect(-lightDirection, normal);
    float specular = pow(max(dot(viewDirection, reflectDirection), 0.0),32.0);
    vec3 speculaire = specular * lightColor * attenuation;

    vec3 phong = ambiant + diffuse + speculaire;
    FragColor = vec4(phong, 1.0);
}