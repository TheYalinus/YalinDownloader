#include "main_window.hpp"

#include <gtkmm/button.h>
#include <gtkmm/enums.h>
#include <memory>

MainWindow::MainWindow():
MainBox(Gtk::Orientation::HORIZONTAL){
    set_title("Yalin Downloader GUI");
    set_child(MainBox);
}
MainWindow::~MainWindow(){

}
