#ifndef FIRE_RENDERER_H
#define FIRE_RENDERER_H

#include "Vec.hh"

using namespace std;

struct Particle
{
    vec3 position;
    vec3 velocity;
    float life;
    float maxLife;
    vec4 color;
    float size;
};

class FireRenderer
{
public:
    FireRenderer();
    ~FireRenderer();

    bool initialize();
    void render(float timeSeconds) const;
    void cleanup();

private:
    unsigned int m_vao;
    unsigned int m_vbo;
    unsigned int m_program;
    int m_particleCount;

    unsigned int compileShader(unsigned int type, const char* source) const;
    unsigned int createProgram(const char* vertexSource,
                               const char* fragmentSource) const;
    string loadShader(const string path) const;
    Particle genParticle(const vec3 origin) const;
    float random(float maxVal) const;
};

#endif
