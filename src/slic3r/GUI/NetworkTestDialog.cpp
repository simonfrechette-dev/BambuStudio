#include "NetworkTestDialog.hpp"
#include "I18N.hpp"
#include "GUI_App.hpp"
#include "libslic3r/AppConfig.hpp"
#include "slic3r/Utils/Http.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QSysInfo>
#include <QMetaObject>
#include <ctime>
#include <sstream>
#include <boost/format.hpp>
#include <boost/log/trivial.hpp>

namespace Slic3r { namespace GUI {

static const QString NA_STR = QStringLiteral("N/A");

// ---------------------------------------------------------------------------
// Constructor / Destructor
// ---------------------------------------------------------------------------

NetworkTestDialog::NetworkTestDialog(QWidget *parent, const QString &title,
                                     const QPoint &pos, const QSize &size)
    : DPIDialog(parent)
{
    setWindowTitle(title.isEmpty() ? _L("Network Test") : title);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint
                   | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint);
    setMinimumSize(size);
    if (!pos.isNull()) move(pos);
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet("background-color: white;");

    auto *vbox = new QVBoxLayout(this);
    vbox->setContentsMargins(0, 0, 0, 0);
    vbox->setSpacing(0);

    vbox->addLayout(create_top_layout());
    vbox->addLayout(create_info_layout());
    vbox->addLayout(create_content_layout());
    vbox->addLayout(create_result_layout(), 1);

    set_default();
    init_bind();
    adjustSize();
}

NetworkTestDialog::~NetworkTestDialog()
{
    m_download_cancel = true;
    m_closing         = true;
    for (int i = 0; i < TEST_JOB_MAX; ++i) {
        if (test_job[i]) {
            test_job[i]->join();
            delete test_job[i];
            test_job[i] = nullptr;
        }
    }
    if (m_sequence_job) {
        m_sequence_job->join();
        delete m_sequence_job;
        m_sequence_job = nullptr;
    }
}

// ---------------------------------------------------------------------------
// Layout builders
// ---------------------------------------------------------------------------

QLayout *NetworkTestDialog::create_top_layout()
{
    auto *hbox = new QHBoxLayout;
    hbox->setContentsMargins(8, 8, 8, 4);
    hbox->setSpacing(6);

    btn_start = new QPushButton(_L("Start Test Multi-Thread"), this);
    hbox->addWidget(btn_start);

    btn_start_sequence = new QPushButton(_L("Start Test Single-Thread"), this);
    hbox->addWidget(btn_start_sequence);

    btn_download_log = new QPushButton(_L("Export Log"), this);
    btn_download_log->hide();
    hbox->addWidget(btn_download_log);

    hbox->addStretch(1);
    return hbox;
}

QLayout *NetworkTestDialog::create_info_layout()
{
    auto *vbox = new QVBoxLayout;
    vbox->setContentsMargins(8, 4, 8, 4);
    vbox->setSpacing(2);

    text_basic_info = new QLabel(_L("Basic Info"), this);
    vbox->addWidget(text_basic_info);

    auto *ver_row = new QHBoxLayout;
    text_version_title = new QLabel(_L("Studio Version:"), this);
    text_version_val   = new QLabel(this);
    ver_row->addWidget(text_version_title);
    ver_row->addWidget(text_version_val);
    ver_row->addStretch(1);
    vbox->addLayout(ver_row);

    auto *sys_row = new QHBoxLayout;
    txt_sys_info_title = new QLabel(_L("System Version:"), this);
    txt_sys_info_value = new QLabel(this);
    sys_row->addWidget(txt_sys_info_title);
    sys_row->addWidget(txt_sys_info_value);
    sys_row->addStretch(1);
    vbox->addLayout(sys_row);

    auto *dns_row = new QHBoxLayout;
    txt_dns_info_title = new QLabel(_L("DNS Server:"), this);
    txt_dns_info_title->hide();
    txt_dns_info_value = new QLabel(this);
    txt_dns_info_value->hide();
    dns_row->addWidget(txt_dns_info_title);
    dns_row->addWidget(txt_dns_info_value);
    dns_row->addStretch(1);
    vbox->addLayout(dns_row);

    return vbox;
}

QLayout *NetworkTestDialog::create_content_layout()
{
    auto *vbox = new QVBoxLayout;
    vbox->setContentsMargins(8, 4, 8, 4);

    auto *grid = new QGridLayout;
    grid->setSpacing(4);

    int row = 0;
    auto add_row = [&](QPushButton *&btn, const QString &btn_text,
                        QLabel *&title_lbl, const QString &title_text,
                        QLabel *&val_lbl) {
        btn       = new QPushButton(btn_text, this);
        title_lbl = new QLabel(title_text, this);
        val_lbl   = new QLabel(NA_STR, this);
        grid->addWidget(btn,       row, 0);
        grid->addWidget(title_lbl, row, 1, Qt::AlignRight);
        grid->addWidget(val_lbl,   row, 2);
        ++row;
    };

    add_row(btn_link,           _L("Test BambuLab"),         text_link_title,          _L("Test BambuLab:"),          text_link_val);
    add_row(btn_bing,           _L("Test Bing.com"),         text_bing_title,          _L("Test bing.com:"),          text_bing_val);
    add_row(btn_iot,            _L("Test HTTP"),             text_iot_title,           _L("Test HTTP Service:"),      text_iot_value);
    add_row(btn_oss,            _L("Test storage"),          text_oss_title,           _L("Test Storage:"),           text_oss_value);
    add_row(btn_oss_upgrade,    _L("Test storage upgrade"),  text_oss_upgrade_title,   _L("Test Storage Upgrade:"),   text_oss_upgrade_value);
    add_row(btn_oss_download,   _L("Test storage download"), text_oss_download_title,  _L("Test Storage Download:"),  text_oss_download_value);
    add_row(btn_network_plugin, _L("Test plugin download"),  text_network_plugin_title,_L("Test Plugin Download:"),   text_network_plugin_value);
    add_row(btn_oss_upload,     _L("Test Storage Upload"),   text_oss_upload_title,    _L("Test Storage Upload:"),    text_oss_upload_value);

    btn_oss_upload->hide();
    text_oss_upload_title->hide();
    text_oss_upload_value->hide();

    vbox->addLayout(grid);
    return vbox;
}

QLayout *NetworkTestDialog::create_result_layout()
{
    auto *vbox = new QVBoxLayout;
    vbox->setContentsMargins(8, 4, 8, 8);
    vbox->setSpacing(4);

    text_result = new QLabel(_L("Log Info"), this);
    vbox->addWidget(text_result);

    txt_log = new QTextEdit(this);
    txt_log->setReadOnly(true);
    vbox->addWidget(txt_log, 1);
    return vbox;
}

void NetworkTestDialog::init_bind()
{
    connect(btn_start,          &QPushButton::clicked, this, [this] { start_all_job(); });
    connect(btn_start_sequence, &QPushButton::clicked, this, [this] { start_all_job_sequence(); });
    connect(btn_link,           &QPushButton::clicked, this, [this] { start_test_bambulab_thread(); });
    connect(btn_bing,           &QPushButton::clicked, this, [this] { start_test_bing_thread(); });
    connect(btn_iot,            &QPushButton::clicked, this, [this] { start_test_iot_thread(); });
    connect(btn_oss,            &QPushButton::clicked, this, [this] { start_test_oss_thread(); });
    connect(btn_oss_upgrade,    &QPushButton::clicked, this, [this] { start_test_oss_upgrade_thread(); });
    connect(btn_oss_download,   &QPushButton::clicked, this, [this] { start_test_oss_download_thread(); });
    connect(btn_network_plugin, &QPushButton::clicked, this, [this] { start_test_plugin_download_thread(); });
}

// ---------------------------------------------------------------------------
// update_status  (thread-safe: routed to main thread via queued connection)
// ---------------------------------------------------------------------------

void NetworkTestDialog::update_status(int job_id, const QString &info)
{
    QMetaObject::invokeMethod(this, [this, job_id, info] {
        auto set_lbl = [&](QLabel *lbl) { if (lbl) lbl->setText(info); };
        switch (job_id) {
        case TEST_BAMBULAB_JOB:     set_lbl(text_link_val);            break;
        case TEST_BING_JOB:         set_lbl(text_bing_val);            break;
        case TEST_IOT_JOB:          set_lbl(text_iot_value);           break;
        case TEST_OSS_JOB:          set_lbl(text_oss_value);           break;
        case TEST_OSS_UPGRADE_JOB:  set_lbl(text_oss_upgrade_value);   break;
        case TEST_OSS_DOWNLOAD_JOB: set_lbl(text_oss_download_value);  break;
        case TEST_OSS_UPLOAD_JOB:   set_lbl(text_oss_upload_value);    break;
        case TEST_PLUGIN_JOB:       set_lbl(text_network_plugin_value);break;
        default: break;
        }
        // Append to log
        std::time_t t  = std::time(nullptr);
        std::tm    *tm = std::localtime(&t);
        char        ts[64];
        std::strftime(ts, sizeof(ts), "%a %b %d %H:%M:%S", tm);
        if (txt_log)
            txt_log->append(QString("%1: %2").arg(ts).arg(info));
    }, Qt::QueuedConnection);
}

// ---------------------------------------------------------------------------
// Default state
// ---------------------------------------------------------------------------

void NetworkTestDialog::set_default()
{
    for (int i = 0; i < TEST_JOB_MAX; ++i) {
        test_job[i]     = nullptr;
        m_in_testing[i] = false;
    }
    m_sequence_job    = nullptr;
    m_download_cancel = false;
    m_closing         = false;

    if (text_version_val)          text_version_val->setText(get_studio_version());
    if (txt_sys_info_value)        txt_sys_info_value->setText(get_os_info());
    if (txt_dns_info_value)        txt_dns_info_value->setText(get_dns_info());
    if (text_link_val)             text_link_val->setText(NA_STR);
    if (text_bing_val)             text_bing_val->setText(NA_STR);
    if (text_iot_value)            text_iot_value->setText(NA_STR);
    if (text_oss_value)            text_oss_value->setText(NA_STR);
    if (text_oss_upgrade_value)    text_oss_upgrade_value->setText(NA_STR);
    if (text_oss_download_value)   text_oss_download_value->setText(NA_STR);
    if (text_oss_upload_value)     text_oss_upload_value->setText(NA_STR);
    if (text_network_plugin_value) text_network_plugin_value->setText(NA_STR);
}

// ---------------------------------------------------------------------------
// Info queries
// ---------------------------------------------------------------------------

QString NetworkTestDialog::get_studio_version() { return QString(SLIC3R_VERSION); }

QString NetworkTestDialog::get_os_info()
{
    return QSysInfo::prettyProductName() + " " + QSysInfo::kernelVersion();
}

QString NetworkTestDialog::get_dns_info() { return NA_STR; }

// ---------------------------------------------------------------------------
// Job launchers
// ---------------------------------------------------------------------------

void NetworkTestDialog::start_all_job()
{
    start_test_bambulab_thread();
    start_test_bing_thread();
    start_test_iot_thread();
    start_test_oss_thread();
    start_test_oss_upgrade_thread();
    start_test_oss_download_thread();
    start_test_plugin_download_thread();
    start_test_ping_thread();
}

void NetworkTestDialog::start_all_job_sequence()
{
    m_sequence_job = new boost::thread([this] {
        update_status(-1, "start_test_sequence");
        start_test_bing();
        if (m_closing) return;
        start_test_bambulab();
        if (m_closing) return;
        start_test_oss();
        if (m_closing) return;
        start_test_oss_upgrade();
        if (m_closing) return;
        start_test_oss_download();
        if (m_closing) return;
        start_test_plugin_download();
        update_status(-1, "end_test_sequence");
    });
}

void NetworkTestDialog::start_test_ping_thread()
{
    test_job[TEST_PING_JOB] = new boost::thread([this] {
        m_in_testing[TEST_PING_JOB] = true;
        m_in_testing[TEST_PING_JOB] = false;
    });
}

void NetworkTestDialog::start_test_bing_thread()
{
    test_job[TEST_BING_JOB] = new boost::thread([this] { start_test_bing(); });
}

void NetworkTestDialog::start_test_bambulab_thread()
{
    if (m_in_testing[TEST_BAMBULAB_JOB]) return;
    test_job[TEST_BAMBULAB_JOB] = new boost::thread([this] { start_test_bambulab(); });
}

void NetworkTestDialog::start_test_iot_thread()
{
    if (m_in_testing[TEST_IOT_JOB]) return;
    test_job[TEST_IOT_JOB] = new boost::thread([this] { start_test_iot(); });
}

void NetworkTestDialog::start_test_oss_thread()
{
    test_job[TEST_OSS_JOB] = new boost::thread([this] { start_test_oss(); });
}

void NetworkTestDialog::start_test_oss_upgrade_thread()
{
    test_job[TEST_OSS_UPGRADE_JOB] = new boost::thread([this] { start_test_oss_upgrade(); });
}

void NetworkTestDialog::start_test_oss_download_thread()
{
    test_job[TEST_OSS_DOWNLOAD_JOB] = new boost::thread([this] { start_test_oss_download(); });
}

void NetworkTestDialog::start_test_oss_upload_thread()
{
    test_job[TEST_OSS_UPLOAD_JOB] = new boost::thread([this] { start_test_oss_upload(); });
}

void NetworkTestDialog::start_test_plugin_download_thread()
{
    test_job[TEST_PLUGIN_JOB] = new boost::thread([this] { start_test_plugin_download(); });
}

// ---------------------------------------------------------------------------
// Individual tests
// ---------------------------------------------------------------------------

void NetworkTestDialog::start_test_bing()
{
    m_in_testing[TEST_BING_JOB] = true;
    update_status(TEST_BING_JOB, "test bing start...");

    std::string url = "http://www.bing.com/";
    Slic3r::Http http = Slic3r::Http::get(url);
    update_status(-1, QString("[test_bing]: url=%1").arg(url.c_str()));

    int result = -1;
    http.timeout_max(10)
        .on_complete([&result](std::string, unsigned status) {
            if (status == 200) result = 0;
        })
        .on_ip_resolve([this](std::string ip) {
            update_status(TEST_BING_JOB, QString("test bing ip resolved = %1").arg(ip.c_str()));
        })
        .on_error([this](std::string body, std::string error, unsigned status) {
            update_status(TEST_BING_JOB, "test bing failed");
            update_status(-1, QString("status=%1, body=%2, error=%3")
                .arg(status).arg(body.c_str()).arg(error.c_str()));
        })
        .perform_sync();

    update_status(TEST_BING_JOB, result == 0 ? "test bing ok" : "test bing failed");
    m_in_testing[TEST_BING_JOB] = false;
}

void NetworkTestDialog::start_test_bambulab()
{
    m_in_testing[TEST_BAMBULAB_JOB] = true;
    update_status(TEST_BAMBULAB_JOB, "test bambulab start...");

    AppConfig *app_config = wxGetApp().app_config;
    if (!app_config) {
        update_status(TEST_BAMBULAB_JOB, "app config is nullptr");
        m_in_testing[TEST_BAMBULAB_JOB] = false;
        return;
    }

    std::string url = wxGetApp().get_http_url(app_config->get_country_code());
    Slic3r::Http http = Slic3r::Http::get(url);
    update_status(-1, QString("[test_bambulab]: url=%1").arg(url.c_str()));

    int result = -1;
    http.header("accept", "application/json")
        .timeout_max(10)
        .on_complete([&result](std::string, unsigned status) {
            if (status == 200) result = 0;
        })
        .on_ip_resolve([this](std::string ip) {
            update_status(TEST_BAMBULAB_JOB, QString("test bambulab ip resolved = %1").arg(ip.c_str()));
        })
        .on_error([this](std::string body, std::string error, unsigned status) {
            update_status(TEST_BAMBULAB_JOB, "test bambulab failed");
            update_status(-1, QString("status=%1, body=%2, error=%3")
                .arg(status).arg(body.c_str()).arg(error.c_str()));
        })
        .perform_sync();

    update_status(TEST_BAMBULAB_JOB, result == 0 ? "test bambulab ok" : "test bambulab failed");
    m_in_testing[TEST_BAMBULAB_JOB] = false;
}

void NetworkTestDialog::start_test_iot()
{
    m_in_testing[TEST_IOT_JOB] = true;
    update_status(TEST_IOT_JOB, "network agent test not available in this build");
    m_in_testing[TEST_IOT_JOB] = false;
}

void NetworkTestDialog::start_test_oss()
{
    m_in_testing[TEST_OSS_JOB] = true;
    update_status(TEST_OSS_JOB, "test storage start...");

    AppConfig *cfg = wxGetApp().app_config;
    std::string url = "http://upload-file.bambulab.com";
    if (cfg && cfg->get_country_code() == "CN")
        url = "http://upload-file.bambulab.cn";

    Slic3r::Http http = Slic3r::Http::get(url);
    update_status(-1, QString("[test_oss]: url=%1").arg(url.c_str()));

    int result = -1;
    http.timeout_max(15)
        .on_complete([&result](std::string, unsigned status) {
            if (status == 200) result = 0;
        })
        .on_error([this, &result](std::string body, std::string error, unsigned status) {
            if (status == 403) { result = 0; return; }
            update_status(TEST_OSS_JOB, "test storage failed");
            update_status(-1, QString("status=%1, body=%2, error=%3")
                .arg(status).arg(body.c_str()).arg(error.c_str()));
        })
        .perform_sync();

    update_status(TEST_OSS_JOB, result == 0 ? "test storage ok" : "test storage failed");
    m_in_testing[TEST_OSS_JOB] = false;
}

void NetworkTestDialog::start_test_oss_upgrade()
{
    m_in_testing[TEST_OSS_UPGRADE_JOB] = true;
    update_status(TEST_OSS_UPGRADE_JOB, "test storage upgrade start...");

    AppConfig *cfg = wxGetApp().app_config;
    std::string url = "http://upgrade-file.bambulab.com";
    if (cfg && cfg->get_country_code() == "CN")
        url = "http://upgrade-file.bambulab.cn";

    Slic3r::Http http = Slic3r::Http::get(url);
    update_status(-1, QString("[test_oss_upgrade]: url=%1").arg(url.c_str()));

    int result = -1;
    http.timeout_max(15)
        .on_complete([&result](std::string, unsigned status) {
            if (status == 200) result = 0;
        })
        .on_error([this, &result](std::string body, std::string error, unsigned status) {
            if (status == 403) { result = 0; return; }
            update_status(TEST_OSS_UPGRADE_JOB, "test storage upgrade failed");
            update_status(-1, QString("status=%1, body=%2, error=%3")
                .arg(status).arg(body.c_str()).arg(error.c_str()));
        })
        .perform_sync();

    update_status(TEST_OSS_UPGRADE_JOB,
        result == 0 ? "test storage upgrade ok" : "test storage upgrade failed");
    m_in_testing[TEST_OSS_UPGRADE_JOB] = false;
}

void NetworkTestDialog::start_test_oss_download()
{
    m_in_testing[TEST_OSS_DOWNLOAD_JOB] = true;
    update_status(TEST_OSS_DOWNLOAD_JOB, "storage download test not available in this build");
    m_in_testing[TEST_OSS_DOWNLOAD_JOB] = false;
}

void NetworkTestDialog::start_test_oss_upload()
{
    // stub
}

void NetworkTestDialog::start_test_plugin_download()
{
    m_in_testing[TEST_PLUGIN_JOB] = true;
    update_status(TEST_PLUGIN_JOB, "plugin download test not available in this build");
    m_in_testing[TEST_PLUGIN_JOB] = false;
}

// ---------------------------------------------------------------------------
// closeEvent
// ---------------------------------------------------------------------------

void NetworkTestDialog::closeEvent(QCloseEvent *event)
{
    m_download_cancel = true;
    m_closing         = true;
    for (int i = 0; i < TEST_JOB_MAX; ++i) {
        if (test_job[i]) {
            test_job[i]->join();
            delete test_job[i];
            test_job[i] = nullptr;
        }
    }
    QDialog::closeEvent(event);
}

}} // namespace Slic3r::GUI
