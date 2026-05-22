#ifndef slic3r_SavePresetDialog_hpp_
#define slic3r_SavePresetDialog_hpp_

#include "libslic3r/Preset.hpp"
#include "wxExtensions.hpp"
#include "GUI_Utils.hpp"
#include "Widgets/RadioBox.hpp"
#include "Widgets/Button.hpp"
#include "Widgets/Label.hpp"
#include "Widgets/TextInput.hpp"
#include <QLayout>
#include <QString>
#include <vector>
#include <string>

namespace Slic3r {
namespace GUI {

class SavePresetDialog : public DPIDialog
{
public:
    SavePresetDialog(QWidget* parent, Preset::Type type, std::string suffix = "");
    SavePresetDialog(QWidget* parent, std::vector<Preset::Type> types, std::string suffix = "");
    ~SavePresetDialog();

    void AddItem(Preset::Type type, const std::string& suffix);

    std::string get_name();
    std::string get_name(Preset::Type type);
    void input_name_from_other(std::string new_preset_name);
    void confirm_from_other();

    bool enable_ok_btn() const;
    void add_info_for_edit_ph_printer(QLayout* sizer);
    void update_info_for_edit_ph_printer(const std::string& preset_name);
    void layout();
    bool get_save_to_project_selection(Preset::Type type);

protected:
    void on_dpi_changed(const QRect& suggested_rect) override;
    void on_sys_color_changed() {}

private:
    void build(std::vector<Preset::Type> types, std::string suffix = "");
    void on_select_cancel();
    void update_physical_printers(const std::string& preset_name);
    void accept();

    std::vector<std::string> m_preset_names;
    std::string m_ph_printer_name;
    std::string m_old_preset_name;
};

} // namespace GUI
} // namespace Slic3r

#endif
