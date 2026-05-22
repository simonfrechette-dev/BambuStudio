#ifndef slic3r_GUI_ObjectTableSettings_hpp_
#define slic3r_GUI_ObjectTableSettings_hpp_

#include <memory>
#include <vector>
#include <map>
#include <QWidget>
#include <QLayout>
#include "wxExtensions.hpp"

namespace Slic3r {
class DynamicPrintConfig;
class ModelConfig;
namespace GUI {
class ConfigOptionsGroup;
class ObjectGridTable;
struct SimpleSettingData;

class OTG_Settings
{
protected:
    std::shared_ptr<ConfigOptionsGroup> m_og;
    QWidget* m_parent{nullptr};
public:
    OTG_Settings(QWidget* parent, const bool staticbox);
    virtual ~OTG_Settings() {}

    virtual bool    IsShown();
    virtual void    Show(const bool show);
    virtual void    Hide();
    virtual void    UpdateAndShow(const bool show);

    virtual QLayout* get_sizer();
    ConfigOptionsGroup* get_og() { return m_og.get(); }
    QWidget*            parent() const { return m_parent; }
};

class ObjectTableSettings : public OTG_Settings
{
    QLayout* m_settings_list_sizer{nullptr};
    std::vector<std::shared_ptr<ConfigOptionsGroup>> m_og_settings;

    DynamicPrintConfig m_current_config;
    DynamicPrintConfig m_origin_config;
    ScalableBitmap m_bmp_reset;
    ScalableBitmap m_bmp_reset_focus;
    ScalableBitmap m_bmp_reset_disable;

    ObjectGridTable* m_table{nullptr};
    int m_current_row{0};
    std::string m_current_category;
    int m_current_different{0};
    std::map<std::string, int> m_different_map;

public:
    ObjectTableSettings(QWidget* parent, ObjectGridTable* table);
    ~ObjectTableSettings() { m_different_map.clear(); }

    bool update_settings_list(bool is_object, bool is_multiple_selection,
        ModelObject* object, ModelConfig* config, const std::string& category);
    bool add_missed_options(ModelConfig *config_to, const DynamicPrintConfig &config_from);
    int  update_extra_column_visible_status(ConfigOptionsGroup* option_group,
        const std::vector<SimpleSettingData>& option_keys, ModelConfig* config);
    void update_config_values(bool is_object, ModelObject* object, ModelConfig* config, const std::string& category);
    void UpdateAndShow(int row, const bool show, bool is_object, bool is_multiple_selection,
        ModelObject* object, ModelConfig* config, const std::string& category);
    void ValueChanged(int row, bool is_object, ModelObject* object, ModelConfig* config,
        const std::string& category, const std::string& key);
    void resetAllValues(int row, bool is_object, ModelObject* object, ModelConfig* config,
        const std::string& category);
    void msw_rescale();
};

// Qt event types for lock enable/disable
inline int getEVT_LOCK_DISABLE() {
    static int t = QEvent::registerEventType(); return t;
}
inline int getEVT_LOCK_ENABLE() {
    static int t = QEvent::registerEventType(); return t;
}

}} // namespace Slic3r::GUI

#endif // slic3r_GUI_ObjectTableSettings_hpp_
