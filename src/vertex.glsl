#version 430 core

uniform float uTime;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 velocity;
layout (location = 2) in float lifetime;
layout (location = 3) in float maxlife;
layout (location = 4) in vec4 color;
layout (location = 5) in float size;

out vec4 vertexColor;
out float vertexSize;
out float vertexLife;
out float vertexMaxLife;

void main() {
    float age = mod(uTime + lifetime, max(maxlife, 1e-4));
    float lifeRatio = 1.0 - (age / max(maxlife, 1e-4));
    vec3 newPosition = position + velocity * age;
    gl_Position = vec4(newPosition, 1.0);
    gl_PointSize = max(1.0, size * 80.0);
    vertexColor = color;
    vertexSize = size;
    vertexLife = lifeRatio;
    vertexMaxLife = 1.0;
}