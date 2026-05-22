#include "SysInfoDialog.hpp"
#include "I18N.hpp"
#include "wxExtensions.hpp"
#include "GUI_App.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTextBrowser>
#include <QPushButton>
#include <QTabWidget>
#include <QClipboard>
#include <QApplication>
#include <QSysInfo>
#include <boost/log/trivial.hpp>

namespace Slic3r { namespace GUI {

SysInfoDialog::SysInfoDialog()
    : DPIDialog(nullptr)
{
    setWindowTitle(_L("System information"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet("background-color: white;");
    setMinimumSize(720, 540);

    auto *vbox = new QVBoxLayout(this);
    vbox->setContentsMargins(16, 16, 16, 12);
    vbox->setSpacing(10);

    // Logo
    m_logo_bmp = ScalableBitmap(this, "BambuStudio_192px", 48);
    m_logo = new QLabel(this);
    if (!m_logo_bmp.bmp().isNull())
        m_logo->setPixmap(m_logo_bmp.bmp());
    m_logo->setAlignment(Qt::AlignHCenter);
    vbox->addWidget(m_logo);

    // Tabs: System info | OpenGL info
    auto *tabs = new QTabWidget(this);

    // --- System info tab ---
    m_html = new QTextBrowser(this);
    m_html->setOpenExternalLinks(false);
    {
        QString info;
        info += "<b>" + _L("BambuStudio version:") + "</b> " + QString(SLIC3R_VERSION) + "<br>";
        info += "<b>" + _L("Operating system:") + "</b> " + QSysInfo::prettyProductName() + "<br>";
        info += "<b>" + _L("CPU:") + "</b> " + QSysInfo::currentCpuArchitecture() + "<br>";
        info += "<b>" + _L("Kernel:") + "</b> " + QSysInfo::kernelType() + " " + QSysInfo::kernelVersion() + "<br>";
        m_html->setHtml(info);
    }
    tabs->addTab(m_html, _L("System"));

    // --- OpenGL info tab ---
    m_opengl_info_html = new QTextBrowser(this);
    m_opengl_info_html->setOpenExternalLinks(false);
    m_opengl_info_html->setHtml("<i>" + _L("OpenGL information not available in stub.") + "</i>");
    tabs->addTab(m_opengl_info_html, _L("OpenGL"));

    vbox->addWidget(tabs, 1);

    // Buttons
    auto *btn_row = new QHBoxLayout;
    btn_row->addStretch(1);

    m_btn_copy_to_clipboard = new QPushButton(_L("Copy to clipboard"), this);
    connect(m_btn_copy_to_clipboard, &QPushButton::clicked, this, [this] {
        QApplication::clipboard()->setText(m_html->toPlainText() + "\n" + m_opengl_info_html->toPlainText());
    });
    btn_row->addWidget(m_btn_copy_to_clipboard);

    auto *close_btn = new QPushButton(_L("Close"), this);
    connect(close_btn, &QPushButton::clicked, this, &QDialog::accept);
    btn_row->addWidget(close_btn);

    vbox->addLayout(btn_row);
    adjustSize();
}

void SysInfoDialog::onCopyToClipboard(QEvent &)
{
    QApplication::clipboard()->setText(m_html->toPlainText() + "\n" + m_opengl_info_html->toPlainText());
}

void SysInfoDialog::onCloseDialog(QEvent &) { accept(); }

void SysInfoDialog::on_dpi_changed(const QRect &) {}

}} // namespace Slic3r::GUI
