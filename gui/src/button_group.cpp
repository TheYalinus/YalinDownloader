#include "button_group.hpp"
#include <functional>
#include <initializer_list>
    #include <algorithm>
#include <sigc++/adaptors/bind.h>
#include <sigc++/functors/mem_fun.h>
#include <iostream>
ButtonGroup::ButtonGroup():
buttons(std::vector< SharedButtonType>())
{

}
ButtonGroup::ButtonGroup(std::initializer_list<SharedButtonType> il):
buttons(std::vector<SharedButtonType>()){
    for(auto &s : il){
        addButton(s);
    }
    setAllSensible();

}
void ButtonGroup::addButton(SharedButtonType p_Button){
    if(this->buttons.size() == 0){
        p_Button->set_sensitive(true);
    }
    else {
        p_Button->set_sensitive(false);
    }
    //p_Button->signal_clicked().connect(std::bind(&ButtonGroup::sigOnButtonPressed,this,p_Button));
    auto signal_act = [this,p_Button](){
        this->sigOnButtonPressed(p_Button);
    };
    p_Button->signal_clicked().connect(signal_act);
    buttons.push_back(p_Button);

}
void ButtonGroup::setAllSensible(){
    for(SharedButtonType n : this->buttons){
        n.get()->set_sensitive(true);
    }
}
void ButtonGroup::removeButton(SharedButtonType p_Button){
    // Source - https://stackoverflow.com/a/3385251
    // Posted by Georg Fritzsche, modified by community. See post 'Timeline' for change history
    // Retrieved 2026-09-04, License - CC BY-SA 3.0
    buttons.erase(std::remove(buttons.begin(), buttons.end(), p_Button), buttons.end());
    if(p_Button->get_sensitive() == false)
    {
        if (buttons.size() != 0) {
            buttons[0]->set_sensitive(false);
        }
    }
}
void ButtonGroup::sigOnButtonPressed(SharedButtonType p_Button){
    this->setAllSensible();
    p_Button->set_sensitive(false);
}
