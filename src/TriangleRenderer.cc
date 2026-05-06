#include "TriangleRenderer.hh"

#include <GL/glew.h>
#include <iostream>

TriangleRenderer::TriangleRenderer()
    : m_vao(0)
    , m_vbo(0)
    , m_program(0)
{}

TriangleRenderer::~TriangleRenderer()
{
    cleanup();
}

bool TriangleRenderer::initialize()
{
    static const char* vertexShaderSource =
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main() {\n"
        "    gl_Position = vec4(aPos, 1.0);\n"
        "}\n";

    static const char* fragmentShaderSource =
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main() {\n"
        "    FragColor = vec4(0.95, 0.40, 0.20, 1.0);\n"
        "}\n";

    m_program = createProgram(vertexShaderSource, fragmentShaderSource);
    if (m_program == 0)
    {
        return false;
    }

    const float vertices[] = { 0.0f, 0.6f, 0.0f,  -0.6f, -0.6f,
                               0.0f, 0.6f, -0.6f, 0.0f };

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          static_cast<void*>(0));
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return true;
}

void TriangleRenderer::render() const
{
    glUseProgram(m_program);
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
    glUseProgram(0);
}

void TriangleRenderer::cleanup()
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

unsigned int TriangleRenderer::compileShader(unsigned int type,
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

unsigned int TriangleRenderer::createProgram(const char* vertexSource,
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
