#ifndef slic3r_PhysicalPrinterDialog_hpp_
#define slic3r_PhysicalPrinterDialog_hpp_
#include <QWidget>
#include <QString>

#include <vector>


#include "libslic3r/Preset.hpp"
#include "GUI_Utils.hpp"
#include "Widgets/RoundedRectangle.hpp"
#include "Widgets/Button.hpp"

class QString;
class QLineEdit;
class QLabel;
class ScalableButton;
class QBoxLayout;

namespace Slic3r {

namespace GUI {

//------------------------------------------
//          PhysicalPrinterDialog
//------------------------------------------

class ConfigOptionsGroup;
class PhysicalPrinterDialog : public DPIDialog
{
    DynamicPrintConfig* m_config            { nullptr };
    ConfigOptionsGroup* m_optgroup          { nullptr };

    ScalableButton*     m_printhost_browse_btn              {nullptr};
    ScalableButton*     m_printhost_test_btn                {nullptr};
    ScalableButton*     m_printhost_cafile_browse_btn       {nullptr};
    ScalableButton*     m_printhost_client_cert_browse_btn  {nullptr};
    ScalableButton*     m_printhost_port_browse_btn         {nullptr};

    RoundedRectangle*   m_input_area                        {nullptr};
    QLabel*       m_valid_label                       {nullptr};
    QLineEdit*         m_input_ctrl                        {nullptr};
    Button*             m_button_ok                         {nullptr};
    Button*             m_button_cancel                     {nullptr};

    void build_printhost_settings(ConfigOptionsGroup* optgroup);
    void OnOK(QMouseEvent& event);

public:
    PhysicalPrinterDialog(QWidget* parent);
    ~PhysicalPrinterDialog();

    enum ValidationType
    {
        Valid,
        NoValid,
        Warning
    };
    PresetCollection* m_presets {nullptr};
    ValidationType  m_valid_type;
    std::string     m_preset_name;

    void        update(bool printer_change = false);
    void        update_host_type(bool printer_change);
    void        update_preset_input();
    void        update_printhost_buttons();
    void        update_printers();

protected:
    void on_dpi_changed(const QRect& suggested_rect) override;
    void on_sys_color_changed() override {};
    void check_host_key_valid();
};


} // namespace GUI
} // namespace Slic3r

#endif
