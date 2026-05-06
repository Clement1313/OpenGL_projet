#ifndef TRIANGLE_RENDERER_H
#define TRIANGLE_RENDERER_H

class TriangleRenderer {
public:
    TriangleRenderer();
    ~TriangleRenderer();

    bool initialize();
    void render() const;
    void cleanup();

private:
    unsigned int m_vao;
    unsigned int m_vbo;
    unsigned int m_program;

    unsigned int compileShader(unsigned int type, const char* source) const;
    unsigned int createProgram(const char* vertexSource, const char* fragmentSource) const;
};

#endif
