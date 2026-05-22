#ifndef slic3r_GUI_hpp_
#define slic3r_GUI_hpp_

namespace boost { class any; }
namespace boost::filesystem { class path; }

#include <QString>
#include <string>
#include "libslic3r/Config.hpp"
#include "libslic3r/Preset.hpp"

class QWidget;
class QMenuBar;
class QComboBox;
class QFileDialog;

namespace Slic3r {

class AppConfig;
class DynamicPrintConfig;
class Print;

namespace GUI {

void disable_screensaver();
void enable_screensaver();
bool debugged();
void break_to_debugger();

extern const std::string& shortkey_ctrl_prefix();
extern const std::string& shortkey_alt_prefix();

extern AppConfig* get_app_config();

// Change option value in config
void change_opt_value(DynamicPrintConfig& config, const t_config_option_key& opt_key, const boost::any& value, int opt_index = 0);

void show_error(QWidget* parent, const QString& message, bool monospaced_font = false);
void show_error(QWidget* parent, const char* message, bool monospaced_font = false);
inline void show_error(QWidget* parent, const std::string& message, bool monospaced_font = false) { show_error(parent, message.c_str(), monospaced_font); }
void show_error_id(int id, const std::string& message);
void show_info(QWidget* parent, const QString& message, const QString& title = QString());
void show_info(QWidget* parent, const char* message, const char* title = nullptr);
inline void show_info(QWidget* parent, const std::string& message, const std::string& title = std::string()) { show_info(parent, message.c_str(), title.c_str()); }
void warning_catcher(QWidget* parent, const QString& message);
void show_substitutions_info(const PresetsConfigSubstitutions& presets_config_substitutions);
void show_substitutions_info(const ConfigSubstitutions& config_substitutions, const std::string& filename);

extern void about();
extern void login();
extern void desktop_open_datadir_folder();
extern void desktop_open_any_folder(const std::string& path);

// QString compat shims
inline QString from_u8(const std::string &str) { return QString::fromUtf8(str.c_str()); }
inline std::string into_u8(const QString &str) { return str.toUtf8().toStdString(); }
inline QString from_path(const boost::filesystem::path &path);
inline boost::filesystem::path into_path(const QString &str);

} // namespace GUI
} // namespace Slic3r

#endif // slic3r_GUI_hpp_
