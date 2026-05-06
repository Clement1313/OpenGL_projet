#include <iostream>

#include "include/App.hh"

int main(int argc, char** argv)
{
    App app(argc, argv);

    if (!app.initialize())
    {
        std::cerr << "Application initialization failed." << std::endl;
        return 1;
    }

    app.run();
    return 0;
}
