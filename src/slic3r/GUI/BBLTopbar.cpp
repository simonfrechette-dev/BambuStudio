#include "BBLTopbar.hpp"
#include "GUI_App.hpp"
#include "MainFrame.hpp"
#include "Plater.hpp"
#include "I18N.hpp"
#include "libslic3r/Utils.hpp"

#include <QSvgRenderer>
#include <QPainter>
#include <QPixmap>
#include <QApplication>
#include <QWindow>
#include <QCursor>

#include <boost/log/trivial.hpp>

using namespace Slic3r;
using namespace Slic3r::GUI;

// ---------------------------------------------------------------------------
// Helper: load SVG icon from resources/images/<name>.svg
// ---------------------------------------------------------------------------

/*static*/ QIcon BBLTopbar::loadSvgIcon(const std::string& name, int size)
{
    const std::string path = Slic3r::var(name + ".svg");
    QSvgRenderer renderer(QString::fromStdString(path));
    if (!renderer.isValid()) {
        BOOST_LOG_TRIVIAL(warning) << "BBLTopbar: SVG not found: " << path;
        return {};
    }
    QPixmap pm(size, size);
    pm.fill(Qt::transparent);
    QPainter p(&pm);
    renderer.render(&p);
    p.end();
    return QIcon(pm);
}

// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------

BBLTopbar::BBLTopbar(QMainWindow* parent)
    : QToolBar(parent)
{
    Init(parent);
}

BBLTopbar::BBLTopbar(QWidget* pwin, QMainWindow* parent)
    : QToolBar(pwin)
{
    Init(parent);
}

void BBLTopbar::Init(QMainWindow* parent)
{
    m_frame = parent;

    setMovable(false);
    setFloatable(false);
    setContextMenuPolicy(Qt::PreventContextMenu);
    setIconSize(QSize(16, 16));
    setFixedHeight(40);

    // ----------------------------------------------------------------
    // Stylesheet: dark background matching Bambu brand color
    // ----------------------------------------------------------------
    setStyleSheet(QStringLiteral(
        "QToolBar {"
        "  background: rgb(38,46,48);"
        "  color: white;"
        "  border: none;"
        "  spacing: 0px;"
        "}"
        "QToolBar::separator {"
        "  background: rgba(255,255,255,40);"
        "  width: 1px;"
        "  margin: 6px 3px;"
        "}"
        "QToolButton {"
        "  color: white;"
        "  background: transparent;"
        "  border: none;"
        "  padding: 2px 6px;"
        "  min-height: 32px;"
        "}"
        "QToolButton:hover {"
        "  background: rgba(255,255,255,25);"
        "  border-radius: 3px;"
        "}"
        "QToolButton:pressed {"
        "  background: rgba(255,255,255,45);"
        "  border-radius: 3px;"
        "}"
        "QToolButton:checked {"
        "  background: rgba(255,255,255,20);"
        "  border-radius: 3px;"
        "}"
        "QToolButton#close_btn:hover {"
        "  background: #e81123;"
        "  border-radius: 0px;"
        "}"
        "QLabel {"
        "  color: white;"
        "  background: transparent;"
        "}"
    ));

    // ----------------------------------------------------------------
    // File menu button (topbar_file.svg)
    // ----------------------------------------------------------------
    m_file_menu_item = new QAction(this);
    {
        QIcon ico = loadSvgIcon("topbar_file", 16);
        if (!ico.isNull())
            m_file_menu_item->setIcon(ico);
        else
            m_file_menu_item->setText(QStringLiteral("☰"));
    }
    m_file_menu_item->setToolTip(_L("File"));
    connect(m_file_menu_item, &QAction::triggered,
            this, &BBLTopbar::OnFileToolItem);
    addAction(m_file_menu_item);

    // Dropdown (Edit/View/…) button
    m_dropdown_menu_item = new QAction(this);
    {
        QIcon ico = loadSvgIcon("topbar_dropdown", 16);
        if (!ico.isNull())
            m_dropdown_menu_item->setIcon(ico);
        else
            m_dropdown_menu_item->setText(QStringLiteral("▾"));
    }
    m_dropdown_menu_item->setToolTip(_L("Menu"));
    connect(m_dropdown_menu_item, &QAction::triggered,
            this, &BBLTopbar::OnDropdownToolItem);
    addAction(m_dropdown_menu_item);

    addSeparator();

    // ----------------------------------------------------------------
    // Save / Undo / Redo
    // ----------------------------------------------------------------
    m_save_item = new QAction(this);
    {
        QIcon ico = loadSvgIcon("topbar_save_inactive", 16);
        if (!ico.isNull())
            m_save_item->setIcon(ico);
        else
            m_save_item->setText(QStringLiteral("💾"));
    }
    m_save_item->setToolTip(_L("Save project"));
    m_save_item->setEnabled(false);
    connect(m_save_item, &QAction::triggered, this, &BBLTopbar::OnSaveProject);
    addAction(m_save_item);

    m_undo_item = new QAction(this);
    {
        QIcon ico = loadSvgIcon("topbar_undo_inactive", 16);
        if (!ico.isNull())
            m_undo_item->setIcon(ico);
        else
            m_undo_item->setText(QStringLiteral("↩"));
    }
    m_undo_item->setToolTip(_L("Undo"));
    m_undo_item->setEnabled(false);
    connect(m_undo_item, &QAction::triggered, this, &BBLTopbar::OnUndo);
    addAction(m_undo_item);

    m_redo_item = new QAction(this);
    {
        QIcon ico = loadSvgIcon("topbar_redo_inactive", 16);
        if (!ico.isNull())
            m_redo_item->setIcon(ico);
        else
            m_redo_item->setText(QStringLiteral("↪"));
    }
    m_redo_item->setToolTip(_L("Redo"));
    m_redo_item->setEnabled(false);
    connect(m_redo_item, &QAction::triggered, this, &BBLTopbar::OnRedo);
    addAction(m_redo_item);

    // ----------------------------------------------------------------
    // Left spacer → pushes title to center
    // ----------------------------------------------------------------
    auto* spacer1 = new QWidget(this);
    spacer1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    spacer1->setAttribute(Qt::WA_TransparentForMouseEvents);
    addWidget(spacer1);

    // ----------------------------------------------------------------
    // Window title label (centred, max 300 px)
    // ----------------------------------------------------------------
    m_title_label = new QLabel(QStringLiteral("BambuStudio"), this);
    m_title_label->setAlignment(Qt::AlignCenter);
    m_title_label->setMaximumWidth(300);
    m_title_label->setStyleSheet(
        QStringLiteral("color: white; background: transparent; font-weight: 500;"));
    m_title_label->setAttribute(Qt::WA_TransparentForMouseEvents);
    addWidget(m_title_label);
    // m_title_item kept for API compat – SetTitle writes to m_title_label
    m_title_item = new QAction(this);
    m_title_item->setVisible(false);
    addAction(m_title_item);

    // ----------------------------------------------------------------
    // Right spacer → pushes right-side buttons to the right
    // ----------------------------------------------------------------
    auto* spacer2 = new QWidget(this);
    spacer2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    spacer2->setAttribute(Qt::WA_TransparentForMouseEvents);
    addWidget(spacer2);

    // ----------------------------------------------------------------
    // Calibration (icon + text, hidden by default)
    // ----------------------------------------------------------------
    m_calib_item = new QAction(this);
    {
        QIcon ico = loadSvgIcon("calib_sf_inactive", 16);
        if (!ico.isNull())
            m_calib_item->setIcon(ico);
    }
    m_calib_item->setText(_L("Calibration"));
    m_calib_item->setToolTip(_L("Calibration"));
    m_calib_item->setVisible(false);
    m_calib_item->setMenu(&m_calib_menu);
    connect(m_calib_item, &QAction::triggered,
            this, &BBLTopbar::OnCalibToolItem);
    addAction(m_calib_item);
    if (QToolButton* btn = qobject_cast<QToolButton*>(widgetForAction(m_calib_item)))
        btn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    // ----------------------------------------------------------------
    // Publish (hidden by default)
    // ----------------------------------------------------------------
    m_publish_item = new QAction(this);
    {
        QIcon ico = loadSvgIcon("topbar_publish", 16);
        if (!ico.isNull())
            m_publish_item->setIcon(ico);
        else
            m_publish_item->setText(_L("Publish"));
    }
    m_publish_item->setToolTip(_L("Publish model"));
    m_publish_item->setVisible(false);
    connect(m_publish_item, &QAction::triggered,
            this, &BBLTopbar::OnPublishClicked);
    addAction(m_publish_item);

    // ----------------------------------------------------------------
    // Account
    // ----------------------------------------------------------------
    m_account_item = new QAction(this);
    {
        QIcon ico = loadSvgIcon("topbar_account", 16);
        if (!ico.isNull())
            m_account_item->setIcon(ico);
        else
            m_account_item->setText(_L("Account"));
    }
    m_account_item->setToolTip(_L("Account"));
    addAction(m_account_item);

    // ----------------------------------------------------------------
    // Model Store
    // ----------------------------------------------------------------
    m_model_store_item = new QAction(this);
    {
        QIcon ico = loadSvgIcon("topbar_store", 16);
        if (!ico.isNull())
            m_model_store_item->setIcon(ico);
        else
            m_model_store_item->setText(_L("Store"));
    }
    m_model_store_item->setToolTip(_L("Model Store"));
    connect(m_model_store_item, &QAction::triggered,
            this, &BBLTopbar::OnModelStoreClicked);
    addAction(m_model_store_item);

    // ----------------------------------------------------------------
    // Settings toggle
    // ----------------------------------------------------------------
    m_settings_item = new QAction(this);
    m_settings_item->setText(QStringLiteral("⚙"));
    m_settings_item->setToolTip(_L("Toggle settings panel"));
    m_settings_item->setCheckable(true);
    m_settings_item->setChecked(false);
    connect(m_settings_item, &QAction::triggered,
            this, &BBLTopbar::OnToggleSettings);
    addAction(m_settings_item);

    addSeparator();

    // ----------------------------------------------------------------
    // Window control buttons: Minimize / Maximize / Close
    // ----------------------------------------------------------------
    m_btn_minimize = new QToolButton(this);
    {
        QIcon ico = loadSvgIcon("topbar_min", 14);
        if (!ico.isNull())
            m_btn_minimize->setIcon(ico);
        else
            m_btn_minimize->setText(QStringLiteral("─"));
    }
    m_btn_minimize->setFixedSize(40, 40);
    m_btn_minimize->setToolTip(_L("Minimize"));
    connect(m_btn_minimize, &QToolButton::clicked, this, &BBLTopbar::OnIconize);
    addWidget(m_btn_minimize);

    m_btn_maximize = new QToolButton(this);
    {
        maximize_bitmap = QPixmap(QString::fromStdString(Slic3r::var("topbar_max.svg")));
        window_bitmap   = QPixmap(QString::fromStdString(Slic3r::var("topbar_win.svg")));
        QIcon ico = loadSvgIcon("topbar_max", 14);
        if (!ico.isNull())
            m_btn_maximize->setIcon(ico);
        else
            m_btn_maximize->setText(QStringLiteral("□"));
    }
    m_btn_maximize->setFixedSize(40, 40);
    m_btn_maximize->setToolTip(_L("Maximize"));
    connect(m_btn_maximize, &QToolButton::clicked, this, [this]() {
        if (!m_frame) return;
        if (m_frame->isMaximized())
            m_frame->showNormal();
        else
            m_frame->showMaximized();
    });
    addWidget(m_btn_maximize);

    m_btn_close = new QToolButton(this);
    m_btn_close->setObjectName(QStringLiteral("close_btn"));
    {
        QIcon ico = loadSvgIcon("topbar_close", 14);
        if (!ico.isNull())
            m_btn_close->setIcon(ico);
        else
            m_btn_close->setText(QStringLiteral("✕"));
    }
    m_btn_close->setFixedSize(40, 40);
    m_btn_close->setToolTip(_L("Close"));
    connect(m_btn_close, &QToolButton::clicked, this, &BBLTopbar::OnCloseFrame);
    addWidget(m_btn_close);
}

BBLTopbar::~BBLTopbar() {}

void BBLTopbar::UpdateToolbarWidth(int /*width*/) {}
void BBLTopbar::Rescale() {}

// ---------------------------------------------------------------------------
// Mouse events — window drag support (X11 and Wayland via startSystemMove)
// ---------------------------------------------------------------------------

void BBLTopbar::mousePressEvent(QMouseEvent* event)
{
    // If click lands on a QToolButton, let it handle normally.
    QWidget* child = childAt(event->pos());
    bool onButton = child && (qobject_cast<QToolButton*>(child) != nullptr);

    if (!onButton && event->button() == Qt::LeftButton) {
        if (QWindow* wh = window()->windowHandle())
            wh->startSystemMove();
        return;
    }
    QToolBar::mousePressEvent(event);
}

void BBLTopbar::mouseDoubleClickEvent(QMouseEvent* event)
{
    QWidget* child = childAt(event->pos());
    bool onButton = child && (qobject_cast<QToolButton*>(child) != nullptr);
    if (!onButton && event->button() == Qt::LeftButton && m_frame) {
        if (m_frame->isMaximized())
            m_frame->showNormal();
        else
            m_frame->showMaximized();
        return;
    }
    QToolBar::mouseDoubleClickEvent(event);
}


void BBLTopbar::OnIconize()
{
    if (m_frame) m_frame->showMinimized();
}

void BBLTopbar::OnFullScreen()
{
    if (!m_frame) return;
    if (m_frame->isFullScreen())
        m_frame->showNormal();
    else
        m_frame->showFullScreen();
}

void BBLTopbar::OnCloseFrame()
{
    if (m_frame) m_frame->close();
}

void BBLTopbar::OnFileToolItem()
{
    if (!m_file_menu_item) return;
    QMenu* menu = m_file_menu ? m_file_menu : &m_top_menu;
    // Find the tool button for the file action and popup the menu under it
    const auto actions = this->actions();
    for (QAction* a : actions) {
        if (a == m_file_menu_item) {
            QWidget* btn = widgetForAction(a);
            if (btn) {
                QPoint pos = btn->mapToGlobal(QPoint(0, btn->height()));
                menu->exec(pos);
            }
            return;
        }
    }
    // Fallback: show at cursor
    menu->exec(QCursor::pos());
}

void BBLTopbar::OnDropdownToolItem()
{
    if (!m_dropdown_menu_item) return;
    QWidget* btn = widgetForAction(m_dropdown_menu_item);
    QPoint pos = btn ? btn->mapToGlobal(QPoint(0, btn->height()))
                     : QCursor::pos();
    m_top_menu.exec(pos);
}

void BBLTopbar::OnCalibToolItem()
{
    if (!m_calib_item) return;
    QWidget* btn = widgetForAction(m_calib_item);
    QPoint pos = btn ? btn->mapToGlobal(QPoint(0, btn->height()))
                     : QCursor::pos();
    m_calib_menu.exec(pos);
}

void BBLTopbar::OnMenuClose() {}

void BBLTopbar::OnOpenProject()
{
    if (Plater* p = wxGetApp().plater())
        p->load_project();
}

void BBLTopbar::show_publish_button(bool show)
{
    if (m_publish_item) m_publish_item->setVisible(show);
}

void BBLTopbar::OnSaveProject()
{
    if (Plater* p = wxGetApp().plater())
        p->save_project();
}

void BBLTopbar::OnUndo()
{
    if (Plater* p = wxGetApp().plater())
        p->undo();
}

void BBLTopbar::OnRedo()
{
    if (Plater* p = wxGetApp().plater())
        p->redo();
}

void BBLTopbar::OnModelStoreClicked()
{
    // Marketplace navigation handled by MainFrame via event; no-op for now.
}

void BBLTopbar::OnPublishClicked()
{
    // handled by MainFrame
}

// ---------------------------------------------------------------------------

void BBLTopbar::SetFileMenu(QMenu* file_menu) { m_file_menu = file_menu; }

void BBLTopbar::AddDropDownSubMenu(QMenu* sub_menu, const QString& title)
{
    m_top_menu.addMenu(sub_menu)->setText(title);
}

void BBLTopbar::AddDropDownMenuItem(QAction* menu_item)
{
    m_top_menu.addAction(menu_item);
}

QMenu* BBLTopbar::GetTopMenu()  { return &m_top_menu; }
QMenu* BBLTopbar::GetCalibMenu() { return &m_calib_menu; }

void BBLTopbar::SetTitle(QString title)
{
    if (m_title_label) m_title_label->setText(title);
    if (m_title_item)  m_title_item->setText(title);
}

void BBLTopbar::SetMaximizedSize()
{
    if (m_btn_maximize) {
        QIcon ico = loadSvgIcon("topbar_win", 14);
        if (!ico.isNull()) m_btn_maximize->setIcon(ico);
    }
}

void BBLTopbar::SetWindowSize()
{
    if (m_btn_maximize) {
        QIcon ico = loadSvgIcon("topbar_max", 14);
        if (!ico.isNull()) m_btn_maximize->setIcon(ico);
    }
}

void BBLTopbar::EnableSaveItem(bool en)
{
    if (!m_save_item) return;
    m_save_item->setEnabled(en);
    QIcon ico = loadSvgIcon(en ? "topbar_save" : "topbar_save_inactive", 16);
    if (!ico.isNull()) m_save_item->setIcon(ico);
}

void BBLTopbar::EnableUndoItem(bool en)
{
    if (!m_undo_item) return;
    m_undo_item->setEnabled(en);
    QIcon ico = loadSvgIcon(en ? "topbar_undo" : "topbar_undo_inactive", 16);
    if (!ico.isNull()) m_undo_item->setIcon(ico);
}

void BBLTopbar::EnableRedoItem(bool en)
{
    if (!m_redo_item) return;
    m_redo_item->setEnabled(en);
    QIcon ico = loadSvgIcon(en ? "topbar_redo" : "topbar_redo_inactive", 16);
    if (!ico.isNull()) m_redo_item->setIcon(ico);
}

void BBLTopbar::EnableUndoRedoItems()
{
    EnableUndoItem(true);
    EnableRedoItem(true);
}

void BBLTopbar::DisableUndoRedoItems()
{
    EnableUndoItem(false);
    EnableRedoItem(false);
}

void BBLTopbar::SaveNormalRect()
{
    if (m_frame) m_normalRect = m_frame->normalGeometry();
}

void BBLTopbar::ShowCalibrationButton(bool show)
{
    if (m_calib_item) m_calib_item->setVisible(show);
}

void BBLTopbar::ShowSettingsButton(bool show)
{
    if (m_settings_item) m_settings_item->setVisible(show);
}

void BBLTopbar::SetSettingsPanelVisible(bool visible)
{
    if (m_settings_item) m_settings_item->setChecked(visible);
}

void BBLTopbar::OnToggleSettings()
{
    // Ask MainFrame to toggle the settings (parameters) panel.
    if (auto* mf = wxGetApp().mainframe) {
        mf->toggle_settings_panel();
    }
}
