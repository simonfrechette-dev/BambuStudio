#include "DesktopIntegrationDialog.hpp"

#ifdef __linux__

#include "I18N.hpp"
#include "GUI_App.hpp"
#include "libslic3r/AppConfig.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFileInfo>
#include <QProcess>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <boost/log/trivial.hpp>

namespace Slic3r { namespace GUI {

DesktopIntegrationDialog::DesktopIntegrationDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(_L("Desktop integration"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    auto *vbox = new QVBoxLayout(this);
    vbox->setContentsMargins(20, 20, 20, 20);
    vbox->setSpacing(12);

    auto *lbl = new QLabel(
        _L("BambuStudio can be integrated into your desktop environment. "
           "This will create application menu entries and associate file types."),
        this);
    lbl->setWordWrap(true);
    vbox->addWidget(lbl);

    auto *btn_row = new QHBoxLayout;
    btn_row->addStretch(1);

    if (is_integrated()) {
        auto *undo_btn = new QPushButton(_L("Undo desktop integration"), this);
        connect(undo_btn, &QPushButton::clicked, this, [this] {
            DesktopIntegrationDialog::undo_desktop_intgration();
            accept();
        });
        btn_row->addWidget(undo_btn);
    } else {
        auto *integrate_btn = new QPushButton(_L("Perform desktop integration"), this);
        connect(integrate_btn, &QPushButton::clicked, this, [this] {
            DesktopIntegrationDialog::perform_desktop_integration();
            accept();
        });
        btn_row->addWidget(integrate_btn);
    }

    auto *cancel_btn = new QPushButton(_L("Cancel"), this);
    connect(cancel_btn, &QPushButton::clicked, this, &QDialog::reject);
    btn_row->addWidget(cancel_btn);

    vbox->addLayout(btn_row);
    adjustSize();
}

DesktopIntegrationDialog::~DesktopIntegrationDialog() = default;

// static
bool DesktopIntegrationDialog::is_integrated()
{
    AppConfig *cfg = wxGetApp().app_config;
    if (!cfg) return false;
    std::string desktop_path = cfg->get("desktop_integration_path");
    return !desktop_path.empty() && QFileInfo::exists(QString::fromStdString(desktop_path));
}

// static
bool DesktopIntegrationDialog::integration_possible()
{
    // Integration is possible if running from an AppImage
    return QFileInfo::exists(QStringLiteral("/proc/self/mountinfo"));
}

// static
void DesktopIntegrationDialog::perform_desktop_integration()
{
    BOOST_LOG_TRIVIAL(info) << "perform_desktop_integration: stub";
    // TODO: create .desktop file and icons in ~/.local/share/
}

// static
void DesktopIntegrationDialog::undo_desktop_intgration()
{
    BOOST_LOG_TRIVIAL(info) << "undo_desktop_integration: stub";
    // TODO: remove .desktop file and icons
}

}} // namespace Slic3r::GUI

#endif // __linux__
