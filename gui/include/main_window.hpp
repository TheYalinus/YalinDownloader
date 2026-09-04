#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H
#include <gtkmm.h>
#include <gtkmm/box.h>
class MainWindow : public Gtk::Window
{
    public:
        MainWindow();
        virtual ~MainWindow();
    private:
        Gtk::Box MainBox;
};
#endif
