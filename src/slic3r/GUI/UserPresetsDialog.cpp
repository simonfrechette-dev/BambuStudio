// UserPresetsDialog.cpp — Qt6 port
#include "UserPresetsDialog.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QMessageBox>

#include "GUI_App.hpp"
#include "I18N.hpp"
#include "Widgets/TabCtrl.hpp"
#include "Widgets/SwitchButton.hpp"
#include "Widgets/TextInput.hpp"
#include "Widgets/CheckBox.hpp"
#include "Widgets/Label.hpp"
#include "Widgets/Button.hpp"

namespace Slic3r { namespace GUI {

UserPresetsDialog::UserPresetsDialog(QWidget *parent)
    : DPIDialog(parent)
    , m_tab_ctrl(nullptr)
    , m_switch_button(nullptr)
    , m_search(nullptr)
    , m_empty_panel(nullptr)
    , m_scrolled(nullptr)
    , m_check_all(nullptr)
    , m_label_check_count(nullptr)
    , m_button_delete(nullptr)
{
    setWindowTitle(_L("User Presets"));
    setMinimumSize(600, 480);

    auto *main_layout = new QVBoxLayout(this);
    main_layout->setContentsMargins(20, 16, 20, 16);
    main_layout->setSpacing(8);

    // Title row
    auto *title_row = new QHBoxLayout();
    auto *title = new Label(this, Label::Head_16);
    title->setText(_L("User Presets"));
    title_row->addWidget(title);
    title_row->addStretch(1);
    // Filament / Non-filament switch
    m_switch_button = new SwitchButton(this);
    title_row->addWidget(m_switch_button);
    main_layout->addLayout(title_row);

    // Tab control: Printer / Filament / Process
    m_tab_ctrl = new TabCtrl(this);
    m_tab_ctrl->AppendItem(_L("Printer"));
    m_tab_ctrl->AppendItem(_L("Filament"));
    m_tab_ctrl->AppendItem(_L("Process"));
    connect(m_tab_ctrl, &TabCtrl::selChanged, this,
            &UserPresetsDialog::on_collection_changed);
    main_layout->addWidget(m_tab_ctrl);

    // Search row
    auto *search_row = new QHBoxLayout();
    m_search = new TextInput(this);
    m_search->GetTextCtrl()->setPlaceholderText(_L("Search..."));
    search_row->addWidget(m_search, 1);
    main_layout->addLayout(search_row);

    connect(m_search->GetTextCtrl(), &QLineEdit::textChanged, this,
            &UserPresetsDialog::on_search);

    // Scrollable preset list
    m_scrolled = new QScrollArea(this);
    m_scrolled->setFrameShape(QFrame::NoFrame);
    m_scrolled->setWidgetResizable(true);

    auto *scroll_content = new QWidget(m_scrolled);
    scroll_content->setLayout(new QVBoxLayout(scroll_content));
    m_scrolled->setWidget(scroll_content);
    main_layout->addWidget(m_scrolled, 1);

    // Empty state panel
    m_empty_panel = new QWidget(this);
    auto *empty_layout = new QVBoxLayout(m_empty_panel);
    auto *empty_label = new Label(m_empty_panel, Label::Body_14);
    empty_label->setText(_L("No user presets in this category."));
    empty_label->setAlignment(Qt::AlignCenter);
    empty_layout->addStretch(1);
    empty_layout->addWidget(empty_label);
    empty_layout->addStretch(1);
    m_empty_panel->hide();
    main_layout->addWidget(m_empty_panel);

    // Footer: select-all + count + delete
    auto *footer_row = new QHBoxLayout();

    m_check_all = new CheckBox(this);
    footer_row->addWidget(m_check_all);

    m_label_check_count = new Label(this, Label::Body_12);
    m_label_check_count->setText(_L("0 selected"));
    footer_row->addWidget(m_label_check_count);

    footer_row->addStretch(1);

    m_button_delete = new Button(this, _L("Delete"));
    m_button_delete->setEnabled(false);
    footer_row->addWidget(m_button_delete);

    main_layout->addLayout(footer_row);

    connect(m_check_all,    &CheckBox::toggled, this,
            [this](bool checked) { on_all_checked(checked, true); });
    connect(m_button_delete, &Button::clicked, this,
            &UserPresetsDialog::delete_checked);

    init_preset_list();
}

void UserPresetsDialog::init_preset_list()
{
    // stub — populate m_presets / m_filament_presets from PresetBundle
    layout_preset_list();
}

void UserPresetsDialog::create_preset_list(QWidget * /*parent*/)
{
    // stub
}

QLayout *UserPresetsDialog::create_preset_line(QWidget *parent,
                                               std::string const & /*preset*/)
{
    auto *row = new QHBoxLayout();
    auto *cb  = new CheckBox(parent);
    auto *lbl = new Label(parent, Label::Body_14);
    row->addWidget(cb);
    row->addWidget(lbl, 1);
    return row;
}

QLayout *UserPresetsDialog::create_filament_group(
    QWidget *parent,
    std::pair<std::string const, std::vector<std::string>> const & /*filament*/)
{
    auto *group = new QVBoxLayout();
    auto *header = new Label(parent, Label::Head_14);
    group->addWidget(header);
    return group;
}

void UserPresetsDialog::layout_preset_list(bool /*delete_old*/)
{
    // stub — rebuild scroll content from m_presets
}

void UserPresetsDialog::on_collection_changed(int collection)
{
    m_collection = collection;
    layout_preset_list(true);
}

void UserPresetsDialog::on_search(QString const & /*keyword*/)
{
    // stub — filter visible sizers
}

void UserPresetsDialog::on_preset_checked(std::string const & /*preset*/,
                                          bool /*checked*/, bool /*from_user*/)
{
    update_preset_counts();
}

void UserPresetsDialog::on_filament_checked(std::string const & /*preset*/,
                                            bool /*checked*/, bool /*from_user*/)
{
    update_preset_counts();
}

void UserPresetsDialog::on_all_checked(bool checked, bool /*from_user*/)
{
    // stub — check/uncheck all visible rows
    (void)checked;
    update_preset_counts();
}

void UserPresetsDialog::update_preset_counts()
{
    int n = (int)(m_checked_presets.size() + m_checked_filaments.size());
    m_label_check_count->setText(QString::number(n) + _L(" selected"));
    m_button_delete->setEnabled(n > 0);
}

void UserPresetsDialog::update_checked()
{
    // stub
}

void UserPresetsDialog::delete_checked()
{
    // stub
}

bool UserPresetsDialog::delete_presets(int /*collection*/,
                                       std::vector<std::string> & /*presets*/)
{
    return false;
}

bool UserPresetsDialog::delete_confirm(int /*collection*/, int preset_num)
{
    return QMessageBox::question(
               this, _L("Delete Presets"),
               _L("Delete %1 preset(s)?").arg(preset_num)) == QMessageBox::Yes;
}

bool UserPresetsDialog::delete_confirm(int /*collection*/,
                                       int filament_preset_num,
                                       int print_preset_num)
{
    return QMessageBox::question(
               this, _L("Delete Presets"),
               _L("Delete %1 filament and %2 process preset(s)?")
                   .arg(filament_preset_num)
                   .arg(print_preset_num)) == QMessageBox::Yes;
}

void UserPresetsDialog::on_dpi_changed(const QRect & /*suggested_rect*/) {}

bool UserPresetsDialog::is_filament_list() const
{
    return m_switch_button && m_switch_button->GetValue();
}

}} // namespace Slic3r::GUI
