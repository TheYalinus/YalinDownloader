#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H
#include <gtkmm.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/label.h>
#include <gtkmm/togglebutton.h>
#include "downloader_table.hpp"
class MainWindow : public Gtk::Window
{
    public:
        MainWindow();
        virtual ~MainWindow();
    private:
        Gtk::Box MainBox, RightPanel, CentralPanel, CentralTopPanel , CentralCenterPanel;
        Gtk::ToggleButton RightButtonAll , RightButtonHistory, RightButtonActive ;
        Gtk::Button TopButtonNew, TopButtonOpen, TopButtonOpeninFolder , TopButtonMediaGrabber, TopButtonSettings;
        Gtk::Label Logo;
        Gtk::Frame MainFrame;
        DownloaderTable MainTable;
};
#endif
