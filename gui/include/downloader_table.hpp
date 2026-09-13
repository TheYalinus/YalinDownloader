#ifndef DOWNLOADER_TABLE_HPP
#define DOWNLOADER_TABLE_HPP
#include <gtkmm/grid.h>
#include <gtkmm/listbox.h>

class DownloaderTable : public Gtk::ListBox{
    public:
        using Gtk::ListBox::ListBox;
        unsigned int getActive();
        void insertDownload(Widget &child);
    private:
        unsigned int active;

};
#endif
