#include "UpgradeNetworkJob.hpp"

#include "slic3r/GUI/GUI.hpp"
#include "slic3r/GUI/GUI_App.hpp"
#include "slic3r/Utils/Http.hpp"

#include <QApplication>
#include <boost/log/trivial.hpp>

namespace Slic3r {
namespace GUI {

UpgradeNetworkJob::UpgradeNetworkJob(std::shared_ptr<ProgressIndicator> pri)
    : Job{std::move(pri)}
{
    name         = "plugins";
    package_name = "networking_plugins.zip";
}

void UpgradeNetworkJob::on_exception(const std::exception_ptr &eptr)
{
    try {
        if (eptr) std::rethrow_exception(eptr);
    } catch (std::exception &) {
        // Networking job failures are reported via status events; swallow here.
    }
}

void UpgradeNetworkJob::on_success(std::function<void()> success)
{
    m_success_fun = success;
}

void UpgradeNetworkJob::update_status(int st, const QString &msg)
{
    BOOST_LOG_TRIVIAL(info) << "UpgradeNetworkJob: percent=" << st
                            << " msg=" << msg.toStdString();
    Job::update_status(st, msg);
    if (m_event_handle)
        QApplication::postEvent(m_event_handle,
                                new QEvent(EVT_UPGRADE_UPDATE_MESSAGE));
}

void UpgradeNetworkJob::process()
{
    AppConfig *app_config = wxGetApp().app_config;
    if (!app_config) return;

    BOOST_LOG_TRIVIAL(info) << "[UpgradeNetworkJob process]: enter";

    auto cancel_fn = [this]() { return was_canceled(); };

    int result = wxGetApp().download_plugin(
        name, package_name,
        [this](int state, int percent, bool & /*cancel*/) {
            if (state == InstallStatusDownloadFailed)
                update_status(percent, _L("Download failed"));
            else
                update_status(percent, _L("Downloading"));
        },
        cancel_fn);

    if (was_canceled()) {
        update_status(0, _L("Cancelled"));
        if (m_event_handle)
            QApplication::postEvent(m_event_handle, new QEvent(QEvent::Close));
        return;
    }

    if (result < 0) {
        update_status(0, _L("Download failed"));
        if (m_event_handle)
            QApplication::postEvent(m_event_handle,
                                    new QEvent(EVT_DOWNLOAD_NETWORK_FAILED));
        return;
    }

    result = wxGetApp().install_plugin(
        name, package_name,
        [this](int state, int percent, bool & /*cancel*/) {
            if (state == InstallStatusInstallCompleted)
                update_status(percent, _L("Install successfully."));
            else
                update_status(percent, _L("Installing"));
        },
        cancel_fn);

    if (was_canceled()) {
        update_status(0, _L("Cancelled"));
        if (m_event_handle)
            QApplication::postEvent(m_event_handle, new QEvent(QEvent::Close));
        return;
    }

    if (result != 0) {
        update_status(0, _L("Install failed"));
        if (m_event_handle)
            QApplication::postEvent(m_event_handle,
                                    new QEvent(EVT_INSTALL_NETWORK_FAILED));
        return;
    }

    if (m_event_handle)
        QApplication::postEvent(m_event_handle,
                                new QEvent(EVT_UPGRADE_NETWORK_SUCCESS));
    BOOST_LOG_TRIVIAL(info) << "[UpgradeNetworkJob process]: exit";
}

void UpgradeNetworkJob::finalize()
{
    if (was_canceled()) return;
    Job::finalize();
}

void UpgradeNetworkJob::set_event_handle(QWidget *handle)
{
    m_event_handle = handle;
}

}} // namespace Slic3r::GUI
