#ifndef __UpgradeNetworkJob_HPP__
#define __UpgradeNetworkJob_HPP__

#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>
#include <functional>
#include <QEvent>
#include <QWidget>
#include <QString>
#include "Job.hpp"

namespace fs = boost::filesystem;

namespace Slic3r {
namespace GUI {

enum PluginInstallStatus {
    InstallStatusNormal = 0,
    InstallStatusDownloadFailed = 1,
    InstallStatusDownloadCompleted = 2,
    InstallStatusUnzipFailed = 3,
    InstallStatusInstallCompleted = 4,
};

typedef std::function<void(int status, int percent, bool& cancel)> InstallProgressFn;

inline const QEvent::Type EVT_UPGRADE_UPDATE_MESSAGE  = static_cast<QEvent::Type>(QEvent::registerEventType());
inline const QEvent::Type EVT_UPGRADE_NETWORK_SUCCESS = static_cast<QEvent::Type>(QEvent::registerEventType());
inline const QEvent::Type EVT_DOWNLOAD_NETWORK_FAILED = static_cast<QEvent::Type>(QEvent::registerEventType());
inline const QEvent::Type EVT_INSTALL_NETWORK_FAILED  = static_cast<QEvent::Type>(QEvent::registerEventType());

class UpgradeNetworkJob : public Job
{
    QWidget *            m_event_handle{nullptr};
    std::function<void()> m_success_fun{nullptr};
    bool                 m_job_finished{ false };
    int                  m_print_job_completed_id = 0;

    InstallProgressFn pro_fn { nullptr };

protected:
    std::string name;
    std::string package_name;

    void on_exception(const std::exception_ptr &) override;
public:
    UpgradeNetworkJob(std::shared_ptr<ProgressIndicator> pri);

    int  status_range() const override
    {
        return 100;
    }

    bool is_finished() { return m_job_finished; }

    void on_success(std::function<void()> success);
    void update_status(int st, const QString &msg);
    void process() override;
    void finalize() override;
    void set_event_handle(QWidget* handle);
};

}} // namespace Slic3r::GUI

#endif // __UpgradeNetworkJob_HPP__
