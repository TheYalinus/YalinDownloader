#include "main_window.hpp"
#include "button_group.hpp"
#include <gtkmm/button.h>
#include <gtkmm/enums.h>
#include <memory>

MainWindow::MainWindow():
MainBox(Gtk::Orientation::HORIZONTAL){
    set_title("Yalin Downloader GUI");
    set_default_size(500, 600);
    auto dumm1 = std::make_shared<Gtk::Button>("Button1");
    auto dumm2 = std::make_shared<Gtk::Button>("Button2");
    auto dumm3 = std::make_shared<Gtk::Button>("Button3");
    ButtonGroup grp1{dumm1 , dumm2 , dumm3};
    MainBox.append(*dumm1);
    MainBox.append(*dumm2);
    MainBox.append(*dumm3);
    set_child(MainBox);
}
MainWindow::~MainWindow(){

}
