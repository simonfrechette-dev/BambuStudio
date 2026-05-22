// FilamentMapDialog.cpp — Qt6 port (stub implementations)
#include "FilamentMapDialog.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>

#include "I18N.hpp"
#include "GUI_App.hpp"
#include "CapsuleButton.hpp"
#include "FilamentMapPanel.hpp"
#include "Widgets/Button.hpp"
#include "Widgets/Label.hpp"

namespace Slic3r { namespace GUI {

// free functions declared in header
bool try_pop_up_before_slice(bool /*is_slice_all*/, Plater * /*plater_ref*/,
                              PartPlate * /*partplate_ref*/, bool /*force*/)
{
    return true; // stub — always continue slicing
}

std::vector<FilamentMapMode> resolve_available_auto_modes(
    Print * /*print_obj*/,
    const std::vector<FilamentMapMode> &requested_modes,
    bool /*machine_synced*/)
{
    return requested_modes;
}

// ─── FilamentMapDialog ────────────────────────────────────────────────────────

FilamentMapDialog::FilamentMapDialog(QWidget *parent,
    const std::vector<std::string> &filament_color,
    const std::vector<std::string> &filament_type,
    const std::vector<int>         &filament_map,
    const std::vector<int>         &filament_volume_map,
    const std::vector<int>         &filaments,
    const FilamentMapMode           mode,
    bool                            machine_synced,
    bool                            show_default,
    bool                            with_checkbox,
    const std::vector<FilamentMapMode> &available_modes)
    : QDialog(parent)
    , m_filament_map(filament_map)
    , m_filament_volume_map(filament_volume_map)
    , m_filament_color(filament_color)
    , m_filament_type(filament_type)
    , m_page_type(is_auto_filament_map_mode(mode) ? PageType::ptAuto : PageType::ptManual)
{
    (void)show_default;
    (void)with_checkbox;

    setWindowTitle(_L("Filament Mapping"));
    setMinimumSize(540, 480);

    auto *main_layout = new QVBoxLayout(this);
    main_layout->setContentsMargins(20, 16, 20, 16);
    main_layout->setSpacing(8);

    make_header(main_layout, false);
    make_body(main_layout, false, available_modes, filaments, mode, machine_synced);
    make_footer(main_layout, mode);
}

FilamentMapMode FilamentMapDialog::get_mode()
{
    return m_page_type == PageType::ptAuto ? fmmAutoForFlush : fmmManual;
}

int FilamentMapDialog::exec()
{
    return QDialog::exec();
}

void FilamentMapDialog::make_header(QBoxLayout *sizer, bool /*only_saving_mode*/)
{
    auto *title = new Label(this, Label::Head_16);
    title->setText(_L("Filament Mapping"));
    sizer->addWidget(title);

    // Mode switcher: Auto / Manual
    auto *mode_row = new QHBoxLayout();

    m_auto_btn = new CapsuleButton(this, _L("Auto"), m_page_type == PageType::ptAuto);
    mode_row->addWidget(m_auto_btn);

    m_manual_btn = new CapsuleButton(this, _L("Manual"), m_page_type == PageType::ptManual);
    mode_row->addWidget(m_manual_btn);
    mode_row->addStretch(1);

    sizer->addLayout(mode_row);

    connect(m_auto_btn,   &CapsuleButton::clicked, this, [this]() {
        m_page_type = PageType::ptAuto;
        on_switch_mode();
    });
    connect(m_manual_btn, &CapsuleButton::clicked, this, [this]() {
        m_page_type = PageType::ptManual;
        on_switch_mode();
    });
}

void FilamentMapDialog::make_body(QBoxLayout *sizer,
                                   bool /*only_saving_mode*/,
                                   const std::vector<FilamentMapMode> & /*modes_to_use*/,
                                   const std::vector<int>             & /*filaments*/,
                                   const FilamentMapMode               /*mode*/,
                                   bool                                /*machine_synced*/)
{
    // Auto and manual panels are abstract (FilamentMapPanel has pure virtual GetMode()).
    // They are instantiated by subclasses elsewhere; here we just leave them null.
    m_auto_panel   = nullptr;
    m_manual_panel = nullptr;

    update_panel_status(m_page_type);
}

void FilamentMapDialog::make_footer(QBoxLayout *sizer, const FilamentMapMode /*mode*/)
{
    auto *btn_row = new QHBoxLayout();
    btn_row->addStretch(1);

    m_cancel_btn = new Button(this, _L("Cancel"));
    btn_row->addWidget(m_cancel_btn);

    m_ok_btn = new Button(this, _L("OK"));
    m_ok_btn->SetValue(true);
    btn_row->addWidget(m_ok_btn);

    sizer->addLayout(btn_row);

    connect(m_ok_btn,     &Button::clicked, this, [this]() { on_ok(); });
    connect(m_cancel_btn, &Button::clicked, this, [this]() { on_cancle(); });
}

void FilamentMapDialog::on_ok()
{
    accept();
}

void FilamentMapDialog::on_cancle()
{
    reject();
}

void FilamentMapDialog::on_switch_mode()
{
    if (m_auto_btn)   m_auto_btn->Select(m_page_type == PageType::ptAuto);
    if (m_manual_btn) m_manual_btn->Select(m_page_type == PageType::ptManual);
    update_panel_status(m_page_type);
}

void FilamentMapDialog::on_smart_filament_checkbox()
{
    m_fila_switch_ready = !m_fila_switch_ready;
}

void FilamentMapDialog::update_panel_status(PageType page)
{
    if (m_auto_panel)   m_auto_panel->setVisible(page == PageType::ptAuto);
    if (m_manual_panel) m_manual_panel->setVisible(page == PageType::ptManual);
}

}} // namespace Slic3r::GUI
