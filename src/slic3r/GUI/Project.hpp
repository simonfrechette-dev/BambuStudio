#ifndef slic3r_Project_hpp_
#define slic3r_Project_hpp_

// Phase 4 TODO: Qt port of ProjectPanel

#include <map>
#include <vector>
#include <string>
#include <QWidget>
#include <QString>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace Slic3r {
namespace GUI {

// Event ID replacing wxDECLARE_EVENT(EVT_PROJECT_RELOAD, QEvent)
static constexpr int EVT_PROJECT_RELOAD = 10400;

class ProjectPanel : public QWidget
{
    Q_OBJECT
public:
    explicit ProjectPanel(QWidget* parent = nullptr);
    ~ProjectPanel() override = default;

    void msw_rescale();
    void update_model_data();
    void clear_model_info();

    bool isVisible() const;
    void OnScriptMessage(const QString& msg);
    void RunScript(const std::string& content);
    bool is_editing_page() const;

    std::map<std::string, std::vector<json>> Reload(const QString& aux_path);
    std::string formatBytes(unsigned long bytes);
    std::string get_model_id(std::string design_id);
    QString to_base64(const std::string& path);
    void save_project();

protected:
    void showEvent(QShowEvent* e) override;

private slots:
    void refresh_info();

private:
    // Info labels (filled by refresh_info)
    QLabel* m_lbl_name     { nullptr };
    QLabel* m_lbl_path     { nullptr };
    QLabel* m_lbl_objects  { nullptr };
    QLabel* m_lbl_print    { nullptr };
    QLabel* m_lbl_filament { nullptr };
    QLabel* m_lbl_printer  { nullptr };

    bool   m_web_init_completed = false;
    bool   m_reload_already     = false;
    QString m_project_home_url;
    QString m_root_dir;
    std::map<std::string, std::string> m_model_id_map;
    static inline int m_sequence_id = 8000;
    json m_last_payload = json::object();
};

} // namespace GUI
} // namespace Slic3r

#endif // slic3r_Project_hpp_
