#pragma once

#include <vector>
#include <QDialog>
#include <QBoxLayout>
#include "CapsuleButton.hpp"
#include "libslic3r/PrintConfig.hpp"

class Button;

namespace Slic3r {
class Print;
}

namespace Slic3r::GUI {
class Plater;
class PartPlate;
class FilamentMapPanel;
class SmartFilamentPanel;

/**
 * @brief Try to pop up the filament map dialog before slicing.
 * 
 * Only pop up in multi extruder machines. If user don't want the pop up, we
 * pop up if the applied filament map mode in manual
 * 
 * @param is_slice_all  In slice all
 * @param plater_ref Plater to get/set global filament map
 * @param partplate_ref Partplate to get/set plate filament map mode
 * @return whether continue slicing
*/
bool try_pop_up_before_slice(bool is_slice_all, Plater* plater_ref, PartPlate* partplate_ref, bool force_pop_up = false);
std::vector<FilamentMapMode> resolve_available_auto_modes(Print *print_obj, const std::vector<FilamentMapMode> &requested_modes, bool machine_synced);

class FilamentMapDialog : public QDialog
{
    enum PageType { ptAuto, ptManual };

public:
    FilamentMapDialog(QWidget *parent,
        const std::vector<std::string>& filament_color,
        const std::vector<std::string>& filament_type,
        const std::vector<int> &filament_map,
        const std::vector<int> &filament_volume_map,
        const std::vector<int> &filaments,
        const FilamentMapMode mode,
        bool machine_synced,
        bool show_default=true,
        bool with_checkbox = false,
        const std::vector<FilamentMapMode>& available_modes = {}
    );

    FilamentMapMode get_mode();
    std::vector<int> get_filament_maps() const {
        if (m_page_type == PageType::ptManual)
            return m_filament_map;
        return {};
    }

    std::vector<int> get_filament_volume_maps() const {
        if (m_page_type == PageType::ptManual)
            return m_filament_volume_map;
        return {};
    }

    int exec() override;

private:
    void make_header(QBoxLayout *sizer, bool only_saving_mode);
    void make_body(QBoxLayout                         *sizer,
                   bool                                only_saving_mode,
                   const std::vector<FilamentMapMode> &modes_to_use,
                   const std::vector<int>             &filaments,
                   const FilamentMapMode               mode,
                   bool                                machine_synced);
    void make_footer(QBoxLayout *sizer, const FilamentMapMode mode);

    void on_ok();
    void on_cancle();
    void on_switch_mode();
    void on_smart_filament_checkbox();

    void update_panel_status(PageType page);

 private:
     FilamentMapPanel   *m_auto_panel{};
     FilamentMapPanel   *m_manual_panel{};
     SmartFilamentPanel *smart_filament{};

     CapsuleButton *m_auto_btn;
     CapsuleButton *m_manual_btn;

     Button *m_ok_btn{};
     Button *m_cancel_btn{};

     PageType m_page_type;
     bool     m_fila_switch_ready{false};

     std::vector<int>         m_filament_map;
     std::vector<int>         m_filament_volume_map;
     std::vector<std::string> m_filament_color;
     std::vector<std::string> m_filament_type;
};

} // namespace Slic3r::GUI