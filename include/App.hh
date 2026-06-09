#ifndef APP_H
#define APP_H

#include "Controller.hh"
#include "TorchRenderer.hh"

class App {
public:
    App(int argc, char** argv);

    bool initialize();
    void run();

private:
    static App* s_instance;

    int m_argc;
    char** m_argv;
    int m_windowId;
    int m_lastFrameTime;
    Controller m_controller;
    TorchRenderer m_renderer;
    std::vector<Vector3> m_chemin  {
        Vector3{0.0f, -1.0f, 0.0f},
       Vector3{0.0f, 1.0f, 0.0f}
    };

    float m_rayonManche = 0.089f;
    float m_rayonBout = 0.3f;
    float m_hauteurBout = 0.5f;
    float m_materialColor[3] = {0.3f,0.2f,0.5f};

    static void displayCallback();
    static void reshapeCallback(int width, int height);
    static void closeCallback();
    static void keyboardDownCallback(unsigned char key, int x, int y);
    static void keyboardUpCallback(unsigned char key, int x, int y);
    static void specialDownCallback(int key, int x, int y);
    static void specialUpCallback(int key, int x, int y);
    static void idleCallback();

    void display();
    void reshape(int width, int height) const;
    void cleanup();
    void keyboardDown(unsigned char key, int x, int y);
    void keyboardUp(unsigned char key, int x, int y);
    void specialDown(int key, int x, int y);
    void specialUp(int key, int x, int y);
    
    void idle();
};

#endif
