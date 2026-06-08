#version 430 core

uniform mat4 uView;
uniform mat4 uProjection;
uniform mat4 uModel;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

out vec3 vertexPosition;
out vec3 vertexNormal;
out vec2 vertexUV;

void main () {

        vec4 worldPosition = uModel * vec4(position, 1.0);
        vertexPosition = worldPosition.xyz;
        vertexNormal = mat3(transpose(inverse(uModel))) * normal;
        vertexUV = uv;
        gl_Position = uProjection * uView * worldPosition;

}
