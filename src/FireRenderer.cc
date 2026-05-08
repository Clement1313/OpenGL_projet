#include "FireRenderer.hh"

#include <GL/glew.h>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

FireRenderer::FireRenderer()
    : m_vao(0)
    , m_vbo(0)
    , m_program(0)
    , m_particleCount(0)
{}

FireRenderer::~FireRenderer()
{
    cleanup();
}

bool FireRenderer::initialize()
{
    const string vertexCode = loadShader("src/vertex.glsl");
    static const char* vertexShaderSource = vertexCode.c_str();

    const string fragmentCode = loadShader("src/fragment.glsl");
    static const char* fragmentShaderSource = fragmentCode.c_str();

    m_program = createProgram(vertexShaderSource, fragmentShaderSource);
    if (m_program == 0)
    {
        return false;
    }

    glEnable(GL_BLEND); // Particles transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glEnable(GL_PROGRAM_POINT_SIZE);

    vec3 origin = { 0.00f, 0.00f, 0.00f };
    size_t nbParticles = 10;
    m_particleCount = static_cast<int>(nbParticles);
    Particle particles[nbParticles];
    for (size_t i = 0; i < nbParticles; i++)
    {
        particles[i] = genParticle(origin);
    }

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(particles), particles, GL_STATIC_DRAW);

    const GLsizei stride = static_cast<GLsizei>(sizeof(Particle));

    // Position
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE, stride,
        reinterpret_cast<void*>(offsetof(Particle, position)));
    glEnableVertexAttribArray(0);

    // Velocity
    glVertexAttribPointer(
        1, 3, GL_FLOAT, GL_FALSE, stride,
        reinterpret_cast<void*>(offsetof(Particle, velocity)));
    glEnableVertexAttribArray(1);

    // Life
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, stride,
                          reinterpret_cast<void*>(offsetof(Particle, life)));
    glEnableVertexAttribArray(2);

    // MaxLife
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, stride,
                          reinterpret_cast<void*>(offsetof(Particle, maxLife)));
    glEnableVertexAttribArray(3);

    // Color
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, stride,
                          reinterpret_cast<void*>(offsetof(Particle, color)));
    glEnableVertexAttribArray(4);

    // Size
    glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, stride,
                          reinterpret_cast<void*>(offsetof(Particle, size)));
    glEnableVertexAttribArray(5);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return true;
}

void FireRenderer::render(float timeSeconds) const
{
    glUseProgram(m_program);
    int timeLoc = glGetUniformLocation(m_program, "uTime");
    glUniform1f(timeLoc, timeSeconds);

    glBindVertexArray(m_vao);
    glDrawArrays(GL_POINTS, 0, m_particleCount);
    glBindVertexArray(0);
    glUseProgram(0);
}

void FireRenderer::cleanup()
{
    if (m_vbo != 0)
    {
        glDeleteBuffers(1, &m_vbo);
        m_vbo = 0;
    }

    if (m_vao != 0)
    {
        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }

    if (m_program != 0)
    {
        glDeleteProgram(m_program);
        m_program = 0;
    }
}

unsigned int FireRenderer::createProgram(const char* vertexSource,
                                         const char* fragmentSource) const
{
    const unsigned int vertexShader =
        compileShader(GL_VERTEX_SHADER, vertexSource);
    const unsigned int fragmentShader =
        compileShader(GL_FRAGMENT_SHADER, fragmentSource);

    if (vertexShader == 0 || fragmentShader == 0)
    {
        if (vertexShader != 0)
        {
            glDeleteShader(vertexShader);
        }
        if (fragmentShader != 0)
        {
            glDeleteShader(fragmentShader);
        }
        return 0;
    }

    const unsigned int program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    int success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        char log[512] = { 0 };
        glGetProgramInfoLog(program, 512, nullptr, log);
        std::cerr << "Program linking failed: " << log << std::endl;
        glDeleteProgram(program);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

unsigned int FireRenderer::compileShader(unsigned int type,
                                         const char* source) const
{
    const unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    int success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char log[512] = { 0 };
        glGetShaderInfoLog(shader, 512, nullptr, log);
        std::cerr << "Shader compilation failed: " << log << std::endl;
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

string FireRenderer::loadShader(const string path) const
{
    std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

float FireRenderer::random(float maxVal) const
{
    return static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / maxVal));
}

Particle FireRenderer::genParticle(const vec3 origin) const
{
    vec3 pos = { origin.x + random(0.4f) - 0.2f, origin.y + random(0.05f),
                 origin.z };
    vec3 vel = { 0.f, 0.3f + random(0.2f), 0.f };
    vec4 col = { 1.00f - random(0.3f), random(0.50f), 0.0f + random(0.2f),
                 1.00f };
    Particle res = { pos, vel, random(1.0f), 1.0f, col, 0.2f };
    return res;
}
