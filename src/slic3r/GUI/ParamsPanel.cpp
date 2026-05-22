// Qt6 port stub for ParamsPanel.cpp
#include "ParamsPanel.hpp"
#include "GUI_Utils.hpp"
#include "wxExtensions.hpp"
#include "GUI_App.hpp"
#include "Tab.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>

namespace Slic3r {
namespace GUI {

// TipsDialog
TipsDialog::TipsDialog(QWidget* parent, const QString& title, const QString& description,
                       std::string app_key, int buttons, std::map<int, QString> option_map)
    : DPIDialog(parent)
    , m_app_key(app_key)
{
    setWindowTitle(title);
}

QBoxLayout* TipsDialog::create_item_checkbox(const QString&, QWidget*, const QString&, const std::string&) { return nullptr; }
Button* TipsDialog::add_button(int, const QString&, bool) { return nullptr; }

// ParamsPanel::Highlighter
void ParamsPanel::Highlighter::set_timer_owner(QObject*, int) {}
void ParamsPanel::Highlighter::init(std::pair<QWidget*, bool*>, QWidget*) {}
void ParamsPanel::Highlighter::blink() {}
void ParamsPanel::Highlighter::invalidate() {}

// ParamsPanel
ParamsPanel::ParamsPanel(QWidget* parent, int, QPoint, QSize size, Qt::WindowFlags style, const QString&)
    : QWidget(parent)
{
    if (!size.isEmpty()) resize(size);
}

ParamsPanel::~ParamsPanel() {}

void ParamsPanel::free_sizers() {}
void ParamsPanel::delete_subwindows() {}
void ParamsPanel::refresh_tabs() {}
void ParamsPanel::rebuild_panels() {}

void ParamsPanel::create_layout()
{
    // Build a simple tabbed layout showing Print / Filament / Printer settings.
    // Tabs were already created via MainFrame::create_preset_tabs() and are in
    // wxGetApp().tabs_list. We just wire them up here.
    auto* vlay = new QVBoxLayout(this);
    vlay->setContentsMargins(0, 0, 0, 0);
    vlay->setSpacing(0);

    auto* tab_widget = new QTabWidget(this);
    tab_widget->setTabPosition(QTabWidget::North);

    for (Tab* tab : wxGetApp().tabs_list) {
        // Only the three main preset tabs (not model/plate/object sub-tabs)
        auto ptype = tab->type();
        if (ptype == Preset::TYPE_PRINT ||
            ptype == Preset::TYPE_FILAMENT ||
            ptype == Preset::TYPE_PRINTER)
        {
            tab->setParent(tab_widget);
            tab_widget->addTab(tab, tab->title());
            // Store in named members for quick access
            if (ptype == Preset::TYPE_PRINT)    m_tab_print    = tab;
            if (ptype == Preset::TYPE_FILAMENT) m_tab_filament = tab;
            if (ptype == Preset::TYPE_PRINTER)  m_tab_printer  = tab;
        }
    }

    vlay->addWidget(tab_widget);

    // Set the initially active tab
    if (tab_widget->count() > 0) {
        m_current_tab = tab_widget->widget(0);
    }
}
void ParamsPanel::clear_page() {}
void ParamsPanel::OnActivate() {}
void ParamsPanel::set_active_tab(QWidget*) {}
bool ParamsPanel::is_active_and_shown_tab(QWidget*) { return false; }
void ParamsPanel::update_mode() {}
void ParamsPanel::msw_rescale() {}
void ParamsPanel::switch_to_global() {}
void ParamsPanel::switch_to_object(bool) {}
void ParamsPanel::notify_object_config_changed() {}
void ParamsPanel::switch_to_object_if_has_object_configs() {}
void ParamsPanel::OnToggled(bool) {}

} // namespace GUI
} // namespace Slic3r
