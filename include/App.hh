#ifndef APP_H
#define APP_H

#include "FireRenderer.hh"

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
    FireRenderer m_renderer;

    static void displayCallback();
    static void reshapeCallback(int width, int height);
    static void closeCallback();

    void display();
    void reshape(int width, int height) const;
    void cleanup();
};

#endif
