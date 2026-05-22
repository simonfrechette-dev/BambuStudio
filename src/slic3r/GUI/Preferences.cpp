// Preferences.cpp — Qt6 port (faithful to original wx implementation)
#include "Preferences.hpp"
#include "GUI_App.hpp"
#include "MainFrame.hpp"
#include "I18N.hpp"
#include "libslic3r/AppConfig.hpp"
#include "libslic3r/Format/bbs_3mf.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QScrollArea>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QWidget>
#include <QApplication>
#include <QScreen>
#include <QIntValidator>
#include <QRegularExpressionValidator>
#include <QRegularExpression>
#include <QLineEdit>

#include "UxProgramTermsDialog.hpp"
#include <QDesktopServices>
#include <QUrl>
#include <algorithm>
#include <functional>

namespace Slic3r { namespace GUI {

// ---------------------------------------------------------------------------
// Constructor / destructor
// ---------------------------------------------------------------------------

PreferencesDialog::PreferencesDialog(QWidget *parent)
    : DPIDialog(parent)
{
    if (auto *c = wxGetApp().app_config)
        m_original_use_12h_time_format = c->get("use_12h_time_format");
    create();
}

PreferencesDialog::~PreferencesDialog()
{
    m_radio_group.clear();
    m_hash_selector.clear();
}

void PreferencesDialog::on_dpi_changed(const QRect &suggested_rect)
{
    (void)suggested_rect;
    for (auto &item : m_button_list)   { item.second->Rescale(); }
    for (auto &item : m_checkbox_list) { item.second->Rescale(); }
    for (auto &item : m_radiobox_list) { item.second->Rescale(); }
    for (auto &item : m_combobox_list) { item.second->Rescale(); }
    if (m_scrolledWindow) m_scrolledWindow->update();
}

void PreferencesDialog::Split(const std::string &src, const std::string &separator, std::vector<QString> &dest)
{
    dest.clear();
    std::string str = src;
    std::string::size_type start = 0, index;
    index = str.find_first_of(separator, start);
    while (index != std::string::npos) {
        dest.push_back(QString::fromStdString(str.substr(start, index - start)));
        start = index + separator.size();
        index = str.find(separator, start);
    }
    dest.push_back(QString::fromStdString(str.substr(start)));
}

void PreferencesDialog::on_select_radio(std::string param)
{
    int groupid = 0;
    for (auto &rs : m_radio_group) {
        if (rs.m_param_name == QString::fromStdString(param)) {
            groupid = rs.m_groupid;
            break;
        }
    }
    for (auto &rs : m_radio_group) {
        if (rs.m_groupid == groupid) {
            bool sel = (rs.m_param_name == QString::fromStdString(param));
            rs.m_radiobox->SetValue(sel);
            rs.m_selected = sel;
        }
    }
}

QString PreferencesDialog::get_select_radio(int groupid)
{
    for (auto &rs : m_radio_group) {
        if (rs.m_groupid == groupid && rs.m_radiobox->GetValue())
            return rs.m_param_name;
    }
    return {};
}

// ---------------------------------------------------------------------------
// create_item_title — section header with expanding horizontal line
// ---------------------------------------------------------------------------
QLayout *PreferencesDialog::create_item_title(QString title, QWidget *parent, QString tooltip)
{
    (void)tooltip;
    auto *lay = new QHBoxLayout();
    lay->setContentsMargins(0, 4, 0, 2);
    lay->setSpacing(0);

    auto *lbl = new QLabel(title, parent);
    lbl->setStyleSheet("font-weight: bold; font-size: 13px; color: #32383D;");

    auto *line = new QFrame(parent);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Plain);
    line->setStyleSheet("color: #A6A9AA;");

    lay->addWidget(lbl);
    lay->addSpacing(9);
    lay->addWidget(line, 1);
    return lay;
}

// ---------------------------------------------------------------------------
// create_item_combobox — title label + ::ComboBox
// ---------------------------------------------------------------------------
QLayout *PreferencesDialog::create_item_combobox(
    QString title, QWidget *parent, QString tooltip, std::string param,
    const std::vector<QString> &label_list,
    const std::vector<std::string> &value_list,
    std::function<void(int)> callback,
    int title_width, int combox_width)
{
    auto get_value_idx = [&value_list](const std::string &value) -> int {
        auto it = std::find(value_list.begin(), value_list.end(), value);
        if (it != value_list.end())
            return static_cast<int>(std::distance(value_list.begin(), it));
        return 0;
    };

    auto *lay = new QHBoxLayout();
    lay->setContentsMargins(23, 0, 0, 0);
    lay->setSpacing(3);

    auto *combo_title = new QLabel(title, parent);
    combo_title->setToolTip(tooltip);
    combo_title->setStyleSheet("color: #262E30; font-size: 13px;");
    combo_title->setMinimumWidth(title_width > 0 ? title_width : 100);
    lay->addWidget(combo_title);

    auto *combobox = new ::ComboBox(parent);
    combobox->setFixedWidth(combox_width > 0 ? combox_width : 160);
    m_combobox_list[static_cast<int>(m_combobox_list.size())] = combobox;

    for (auto &label : label_list)
        combobox->Append(label);

    std::string old_value = wxGetApp().app_config ? wxGetApp().app_config->get(param) : "";
    combobox->SetSelection(!old_value.empty() ? get_value_idx(old_value) : 0);

    lay->addWidget(combobox);
    lay->addStretch(1);

    connect(combobox, &::ComboBox::selectionChanged, this,
            [this, param, value_list, callback](int idx) {
        if (idx < 0 || idx >= (int)value_list.size()) return;
        if (auto *c = wxGetApp().app_config) {
            c->set(param, value_list[idx]);
            c->save();
        }
        if (callback) callback(idx);
    });

    return lay;
}

// ---------------------------------------------------------------------------
// create_item_language_combobox
// ---------------------------------------------------------------------------
QLayout *PreferencesDialog::create_item_language_combobox(
    QString title, QWidget *parent, QString tooltip,
    int /*padding_left*/, std::string param, std::vector<std::string> vlist)
{
    auto *lay = new QHBoxLayout();
    lay->setContentsMargins(23, 0, 0, 0);
    lay->setSpacing(3);

    auto *combo_title = new QLabel(title, parent);
    combo_title->setToolTip(tooltip);
    combo_title->setStyleSheet("color: #262E30; font-size: 13px;");
    combo_title->setMinimumWidth(100);
    lay->addWidget(combo_title);

    auto *combobox = new ::ComboBox(parent);
    combobox->setFixedWidth(160);
    m_combobox_list[static_cast<int>(m_combobox_list.size())] = combobox;

    std::string current_lang = wxGetApp().app_config ? wxGetApp().app_config->get(param) : "";
    m_current_language_selected = 0;
    for (int i = 0; i < (int)vlist.size(); ++i) {
        combobox->Append(QString::fromStdString(vlist[i]));
        if (vlist[i] == current_lang)
            m_current_language_selected = i;
    }
    combobox->SetSelection(m_current_language_selected);

    lay->addWidget(combobox);
    lay->addStretch(1);

    connect(combobox, &::ComboBox::selectionChanged, this,
            [this, param, vlist, combobox](int idx) {
        if (idx == m_current_language_selected) return;
        if (idx < 0 || idx >= (int)vlist.size()) return;
        auto btn = QMessageBox::question(this,
            _L("Language selection"),
            _L("Switching the language requires application restart.\n") + "\n" +
            _L("Do you want to continue?"),
            QMessageBox::Ok | QMessageBox::Cancel);
        if (btn != QMessageBox::Ok) {
            combobox->SetSelection(m_current_language_selected);
            return;
        }
        m_current_language_selected = idx;
        if (auto *c = wxGetApp().app_config) {
            c->set(param, vlist[idx]);
            c->save();
        }
    });

    return lay;
}

// ---------------------------------------------------------------------------
// create_item_region_combobox
// ---------------------------------------------------------------------------
QLayout *PreferencesDialog::create_item_region_combobox(
    QString title, QWidget *parent, QString tooltip, std::vector<QString> vlist)
{
    static const std::vector<std::string> local_regions = {
        "Asia-Pacific", "China", "Europe", "North America", "Others"
    };

    auto *lay = new QHBoxLayout();
    lay->setContentsMargins(23, 0, 0, 0);
    lay->setSpacing(3);

    auto *combo_title = new QLabel(title, parent);
    combo_title->setToolTip(tooltip);
    combo_title->setStyleSheet("color: #262E30; font-size: 13px;");
    combo_title->setMinimumWidth(100);
    lay->addWidget(combo_title);

    auto *combobox = new ::ComboBox(parent);
    combobox->setFixedWidth(160);
    m_combobox_list[static_cast<int>(m_combobox_list.size())] = combobox;

    for (auto &lbl : vlist)
        combobox->Append(lbl);

    AppConfig *config = wxGetApp().app_config;
    int current_region = 0;
    if (config && !config->get("region").empty()) {
        std::string region_code = config->get("region");
        for (int i = 0; i < (int)local_regions.size(); ++i) {
            if (local_regions[i] == region_code) {
                combobox->SetSelection(i);
                current_region = i;
                break;
            }
        }
    }

    lay->addWidget(combobox);
    lay->addStretch(1);

    connect(combobox, &::ComboBox::selectionChanged, this,
            [this, combobox, current_region](int idx) {
        static const std::vector<std::string> &regions = local_regions;
        if (idx < 0 || idx >= (int)regions.size()) return;
        AppConfig *c = wxGetApp().app_config;
        if (wxGetApp().getAgent()) {
            auto btn = QMessageBox::question(this,
                _L("Region selection"),
                _L("Changing the region will log out your account.\n") + "\n" +
                _L("Do you want to continue?"),
                QMessageBox::Ok | QMessageBox::Cancel);
            if (btn != QMessageBox::Ok) {
                combobox->SetSelection(current_region);
                return;
            }
            wxGetApp().request_user_logout();
        }
        if (c) c->set("region", regions[idx]);
        wxGetApp().update_publish_status();
    });

    return lay;
}

// ---------------------------------------------------------------------------
// create_item_loglevel_combobox
// ---------------------------------------------------------------------------
QLayout *PreferencesDialog::create_item_loglevel_combobox(
    QString title, QWidget *parent, QString tooltip, std::vector<QString> vlist)
{
    auto *lay = new QHBoxLayout();
    lay->setContentsMargins(23, 0, 0, 0);
    lay->setSpacing(3);

    auto *combo_title = new QLabel(title, parent);
    combo_title->setToolTip(tooltip);
    combo_title->setStyleSheet("color: #262E30; font-size: 13px;");
    combo_title->setMinimumWidth(100);
    lay->addWidget(combo_title);

    auto *combobox = new ::ComboBox(parent);
    combobox->setFixedWidth(160);
    m_combobox_list[static_cast<int>(m_combobox_list.size())] = combobox;

    for (auto &lbl : vlist)
        combobox->Append(lbl);

    int cur_level = 1;
    if (auto *c = wxGetApp().app_config) {
        std::string lv = c->get("loglevel");
        if (!lv.empty()) try { cur_level = std::stoi(lv); } catch (...) {}
    }
    if (cur_level >= 0 && cur_level < (int)vlist.size())
        combobox->SetSelection(cur_level);

    lay->addWidget(combobox);
    lay->addStretch(1);

    connect(combobox, &::ComboBox::selectionChanged, this, [this](int idx) {
        if (auto *c = wxGetApp().app_config) {
            c->set("loglevel", std::to_string(idx));
            c->save();
        }
    });

    return lay;
}

// ---------------------------------------------------------------------------
// create_item_checkbox
// ---------------------------------------------------------------------------
QLayout *PreferencesDialog::create_item_checkbox(
    QString title, QWidget *parent, QString tooltip, int padding_left, std::string param)
{
    auto *lay = new QHBoxLayout();
    lay->setContentsMargins(padding_left, 0, 0, 0);
    lay->setSpacing(8);

    auto *checkbox = new ::CheckBox(parent);
    m_checkbox_list[static_cast<int>(m_checkbox_list.size())] = checkbox;

    if (auto *ac = wxGetApp().app_config) {
        bool val = false;
        if (param == "privacyuse")
            val = (ac->get("firstguide", "privacyuse") == "true");
        else if (param == "auto_stop_liveview")
            val = (ac->get("liveview", "auto_stop_liveview") != "true");
        else
            val = (ac->get(param) == "true");
        checkbox->setChecked(val);
    }

    if (param == "developer_mode")          m_developer_mode_ckeckbox = checkbox;
    if (param == "internal_developer_mode") m_internal_developer_mode_ckeckbox = checkbox;

    auto *lbl = new QLabel(title, parent);
    lbl->setToolTip(tooltip);
    lbl->setStyleSheet("color: #262E30; font-size: 13px;");

    lay->addWidget(checkbox);
    lay->addWidget(lbl);
    lay->addStretch(1);

    connect(checkbox, &QAbstractButton::toggled, this,
            [this, param](bool checked) {
        AppConfig *ac2 = wxGetApp().app_config;
        if (!ac2) return;
        if (param == "privacyuse") {
            ac2->set("firstguide", "privacyuse", checked ? std::string("true") : std::string("false"));
            if (!checked) {
                if (auto *agent = wxGetApp().getAgent()) {
                    agent->track_enable(false);
                    agent->track_remove_files();
                }
            }
            wxGetApp().save_privacy_policy_history(checked, "preferences");
            ac2->save();
        } else if (param == "auto_stop_liveview") {
            ac2->set("liveview", "auto_stop_liveview", !checked ? std::string("true") : std::string("false"));
        } else {
            ac2->set_bool(param, checked);
            ac2->save();
        }
        if (param == "staff_pick_switch") {
            wxGetApp().switch_staff_pick(ac2->get("staff_pick_switch") == "true");
        }
        if (param == "backup_switch") {
            bool pbool = (ac2->get("backup_switch") == "true");
            std::string backup_interval = "10";
            ac2->get("backup_interval", backup_interval);
            try { Slic3r::set_backup_interval(pbool ? std::stol(backup_interval) : 0); } catch (...) {}
            if (m_backup_interval_textinput)
                m_backup_interval_textinput->Enable(pbool);
        }
    });

    return lay;
}

// ---------------------------------------------------------------------------
// create_item_darkmode_checkbox
// ---------------------------------------------------------------------------
QLayout *PreferencesDialog::create_item_darkmode_checkbox(
    QString title, QWidget *parent, QString tooltip, int padding_left, std::string param)
{
    auto *lay = create_item_checkbox(title, parent, tooltip, padding_left, param);
    if (!m_checkbox_list.empty())
        m_dark_mode_ckeckbox = m_checkbox_list.at(static_cast<int>(m_checkbox_list.size()) - 1);
    return lay;
}

void PreferencesDialog::set_dark_mode() {}

// ---------------------------------------------------------------------------
// create_item_button — description label + ::Button
// ---------------------------------------------------------------------------
QLayout *PreferencesDialog::create_item_button(
    QString title, QString title2, QWidget *parent, QString tooltip,
    std::function<void()> onclick)
{
    auto *lay = new QHBoxLayout();
    lay->setContentsMargins(23, 0, 0, 0);
    lay->setSpacing(5);

    auto *lbl = new QLabel(title, parent);
    lbl->setStyleSheet("color: #262E30; font-size: 13px;");
    lay->addWidget(lbl);

    auto *btn = new ::Button(parent, title2);
    m_button_list[static_cast<int>(m_button_list.size())] = btn;
    btn->setMinimumSize(QSize(58, 22));
    btn->SetCornerRadius(12);
    if (!tooltip.isEmpty()) btn->setToolTip(tooltip);

    connect(btn, &::Button::clicked, this, [onclick]() {
        if (onclick) onclick();
    });

    lay->addWidget(btn);
    lay->addStretch(1);
    return lay;
}

// ---------------------------------------------------------------------------
// create_item_downloads — download path label + Browse button
// ---------------------------------------------------------------------------
QWidget *PreferencesDialog::create_item_downloads(
    QWidget *parent, int /*padding_left*/, std::string /*param*/)
{
    AppConfig *config   = wxGetApp().app_config;
    QString download_path = config ? QString::fromStdString(config->get("download_path")) : QString{};

    auto *item_panel = new QWidget(parent);
    auto *lay        = new QHBoxLayout(item_panel);
    lay->setContentsMargins(23, 0, 0, 0);
    lay->setSpacing(5);

    auto *path_lbl = new QLabel(download_path, item_panel);
    path_lbl->setStyleSheet("color: #909090; font-size: 13px;");
    lay->addWidget(path_lbl);

    auto *btn = new ::Button(item_panel, _L("Browse"));
    m_button_list[static_cast<int>(m_button_list.size())] = btn;
    btn->setMinimumSize(QSize(58, 22));
    btn->SetCornerRadius(12);

    connect(btn, &::Button::clicked, this, [this, path_lbl]() {
        QString dir = QFileDialog::getExistingDirectory(
            this, _L("Choose Download Directory"),
            path_lbl->text().isEmpty() ? QDir::homePath() : path_lbl->text());
        if (!dir.isEmpty()) {
            path_lbl->setText(dir);
            if (auto *c = wxGetApp().app_config) {
                c->set("download_path", dir.toStdString());
                c->save();
            }
        }
    });

    lay->addWidget(btn);
    lay->addStretch(1);
    return item_panel;
}

// ---------------------------------------------------------------------------
// create_item_input — title label + ::TextInput
// ---------------------------------------------------------------------------
QLayout *PreferencesDialog::create_item_input(
    QString title, QString title2, QWidget *parent, QString tooltip,
    std::string param, std::function<void(QString)> onchange)
{
    auto *lay = new QHBoxLayout();
    lay->setContentsMargins(23, 0, 0, 0);
    lay->setSpacing(3);

    auto *input_title = new QLabel(title, parent);
    input_title->setToolTip(tooltip);
    input_title->setStyleSheet("color: #262E30; font-size: 13px;");
    lay->addWidget(input_title);

    auto *input = new ::TextInput(parent, QString{}, QString{}, QString{},
                                  QPoint{}, QSize{100, -1}, 0);
    if (auto *c = wxGetApp().app_config)
        input->GetTextCtrl()->setText(QString::fromStdString(c->get(param)));
    input->GetTextCtrl()->setValidator(new QRegularExpressionValidator(
        QRegularExpression("[0-9]*"), input->GetTextCtrl()));

    lay->addWidget(input);
    lay->addSpacing(3);
    if (!title2.isEmpty()) {
        auto *second_title = new QLabel(title2, parent);
        second_title->setToolTip(tooltip);
        second_title->setStyleSheet("color: #262E30; font-size: 13px;");
        lay->addWidget(second_title);
    }
    lay->addStretch(1);

    auto save_fn = [this, param, input, onchange]() {
        QString val = input->GetTextCtrl()->text();
        if (auto *c = wxGetApp().app_config) {
            c->set(param, val.toStdString());
            c->save();
        }
        if (onchange) onchange(val);
    };
    connect(input->GetTextCtrl(), &QLineEdit::returnPressed, this, save_fn);
    connect(input->GetTextCtrl(), &QLineEdit::editingFinished, this, save_fn);
    return lay;
}

// ---------------------------------------------------------------------------
// create_item_range_input — validated numeric input within [range_min, range_max]
// ---------------------------------------------------------------------------
QLayout *PreferencesDialog::create_item_range_input(
    QString title, QWidget *parent, QString tooltip,
    std::string param, float range_min, float range_max,
    int /*keep_digital*/, std::function<void(QString)> onchange)
{
    auto *lay = new QHBoxLayout();
    lay->setContentsMargins(23, 0, 0, 0);
    lay->setSpacing(3);

    auto *input_title = new QLabel(title, parent);
    input_title->setToolTip(tooltip);
    input_title->setStyleSheet("color: #262E30; font-size: 13px;");
    lay->addWidget(input_title);

    auto *input = new ::TextInput(parent, QString{}, QString{}, QString{},
                                  QPoint{}, QSize{100, -1}, 0);
    if (auto *c = wxGetApp().app_config) {
        double float_value = 0.0;
        try { float_value = std::stod(c->get(param)); } catch (...) {}
        if (float_value < range_min || float_value > range_max) {
            float_value = range_min;
            c->set(param, std::to_string(range_min));
            c->save();
        }
        input->GetTextCtrl()->setText(QString::fromStdString(c->get(param)));
    }
    input->GetTextCtrl()->setValidator(new QRegularExpressionValidator(
        QRegularExpression("[0-9]*\\.?[0-9]*"), input->GetTextCtrl()));

    lay->addWidget(input);
    lay->addStretch(1);

    auto save_fn = [this, param, input, range_min, range_max, onchange]() {
        QString val = input->GetTextCtrl()->text();
        bool ok = false;
        double d = val.toDouble(&ok);
        if (!ok || d < range_min || d > range_max) return;
        if (auto *c = wxGetApp().app_config) {
            c->set(param, val.toStdString());
            c->save();
        }
        if (onchange) onchange(val);
    };
    connect(input->GetTextCtrl(), &QLineEdit::returnPressed, this, save_fn);
    connect(input->GetTextCtrl(), &QLineEdit::editingFinished, this, save_fn);
    return lay;
}

// ---------------------------------------------------------------------------
// create_item_range_two_input — two validated inputs (tooltip offset x/y)
// ---------------------------------------------------------------------------
QLayout *PreferencesDialog::create_item_range_two_input(
    QString title, QWidget *parent, QString tooltip,
    std::string param, std::string param1,
    float range_min, float range_max, int /*keep_digital*/,
    std::function<void(QString)> onchange,
    std::function<void(QString)> onchange1)
{
    auto *lay = new QHBoxLayout();
    lay->setContentsMargins(23, 0, 0, 0);
    lay->setSpacing(3);

    auto *input_title = new QLabel(title, parent);
    input_title->setToolTip(tooltip);
    input_title->setStyleSheet("color: #262E30; font-size: 13px;");
    lay->addWidget(input_title);

    auto *input_x = new ::TextInput(parent, QString{}, QString{}, QString{},
                                    QPoint{}, QSize{100, -1}, 0);
    auto *input_y = new ::TextInput(parent, QString{}, QString{}, QString{},
                                    QPoint{}, QSize{100, -1}, 0);
    if (auto *c = wxGetApp().app_config) {
        input_x->GetTextCtrl()->setText(QString::fromStdString(c->get(param)));
        input_y->GetTextCtrl()->setText(QString::fromStdString(c->get(param1)));
    }
    lay->addWidget(input_x);
    lay->addWidget(input_y);
    lay->addStretch(1);

    auto save_x = [this, param, input_x, range_min, range_max, onchange]() {
        QString val = input_x->GetTextCtrl()->text();
        bool ok = false; double d = val.toDouble(&ok);
        if (!ok || d < range_min || d > range_max) return;
        if (auto *c = wxGetApp().app_config) c->set(param, val.toStdString());
        if (onchange) onchange(val);
    };
    auto save_y = [this, param1, input_y, range_min, range_max, onchange1]() {
        QString val = input_y->GetTextCtrl()->text();
        bool ok = false; double d = val.toDouble(&ok);
        if (!ok || d < range_min || d > range_max) return;
        if (auto *c = wxGetApp().app_config) c->set(param1, val.toStdString());
        if (onchange1) onchange1(val);
    };
    connect(input_x->GetTextCtrl(), &QLineEdit::editingFinished, this, save_x);
    connect(input_y->GetTextCtrl(), &QLineEdit::editingFinished, this, save_y);
    return lay;
}

// ---------------------------------------------------------------------------
// create_item_backup_input — specific backup interval input
// ---------------------------------------------------------------------------
QLayout *PreferencesDialog::create_item_backup_input(
    QString title, QWidget *parent, QString tooltip, std::string param)
{
    auto *lay = new QHBoxLayout();
    lay->setContentsMargins(23, 0, 0, 0);
    lay->setSpacing(3);

    auto *input_title = new QLabel(title, parent);
    input_title->setToolTip(tooltip);
    input_title->setStyleSheet("color: #262E30; font-size: 13px;");
    lay->addWidget(input_title);

    auto *input = new ::TextInput(parent, QString{}, QString{}, QString{},
                                  QPoint{}, QSize{100, -1}, 0);
    if (auto *c = wxGetApp().app_config)
        input->GetTextCtrl()->setText(QString::fromStdString(c->get(param)));
    input->GetTextCtrl()->setValidator(new QIntValidator(1, 86400, input->GetTextCtrl()));

    lay->addWidget(input);
    lay->addSpacing(3);

    auto *second = new QLabel(_L("Second"), parent);
    second->setStyleSheet("color: #262E30; font-size: 13px;");
    lay->addWidget(second);
    lay->addStretch(1);

    if (param == "backup_interval") {
        m_backup_interval_textinput = input;
        if (auto *c = wxGetApp().app_config)
            input->Enable(c->get("backup_switch") == "true");
    }

    connect(input->GetTextCtrl(), &QLineEdit::textChanged, this, [this, input](const QString &) {
        m_backup_interval_time = input->GetTextCtrl()->text();
    });

    auto save_fn = [this, input]() {
        m_backup_interval_time = input->GetTextCtrl()->text();
        if (auto *c = wxGetApp().app_config) {
            c->set("backup_interval", m_backup_interval_time.toStdString());
            c->save();
            bool ok = false;
            long interval = m_backup_interval_time.toLong(&ok);
            if (ok) Slic3r::set_backup_interval(interval);
        }
    };
    connect(input->GetTextCtrl(), &QLineEdit::returnPressed, this, save_fn);
    connect(input->GetTextCtrl(), &QLineEdit::editingFinished, this, save_fn);
    return lay;
}

// ---------------------------------------------------------------------------
// create_item_multiple_combobox (stub)
// ---------------------------------------------------------------------------
QLayout *PreferencesDialog::create_item_multiple_combobox(
    QString title, QWidget *parent, QString tooltip, int padding_left,
    std::string parama, std::vector<QString> vlista, std::vector<QString> vlistb)
{
    (void)title; (void)parent; (void)tooltip; (void)padding_left;
    (void)parama; (void)vlista; (void)vlistb;
    return new QHBoxLayout();
}

// ---------------------------------------------------------------------------
// create_item_switch (stub — not used in current release)
// ---------------------------------------------------------------------------
QLayout *PreferencesDialog::create_item_switch(
    QString title, QWidget *parent, QString tooltip, std::string param)
{
    (void)title; (void)parent; (void)tooltip; (void)param;
    return new QHBoxLayout();
}

// ---------------------------------------------------------------------------
// create_item_radiobox — RadioBox + label
// ---------------------------------------------------------------------------
QWidget *PreferencesDialog::create_item_radiobox(
    QString title, QWidget *parent, QString tooltip,
    int padding_left, int groupid, std::string param)
{
    auto *item = new QWidget(parent);
    item->setFixedHeight(28);

    auto *lay = new QHBoxLayout(item);
    lay->setContentsMargins(padding_left, 0, 0, 0);
    lay->setSpacing(8);

    auto *radiobox = new RadioBox(item);
    m_radiobox_list[static_cast<int>(m_radiobox_list.size())] = radiobox;

    RadioSelector rs;
    rs.m_groupid    = groupid;
    rs.m_param_name = QString::fromStdString(param);
    rs.m_radiobox   = radiobox;
    rs.m_selected   = false;
    m_radio_group.push_back(rs);

    auto *text = new QLabel(title, item);
    text->setToolTip(tooltip);
    text->setStyleSheet("color: #262E30; font-size: 13px;");

    lay->addWidget(radiobox);
    lay->addWidget(text);
    lay->addStretch(1);

    connect(radiobox, &QAbstractButton::clicked, this, [this, param]() {
        on_select_radio(param);
    });

    return item;
}

// ---------------------------------------------------------------------------
// create_general_page
// ---------------------------------------------------------------------------
QWidget *PreferencesDialog::create_general_page()
{
    auto *page = new QWidget(m_scrolledWindow);
    page->setStyleSheet("background: white;");
    auto *sizer_page = new QVBoxLayout(page);
    sizer_page->setContentsMargins(0, 0, 0, 0);
    sizer_page->setSpacing(3);

    // ---- General Settings -------------------------------------------------
    auto *title_general_settings = create_item_title(
        _L("General Settings"), page, _L("General Settings"));

    std::vector<std::string> language_list = {
        "English",
        u8"\u4e2d\u6587(\u7b80\u4f53)",
        u8"\u4e2d\u6587(\u7e41\u9ad4)",
        "Deutsch",
        u8"Espa\u00f1ol",
        u8"Fran\u00e7ais",
        "italiano",
        u8"\u65e5\u672c\u8a9e",
        u8"\ud55c\uad6d\uc5b4",
        "Nederlands",
        "Polski",
        u8"\u0420\u0443\u0441\u0441\u043a\u0438\u0439",
        "Svenska",
        "Magyar",
        u8"\u0423\u043a\u0440\u0430\u0457\u043d\u0441\u044c\u043a\u0430",
        u8"Portugu\u00eas (Brasil)",
        u8"T\u00fcrk\u00e7e",
        u8"\u010de\u0161tina"
    };
    auto *item_language = create_item_language_combobox(
        _L("Language"), page, _L("Language"), 50, "language", language_list);

    std::vector<QString> Regions = {
        _L("Asia-Pacific"), _L("Chinese Mainland"), _L("Europe"),
        _L("North America"), _L("Others")
    };
    auto *item_region = create_item_region_combobox(
        _L("Login Region"), page, _L("Login Region"), Regions);

    std::vector<QString>      Units       = {_L("Metric") + " (mm, g)", _L("Imperial") + " (in, oz)"};
    std::vector<std::string>  UnitsValues = {"0", "1"};
    auto *item_currency = create_item_combobox(
        _L("Units"), page, _L("Units"), "use_inches", Units, UnitsValues);

    std::vector<QString>     FlushOptionLabels = {_L("All"), _L("Color change"), _L("Disabled")};
    std::vector<std::string> FlushOptionValues = {"all", "color change", "disabled"};
    auto *item_auto_flush = create_item_combobox(
        _L("Auto Flush"), page, _L("Auto calculate flush volumes"),
        "auto_calculate_flush", FlushOptionLabels, FlushOptionValues);

    auto *item_single_instance = create_item_checkbox(
        _L("Keep only one Bambu Studio instance"), page,
        _L("If this is enabled, when starting Bambu Studio and another instance is already running, "
           "that instance will be reactivated instead."),
        50, "single_instance");

    auto *item_bed_type_follow_preset = create_item_checkbox(
        _L("Auto plate type"), page,
        _L("Studio will remember build plate selected last time for certain printer model."),
        50, "user_bed_type");

    auto *item_multi_machine = create_item_checkbox(
        _L("Multi-device Management(Take effect after restarting Studio)."), page,
        _L("With this option enabled, you can send a task to multiple devices at the same time "
           "and manage multiple devices."),
        50, "enable_multi_machine");

    auto *item_12h_time_format = create_item_checkbox(
        _L("Use 12-hour time format"), page,
        _L("Display time in 12-hour format with AM/PM instead of 24-hour format"),
        50, "use_12h_time_format");

    auto *item_step_mesh_setting = create_item_checkbox(
        _L("Show the step mesh parameter setting dialog."), page,
        _L("If enabled, a parameter settings dialog will appear during STEP file import."),
        50, "enable_step_mesh_setting");

    auto *item_beta_version_update = create_item_checkbox(
        _L("Support beta version update."), page,
        _L("With this option enabled, you can receive beta version updates."),
        50, "enable_beta_version_update");

    auto *item_auto_transfer_when_switch_preset = create_item_checkbox(
        _L("Automatically transfer modified value when switching process and filament presets"), page,
        _L("After closing, a popup will appear to ask each time"),
        50, "auto_transfer_when_switch_preset");

    auto *item_mix_print_high_low_temperature = create_item_checkbox(
        _L("Remove the restriction on mixed printing of high and low temperature filaments."), page,
        _L("With this option enabled, you can print materials with a large temperature difference together."),
        50, "enable_high_low_temp_mixed_printing");

    auto *item_camera_fullscreen_active_monitor_only = create_item_checkbox(
        _L("Open full screen camera view on active monitor only."), page,
        _L("When enabled, the camera full screen view opens only on the monitor that contains Bambu Studio."),
        50, "camera_fullscreen_active_monitor_only");

    auto *item_restore_hide_pop_ups = create_item_button(
        _L("Clear my choice for synchronizing printer preset after loading the file."),
        _L("Clear"), page, {}, []() {
            if (auto *c = wxGetApp().app_config)
                c->erase("app", "sync_after_load_file_show_flag");
        });

    auto *item_restore_hide_3mf_info = create_item_button(
        _L("Clear my choice for Load 3mf dialog settings."),
        _L("Clear"), page, {}, []() {
            if (auto *c = wxGetApp().app_config)
                c->erase("app", "skip_non_bambu_3mf_warning");
        });

    // ---- 3D Settings -------------------------------------------------------
    auto *_3d_settings = create_item_title(_L("3D Settings"), page, _L("3D Settings"));

    auto *item_mouse_zoom_settings = create_item_checkbox(
        _L("Zoom to mouse position"), page,
        _L("Zoom in towards the mouse pointer's position in the 3D view, rather than the 2D window center."),
        50, "zoom_to_mouse");

    auto *item_show_shells_in_preview_settings = create_item_checkbox(
        _L("Always show shells in preview"), page,
        _L("Always show shells or not in preview view tab. If change value, you should reslice."),
        50, "show_shells_in_preview");

    auto *item_import_single_svg_and_split = create_item_checkbox(
        _L("Import a single SVG and split it"), page,
        _L("Import a single SVG and then split it to several parts."),
        50, "import_single_svg_and_split");

    auto *item_gamma_correct_in_import_obj = create_item_checkbox(
        _L("Enable gamma correction for the imported obj file"), page,
        _L("Perform gamma correction on color after importing the obj model."),
        50, "gamma_correct_in_import_obj");

    auto *item_enable_record_gcodeviewer_option_item = create_item_checkbox(
        _L("Remember last used color scheme"), page,
        _L("When enabled, the last used color scheme will be automatically applied on next startup."),
        50, "enable_record_gcodeviewer_option_item");

    std::vector<QString>     assemble_options = {_L("Auto"), _L("Open"), _L("Close")};
    std::vector<std::string> assemble_values  = {"Auto", "Open", "Close"};
    auto *enable_assemble_view_preview_settings = create_item_combobox(
        _L("Display overview"), page, _L("Display overview"),
        "enable_assemble_view_preview", assemble_options, assemble_values);

    auto *enable_lod_settings = create_item_checkbox(
        _L("Improve rendering performance by lod"), page,
        _L("Improved rendering performance under the scene of multiple plates and many models."),
        50, "enable_lod");

    auto *enable_advanced_gcode_viewer = create_item_checkbox(
        _L("Enable advanced gcode viewer"), page,
        _L("Enable advanced gcode viewer."),
        50, "enable_advanced_gcode_viewer_");

    std::vector<QString>     toolbar_style_labels = {_L("Collapsible"), _L("Uncollapsible")};
    std::vector<std::string> toolbar_style_values = {"0", "1"};
    auto *item_toolbar_style = create_item_combobox(
        _L("Toolbar Style"), page, _L("Toolbar Style"),
        "toolbar_style", toolbar_style_labels, toolbar_style_values);

    float range_min = 1.0f, range_max = 2.5f;
    auto *item_grabber_size_settings = create_item_range_input(
        _L("Grabber scale"), page,
        _L("Set grabber size for move, rotate, scale tool. Value range: [1.0, 2.5]"),
        "grabber_size_factor", range_min, range_max, 1, nullptr);

    range_min = 0.0f; range_max = 150.0f;
    auto *item_tooltip_offset_size_settings = create_item_range_two_input(
        _L("Tooltip offset"), page,
        _L("Set tooltip offset for different mouse size. Value range: [0, 150]"),
        "3d_middle_tooltip_offset_x", "3d_middle_tooltip_offset_y",
        range_min, range_max, 1, nullptr, nullptr);

    // ---- Presets -----------------------------------------------------------
    auto *title_presets = create_item_title(_L("Presets"), page, _L("Presets"));

    auto *item_user_sync = create_item_checkbox(
        _L("Auto sync user presets (Printer/Filament/Process)"), page,
        _L("If enabled, auto sync user presets with cloud after Bambu Studio startup or presets modified."),
        50, "sync_user_preset");

    auto *item_system_sync = create_item_checkbox(
        _L("Auto check for system presets updates"), page,
        _L("If enabled, auto check whether there are system presets updates after Bambu Studio startup."),
        50, "sync_system_preset");

    // ---- Online Models (only shown when has_model_mall() is true) -----------
    // Declared as pointers but only built + added when the condition is met;
    // mirrors wx behaviour: items are shown/hidden based on has_model_mall().
    const bool show_modelmall = wxGetApp().has_model_mall();
    QLayout *title_modelmall   = nullptr;
    QLayout *item_modelmall    = nullptr;
    QLayout *item_show_history = nullptr;
    if (show_modelmall) {
        title_modelmall = create_item_title(_L("Online Models"), page, _L("Online Models"));
        item_modelmall  = create_item_checkbox(
            _L("Show online staff-picked models on the home page"), page,
            _L("Show online staff-picked models on the home page"),
            50, "staff_pick_switch");
        item_show_history = create_item_checkbox(
            _L("Show history on the home page"), page,
            _L("Show history on the home page"),
            50, "show_print_history");
    }

    // ---- Project -----------------------------------------------------------
    auto *title_project = create_item_title(_L("Project"), page, {});

    auto *item_max_recent_count = create_item_input(
        _L("Maximum recent projects"), {}, page,
        _L("Maximum count of recent projects"), "max_recent_count",
        [](QString value) {
            bool ok = false;
            int max = value.toInt(&ok);
            if (ok && wxGetApp().mainframe)
                wxGetApp().mainframe->set_max_recent_count(max);
        });

    auto *item_save_choise = create_item_button(
        _L("Clear my choice on the unsaved projects."), _L("Clear"), page, {}, []() {
            if (auto *c = wxGetApp().app_config)
                c->set("save_project_choise", "");
        });

    auto *item_gcodes_warning = create_item_checkbox(
        _L("No warnings when loading 3MF with modified G-codes"), page,
        _L("No warnings when loading 3MF with modified G-codes"),
        50, "no_warn_when_modified_gcodes");

    auto *item_backup = create_item_checkbox(
        _L("Auto-Backup"), page,
        _L("Backup your project periodically for restoring from the occasional crash."),
        50, "backup_switch");

    auto *item_backup_interval = create_item_backup_input(
        _L("every"), page, _L("The period of backup in seconds."), "backup_interval");

    // ---- Downloads ---------------------------------------------------------
    auto *title_downloads = create_item_title(_L("Downloads"), page, _L("Downloads"));
    auto *item_downloads  = create_item_downloads(page, 50, "download_path");

    // ---- Media -------------------------------------------------------------
    auto *title_media = create_item_title(_L("Media"), page, _L("Media"));

    auto *item_auto_stop_liveview = create_item_checkbox(
        _L("Keep liveview when printing."), page,
        _L("By default, Liveview will pause after 15 minutes of inactivity on the computer. "
           "Check this box to disable this feature during printing."),
        50, "auto_stop_liveview");

    // ---- User Experience ---------------------------------------------------
    auto *title_user_experience = create_item_title(
        _L("User Experience"), page, _L("User Experience"));

    auto *item_priv_policy = create_item_checkbox(
        _L("Join the User Experience Improvement Program."), page,
        {}, 50, "privacyuse");
    // "Learn more" hyperlink — added inline to same row (mirrors wx: item_priv_policy->Add(hyperlink))
    auto *hyperlink = new QLabel(
        QStringLiteral("<a href='#' style='color:#0078D4;font-weight:bold;'>%1</a>")
            .arg(_L("Learn more")),
        page);
    hyperlink->setCursor(Qt::PointingHandCursor);
    hyperlink->setOpenExternalLinks(false);
    connect(hyperlink, &QLabel::linkActivated, this, [this](const QString &) {
        UxProgramTermsDialog dlg(this);
        dlg.exec();
    });
    // Add hyperlink to the same QHBoxLayout row as the checkbox
    if (auto *row = qobject_cast<QHBoxLayout*>(item_priv_policy))
        row->addWidget(hyperlink);
    else
        item_priv_policy->addItem(new QWidgetItem(hyperlink));

    // ---- Develop Mode ------------------------------------------------------
    auto *title_develop_mode = create_item_title(_L("Develop Mode"), page, _L("Develop Mode"));

    auto *item_develop_mode = create_item_checkbox(
        _L("Develop mode"), page, _L("Develop mode"), 50, "developer_mode");

    auto *item_skip_ams_blacklist_check = create_item_checkbox(
        _L("Skip AMS blacklist check"), page,
        _L("Skip AMS blacklist check"), 50, "skip_ams_blacklist_check");

    // ---- Assemble sizer — ordering matches original wx exactly ---------------
    sizer_page->addLayout(title_general_settings);
    sizer_page->addSpacing(3);
    sizer_page->addLayout(item_language);
    sizer_page->addLayout(item_region);
    sizer_page->addLayout(item_currency);
    sizer_page->addLayout(item_12h_time_format);          // same position as wx
    sizer_page->addLayout(item_single_instance);
    sizer_page->addLayout(item_auto_transfer_when_switch_preset);
    sizer_page->addLayout(item_bed_type_follow_preset);
    sizer_page->addLayout(item_auto_flush);
    sizer_page->addLayout(item_multi_machine);
    sizer_page->addLayout(item_step_mesh_setting);
    sizer_page->addLayout(item_beta_version_update);
    sizer_page->addLayout(item_mix_print_high_low_temperature);
    sizer_page->addLayout(item_camera_fullscreen_active_monitor_only);
    sizer_page->addLayout(item_restore_hide_pop_ups);
    sizer_page->addLayout(item_restore_hide_3mf_info);

    sizer_page->addSpacing(20);
    sizer_page->addLayout(_3d_settings);
    sizer_page->addSpacing(3);
    sizer_page->addLayout(item_mouse_zoom_settings);
    sizer_page->addLayout(item_show_shells_in_preview_settings);
    sizer_page->addLayout(item_import_single_svg_and_split);
    sizer_page->addLayout(item_gamma_correct_in_import_obj);
    sizer_page->addLayout(item_enable_record_gcodeviewer_option_item);
    sizer_page->addLayout(enable_assemble_view_preview_settings);
    sizer_page->addLayout(enable_lod_settings);
    sizer_page->addLayout(enable_advanced_gcode_viewer);
    sizer_page->addLayout(item_toolbar_style);
    sizer_page->addLayout(item_grabber_size_settings);
    sizer_page->addLayout(item_tooltip_offset_size_settings);

    sizer_page->addSpacing(20);
    sizer_page->addLayout(title_presets);
    sizer_page->addSpacing(3);
    sizer_page->addLayout(item_user_sync);
    sizer_page->addLayout(item_system_sync);

    // Online Models — conditionally shown based on has_model_mall() (mirrors wx)
    if (show_modelmall && title_modelmall && item_modelmall && item_show_history) {
        sizer_page->addSpacing(20);
        sizer_page->addLayout(title_modelmall);
        sizer_page->addSpacing(3);
        sizer_page->addLayout(item_modelmall);
        sizer_page->addLayout(item_show_history);
    }

    sizer_page->addSpacing(20);
    sizer_page->addLayout(title_project);
    sizer_page->addSpacing(3);
    sizer_page->addLayout(item_max_recent_count);
    sizer_page->addLayout(item_save_choise);
    sizer_page->addLayout(item_gcodes_warning);
    {
        auto *backup_row = new QHBoxLayout();
        backup_row->setContentsMargins(0, 0, 0, 0);
        backup_row->setSpacing(0);
        backup_row->addLayout(item_backup);
        backup_row->addLayout(item_backup_interval);
        sizer_page->addLayout(backup_row);
    }

    sizer_page->addSpacing(20);
    sizer_page->addLayout(title_downloads);
    sizer_page->addSpacing(3);
    sizer_page->addWidget(item_downloads);

    sizer_page->addSpacing(20);
    sizer_page->addLayout(title_media);
    sizer_page->addSpacing(3);
    sizer_page->addLayout(item_auto_stop_liveview);

    sizer_page->addSpacing(20);
    sizer_page->addLayout(title_user_experience);
    sizer_page->addSpacing(3);
    sizer_page->addLayout(item_priv_policy);  // hyperlink already appended inline

    sizer_page->addSpacing(20);
    sizer_page->addLayout(title_develop_mode);
    sizer_page->addSpacing(3);
    sizer_page->addLayout(item_develop_mode);
    sizer_page->addLayout(item_skip_ams_blacklist_check);

    sizer_page->addStretch(1);
    return page;
}

// ---------------------------------------------------------------------------
// create_debug_page — developer/debug options
// ---------------------------------------------------------------------------
QWidget *PreferencesDialog::create_debug_page()
{
    auto *page = new QWidget(m_scrolledWindow);
    page->setStyleSheet("background: white;");

    if (auto *ac = wxGetApp().app_config) {
        m_internal_developer_mode_def = QString::fromStdString(ac->get("internal_developer_mode"));
        m_backup_interval_def         = QString::fromStdString(ac->get("backup_interval"));
        m_iot_environment_def         = QString::fromStdString(ac->get("iot_environment"));
    }

    auto *bSizer = new QVBoxLayout(page);
    bSizer->setContentsMargins(0, 0, 0, 0);
    bSizer->setSpacing(3);

    auto *enable_ssl_for_mqtt = create_item_checkbox(
        _L("Enable SSL(MQTT)"), page, _L("Enable SSL(MQTT)"), 50, "enable_ssl_for_mqtt");
    auto *enable_ssl_for_ftp = create_item_checkbox(
        _L("Enable SSL(FTP)"), page, _L("Enable SSL(FTP)"), 50, "enable_ssl_for_ftp");
    auto *item_internal_developer = create_item_checkbox(
        _L("Internal developer mode"), page, _L("Internal developer mode"), 50, "internal_developer_mode");

    auto *title_log_level = create_item_title(_L("Log Level"), page, _L("Log Level"));
    std::vector<QString> log_level_list = {
        _L("fatal"), _L("error"), _L("warning"), _L("info"), _L("debug"), _L("trace")
    };
    auto *loglevel_combox = create_item_loglevel_combobox(
        _L("Log Level"), page, _L("Log Level"), log_level_list);

    auto *title_host = create_item_title(_L("Host Setting"), page, _L("Host Setting"));
    auto *radio1 = create_item_radiobox(
        _L("DEV host: api-dev.bambu-lab.com/v1"), page, {}, 50, 1, "dev_host");
    auto *radio2 = create_item_radiobox(
        _L("QA  host: api-qa.bambu-lab.com/v1"), page, {}, 50, 1, "qa_host");
    auto *radio3 = create_item_radiobox(
        _L("PRE host: api-pre.bambu-lab.com/v1"), page, {}, 50, 1, "pre_host");
    auto *radio4 = create_item_radiobox(
        _L("Product host"), page, {}, 50, 1, "product_host");

    std::string iot_env = m_iot_environment_def.toStdString();
    if      (iot_env == ENV_DEV_HOST)     on_select_radio("dev_host");
    else if (iot_env == ENV_QAT_HOST)     on_select_radio("qa_host");
    else if (iot_env == ENV_PRE_HOST)     on_select_radio("pre_host");
    else if (iot_env == ENV_PRODUCT_HOST) on_select_radio("product_host");

    auto *debug_button = new ::Button(page, _L("debug save button"));
    m_button_list[static_cast<int>(m_button_list.size())] = debug_button;

    connect(debug_button, &::Button::clicked, this, [this]() {
        auto btn = QMessageBox::question(this,
            _L("DEBUG settings have saved successfully!"),
            _L("save debug settings"),
            QMessageBox::Yes | QMessageBox::No | QMessageBox::NoButton);

        if (btn == QMessageBox::No) {
            if (m_backup_interval_def != m_backup_interval_time && m_backup_interval_textinput)
                m_backup_interval_textinput->GetTextCtrl()->setText(m_backup_interval_def);
            std::string iot = m_iot_environment_def.toStdString();
            if      (iot == ENV_DEV_HOST)     on_select_radio("dev_host");
            else if (iot == ENV_QAT_HOST)     on_select_radio("qa_host");
            else if (iot == ENV_PRE_HOST)     on_select_radio("pre_host");
            else if (iot == ENV_PRODUCT_HOST) on_select_radio("product_host");
        } else if (btn == QMessageBox::Yes) {
            auto param = get_select_radio(1).toStdString();
            AppConfig *config = wxGetApp().app_config;
            if (!config) return;
            if      (param == "dev_host")     config->set("iot_environment", ENV_DEV_HOST);
            else if (param == "qa_host")      config->set("iot_environment", ENV_QAT_HOST);
            else if (param == "pre_host")     config->set("iot_environment", ENV_PRE_HOST);
            else if (param == "product_host") config->set("iot_environment", ENV_PRODUCT_HOST);
            wxGetApp().update_publish_status();
            std::string country_code = config->get_country_code();
            if (auto *agent = wxGetApp().getAgent()) {
                wxGetApp().request_user_logout();
                agent->set_country_code(country_code);
            }
            config->save();
            try {
                Slic3r::set_backup_interval(std::stol(config->get("backup_interval")));
            } catch (...) {}
            accept();
        }
    });

    bSizer->addLayout(enable_ssl_for_mqtt);
    bSizer->addLayout(enable_ssl_for_ftp);
    bSizer->addLayout(item_internal_developer);
    bSizer->addSpacing(20);
    bSizer->addLayout(title_log_level);
    bSizer->addSpacing(3);
    bSizer->addLayout(loglevel_combox);
    bSizer->addSpacing(20);
    bSizer->addLayout(title_host);
    bSizer->addSpacing(3);
    bSizer->addWidget(radio1);
    bSizer->addWidget(radio2);
    bSizer->addWidget(radio3);
    bSizer->addWidget(radio4);
    bSizer->addSpacing(15);
    bSizer->addWidget(debug_button, 0, Qt::AlignHCenter);
    bSizer->addStretch(1);

    return page;
}

// ---------------------------------------------------------------------------
// Stubs for other pages
// ---------------------------------------------------------------------------
void PreferencesDialog::create_gui_page() {}
void PreferencesDialog::create_sync_page() {}
void PreferencesDialog::create_shortcuts_page() {}
void PreferencesDialog::create_select_domain_widget() {}
void PreferencesDialog::OnSelectTabel(int idx) { (void)idx; }
void PreferencesDialog::OnSelectRadio() {}

// ---------------------------------------------------------------------------
// create() — single-scrolled-page dialog
// ---------------------------------------------------------------------------
void PreferencesDialog::create()
{
    app_config = wxGetApp().app_config;
    if (app_config)
        m_backup_interval_time = QString::fromStdString(app_config->get("backup_interval"));

    setWindowTitle(_L("Preferences"));
    setMinimumWidth(580);

    auto *main_sizer = new QVBoxLayout(this);
    main_sizer->setContentsMargins(0, 0, 0, 0);
    main_sizer->setSpacing(0);

    // Top separator line
    auto *top_line = new QFrame(this);
    top_line->setFrameShape(QFrame::HLine);
    top_line->setFrameShadow(QFrame::Plain);
    top_line->setStyleSheet("color: #A6A9AA;");
    top_line->setFixedHeight(1);
    main_sizer->addWidget(top_line);

    m_scrolledWindow = new QScrollArea(this);
    m_scrolledWindow->setWidgetResizable(true);
    m_scrolledWindow->setFrameShape(QFrame::NoFrame);
    m_scrolledWindow->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    auto *body_widget = new QWidget(m_scrolledWindow);
    body_widget->setStyleSheet("background: white;");

    auto *body_layout = new QVBoxLayout(body_widget);
    body_layout->setContentsMargins(38, 28, 38, 28);
    body_layout->setSpacing(0);

    m_sizer_body = body_layout;

    auto *gp = create_general_page();
    body_layout->addWidget(gp);

#if !BBL_RELEASE_TO_PUBLIC
    auto *dp = create_debug_page();
    body_layout->addWidget(dp);
#endif

    m_scrolledWindow->setWidget(body_widget);
    main_sizer->addWidget(m_scrolledWindow, 1);

    QScreen *screen = QApplication::primaryScreen();
    if (screen) {
        int screen_h = screen->geometry().height();
        resize(sizeHint().width() + 40, static_cast<int>(screen_h * 0.7));
    }

    QPoint pos = this->pos();
    if (pos.y() < 0) move(pos.x(), 0);
}


}} // namespace Slic3r::GUI
