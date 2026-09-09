#include "main_window.hpp"

#include <glib.h>
#include <gtkmm/button.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/enums.h>
#include <memory>

MainWindow::MainWindow():
MainBox(Gtk::Orientation::VERTICAL)
,CentralPanel(Gtk::Orientation::HORIZONTAL),
RightPanel(Gtk::Orientation::VERTICAL),
CentralCenterPanel(Gtk::Orientation::HORIZONTAL),
RightButtonActive("Active")
,RightButtonHistory("History")
,RightButtonAll("All"),
TopButtonMediaGrabber("Media Grabber"),
TopButtonNew("New"),
TopButtonOpen("Open"),
TopButtonSettings("Settings"),
TopButtonOpeninFolder("Open in Folder"),
Logo("Yalin Downloader"),
MainFrame("Active Downloads"){
    set_title("Yalin Downloader GUI");
    set_child(MainBox);
    //Expand
    MainBox.set_expand(true);
    CentralPanel.set_expand(true);
    CentralCenterPanel.set_expand(true);
    CentralTopPanel.set_hexpand(true);
    MainFrame.set_expand(true);
    //Append

    MainBox.append(CentralTopPanel);
    MainBox.append(CentralPanel);
    CentralPanel.append(RightPanel);
    CentralPanel.append(CentralCenterPanel);
    //Right Panel
    RightButtonAll.set_group(RightButtonActive);
    RightButtonHistory.set_group(RightButtonActive);
    RightButtonActive.set_active(true);
    RightPanel.append(RightButtonActive);
    RightPanel.append(RightButtonHistory);
    RightPanel.append(RightButtonAll);

    RightPanel.set_spacing(4);
    RightPanel.set_margin(5);
    //Top Panel
    CentralTopPanel.append(Logo);
    CentralTopPanel.append(TopButtonNew);
    CentralTopPanel.append(TopButtonOpen);
    CentralTopPanel.append(TopButtonOpeninFolder);
    CentralTopPanel.append(TopButtonSettings);
    CentralTopPanel.append(TopButtonMediaGrabber);
    CentralTopPanel.set_spacing(4);
    CentralTopPanel.set_margin(3);

    CentralTopPanel.set_homogeneous(true);
    //Center
    CentralCenterPanel.append(MainFrame);
    MainFrame.set_margin(5);
    MainFrame.set_child(MainTable);
    MainTable.set_margin(2);



}
MainWindow::~MainWindow(){

}
