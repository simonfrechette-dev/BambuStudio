#ifndef slic3r_GUI_NetworkTestDialog_hpp_
#define slic3r_GUI_NetworkTestDialog_hpp_

#include <boost/thread.hpp>
#include "GUI_Utils.hpp"

#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QLayout>
#include <QCloseEvent>

namespace Slic3r {
namespace GUI {

enum TestJob {
    TEST_BING_JOB = 0,
    TEST_BAMBULAB_JOB = 1,
    TEST_IOT_JOB = 2,
    TEST_OSS_JOB = 3,
    TEST_OSS_UPGRADE_JOB = 4,
    TEST_OSS_DOWNLOAD_JOB = 5,
    TEST_OSS_UPLOAD_JOB = 6,
    TEST_PING_JOB = 7,
    TEST_PLUGIN_JOB = 8,
    TEST_JOB_MAX = 9
};

class NetworkTestDialog : public DPIDialog
{
    Q_OBJECT
public:
    explicit NetworkTestDialog(QWidget* parent = nullptr,
                               const QString& title = QString(),
                               const QPoint& pos = QPoint(),
                               const QSize& size = QSize(1000, 700));
    ~NetworkTestDialog() override;

    void on_dpi_changed(const QRect&) override {}

    void set_default();
    QString get_studio_version();
    QString get_os_info();
    QString get_dns_info();

    void start_all_job();
    void start_all_job_sequence();
    void start_test_bing_thread();
    void start_test_bambulab_thread();
    void start_test_iot_thread();
    void start_test_oss_thread();
    void start_test_oss_upgrade_thread();
    void start_test_oss_download_thread();
    void start_test_oss_upload_thread();
    void start_test_ping_thread();
    void start_test_plugin_download_thread();

    void start_test_bing();
    void start_test_bambulab();
    void start_test_iot();
    void start_test_oss();
    void start_test_oss_upgrade();
    void start_test_oss_download();
    void start_test_oss_upload();
    void start_test_plugin_download();

    void update_status(int job_id, const QString& info);

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    // layout builders
    QLayout* create_top_layout();
    QLayout* create_info_layout();
    QLayout* create_content_layout();
    QLayout* create_result_layout();
    void init_bind();

    // top row widgets
    QPushButton* btn_start              = nullptr;
    QPushButton* btn_start_sequence     = nullptr;
    QPushButton* btn_download_log       = nullptr;

    // info section
    QLabel* text_basic_info             = nullptr;
    QLabel* text_version_title          = nullptr;
    QLabel* text_version_val            = nullptr;
    QLabel* txt_sys_info_title          = nullptr;
    QLabel* txt_sys_info_value          = nullptr;
    QLabel* txt_dns_info_title          = nullptr;
    QLabel* txt_dns_info_value          = nullptr;

    // content grid
    QPushButton* btn_link               = nullptr;
    QLabel*      text_link_title        = nullptr;
    QLabel*      text_link_val          = nullptr;
    QPushButton* btn_bing               = nullptr;
    QLabel*      text_bing_title        = nullptr;
    QLabel*      text_bing_val          = nullptr;
    QPushButton* btn_iot                = nullptr;
    QLabel*      text_iot_title         = nullptr;
    QLabel*      text_iot_value         = nullptr;
    QPushButton* btn_oss                = nullptr;
    QLabel*      text_oss_title         = nullptr;
    QLabel*      text_oss_value         = nullptr;
    QPushButton* btn_oss_upgrade        = nullptr;
    QLabel*      text_oss_upgrade_title = nullptr;
    QLabel*      text_oss_upgrade_value = nullptr;
    QPushButton* btn_oss_download       = nullptr;
    QLabel*      text_oss_download_title= nullptr;
    QLabel*      text_oss_download_value= nullptr;
    QPushButton* btn_network_plugin     = nullptr;
    QLabel*      text_network_plugin_title  = nullptr;
    QLabel*      text_network_plugin_value  = nullptr;
    QPushButton* btn_oss_upload         = nullptr;
    QLabel*      text_oss_upload_title  = nullptr;
    QLabel*      text_oss_upload_value  = nullptr;

    // result section
    QLabel*    text_result              = nullptr;
    QTextEdit* txt_log                  = nullptr;

    // threading
    boost::thread* test_job[TEST_JOB_MAX] = {};
    boost::thread* m_sequence_job = nullptr;
    bool m_in_testing[TEST_JOB_MAX] = {};
    bool m_download_cancel = false;
    bool m_closing = false;
};

} // namespace GUI
} // namespace Slic3r

#endif // slic3r_GUI_NetworkTestDialog_hpp_
