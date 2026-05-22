#ifndef slic3r_Auxiliary_hpp_
#define slic3r_Auxiliary_hpp_

// Phase 4: Qt port of AuxiliaryPanel

#include <map>
#include <vector>
#include <string>
#include <QWidget>
#include <QString>
#include <boost/filesystem.hpp>

class QListWidget;
class QListWidgetItem;
class QLabel;

namespace fs = boost::filesystem;

namespace Slic3r {
namespace GUI {

class AuFolderPanel;
class DesignerPanel;
class Tabbook;
class AuFile;
class AuFiles;

// Event IDs replacing wxDECLARE_EVENT
static constexpr int EVT_AUXILIARY_IMPORT        = 10300;
static constexpr int EVT_AUXILIARY_UPDATE_COVER  = 10301;
static constexpr int EVT_AUXILIARY_UPDATE_DELETE = 10302;
static constexpr int EVT_AUXILIARY_UPDATE_RENAME = 10303;

class AuxiliaryPanel : public QWidget
{
    Q_OBJECT
public:
    explicit AuxiliaryPanel(QWidget* parent = nullptr);
    ~AuxiliaryPanel() override = default;

    void init_bitmap();
    void init_tabpanel();

    void Split(const std::string& src, const std::string& separator, std::vector<std::string>& dest);
    void msw_rescale();
    void on_size();
    bool isVisible() const;

    // core logic
    std::map<std::string, std::vector<fs::path>> m_paths_list;
    QString m_root_dir;
    void    init_auxiliary();
    void    create_folder(const QString& name = QString());
    std::string replaceSpace(std::string s, std::string ts, std::string ns);
    void    Reload(const QString& aux_path);
    void    update_all_panel();
    void    update_all_cover();

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private:
    QListWidget* m_file_list { nullptr };
    QLabel*      m_hint_lbl  { nullptr };

    void add_file_item(const QString& path);
    void update_hint_visibility();
};

} // namespace GUI
} // namespace Slic3r

#endif // slic3r_Auxiliary_hpp_
