#include "KBShortcutsDialog.hpp"
#include "I18N.hpp"
#include "wxExtensions.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QLabel>
#include <QFont>
#include <QStackedWidget>
#include <QPushButton>
#include <QFrame>

namespace Slic3r { namespace GUI {

// ---------------------------------------------------------------------------
// KBShortcutsDialog
// ---------------------------------------------------------------------------

void KBShortcutsDialog::fill_shortcuts()
{
    // --- 3D Scene shortcuts ---
    Shortcuts scene_shortcuts = {
        {"A",                  std::string(_L("Arrange objects").toStdString())},
        {"Shift+A",            std::string(_L("Arrange objects on current plate").toStdString())},
        {"L",                  std::string(_L("Split to objects").toStdString())},
        {"M",                  std::string(_L("Split to parts").toStdString())},
        {"Delete",             std::string(_L("Delete selected").toStdString())},
        {"Ctrl+A",             std::string(_L("Select all objects").toStdString())},
        {"Ctrl+C",             std::string(_L("Copy").toStdString())},
        {"Ctrl+V",             std::string(_L("Paste").toStdString())},
        {"Ctrl+D",             std::string(_L("Duplicate").toStdString())},
        {"Ctrl+Z",             std::string(_L("Undo").toStdString())},
        {"Ctrl+Y",             std::string(_L("Redo").toStdString())},
        {"Ctrl+S",             std::string(_L("Save").toStdString())},
        {"Ctrl+Shift+S",       std::string(_L("Save as").toStdString())},
        {"F5",                 std::string(_L("Slice").toStdString())},
        {"+",                  std::string(_L("Zoom in").toStdString())},
        {"-",                  std::string(_L("Zoom out").toStdString())},
        {"Z",                  std::string(_L("Zoom to fit").toStdString())},
        {"Shift+Z",            std::string(_L("Zoom to selection").toStdString())},
        {"B",                  std::string(_L("Zoom to bed").toStdString())},
        {"N",                  std::string(_L("Show next layer").toStdString())},
        {"I",                  std::string(_L("Scale up by 10%").toStdString())},
        {"O",                  std::string(_L("Scale down by 10%").toStdString())},
    };
    m_full_shortcuts.push_back({{ _L("3D Scene"), "" }, scene_shortcuts});

    // --- Preview shortcuts ---
    Shortcuts preview_shortcuts = {
        {"Arrow Up",           std::string(_L("Move layer up").toStdString())},
        {"Arrow Down",         std::string(_L("Move layer down").toStdString())},
        {"Shift+Arrow Up",     std::string(_L("Move layer up by 10").toStdString())},
        {"Shift+Arrow Down",   std::string(_L("Move layer down by 10").toStdString())},
        {"W/S",                std::string(_L("Move layer range selection up/down").toStdString())},
    };
    m_full_shortcuts.push_back({{ _L("Preview"), "" }, preview_shortcuts});

    // --- Gizmo: Move ---
    Shortcuts move_shortcuts = {
        {"Arrow Left/Right",   std::string(_L("Move along X axis").toStdString())},
        {"Arrow Up/Down",      std::string(_L("Move along Y axis").toStdString())},
        {"Shift+...",          std::string(_L("Move by 1 mm step").toStdString())},
    };
    m_full_shortcuts.push_back({{ _L("Gizmo: Move"), "" }, move_shortcuts});

    // --- Gizmo: Rotate ---
    Shortcuts rotate_shortcuts = {
        {"Arrow Left/Right",   std::string(_L("Rotate around Z").toStdString())},
        {"Arrow Up/Down",      std::string(_L("Rotate around X").toStdString())},
        {"Shift+...",          std::string(_L("Rotate by 1 deg step").toStdString())},
    };
    m_full_shortcuts.push_back({{ _L("Gizmo: Rotate"), "" }, rotate_shortcuts});
}

KBShortcutsDialog::KBShortcutsDialog()
    : DPIDialog(nullptr)
{
    setWindowTitle(_L("Keyboard shortcuts"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet("background-color: white;");
    setMinimumSize(640, 480);

    fill_shortcuts();

    QFont normal_font = font();
    QFont bold_font = normal_font;
    bold_font.setBold(true);

    auto *outer = new QVBoxLayout(this);
    outer->setContentsMargins(0, 0, 0, 0);
    outer->setSpacing(0);

    // Header with logo
    auto *header_widget = create_header(this, bold_font);
    outer->addWidget(header_widget);

    // Body: tab selector (left) + stacked pages (right)
    m_sizer_body = new QHBoxLayout;
    m_sizer_body->setContentsMargins(8, 8, 8, 8);
    m_sizer_body->setSpacing(0);

    m_panel_selects = new QWidget(this);
    auto *select_vbox = new QVBoxLayout(m_panel_selects);
    select_vbox->setContentsMargins(0, 0, 0, 0);
    select_vbox->setSpacing(0);

    m_simplebook = new QStackedWidget(this);

    for (int i = 0; i < (int)m_full_shortcuts.size(); ++i) {
        const auto &item = m_full_shortcuts[i];
        auto *tab_btn = create_button(i, item.first.first);
        select_vbox->addWidget(tab_btn);

        auto *page = create_page(m_simplebook, item, normal_font, bold_font);
        m_pages.push_back(page);
        m_simplebook->addWidget(page);
    }
    select_vbox->addStretch(1);

    // Select the first tab
    if (!m_hash_selector.empty()) {
        auto *sel = m_hash_selector[0];
        if (sel) {
            m_simplebook->setCurrentIndex(0);
        }
    }

    m_sizer_right = new QVBoxLayout;
    m_sizer_right->addWidget(m_simplebook, 1);

    m_sizer_body->addWidget(m_panel_selects);
    m_sizer_body->addLayout(m_sizer_right, 1);

    outer->addLayout(m_sizer_body, 1);

    // Close button
    auto *btn_row = new QHBoxLayout;
    btn_row->setContentsMargins(8, 4, 8, 8);
    btn_row->addStretch(1);
    auto *close_btn = new QPushButton(_L("Close"), this);
    connect(close_btn, &QPushButton::clicked, this, &QDialog::accept);
    btn_row->addWidget(close_btn);
    outer->addLayout(btn_row);

    adjustSize();
}

QWidget *KBShortcutsDialog::create_button(int id, QString text)
{
    auto *btn = new QPushButton(text, m_panel_selects);
    btn->setFlat(true);
    btn->setCheckable(true);
    if (id == 0) btn->setChecked(true);

    auto *sel = new Select;
    sel->m_index      = id;
    sel->m_tab_button = btn;
    sel->m_tab_text   = btn;
    m_hash_selector[id] = sel;

    connect(btn, &QPushButton::clicked, this, [this, id] {
        m_simplebook->setCurrentIndex(id);
        for (auto &[idx, s] : m_hash_selector) {
            auto *b = qobject_cast<QPushButton *>(s->m_tab_button);
            if (b) b->setChecked(idx == id);
        }
    });
    return btn;
}

void KBShortcutsDialog::OnSelectTabel(QEvent &) {}

QWidget *KBShortcutsDialog::create_header(QWidget *parent, const QFont &bold_font)
{
    auto *w = new QWidget(parent);
    w->setStyleSheet("background-color: #1F2637;");
    auto *hbox = new QHBoxLayout(w);
    hbox->setContentsMargins(16, 12, 16, 12);

    m_logo_bmp   = ScalableBitmap(w, "BambuStudio_about", 32);
    m_header_bitmap = new QLabel(w);
    if (!m_logo_bmp.bmp().isNull())
        m_header_bitmap->setPixmap(m_logo_bmp.bmp());
    hbox->addWidget(m_header_bitmap);

    auto *title = new QLabel(_L("Keyboard shortcuts"), w);
    title->setFont(bold_font);
    title->setStyleSheet("color: white;");
    hbox->addWidget(title);
    hbox->addStretch(1);
    return w;
}

QWidget *KBShortcutsDialog::create_page(QWidget *parent,
                                         const ShortcutsItem &item,
                                         const QFont &font,
                                         const QFont &bold_font)
{
    auto *scroll = new QScrollArea(parent);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);

    auto *inner = new QWidget;
    auto *grid  = new QGridLayout(inner);
    grid->setContentsMargins(12, 8, 12, 8);
    grid->setSpacing(6);

    // Column header
    auto *hdr_key = new QLabel(_L("Key"), inner);
    hdr_key->setFont(bold_font);
    auto *hdr_desc = new QLabel(_L("Description"), inner);
    hdr_desc->setFont(bold_font);
    grid->addWidget(hdr_key,  0, 0);
    grid->addWidget(hdr_desc, 0, 1);

    // Separator
    auto *sep = new QFrame(inner);
    sep->setFrameShape(QFrame::HLine);
    sep->setFrameShadow(QFrame::Sunken);
    grid->addWidget(sep, 1, 0, 1, 2);

    int row = 2;
    for (const auto &sh : item.second) {
        auto *key_lbl  = new QLabel(QString::fromStdString(sh.first),  inner);
        auto *desc_lbl = new QLabel(QString::fromStdString(sh.second), inner);
        key_lbl->setFont(font);
        desc_lbl->setFont(font);
        grid->addWidget(key_lbl,  row, 0);
        grid->addWidget(desc_lbl, row, 1);
        ++row;
    }
    grid->setColumnStretch(1, 1);
    scroll->setWidget(inner);
    return scroll;
}

void KBShortcutsDialog::on_dpi_changed(const QRect &) {}

}} // namespace Slic3r::GUI
