// Phase 4 TODO: Qt port of GUI_ObjectSettings
#ifndef slic3r_GUI_ObjectSettings_hpp_
#define slic3r_GUI_ObjectSettings_hpp_

#include <memory>
#include <vector>
#include <QWidget>
#include <QLayout>

#define NEW_OBJECT_SETTING 1

namespace Slic3r {
class DynamicPrintConfig;
class ModelConfig;
namespace GUI {
class ConfigOptionsGroup;

class OG_Settings
{
protected:
    std::shared_ptr<ConfigOptionsGroup> m_og;
    QWidget* m_parent = nullptr;
public:
    OG_Settings(QWidget* parent, bool staticbox);
    virtual ~OG_Settings() {}

    virtual bool    IsShown();
    virtual void    Show(bool show);
    virtual void    Hide();
    virtual void    UpdateAndShow(bool show);
    virtual QLayout* get_sizer();
    ConfigOptionsGroup* get_og() { return m_og.get(); }
    QWidget*        parent() const { return m_parent; }
};

class TabPrintModel;

class ObjectSettings
{
    QWidget*       m_parent     = nullptr;
    TabPrintModel* m_tab_active = nullptr;
public:
    ObjectSettings(QWidget* parent);
    ~ObjectSettings() {}

    bool update_settings_list();
    bool add_missed_options(ModelConfig *config_to, const DynamicPrintConfig &config_from);
    void update_config_values(ModelConfig *config);
    void UpdateAndShow(bool show);
    void msw_rescale();
    void sys_color_changed();
};

}} // namespace Slic3r::GUI

#endif // slic3r_GUI_ObjectSettings_hpp_
