#version 430 core

out vec4 FragColor;

in vec4 vertexColor;
in float vertexSize;
in float vertexLife;
in float vertexMaxLife;

void main() {
    vec2 coord = gl_PointCoord - vec2(0.5);
    float dist = length(coord);
    
    if (dist > 0.5) discard;
    
    float t = clamp(vertexLife / max(vertexMaxLife, 1e-6), 0.0, 1.0);
    float alpha = 1.0 - t;
    
    float falloff = smoothstep(0.5, 0.0, dist);
    
    vec3 color = vertexColor.rgb;
    FragColor = vec4(color * (0.6 + 0.8 * falloff), alpha * falloff);
    
    FragColor = vec4(color, alpha);
}