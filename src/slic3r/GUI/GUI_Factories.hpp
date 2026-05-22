#ifndef slic3r_GUI_Factories_hpp_
#define slic3r_GUI_Factories_hpp_

#include <map>
#include <vector>
#include <array>


#include <QPixmap>
#include <QMenu>
#include <QAction>
#include <QMenuBar>
#include <QWidget>
#include "libslic3r/PrintConfig.hpp"
#include "wxExtensions.hpp"

namespace Slic3r {

enum class ModelVolumeType : int;

namespace GUI {

struct SimpleSettingData
{
    std::string name;
    std::string label;
    int priority;
};

struct SettingsFactory
{
//				     category ->       vector ( option )
    typedef std::map<std::string, std::vector<std::string>> Bundle;
    static std::map<std::string, std::string>               CATEGORY_ICON;

    //BBS: add setting data for table
    static std::map<std::string, std::vector<SimpleSettingData>>  OBJECT_CATEGORY_SETTINGS;
    static std::map<std::string, std::vector<SimpleSettingData>>  PART_CATEGORY_SETTINGS;

    static QPixmap                             get_category_bitmap(const std::string& category_name, bool menu_bmp = true);
    static Bundle                               get_bundle(const DynamicPrintConfig* config, bool is_object_settings, bool is_layer_settings = false);
    static std::vector<std::string>             get_options(bool is_part);
    //BBS: add api to get options for catogary
    static std::vector<SimpleSettingData> get_visible_options(const std::string& category, const bool is_part);
    static std::map<std::string, std::vector<SimpleSettingData>> get_all_visible_options(const bool is_part);
};

class MenuFactory
{
public:
    static std::vector<QPixmap>    get_volume_bitmaps();
    static std::vector<QPixmap>    get_text_volume_bitmaps();
    static std::vector<QPixmap>    get_svg_volume_bitmaps();

    MenuFactory();
    ~MenuFactory() = default;

    void    init(QWidget* parent);
    void    update();
    void    update_object_menu();
    void    update_default_menu();
    void    msw_rescale();
    void    sys_color_changed();

    static void sys_color_changed(QMenuBar* menu_bar);

    QMenu* default_menu();
    QMenu* object_menu();
    QMenu* sla_object_menu();
    QMenu* part_menu();
    QMenu *text_part_menu();
    QMenu *svg_part_menu();
    QMenu* cut_connector_menu();
    QMenu* instance_menu();
    QMenu* layer_menu();
    QMenu* multi_selection_menu();
    //BBS: add part plate related logic
    QMenu* plate_menu();
    QMenu* assemble_object_menu();
    QMenu* assemble_part_menu();
    QMenu* assemble_multi_selection_menu();

    QMenu *filament_action_menu(int active_filament_menu_id);

private:
    enum MenuType {
        mtObjectFFF = 0,
        mtObjectSLA,
        mtCount
    };

    QWidget* m_parent {nullptr};

    MenuWithSeparators m_object_menu;
    MenuWithSeparators m_part_menu;
    MenuWithSeparators m_text_part_menu;
    MenuWithSeparators m_svg_part_menu;
    MenuWithSeparators m_cut_cutter_menu;
    MenuWithSeparators m_sla_object_menu;
    MenuWithSeparators m_default_menu;
    MenuWithSeparators m_instance_menu;
    //BBS: add part plate related logic
    MenuWithSeparators m_plate_menu;
    MenuWithSeparators m_assemble_object_menu;
    MenuWithSeparators m_assemble_part_menu;

    QMenu m_filament_action_menu;

    int object_menu_count{ 0 };
    int part_menu_count{ 0 };
    int default_menu_count{ 0 };
    int plate_menu_count{ 0 };
    int multi_selection_menu_count{ 0 };
    int assemble_object_menu_ocunt{ 0 };
    int assemble_multi_selection_menu_count{ 0 };

    // Removed/Prepended Items according to the view mode
    std::array<QAction*, mtCount> items_increase;
    std::array<QAction*, mtCount> items_decrease;
    std::array<QAction*, mtCount> items_set_number_of_copies;

    void        create_default_menu();
    void        create_common_object_menu(QMenu *menu);
    void        create_object_menu();
    void        create_sla_object_menu();
    void        create_part_menu();
    void        create_text_part_menu();
    void        create_svg_part_menu();
    //BBS: add part plate related logic
    void        create_plate_menu();
    //BBS: add bbl object menu
    void        create_bbl_object_menu();
    void        create_bbl_part_menu();
    void        create_bbl_assemble_object_menu();
    void        create_bbl_assemble_part_menu();
    void        create_cut_cutter_menu();

    void        create_filament_action_menu(bool init, int active_filament_menu_id);

    QMenu*     append_submenu_add_generic(QMenu* menu, ModelVolumeType type);
    void        append_menu_item_add_svg(QMenu *menu, ModelVolumeType type, bool is_submenu_item = true);
    void        append_menu_items_add_volume(QMenu* menu);
    QAction* append_menu_item_layers_editing(QMenu* menu);
    QAction* append_menu_item_settings(QMenu* menu);
    QAction* append_menu_item_change_type(QMenu* menu);
    QAction* append_menu_item_instance_to_object(QMenu* menu);
    QAction* append_menu_item_printable(QMenu* menu);
    void        append_menu_item_rename(QMenu* menu);
    QAction* append_menu_item_fix_through_netfabb(QMenu* menu);
    //QAction* append_menu_item_simplify(QMenu* menu);
    void        append_menu_item_export_stl(QMenu* menu, bool is_mulity_menu = false);
    void        append_menu_item_reload_from_disk(QMenu* menu);
    void        append_menu_item_replace_with_stl(QMenu* menu);
    void        append_menu_item_change_extruder(QMenu* menu);
    void        append_menu_item_set_visible(QMenu* menu);
    void        append_menu_item_delete(QMenu* menu);
    void        append_menu_item_delete_all_cutter(QMenu *menu);
    void        append_menu_item_scale_selection_to_fit_print_volume(QMenu* menu);
    void        append_menu_items_convert_unit(QMenu* menu); // Add "Conver/Revert..." menu items (from/to inches/meters) after "Reload From Disk"
    void        append_menu_items_flush_options(QMenu* menu);
    void        append_menu_item_merge_to_multipart_object(QMenu *menu);
    void        append_menu_item_merge_to_single_object(QMenu* menu);
    void        append_menu_item_merge_parts_to_single_part(QMenu *menu);
    void        append_menu_item_merge_some_parts_to_single_part(QMenu *menu);
    void        append_menu_items_mirror(QMenu *menu);
    void        append_menu_item_invalidate_cut_info(QMenu *menu);
    void        append_menu_item_edit_text(QMenu *menu);
    void        append_menu_item_edit_svg(QMenu *menu);

    //void        append_menu_items_instance_manipulation(QMenu *menu);
    //void        update_menu_items_instance_manipulation(MenuType type);
    //BBS add bbl menu item
    void        append_menu_item_clone(QMenu* menu);
    void        append_menu_item_simplify(QMenu* menu);
    void        append_menu_item_smooth_mesh(QMenu *menu);
    void        append_menu_item_center(QMenu* menu);
    void        append_menu_item_sub_merge(QMenu *menu);
    void        append_menu_item_per_object_process(QMenu* menu);
    void        append_menu_item_per_object_settings(QMenu* menu);
    void        append_menu_item_change_filament(QMenu* menu);
    void        append_menu_item_set_printable(QMenu* menu);
    void        append_menu_item_locked(QMenu* menu);
    void        append_menu_item_fill_bed(QMenu *menu);
    void        append_menu_item_plate_name(QMenu *menu);
    void        append_menu_item_align_distribute(QMenu *menu);
};

}}

#endif //slic3r_GUI_Factories_hpp_
