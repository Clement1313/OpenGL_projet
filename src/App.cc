#include "App.hh"

#include <GL/freeglut.h>
#include <GL/glew.h>
#include <iostream>

App* App::s_instance = nullptr;

App::App(int argc, char** argv)
    : m_argc(argc)
    , m_argv(argv)
    , m_windowId(0)
{
    s_instance = this;
}

bool App::initialize()
{
    glutInit(&m_argc, m_argv);
    glutInitContextVersion(3, 3);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(800, 600);

    m_windowId = glutCreateWindow("OpenGL Triangle");
    if (m_windowId <= 0)
    {
        std::cerr << "Failed to create GLUT window." << std::endl;
        return false;
    }

    glewExperimental = GL_TRUE;
    const GLenum glewStatus = glewInit();
    if (glewStatus != GLEW_OK)
    {
        std::cerr << "GLEW init failed: " << glewGetErrorString(glewStatus)
                  << std::endl;
        return false;
    }

    glViewport(0, 0, 800, 600);

    if (!m_renderer.initialize())
    {
        std::cerr << "Renderer initialization failed." << std::endl;
        return false;
    }

    glutDisplayFunc(displayCallback);
    glutReshapeFunc(reshapeCallback);
    glutCloseFunc(closeCallback);

    return true;
}

void App::run()
{
    glutMainLoop();
}

void App::displayCallback()
{
    if (s_instance != nullptr)
    {
        s_instance->display();
    }
}

void App::reshapeCallback(int width, int height)
{
    if (s_instance != nullptr)
    {
        s_instance->reshape(width, height);
    }
}

void App::closeCallback()
{
    if (s_instance != nullptr)
    {
        s_instance->cleanup();
    }
}

void App::display()
{
    glClearColor(0.08f, 0.10f, 0.12f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    m_renderer.render();

    glutSwapBuffers();
    glutPostRedisplay();
}

void App::reshape(int width, int height) const
{
    glViewport(0, 0, width, height);
}

void App::cleanup()
{
    m_renderer.cleanup();
}
