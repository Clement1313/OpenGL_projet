#ifndef FIRE_RENDERER_H
#define FIRE_RENDERER_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include "Vec.hh"

using namespace std;

struct Particle {
    vec3 position;
    vec3 velocity;
    float life;
    float maxLife;
    vec4 color;
    float size;
};

class FireRenderer {
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

    unsigned int compileShader(unsigned int type, const char* source) const;
    unsigned int createProgram(const char* vertexSource, const char* fragmentSource) const;
    string loadShader(const string path) const;
};

#endif
