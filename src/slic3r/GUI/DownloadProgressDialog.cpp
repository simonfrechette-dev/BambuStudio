#include "DownloadProgressDialog.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QStackedWidget>
#include <QApplication>
#include <boost/format.hpp>

#include "libslic3r/Utils.hpp"
#include "GUI.hpp"
#include "I18N.hpp"
#include "GUI_App.hpp"
#include "MainFrame.hpp"

namespace Slic3r {
namespace GUI {

DownloadProgressDialog::DownloadProgressDialog(QString title, bool post_login)
    : DPIDialog(static_cast<QWidget *>(wxGetApp().mainframe),
                Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint)
    , m_post_login(post_login)
{
    setWindowTitle(title);
    setAttribute(Qt::WA_DeleteOnClose, false);
    setStyleSheet("background-color: white;");

    const QString download_failed_msg =
        _L("Failed to download the plug-in. Please check your firewall settings "
           "and vpn software, check and retry.");
    const QString install_failed_msg =
        _L("Failed to install the plug-in. Please check whether it is blocked or "
           "deleted by anti-virus software.");
    const QString wiki_url = "https://wiki.bambulab.com/en/software/bambu-studio/"
                             "failed-to-get-network-plugin";

    auto *m_sizer_main = new QVBoxLayout(this);
    m_sizer_main->setContentsMargins(0, 0, 0, 0);
    m_sizer_main->setSpacing(0);

    // 1-pixel top accent line
    auto *m_line_top = new QWidget(this);
    m_line_top->setFixedHeight(1);
    m_line_top->setStyleSheet("background-color: #a6a9aa;");
    m_sizer_main->addWidget(m_line_top);

    // Stacked pages: 0 = progress, 1 = download failed, 2 = install failed
    m_simplebook_status = new QStackedWidget(this);
    m_simplebook_status->setMinimumWidth(400);

    // Page 0: progress bar panel
    m_status_bar    = std::make_shared<BBLStatusBarSend>(m_simplebook_status);
    m_panel_download = m_status_bar->get_panel();
    m_panel_download->setMinimumSize(400, 90);
    m_simplebook_status->addWidget(m_panel_download);   // index 0

    // Page 1: download failed
    auto *m_panel_download_failed = new QWidget(m_simplebook_status);
    {
        auto *sizer = new QVBoxLayout(m_panel_download_failed);
        sizer->setContentsMargins(10, 10, 10, 10);
        auto *lbl = new QLabel(download_failed_msg, m_panel_download_failed);
        lbl->setStyleSheet("color: black;");
        lbl->setWordWrap(true);
        lbl->setMaximumWidth(360);
        sizer->addWidget(lbl, 0, Qt::AlignHCenter);
        auto *link = new QLabel(
            QString("<a href=\"%1\">%2</a>")
                .arg(wiki_url, _L("click here to see more info")),
            m_panel_download_failed);
        link->setOpenExternalLinks(true);
        sizer->addWidget(link, 0, Qt::AlignHCenter);
    }
    m_simplebook_status->addWidget(m_panel_download_failed); // index 1

    // Page 2: install failed
    auto *m_panel_install_failed = new QWidget(m_simplebook_status);
    {
        auto *sizer = new QVBoxLayout(m_panel_install_failed);
        sizer->setContentsMargins(10, 10, 10, 10);
        auto *lbl = new QLabel(install_failed_msg, m_panel_install_failed);
        lbl->setStyleSheet("color: black;");
        lbl->setWordWrap(true);
        lbl->setMaximumWidth(360);
        sizer->addWidget(lbl, 0, Qt::AlignHCenter);
        auto *link = new QLabel(
            QString("<a href=\"%1\">%2</a>")
                .arg(wiki_url, _L("click here to see more info")),
            m_panel_install_failed);
        link->setOpenExternalLinks(true);
        sizer->addWidget(link, 0, Qt::AlignHCenter);
    }
    m_simplebook_status->addWidget(m_panel_install_failed); // index 2

    // Wrap stacked widget with 20px margins on all sides (matches wx wxALL, FromDIP(20))
    auto *m_content_wrapper = new QWidget(this);
    auto *m_content_layout  = new QHBoxLayout(m_content_wrapper);
    m_content_layout->setContentsMargins(20, 20, 20, 20);
    m_content_layout->addWidget(m_simplebook_status);
    m_sizer_main->addWidget(m_content_wrapper);

    adjustSize();
}

QString DownloadProgressDialog::format_text(QLabel *st, QString str, int warp)
{
    if (!wxGetApp().app_config ||
        wxGetApp().app_config->get("language") != "zh_CN")
        return str;

    QString out = str, cur;
    for (int i = 0; i < str.length(); i++) {
        if (st->fontMetrics().horizontalAdvance(cur) < warp) {
            cur += str[i];
        } else {
            out.insert(i - 1, '\n');
            cur.clear();
        }
    }
    return out;
}

void DownloadProgressDialog::setVisible(bool show)
{
    if (show) {
        m_simplebook_status->setCurrentIndex(0);
        m_upgrade_job = make_job(m_status_bar);
        m_upgrade_job->set_event_handle(this);
        m_status_bar->set_progress(0);

        // Cancel button: cancel the job
        m_status_bar->set_cancel_callback_fina([this]() {
            if (m_upgrade_job) m_upgrade_job->cancel();
        });

        m_upgrade_job->start();
    }
    DPIDialog::setVisible(show);
}

bool DownloadProgressDialog::event(QEvent *e)
{
    if (e->type() == EVT_UPGRADE_NETWORK_SUCCESS) {
        m_status_bar->change_button_label(_L("OK"));
        on_finish();
        if (m_post_login) {
            m_status_bar->set_cancel_callback_fina([this]() {
                close();
                wxGetApp().request_login();
            });
        } else {
            m_status_bar->set_cancel_callback_fina([this]() { close(); });
        }
        return true;
    }
    if (e->type() == EVT_DOWNLOAD_NETWORK_FAILED) {
        m_status_bar->change_button_label(_L("Close"));
        m_status_bar->set_progress(0);
        m_simplebook_status->setCurrentIndex(1);
        m_status_bar->set_cancel_callback_fina([this]() { close(); });
        return true;
    }
    if (e->type() == EVT_INSTALL_NETWORK_FAILED) {
        m_status_bar->change_button_label(_L("Close"));
        m_status_bar->set_progress(0);
        m_simplebook_status->setCurrentIndex(2);
        m_status_bar->set_cancel_callback_fina([this]() { close(); });
        return true;
    }
    if (e->type() == QEvent::Close) {
        // Job posted a Close event to signal cancellation complete
        hide();
        return true;
    }
    return DPIDialog::event(e);
}

void DownloadProgressDialog::closeEvent(QCloseEvent *event)
{
    on_close(*event);
    event->accept();
}

void DownloadProgressDialog::on_close(QCloseEvent & /*event*/)
{
    if (m_upgrade_job) {
        m_upgrade_job->cancel();
        m_upgrade_job->join();
    }
}

DownloadProgressDialog::~DownloadProgressDialog() {}

void DownloadProgressDialog::on_dpi_changed(const QRect & /*suggested_rect*/) {}

void DownloadProgressDialog::update_release_note(std::string /*release_note*/,
                                                 std::string /*version*/) {}

std::shared_ptr<UpgradeNetworkJob>
DownloadProgressDialog::make_job(std::shared_ptr<ProgressIndicator> pri)
{
    return std::make_shared<UpgradeNetworkJob>(pri);
}

void DownloadProgressDialog::on_finish()
{
    wxGetApp().restart_networking();
}

}} // namespace Slic3r::GUI
