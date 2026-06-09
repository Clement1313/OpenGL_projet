#include "App.hh"

#include "../include/TorchRenderer.hh"
#include "TorchRenderer.hh"
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <iostream>

#include <vector>

#include "../external/imgui/imgui.h"
#include "imgui.h"
#include "imgui_impl_glut.h"
#include "imgui_impl_opengl3.h"
App * App::s_instance = nullptr;

App::App(int argc, char** argv)
    : m_argc(argc)
    , m_argv(argv)
    , m_windowId(0)
    , m_lastFrameTime(0)
    , m_renderer(
        Vector3{0.0f, -1.0f, 0.0f},
        2.0f,0.1f,std::vector<Vector3> {Vector3{0.0f, -1.0f, 0.0f},
       Vector3{0.0f, 1.0f, 0.0f}}, 16)
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


    //UI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGLUT_Init();
    glutMouseFunc(ImGui_ImplGLUT_MouseFunc);
    glutMotionFunc(ImGui_ImplGLUT_MotionFunc);
    ImGui_ImplOpenGL3_Init("#version 430");

    glViewport(0, 0, 800, 600);

    m_controller.initialize(800, 600);
    m_lastFrameTime = glutGet(GLUT_ELAPSED_TIME);

    if (!m_renderer.initialize())
    {
        std::cerr << "Renderer initialization failed." << std::endl;
        return false;
    }

    glutDisplayFunc(displayCallback);
    glutReshapeFunc(reshapeCallback);
    glutCloseFunc(closeCallback);
    glutKeyboardFunc(keyboardDownCallback);
    glutKeyboardUpFunc(keyboardUpCallback);
    glutSpecialFunc(specialDownCallback);
    glutSpecialUpFunc(specialUpCallback);
    glutIdleFunc(idleCallback);

    // ImGui_ImplGLUT_InstallFuncs();
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

void App::keyboardDownCallback(unsigned char key, int x, int y)
{
    if (s_instance != nullptr)
    {
        s_instance->keyboardDown(key, x, y);
    }
}

void App::keyboardUpCallback(unsigned char key, int x, int y)
{
    if (s_instance != nullptr)
    {
        s_instance->keyboardUp(key, x, y);
    }
}

void App::specialDownCallback(int key, int x, int y)
{
    if (s_instance != nullptr)
    {
        s_instance->specialDown(key, x, y);
    }
}

void App::specialUpCallback(int key, int x, int y)
{
    if (s_instance != nullptr)
    {
        s_instance->specialUp(key, x, y);
    }
}

void App::idleCallback()
{
    if (s_instance != nullptr)
    {
        s_instance->idle();
    }
}

void App::display()
{

    const int currentTime = glutGet(GLUT_ELAPSED_TIME);
    const float deltaSeconds = static_cast<float>(currentTime - m_lastFrameTime) / 1000.0f;
    m_lastFrameTime = currentTime;

    m_controller.update(deltaSeconds);
    m_renderer.setCameraMatrices(m_controller.viewMatrix(),
                                 m_controller.projectionMatrix(),m_controller.cameraPosition());

    glClearColor(0.0f, 0.05f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    float timeSeconds = static_cast<float>(glutGet(GLUT_ELAPSED_TIME)) / 1000.0f;


    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGLUT_NewFrame();

    bool changement=false;
    int toErase = -1;
    ImGui::NewFrame();
    ImGui::Begin("Paramètre Torche");
    ImGui::Text("Point de Génération de la Torche");
    ImGui::Separator();
    for (int i = 0; i < m_chemin.size(); i++) {
        ImGui::PushID(i);
        ImGui::Text("Point %d", i);
        ImGui::SameLine();
        changement  = changement ||ImGui::SliderFloat3("Coordonée",&m_chemin[i].x,-15.0f,15.0f);
        ImGui::SameLine();
        if (ImGui::Button("X") && m_chemin.size() > 2) {
            toErase = i;
            changement = true;
        }
        ImGui::PopID();
    }
    if (toErase != -1) {
        m_chemin.erase(m_chemin.begin() + toErase);
    }
    if (ImGui::Button("Ajouter Point")) {
            Vector3 newPoint = {m_chemin.back().x, m_chemin.back().y + 1.f, m_chemin.back().z};
            m_chemin.push_back(newPoint);
            changement = true;
    }

    ImGui::End();
    ImGui::Render();

    if (changement) {
        m_renderer.updateChemin(m_chemin);
        changement = false;
    }

    m_renderer.render(timeSeconds);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glutSwapBuffers();
    glutPostRedisplay();
}

void App::reshape(int width, int height) const
{
    glViewport(0, 0, width, height);
    if (ImGui::GetCurrentContext() != nullptr) {
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2(static_cast<float>(width), static_cast<float>(height));
    }
}

void App::cleanup()
{
    m_renderer.cleanup();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGLUT_Shutdown();
    ImGui::DestroyContext();
}

void App::keyboardDown(unsigned char key, int, int)
{
    m_controller.keyDown(key);
}

void App::keyboardUp(unsigned char key, int, int)
{
    m_controller.keyUp(key);
}

void App::specialDown(int key, int, int)
{
    m_controller.specialDown(key);
}

void App::specialUp(int key, int, int)
{
    m_controller.specialUp(key);
}

void App::idle()
{
    glutPostRedisplay();
}
