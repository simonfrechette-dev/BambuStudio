// AMSSetting.cpp — stubbed for Qt6 port
// Phantom forward-declaration must precede ALL headers.
namespace Slic3r { namespace GUI { class MachineObject; } }
#include "AMSSetting.hpp"
#include "GUI_App.hpp"
#include "I18N.hpp"
#include "slic3r/GUI/DeviceCore/DevFilaSystem.h"
#include "slic3r/GUI/DeviceCore/DevUpgrade.h"
#include "slic3r/GUI/DeviceCore/DevManager.h"
#include "slic3r/GUI/Widgets/AnimaController.hpp"
#include "slic3r/GUI/Widgets/Label.hpp"
#include "slic3r/GUI/Widgets/ComboBox.hpp"

namespace Slic3r { namespace GUI {

// ---------------------------------------------------------------------------
// AMSSetting
// ---------------------------------------------------------------------------

AMSSetting::AMSSetting(QWidget *parent)
    : DPIDialog(parent)
    , m_obj(nullptr)
    , m_static_ams_settings(nullptr)
    , m_switching(false)
    , m_ams_type(nullptr)
    , m_ams_arrange_order(nullptr)
    , m_am_img(nullptr)
    , m_panel_body(nullptr)
    , m_panel_Insert_material(nullptr)
    , m_checkbox_Insert_material_auto_read(nullptr)
    , m_title_Insert_material_auto_read(nullptr)
    , m_tip_Insert_material_line1(nullptr)
    , m_tip_Insert_material_line2(nullptr)
    , m_tip_Insert_material_line3(nullptr)
    , m_checkbox_starting_auto_read(nullptr)
    , m_title_starting_auto_read(nullptr)
    , m_tip_starting_line1(nullptr)
    , m_tip_starting_line2(nullptr)
    , m_checkbox_remain(nullptr)
    , m_title_remain(nullptr)
    , m_tip_remain_line1(nullptr)
    , m_checkbox_switch_filament(nullptr)
    , m_title_switch_filament(nullptr)
    , m_tip_switch_filament_line1(nullptr)
    , m_checkbox_air_print(nullptr)
    , m_title_air_print(nullptr)
    , m_tip_air_print_line(nullptr)
    , m_tip_ams_img(nullptr)
    , m_button_auto_demarcate(nullptr)
    , m_sizer_Insert_material_tip_inline(nullptr)
    , m_sizer_starting_tip_inline(nullptr)
    , m_sizer_remain_inline(nullptr)
    , m_sizer_switch_filament_inline(nullptr)
    , m_sizer_remain_block(nullptr)
{
    create();
}

AMSSetting::~AMSSetting() {}

void AMSSetting::create() {}
void AMSSetting::UpdateByObj(MachineObject* obj) { m_obj = obj; }
void AMSSetting::update_ams_img(MachineObject* obj) { (void)obj; }
void AMSSetting::update_starting_read_mode(bool selected) { (void)selected; }
void AMSSetting::update_remain_mode(bool selected) { (void)selected; }
void AMSSetting::update_switch_filament(bool selected) { (void)selected; }
void AMSSetting::update_insert_material_read_mode(MachineObject* obj) { (void)obj; }
void AMSSetting::update_insert_material_read_mode(bool selected, std::string version) { (void)selected; (void)version; }
void AMSSetting::update_air_printing_detection(MachineObject* obj) { (void)obj; }
void AMSSetting::update_firmware_switching_status() {}
void AMSSetting::on_insert_material_read() {}
void AMSSetting::on_starting_read() {}
void AMSSetting::on_remain() {}
void AMSSetting::on_switch_filament() {}
void AMSSetting::on_air_print_detect() {}
void AMSSetting::on_dpi_changed(const QRect& r) { (void)r; }

// ---------------------------------------------------------------------------
// AMSSettingTypePanel
// ---------------------------------------------------------------------------

AMSSettingTypePanel::AMSSettingTypePanel(QWidget* parent, AMSSetting* setting_dlg)
    : QWidget(parent)
    , m_ams_firmware_current_idx(-1)
    , m_setting_dlg(setting_dlg)
    , m_type_combobox(nullptr)
    , m_switching_tips(nullptr)
    , m_switching_icon(nullptr)
{}

AMSSettingTypePanel::~AMSSettingTypePanel() {}

void AMSSettingTypePanel::Update(const MachineObject* obj) { (void)obj; }
void AMSSettingTypePanel::CreateGui() {}
void AMSSettingTypePanel::OnAmsTypeChanged(int index) { (void)index; }

// ---------------------------------------------------------------------------
// AMSSettingArrangeAMSOrder
// ---------------------------------------------------------------------------

AMSSettingArrangeAMSOrder::AMSSettingArrangeAMSOrder(QWidget* parent)
    : QWidget(parent)
    , m_btn_rearrange(nullptr)
{
    CreateGui();
}

void AMSSettingArrangeAMSOrder::Update(const MachineObject* obj) { (void)obj; }
void AMSSettingArrangeAMSOrder::CreateGui() {}
void AMSSettingArrangeAMSOrder::OnBtnRearrangeClicked() {}

}} // namespace Slic3r::GUI
