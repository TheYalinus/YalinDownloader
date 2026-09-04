#include <gtkmm/application.h>
#include <iostream>
#include "download_library.hpp"
#include "main_window.hpp"
#include "gui.hpp"

int main(int argc, char* argv[]){

    auto app = Gtk::Application::create("org.yalindownloader.gui");
    app->make_window_and_run<MainWindow>(argc, argv);
    return 0;
}
