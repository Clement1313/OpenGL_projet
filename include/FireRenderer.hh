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
    void setCameraMatrices(const float* viewMatrix, const float* projectionMatrix);
    void render(float timeSeconds) const;
    void cleanup();
    void setOrigin(vec3 origin) {
        m_origin = origin;
    }
    void setRayonBase(float rayonBase) {
        m_rayonBase = rayonBase;
    }
private:
    vec3 m_origin;
    unsigned int m_vao;
    unsigned int m_vbo;
    unsigned int m_program;
    int m_particleCount;
    float m_viewMatrix[16];
    float m_projectionMatrix[16];
    float m_rayonBase = 0.0f;

    unsigned int compileShader(unsigned int type, const char* source) const;
    unsigned int createProgram(const char* vertexSource,
                               const char* fragmentSource) const;
    string loadShader(const string path) const;
    Particle genParticle(const vec3 origin) const;
    float random(float maxVal) const;
};

#endif
