#include "GUI_App.hpp"
#include "BBLSplashScreen.hpp"
#include "RemovableDriveManager.hpp"
#include "InstanceCheck.hpp"
#include "Widgets/Label.hpp"
#include "Widgets/StateColor.hpp"
#include "MainFrame.hpp"
#include "Tab.hpp"
#include "Plater.hpp"
#include "ConfigWizard.hpp"
#include "libslic3r/AppConfig.hpp"
#include "libslic3r/PresetBundle.hpp"
#include "slic3r/GUI/GUI_Init.hpp"
#include "../Utils/NetworkAgent.hpp"
#include "DeviceManager.hpp"
#include "DeviceCore/DevManager.h"
#include "UserManager.hpp"
#include "GUI.hpp"
#include <QLockFile>
#include <QApplication>
#include <QPalette>
#include <QStandardPaths>
#include <QDir>
#include <QMetaObject>
#include <QTimer>
#include <QMessageBox>
#include <sstream>
#include <iomanip>
#include <boost/log/trivial.hpp>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include "libslic3r/miniz_extension.hpp"
#include "libslic3r/Utils.hpp"
#include "../Utils/Http.hpp"
#include "DownloadProgressDialog.hpp"
#include "NotificationManager.hpp"
#include <QSysInfo>
#include <sys/stat.h>

// The header forward-declares Slic3r::GUI::QLockFile as an incomplete type.
// Provide a concrete definition here so unique_ptr<QLockFile> can be destroyed.
namespace Slic3r { namespace GUI {
class QLockFile : public ::QLockFile {
public:
    using ::QLockFile::QLockFile;
};
}}

static std::string convert_studio_language_to_api(std::string lang_code)
{
    boost::replace_all(lang_code, "_", "-");
    return lang_code;
}

namespace Slic3r {
namespace GUI {

TryLoadLastMachine::~TryLoadLastMachine() {}

void TryLoadLastMachine::InnerLoad(NetworkAgent* /*agent*/, Slic3r::DeviceManager* /*dev*/) {}

// VersionInfo
std::string VersionInfo::convert_full_version(std::string short_version)
{
    std::vector<std::string> items;
    boost::split(items, short_version, boost::is_any_of("."));
    if (items.size() == VERSION_LEN) {
        std::string result;
        for (int i = 0; i < (int)VERSION_LEN; ++i) {
            std::ostringstream ss;
            ss << std::setw(2) << std::setfill('0') << items[i];
            result += ss.str();
            if (i + 1 < (int)VERSION_LEN) result += ".";
        }
        return result;
    }
    return short_version;
}

Plater* GUI_App::plater()             { return plater_; }
const Plater* GUI_App::plater() const { return plater_; }

bool GUI_App::dark_mode()
{
    QPalette p = QApplication::palette();
    return p.color(QPalette::Window).lightness() < 128;
}

void GUI_App::Update_dark_mode_flag()
{
    m_is_dark_mode = dark_mode();
}

void GUI_App::init_label_colours()
{
    bool is_dark = dark_mode();
    m_color_label_modified          = QColor("#F1754E");
    m_color_label_sys               = is_dark ? QColor("#B2B3B5")   : QColor("#363636");
    m_color_label_default           = is_dark ? QColor(250,250,250) : m_color_label_sys;
    m_color_window_default          = is_dark ? QColor(43,43,43)    : QApplication::palette().color(QPalette::Window);
    m_color_highlight_label_default = is_dark ? QColor(230,230,230) : QApplication::palette().color(QPalette::WindowText);
    m_color_hovered_btn_label       = is_dark ? QColor(255,255,254) : QColor(0,0,0);
    m_color_default_btn_label       = m_color_hovered_btn_label;
    m_color_highlight_default       = is_dark ? QColor(78,78,78)    : QApplication::palette().color(QPalette::Light);
    m_color_selected_btn_bg         = is_dark ? QColor(84,84,91)    : QColor(206,206,206);
    StateColor::SetDarkMode(is_dark);
}

void GUI_App::init_fonts()
{
    m_small_font  = Label::Body_10;
    m_normal_font = Label::Body_10;
    m_bold_font   = Label::Body_10;
    m_bold_font.setBold(true);
    QFont mono;
    mono.setFamily("monospace");
    mono.setStyleHint(QFont::Monospace);
    m_code_font = mono;
    m_code_font.setPointSize(m_normal_font.pointSize());
}

// Called once after the main window is shown, from the idle handler.
void GUI_App::post_init() {}

// Stubs for network/cloud features not yet ported
void GUI_App::copy_network_if_available() {}
void GUI_App::enable_user_preset_folder(bool /*enable*/) {}

QString GUI_App::current_language_code_safe() const
{
    return QStringLiteral("en");
}
bool GUI_App::load_language(QString /*language*/, bool /*initial*/) { return true; }
void GUI_App::check_printer_presets() {}

void GUI_App::load_current_presets(bool active_preset_combox, bool check_printer_presets_)
{
    if (check_printer_presets_)
        check_printer_presets();
    if (!plater_ || !preset_bundle)
        return;
    PrinterTechnology pt = preset_bundle->printers.get_edited_preset().printer_technology();
    plater_->set_printer_technology(pt);
    for (Tab* tab : tabs_list) {
        if (!tab->supports_printer_technology(pt))
            continue;
        if (tab->type() == Preset::TYPE_PRINTER)
            static_cast<TabPrinter*>(tab)->update_pages();
        tab->load_current_preset();
        if (active_preset_combox)
            tab->reactive_preset_combo_box();
    }
}

GUI_App::GUI_App(int& argc, char** argv)
    : QApplication(argc, argv)
{
    // Set application name so QStandardPaths and QSettings use it
    setApplicationName(QString::fromUtf8(SLIC3R_APP_KEY));

    // Establish data_dir before anything else (mirrors original wx logic)
    if (Slic3r::data_dir().empty()) {
        QString dir;
        const char* xdg = std::getenv("XDG_CONFIG_HOME");
        if (xdg && xdg[0] != '\0')
            dir = QString::fromUtf8(xdg);
        else
            dir = QDir::homePath() + QStringLiteral("/.config");
        std::string data_dir_str = (dir + "/" + applicationName()).toStdString();
#if BBL_INTERNAL_TESTING
        data_dir_str += BBL_INTERNAL_TESTING == 1 ? "Internal" : "Beta";
#endif
        namespace fs = boost::filesystem;
        fs::path data_dir_path(data_dir_str);
        fs::path log_dir_path  = data_dir_path / "log";
        if (!fs::exists(data_dir_path)) fs::create_directories(data_dir_path);
        if (!fs::exists(log_dir_path))  fs::create_directories(log_dir_path);
        Slic3r::set_data_dir(data_dir_str);
    } else {
        m_datadir_redefined = true;
    }

    app_config = new AppConfig();
    m_app_conf_exists = app_config->exists();
    if (m_app_conf_exists) {
        std::string error = app_config->load();
        if (!error.empty())
            BOOST_LOG_TRIVIAL(error) << "AppConfig load error: " << error;
    }
}

GUI_App::~GUI_App() {}

bool GUI_App::OnInit()
{
    try {
        return on_init_inner();
    } catch (const std::exception& e) {
        BOOST_LOG_TRIVIAL(fatal) << "GUI_App::OnInit exception: " << e.what();
        return false;
    }
}

bool GUI_App::on_init_inner()
{
    BOOST_LOG_TRIVIAL(info) << boost::format("BambuStudio %1%") % SLIC3R_VERSION;

    // Show splash screen as early as possible.
    BBLSplashScreen* splash = BBLSplashScreen::create();
    splash->show();
    processEvents();

    // Fonts & colours must come before any widget creation.
    ::Label::initSysFont();
    init_label_colours();
    init_fonts();
    Update_dark_mode_flag();

    app_config->set("version", SLIC3R_VERSION);
    app_config->save();

    // Load system + user preset profiles.
    BOOST_LOG_TRIVIAL(info) << "loading presets...";
    preset_bundle = new PresetBundle();
    preset_bundle->setup_directories();
    preset_bundle->set_default_suppressed(true);

    if (!on_init_network())
        BOOST_LOG_TRIVIAL(warning) << "network init failed (non-fatal)";

    // networking update is deferred until after mainframe is shown (see QTimer below)

    if (m_agent && m_agent->is_user_login())
        enable_user_preset_folder(true);
    else
        enable_user_preset_folder(false);

    try {
        std::string errors;
        if (init_params)
            std::tie(init_params->preset_substitutions, errors) =
                preset_bundle->load_presets(*app_config, ForwardCompatibilitySubstitutionRule::EnableSystemSilent);
        else
            preset_bundle->load_presets(*app_config, ForwardCompatibilitySubstitutionRule::EnableSystemSilent);
        if (!errors.empty())
            BOOST_LOG_TRIVIAL(warning) << "Preset load warnings: " << errors;
    } catch (const std::exception& ex) {
        BOOST_LOG_TRIVIAL(error) << "Preset load error: " << ex.what();
    }

    BOOST_LOG_TRIVIAL(info) << "creating main window...";
    mainframe = new MainFrame();

    // load_current_presets requires plater_ which MainFrame sets up.
    load_current_presets();

    mainframe->show();
    setActiveWindow(mainframe);
    if (splash) {
        splash->finish(mainframe);
        splash->deleteLater();
    }

    // Wire up idle-driven post_init (runs once after event loop starts).
    connect(this, &QApplication::focusChanged, this, [this](QWidget*, QWidget*) {
        if (!m_post_initialized && !m_adding_script_handler) {
            m_post_initialized = true;
            post_init();
        }
    });

    // Deferred first-run check (mirrors original CallAfter behaviour)
    QTimer::singleShot(0, this, [this] {
        config_wizard_startup();
    });

    // Deferred network plugin install/update (needs mainframe to exist for dialog parent)
    if (m_networking_need_update) {
        QTimer::singleShot(200, this, [this] {
            BOOST_LOG_TRIVIAL(info) << "on_init_inner: networking plugin needs update";
            int ret = updating_bambu_networking();
            if (ret != 0)
                BOOST_LOG_TRIVIAL(warning) << "networking plugin download failed";
        });
    }

    m_initialized = true;
    BOOST_LOG_TRIVIAL(info) << "GUI init done";
    return true;
}

// ---------------------------------------------------------------------------
// Networking version check
// ---------------------------------------------------------------------------

bool GUI_App::check_networking_version()
{
    std::string network_ver = Slic3r::NetworkAgent::get_version();
    if (!network_ver.empty())
        BOOST_LOG_TRIVIAL(info) << "get_network_agent_version=" << network_ver;
    std::string studio_ver = SLIC3R_VERSION;
    if (network_ver.length() >= 8 && network_ver.substr(0, 8) == studio_ver.substr(0, 8)) {
        m_networking_compatible = true;
        return true;
    }
    m_networking_compatible = false;
    return false;
}

bool GUI_App::is_compatibility_version()
{
    return m_networking_compatible;
}

void GUI_App::cancel_networking_install()
{
    m_networking_cancel_update = true;
    BOOST_LOG_TRIVIAL(info) << "plugin install cancelled";
}

// ---------------------------------------------------------------------------
// HTTP extra headers
// ---------------------------------------------------------------------------

std::map<std::string, std::string> GUI_App::get_extra_header()
{
    std::map<std::string, std::string> hdrs;
    hdrs["X-BBL-Client-Type"]    = "slicer";
    hdrs["X-BBL-Client-Name"]    = SLIC3R_APP_NAME;
    hdrs["X-BBL-Client-Version"] = VersionInfo::convert_full_version(SLIC3R_VERSION);
#if defined(__WINDOWS__)
    hdrs["X-BBL-OS-Type"] = "windows";
#elif defined(__APPLE__)
    hdrs["X-BBL-OS-Type"] = "macos";
#else
    hdrs["X-BBL-OS-Type"] = "linux";
#endif
    hdrs["X-BBL-OS-Version"] = QSysInfo::kernelVersion().toStdString();
    if (app_config)
        hdrs["X-BBL-Device-ID"] = app_config->get("slicer_uuid");
    hdrs["X-BBL-Language"] = convert_studio_language_to_api(into_u8(current_language_code_safe()));
    return hdrs;
}

void GUI_App::init_http_extra_header()
{
    auto hdrs = get_extra_header();
    Slic3r::Http::set_extra_headers(hdrs);  // fix: also set global Http headers
    if (m_agent)
        m_agent->set_extra_http_header(hdrs);
}

void GUI_App::update_http_extra_header()
{
    auto hdrs = get_extra_header();
    Slic3r::Http::set_extra_headers(hdrs);
    if (m_agent)
        m_agent->set_extra_http_header(hdrs);
}

// ---------------------------------------------------------------------------
// Network plugin init
// ---------------------------------------------------------------------------

bool GUI_App::on_init_network(bool try_backup)
{
    int load_ok = Slic3r::NetworkAgent::initialize_network_module(
        false, !app_config->get_bool("ignore_module_cert"));
    bool create_agent = false;

retry:
    if (!load_ok) {
        BOOST_LOG_TRIVIAL(info) << "on_init_network: dll loaded";
        if (check_networking_version()) {
            auto src = Slic3r::NetworkAgent::get_bambu_source_entry();
            if (!src) {
                BOOST_LOG_TRIVIAL(info) << "on_init_network: no bambu source module";
                m_networking_compatible = false;
                m_networking_need_update = true;
            } else {
                create_agent = true;
            }
        } else {
            if (try_backup) {
                Slic3r::NetworkAgent::unload_network_module();
                load_ok = Slic3r::NetworkAgent::initialize_network_module(
                    true, !app_config->get_bool("ignore_module_cert"));
                try_backup = false;
                goto retry;
            }
            BOOST_LOG_TRIVIAL(info) << "on_init_network: version mismatch";
            m_networking_need_update = true;
        }
    } else {
        BOOST_LOG_TRIVIAL(info) << "on_init_network: dll load failed";
        m_networking_need_update = true;
    }

    if (create_agent) {
        BOOST_LOG_TRIVIAL(info) << "on_init_network: creating agent";
        std::string data_directory = data_dir();
        m_agent = new Slic3r::NetworkAgent(data_directory);

        if (!m_device_manager)
            m_device_manager = new Slic3r::DeviceManager(m_agent);
        else
            m_device_manager->set_agent(m_agent);

        if (!m_user_manager)
            m_user_manager = new Slic3r::UserManager(m_agent);
        else
            m_user_manager->set_agent(m_agent);

        m_device_manager->EnableMultiMachine(is_enable_multi_machine());

        m_agent->set_config_dir(data_directory);
        m_agent->init_log();
        m_agent->set_cert_file(resources_dir() + "/cert", "slicer_base64.cer");

        init_http_extra_header();
        init_networking_callbacks();

        std::string country_code = app_config->get_country_code();
        m_agent->set_country_code(country_code);
        m_agent->start();
        m_load_last_machine.InnerLoad(m_agent, getDeviceManager());
    } else {
        Slic3r::NetworkAgent::unload_network_module();
        if (!m_device_manager) m_device_manager = new Slic3r::DeviceManager();
        if (!m_user_manager)   m_user_manager   = new Slic3r::UserManager();
    }
    return true;
}

// ---------------------------------------------------------------------------
// Networking callbacks — CallAfter → QMetaObject::invokeMethod queued
// ---------------------------------------------------------------------------

void GUI_App::init_networking_callbacks()
{
    BOOST_LOG_TRIVIAL(info) << "init_networking_callbacks: enter, agent=" << m_agent;
    if (!m_agent) return;

    m_agent->set_on_server_connected_fn([this](int return_code, int /*reason_code*/) {
        if (is_closing()) return;
        QMetaObject::invokeMethod(this, [this, return_code] {
            if (is_closing()) return;
            BOOST_LOG_TRIVIAL(trace) << "server_connected return_code=" << return_code;
            if (return_code == 5) {
                request_user_logout();
                return;
            }
            if (return_code < 0) return;
            // Server connected — load last machine
            if (m_agent->is_user_login()) {
                DeviceManager* dev = getDeviceManager();
                if (dev) m_load_last_machine.TryLoadFromMqttCB(m_agent, dev);
            }
        }, Qt::QueuedConnection);
    });

    m_agent->set_on_printer_connected_fn([this](std::string dev_id) {
        if (is_closing()) return;
        QMetaObject::invokeMethod(this, [this, dev_id] {
            if (is_closing() || !m_device_manager) return;
            bool tunnel = dev_id.rfind("tunnel/", 0) == 0;
            auto* obj = m_device_manager->get_my_machine(
                tunnel ? dev_id.substr(7) : dev_id);
            if (obj) {
                obj->is_tunnel_mqtt = tunnel;
                obj->command_request_push_all(true);
                obj->command_get_version();
                obj->erase_user_access_code();
                obj->command_get_access_code();
                if (m_agent)
                    m_agent->install_device_cert(obj->get_dev_id(), obj->is_lan_mode_printer());
            }
        }, Qt::QueuedConnection);
    });

    m_agent->set_get_country_code_fn([this]() -> std::string {
        return app_config ? app_config->get_country_code() : std::string();
    });

    m_agent->set_on_subscribe_failure_fn([this](std::string dev_id) {
        QMetaObject::invokeMethod(this, [this, dev_id] {
            on_start_subscribe_again(dev_id);
        }, Qt::QueuedConnection);
    });

    m_agent->set_on_local_connect_fn([this](int state, std::string dev_id, std::string msg) {
        if (is_closing()) return;
        QMetaObject::invokeMethod(this, [this, state, dev_id, msg] {
            if (is_closing() || !m_device_manager) return;
            auto* obj = m_device_manager->get_my_machine(dev_id);
            if (!obj) return;
            if (obj->is_lan_mode_printer()) {
                if (state == ConnectStatus::ConnectStatusOk) {
                    obj->command_request_push_all(true);
                    obj->command_get_version();
                    obj->record_user_access_dev_ip();
                    if (app_config)
                        app_config->set_str("access_code", obj->get_dev_id(), obj->get_access_code());
                } else if (state == ConnectStatus::ConnectStatusFailed) {
                    m_device_manager->erase_local_machine(obj->get_dev_id());
                    m_device_manager->set_selected_machine("");
                    if (msg == "5") {
                        obj->set_access_code("");
                        obj->erase_user_access_code();
                        obj->erase_user_access_dev_ip();
                    }
                } else if (state == ConnectStatus::ConnectStatusLost) {
                    m_device_manager->set_selected_machine("");
                }
            }
        }, Qt::QueuedConnection);
    });

    auto message_arrive_fn = [this](std::string dev_id, std::string msg) {
        if (is_closing()) return;
        QMetaObject::invokeMethod(this, [this, dev_id, msg] {
            if (is_closing() || !m_device_manager) return;
            if (process_network_msg(dev_id, msg)) return;
            if (auto* obj = m_device_manager->get_user_machine(dev_id)) {
                auto* sel = m_device_manager->get_selected_machine();
                if (sel && sel->get_dev_id() == dev_id)
                    obj->parse_json("cloud", msg);
                else
                    obj->parse_json("cloud", msg, true);
            }
        }, Qt::QueuedConnection);
    };
    m_agent->set_on_message_fn(message_arrive_fn);

    auto user_message_fn = [this](std::string user_id, std::string msg) {
        if (is_closing()) return;
        QMetaObject::invokeMethod(this, [this, user_id, msg] {
            if (is_closing() || !m_user_manager) return;
            if (m_agent && user_id == m_agent->get_user_id())
                m_user_manager->parse_json(msg);
        }, Qt::QueuedConnection);
    };
    m_agent->set_on_user_message_fn(user_message_fn);

    auto lan_message_fn = [this](std::string dev_id, std::string msg) {
        if (is_closing()) return;
        QMetaObject::invokeMethod(this, [this, dev_id, msg] {
            if (is_closing() || !m_device_manager) return;
            if (process_network_msg(dev_id, msg)) return;
            if (auto* obj = m_device_manager->get_my_machine(dev_id))
                obj->parse_json("lan", msg);
        }, Qt::QueuedConnection);
    };
    m_agent->set_on_local_message_fn(lan_message_fn);

    // This is the critical bridge: plugin background threads → Qt main thread
    m_agent->set_queue_on_main_fn([this](std::function<void()> callback) {
        QMetaObject::invokeMethod(this, std::move(callback), Qt::QueuedConnection);
    });

    BOOST_LOG_TRIVIAL(info) << "init_networking_callbacks: done";
}

// ---------------------------------------------------------------------------
// First-run wizard gate
// ---------------------------------------------------------------------------

bool GUI_App::config_wizard_startup()
{
    if (!m_app_conf_exists || preset_bundle->printers.only_default_printers()) {
        BOOST_LOG_TRIVIAL(info) << "config_wizard_startup: first run detected";
        return run_wizard(ConfigWizard::RR_DATA_EMPTY);
    }
    return false;
}

bool GUI_App::run_wizard(ConfigWizard::RunReason /*reason*/,
                         ConfigWizard::StartPage /*start_page*/)
{
    // GuideFrame (the BBL first-run UI) is not yet ported.
    // Mark first-run as done so we don't block startup on every launch.
    std::string finish = app_config->get("firstguide", "finish");
    if (finish != "true") {
        BOOST_LOG_TRIVIAL(info) << "run_wizard: marking first-run complete (GuideFrame not yet ported)";
        app_config->set("firstguide", "finish", "true");
        app_config->save();

        // Inform the user once.
        QMessageBox::information(
            mainframe,
            _L("First Run"),
            _L("The first-run setup wizard (GuideFrame) is not yet ported to Qt.\n"
               "Printer profiles can be configured manually via the Settings menu.\n"
               "Network plugin will be downloaded automatically when available."));
    }
    return false;
}

// ---------------------------------------------------------------------------
// Stubs for features not yet ported
// ---------------------------------------------------------------------------

void GUI_App::on_start_subscribe_again(std::string /*dev_id*/) {}

bool GUI_App::process_network_msg(std::string /*dev_id*/, std::string /*msg*/)
{
    return false;
}

void GUI_App::show_dialog(QString msg)
{
    QMessageBox::information(mainframe, _L("Notice"), msg);
}

void GUI_App::request_user_logout()
{
    if (m_agent) m_agent->user_logout();
}

std::string GUI_App::get_plugin_url(std::string name, std::string country_code)
{
    std::string url = get_http_url(country_code);
    std::string curr_version = SLIC3R_VERSION;
    std::string using_version = curr_version.substr(0, 9) + "00";
    if (name == "cameratools")
        using_version = curr_version.substr(0, 6) + "00.00";
    url += (boost::format("?slicer/%1%/cloud=%2%") % name % using_version).str();
    return url;
}

std::string GUI_App::get_http_url(std::string country_code, std::string path)
{
    std::string url;
    if (country_code == "CN") {
        url = "https://api.bambulab.cn/";
    } else if (country_code == "ENV_CN_DEV") {
        url = "https://api-dev.bambu-lab.com/";
    } else if (country_code == "ENV_CN_QA") {
        url = "https://api-qa.bambu-lab.com/";
    } else if (country_code == "ENV_CN_PRE") {
        url = "https://api-pre.bambu-lab.com/";
    } else if (country_code == "NEW_ENV_DEV_HOST") {
        url = "https://api-dev.bambulab.net/";
    } else if (country_code == "NEW_ENV_QAT_HOST") {
        url = "https://api-qa.bambulab.net/";
    } else if (country_code == "NEW_ENV_PRE_HOST") {
        url = "https://api-pre.bambulab.net/";
    } else {
        url = "https://api.bambulab.com/";
    }
    url += path.empty() ? "v1/iot-service/api/slicer/resource" : path;
    return url;
}

void GUI_App::update_publish_status() { /* TODO: Qt port stub */ }

bool GUI_App::has_model_mall()
{
    // Mirrors wx: returns true when the region allows model mall
    const std::string region = app_config ? app_config->get("region") : "";
    // model mall available for all regions except mainland China
    return region != "CHN";
}

void GUI_App::save_privacy_policy_history(bool /*agree*/, std::string /*source*/) { /* TODO: Qt port stub */ }

void GUI_App::switch_staff_pick(bool /*on*/) { /* TODO: Qt port stub */ }

// ---------------------------------------------------------------------------
// Stubs for methods not yet fully ported (pending wider Qt migration)
// ---------------------------------------------------------------------------
void GUI_App::request_login(bool /*show_user_info*/) { /* TODO: Qt port stub */ }
void GUI_App::handle_http_error(unsigned int /*status*/, std::string /*body*/) { /* TODO: Qt port stub */ }
void GUI_App::remove_user_presets() { /* TODO: Qt port stub */ }
void GUI_App::start_sync_user_preset(bool /*with_progress_dlg*/) { /* TODO: Qt port stub */ }

// ---------------------------------------------------------------------------
// Network plugin download / install / restart helpers
// ---------------------------------------------------------------------------

// Parse zip extra-field for UTF-8 filename (Info-ZIP 0x7075 block).
// Falls back to decode_path() when absent.
static std::string decode(std::string const &extra,
                          std::string const &path = {})
{
    const char *p = extra.data();
    const char *e = p + extra.length();
    while (p + 4 < e) {
        boost::uint16_t len =
            (boost::uint16_t)(unsigned char)p[2] |
            ((boost::uint16_t)(unsigned char)p[3] << 8);
        if (p[0] == '\x75' && p[1] == '\x70' && len >= 5 &&
            p + 4 + (ptrdiff_t)len <= e && p[4] == '\x01')
            return std::string(p + 9, p + 4 + len);
        else
            p += 4 + len;
    }
    return Slic3r::decode_path(path.c_str());
}

int GUI_App::download_plugin(std::string name, std::string package_name,
                             InstallProgressFn pro_fn,
                             WasCancelledFn cancel_fn)
{
    using nlohmann::json;

    int result = 0;
    json j;
    std::string err_msg;

    if (!app_config) {
        j["result"]    = "failed";
        j["error_msg"] = "app_config is nullptr";
        if (m_agent) m_agent->track_event("networkplugin_download", j.dump());
        return -1;
    }

    BOOST_LOG_TRIVIAL(info) << "[download_plugin]: enter";
    m_networking_cancel_update = false;

    // Ensure X-BBL-* identification headers are on the global Http client
    Slic3r::Http::set_extra_headers(get_extra_header());

    fs::path target_file_path = fs::temp_directory_path() / package_name;
    fs::path tmp_path = target_file_path;
    tmp_path += "." + std::to_string(Slic3r::get_current_pid()) + ".tmp";

    // Step 1: fetch version manifest to get the real download URL
    std::string url = get_plugin_url(name, app_config->get_country_code());
    std::string download_url;
    BOOST_LOG_TRIVIAL(info) << "[download_plugin]: check the plugin from " << url;

    Slic3r::Http::get(url)
        .timeout_connect(TIMEOUT_CONNECT)
        .timeout_max(TIMEOUT_RESPONSE)
        .on_complete([&download_url](std::string body, unsigned /*status*/) {
            try {
                json j = json::parse(body);
                if (j["message"].get<std::string>() == "success") {
                    auto &resources = j.at("resources");
                    if (resources.is_array()) {
                        for (auto &item : resources) {
                            for (auto it = item.begin(); it != item.end(); ++it)
                                if (boost::iequals(it.key(), "url"))
                                    download_url = it.value();
                        }
                    }
                } else {
                    BOOST_LOG_TRIVIAL(info)
                        << "[download_plugin 1]: message!=success body=" << body;
                }
            } catch (...) {
                BOOST_LOG_TRIVIAL(error)
                    << "[download_plugin 1]: exception parsing manifest";
            }
        })
        .on_error([&result, &err_msg](std::string body, std::string error,
                                      unsigned /*status*/) {
            BOOST_LOG_TRIVIAL(error)
                << "[download_plugin 1] on_error: " << error
                << ", body=" << body;
            err_msg += "[download_plugin 1] on_error: " + error;
            result = -1;
        })
        .perform_sync();

    bool cancel = false;
    if (result < 0) {
        j["result"] = "failed"; j["error_msg"] = err_msg;
        if (m_agent) m_agent->track_event("networkplugin_download", j.dump());
        if (pro_fn) pro_fn(InstallStatusDownloadFailed, 0, cancel);
        return result;
    }
    if (download_url.empty()) {
        BOOST_LOG_TRIVIAL(info)
            << "[download_plugin 1]: no available plugin for version "
            << SLIC3R_VERSION;
        if (pro_fn) pro_fn(InstallStatusDownloadFailed, 0, cancel);
        j["result"]    = "failed";
        j["error_msg"] = std::string("no available plugin for version ") +
                         SLIC3R_VERSION;
        if (m_agent) m_agent->track_event("networkplugin_download", j.dump());
        return -1;
    } else if (pro_fn) {
        pro_fn(InstallStatusNormal, 5, cancel);
    }

    if (m_networking_cancel_update || cancel) {
        j["result"] = "failed"; j["error_msg"] = "cancelled by user";
        if (m_agent) m_agent->track_event("networkplugin_download", j.dump());
        return -1;
    }

    BOOST_LOG_TRIVIAL(info) << "[download_plugin] download_url=" << download_url;

    // Step 2: download the zip
    int reported_percent = 0;
    Slic3r::Http::get(download_url)
        .on_progress([this, &pro_fn, cancel_fn, &result, &reported_percent,
                      &err_msg](Slic3r::Http::Progress progress, bool &cancel) {
            int percent = 0;
            if (progress.dltotal != 0)
                percent = (int)(progress.dlnow * 50 / progress.dltotal);
            bool was_cancel = false;
            if (pro_fn && (percent - reported_percent) >= 10) {
                pro_fn(InstallStatusNormal, percent, was_cancel);
                reported_percent = percent;
            }
            cancel = m_networking_cancel_update || was_cancel;
            if (cancel_fn && cancel_fn()) cancel = true;
            if (cancel) { err_msg += "[download_plugin] cancel"; result = -1; }
        })
        .on_complete([&pro_fn, tmp_path, target_file_path](std::string body,
                                                           unsigned /*status*/) {
            bool cancel = false;
            fs::fstream file(tmp_path,
                             std::ios::out | std::ios::binary | std::ios::trunc);
            file.write(body.c_str(), (std::streamsize)body.size());
            file.close();
            fs::rename(tmp_path, target_file_path);
            if (pro_fn) pro_fn(InstallStatusDownloadCompleted, 80, cancel);
        })
        .on_error([&pro_fn, &result, &err_msg](std::string body,
                                               std::string error,
                                               unsigned /*status*/) {
            bool cancel = false;
            if (pro_fn) pro_fn(InstallStatusDownloadFailed, 0, cancel);
            BOOST_LOG_TRIVIAL(error)
                << "[download_plugin 2] on_error: " << error
                << ", body=" << body;
            err_msg += "[download_plugin 2] on_error: " + error;
            result = -1;
        })
        .perform_sync();

    j["result"]    = result < 0 ? "failed" : "success";
    j["error_msg"] = err_msg;
    if (m_agent) m_agent->track_event("networkplugin_download", j.dump());
    return result;
}

int GUI_App::install_plugin(std::string name, std::string package_name,
                            InstallProgressFn pro_fn,
                            WasCancelledFn /*cancel_fn*/)
{
    bool cancel = false;
    std::string target_file_path =
        (fs::temp_directory_path() / package_name).string();

    BOOST_LOG_TRIVIAL(info) << "[install_plugin] enter";

    boost::filesystem::path data_dir_path(data_dir());
    auto plugin_folder = data_dir_path / name;
    auto backup_folder = plugin_folder / "backup";

    if (!boost::filesystem::exists(plugin_folder)) {
        BOOST_LOG_TRIVIAL(info) << "[install_plugin] creating plugin dir";
        boost::filesystem::create_directory(plugin_folder);
    }
    if (!boost::filesystem::exists(backup_folder)) {
        BOOST_LOG_TRIVIAL(info) << "[install_plugin] creating backup dir";
        boost::filesystem::create_directory(backup_folder);
    }

    if (m_networking_cancel_update) return -1;
    if (pro_fn) pro_fn(InstallStatusNormal, 50, cancel);

    mz_zip_archive archive;
    mz_zip_zero_struct(&archive);
    if (!open_zip_reader(&archive, target_file_path)) {
        BOOST_LOG_TRIVIAL(error) << "[install_plugin] open zip failed";
        if (pro_fn) pro_fn(InstallStatusDownloadFailed, 0, cancel);
        return InstallStatusUnzipFailed;
    }

    mz_uint num_entries = mz_zip_reader_get_num_files(&archive);
    BOOST_LOG_TRIVIAL(info)
        << boost::format("[install_plugin] %1% entries") % num_entries;

    for (mz_uint i = 0; i < num_entries; i++) {
        if (m_networking_cancel_update || cancel) {
            close_zip_reader(&archive);
            return -1;
        }
        mz_zip_archive_file_stat stat;
        if (!mz_zip_reader_file_stat(&archive, i, &stat)) {
            BOOST_LOG_TRIVIAL(error)
                << boost::format("[install_plugin] stat failed for entry %1%") % i;
            continue;
        }
        if (stat.m_uncomp_size == 0) continue;

        std::string dest_file;
        if (stat.m_is_utf8) {
            dest_file = stat.m_filename;
        } else {
            std::string extra(1024, 0);
            size_t n = mz_zip_reader_get_extra(
                &archive, stat.m_file_index, extra.data(), extra.size());
            dest_file = decode(extra.substr(0, n), stat.m_filename);
        }

        auto dest_path = plugin_folder / dest_file;
        boost::filesystem::create_directories(dest_path.parent_path());
        std::string dest_zip_file =
            Slic3r::encode_path(dest_path.string().c_str());

        try {
            if (fs::exists(dest_path)) fs::remove(dest_path);
            mz_bool res = 0;
#ifndef WIN32
            if (S_ISLNK(stat.m_external_attr >> 16)) {
                std::string link(stat.m_uncomp_size + 1, 0);
                res = mz_zip_reader_extract_to_mem(
                    &archive, stat.m_file_index, link.data(),
                    stat.m_uncomp_size, 0);
                try {
                    boost::filesystem::create_symlink(link, dest_path);
                } catch (const std::exception &ex) {
                    BOOST_LOG_TRIVIAL(info)
                        << "[install_plugin] create_symlink: " << ex.what();
                }
            } else {
#endif
                res = mz_zip_reader_extract_to_file(
                    &archive, stat.m_file_index, dest_zip_file.c_str(), 0);
#ifndef WIN32
            }
#endif
            BOOST_LOG_TRIVIAL(info)
                << boost::format("[install_plugin] extracted %1%") % dest_file;

            if (res == 0) {
                mz_zip_error zip_error = mz_zip_get_last_error(&archive);
                BOOST_LOG_TRIVIAL(error)
                    << "[install_plugin] zip error: "
                    << mz_zip_get_error_string(zip_error);
                close_zip_reader(&archive);
                if (pro_fn) pro_fn(InstallStatusUnzipFailed, 0, cancel);
                return InstallStatusUnzipFailed;
            }
        } catch (const std::exception &ex) {
            close_zip_reader(&archive);
            BOOST_LOG_TRIVIAL(error) << "[install_plugin] exception: " << ex.what();
            if (pro_fn) pro_fn(InstallStatusUnzipFailed, 0, cancel);
            return InstallStatusUnzipFailed;
        }
    }

    close_zip_reader(&archive);

    // Copy extracted files to backup folder
    if (fs::exists(plugin_folder) && fs::is_directory(plugin_folder)) {
        int file_count = 0, file_index = 0;
        for (fs::directory_iterator it(plugin_folder);
             it != fs::directory_iterator(); ++it)
            if (fs::is_regular_file(it->status())) ++file_count;

        for (fs::directory_iterator it(plugin_folder);
             it != fs::directory_iterator(); ++it) {
            if (it->path() == backup_folder) continue;
            std::string dest =
                (backup_folder / it->path().filename()).string();
            if (fs::is_regular_file(it->status())) {
                try {
                    if (file_count > 0 && pro_fn)
                        pro_fn(InstallStatusNormal,
                               50 + file_index / file_count, cancel);
                    ++file_index;
                    if (fs::exists(dest)) fs::remove(dest);
                    std::string err;
                    Slic3r::copy_file(it->path().string(), dest, err, false);
                } catch (const std::exception &ex) {
                    BOOST_LOG_TRIVIAL(error)
                        << "[install_plugin] backup copy failed: " << ex.what();
                }
            } else {
                Slic3r::copy_framework(it->path().string(), dest);
            }
        }
    }

    if (pro_fn) pro_fn(InstallStatusInstallCompleted, 100, cancel);
    if (name == "plugins")
        app_config->set_str("app", "installed_networking", "1");
    BOOST_LOG_TRIVIAL(info) << "[install_plugin] success";
    return 0;
}

void GUI_App::restart_networking()
{
    BOOST_LOG_TRIVIAL(info)
        << __FUNCTION__
        << boost::format(" enter, mainframe %1%") % mainframe;

    on_init_network(true);
    m_networking_need_update = false;

    if (m_agent) {
        init_networking_callbacks();

        m_agent->set_on_ssdp_msg_fn([this](std::string json_str) {
            if (is_closing()) return;
            QMetaObject::invokeMethod(this, [this, json_str]() {
                if (m_device_manager)
                    m_device_manager->on_machine_alive(json_str);
            }, Qt::QueuedConnection);
        });

        m_agent->set_on_http_error_fn([this](unsigned int status,
                                             std::string body) {
            this->handle_http_error(status, body);
        });

        m_agent->start_discovery(true, false);

        if (mainframe)
            mainframe->refresh_plugin_tips();

        if (plater_) {
            auto *nm = plater_->get_notification_manager();
            if (nm) nm->bbl_close_plugin_install_notification();
        }

        if (m_agent->is_user_login()) {
            remove_user_presets();
            enable_user_preset_folder(true);
            preset_bundle->load_user_presets(
                m_agent->get_user_id(),
                ForwardCompatibilitySubstitutionRule::Enable);
            if (mainframe) mainframe->update_side_preset_ui();
        }

        if (app_config && app_config->get("sync_user_preset") == "true")
            start_sync_user_preset();
    }

    BOOST_LOG_TRIVIAL(info)
        << __FUNCTION__
        << boost::format(" exit, m_agent=%1%") % m_agent;
}

int GUI_App::updating_bambu_networking()
{
    // First-time install: ask the user before downloading.
    if (app_config->get("installed_networking") != "1") {
        int answer = QMessageBox::question(
            mainframe,
            _L("Install Network Plugin"),
            _L("Bambu Studio requires a network plugin to connect to your printer.\n"
               "Would you like to download and install it now?"),
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::Yes);
        if (answer != QMessageBox::Yes)
            return 0;
    }
    DownloadProgressDialog dlg(_L("Downloading Bambu Network Plug-in"), false);
    dlg.exec();
    return 0;
}

} // namespace GUI
} // namespace Slic3r
