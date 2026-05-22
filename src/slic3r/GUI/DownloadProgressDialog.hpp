#include <QDialog>
#include <QWidget>
#include <QString>
#include <QCloseEvent>
#ifndef slic3r_DownloadProgressDialog_hpp_
#define slic3r_DownloadProgressDialog_hpp_

#include <string>
#include <unordered_map>

#include "GUI_Utils.hpp"
#include "Widgets/Button.hpp"
#include "BBLStatusBar.hpp"
#include "BBLStatusBarSend.hpp"
#include "Jobs/UpgradeNetworkJob.hpp"

class QBoxLayout;
class QCheckBox;
class QLabel;

#define MSG_DIALOG_BUTTON_SIZE QSize(FromDIP(58), FromDIP(24))
#define MSG_DIALOG_MIDDLE_BUTTON_SIZE QSize(FromDIP(76), FromDIP(24))
#define MSG_DIALOG_LONG_BUTTON_SIZE QSize(FromDIP(90), FromDIP(24))


namespace Slic3r {
namespace GUI {


class DownloadProgressDialog : public DPIDialog
{
protected:
    void setVisible(bool show) override;
    void on_close(QCloseEvent& event);
    bool event(QEvent *e) override;
    void closeEvent(QCloseEvent *event) override;

public:
    DownloadProgressDialog(QString title, bool post_login = false);
    QString format_text(QLabel* st, QString str, int warp);
    ~DownloadProgressDialog();

    void on_dpi_changed(const QRect &suggested_rect) override;
    void update_release_note(std::string release_note, std::string version);

    QStackedWidget* m_simplebook_status{nullptr};

	std::shared_ptr<BBLStatusBarSend> m_status_bar;
    std::shared_ptr<UpgradeNetworkJob> m_upgrade_job { nullptr };
    QWidget *                         m_panel_download;

protected:
    virtual std::shared_ptr<UpgradeNetworkJob> make_job(std::shared_ptr<ProgressIndicator> pri);
    virtual void                               on_finish();

    bool m_post_login { false };
};


}
}

#endif
