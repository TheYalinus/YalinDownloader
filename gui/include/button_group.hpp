#include <gtkmm/button.h>
#include <gtkmm/object.h>
#include <initializer_list>
#include <memory>
#include <vector>

using SharedButtonType = std::shared_ptr<Gtk::Button>;
class ButtonGroup : public  std::enable_shared_from_this<ButtonGroup>{
    public:
        ButtonGroup();
        ButtonGroup(std::initializer_list<SharedButtonType> );
        void addButton(SharedButtonType);
        void removeButton(SharedButtonType);
        void sigOnButtonPressed ( SharedButtonType);
    private:
        std::vector<SharedButtonType> buttons;
        void setAllSensible();


};
