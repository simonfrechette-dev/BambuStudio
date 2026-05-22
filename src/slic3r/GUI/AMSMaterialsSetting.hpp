#ifndef slic3r_AMSMaterialsSetting_hpp_
#define slic3r_AMSMaterialsSetting_hpp_

#include "libslic3r/Preset.hpp"
#include "wxExtensions.hpp"
#include <QColor>
#include <QLabel>
#include <QComboBox>
#include <QColorDialog>
#include <QGridLayout>
#include "GUI_Utils.hpp"
#include "DeviceManager.hpp"
#include "Widgets/RadioBox.hpp"
#include "Widgets/Button.hpp"
#include "Widgets/RoundedRectangle.hpp"
#include "Widgets/Label.hpp"
#include "Widgets/CheckBox.hpp"
#include "Widgets/ComboBox.hpp"
#include "Widgets/TextInput.hpp"
#include "../slic3r/Utils/CalibUtils.hpp"
#include "DeviceCore/DevNozzleRack.h"

#define AMS_MATERIALS_SETTING_DEF_COLOUR QColor(255, 255, 255)
#define AMS_MATERIALS_SETTING_GREY900 QColor(38, 46, 48)
#define AMS_MATERIALS_SETTING_GREY800 QColor(50, 58, 61)
#define AMS_MATERIALS_SETTING_GREY700 QColor(107, 107, 107)
#define AMS_MATERIALS_SETTING_GREY300 QColor(174,174,174)
#define AMS_MATERIALS_SETTING_GREY200 QColor(248, 248, 248)
#define AMS_MATERIALS_SETTING_BODY_WIDTH FromDIP(380)
#define AMS_MATERIALS_SETTING_LABEL_WIDTH FromDIP(80)
#define AMS_MATERIALS_SETTING_COMBOX_WIDTH QSize(250, 30)
#define AMS_MATERIALS_SETTING_BUTTON_SIZE QSize(90, 24)
#define AMS_MATERIALS_SETTING_INPUT_SIZE QSize(90, 24)

namespace Slic3r { namespace GUI {

class ColorPicker : public QWidget
{
public:
    QPixmap         m_bitmap_border;
    QPixmap         m_bitmap_border_dark;
    QPixmap         m_bitmap_transparent;
    ScalableBitmap  m_bitmap_transparent_def; //default transparent material

    QColor          m_colour;
    std::vector<QColor>          m_cols;
    bool            m_selected{false};
    bool            m_show_full{false};
    bool            m_is_empty{false};
    int             ctype;

    bool            transparent_changed{false};

    ColorPicker(QWidget* parent = nullptr);
    ~ColorPicker();

    void msw_rescale();
    void set_color(QColor col);
    void set_colors(std::vector<QColor> cols);
    void set_selected(bool sel) {m_selected = sel; update();};
    void set_show_full(bool full) {m_show_full = full; update();};
    void is_empty(bool empty) {m_is_empty = empty;};

    void paintEvent(QPaintEvent* evt) override;
    void render(QPainter& dc);
    void doRender(QPainter& dc);
};

class ColorPickerPopup : public PopupWindow
{
public:
    ScalableBitmap m_ts_bitmap_custom;
    QLabel* m_ts_stbitmap_custom;
    StaticBox* m_custom_cp;
    // QColorDialog used on demand instead of wxColourData
    StaticBox* m_def_color_box;
    QGridLayout* m_ams_fg_sizer;
    QColor m_def_col;
    std::vector<QColor> m_def_colors;
    std::vector<QColor> m_ams_colors;
    std::vector<ColorPicker*> m_color_pickers;
    std::vector<ColorPicker*> m_ams_color_pickers;

public:
    ColorPickerPopup(QWidget* parent);
    ~ColorPickerPopup() {};
    void on_custom_clr_picker();
    void set_ams_colours(std::vector<QColor> ams);
    void set_def_colour(QColor col);
    void paintEvent(QPaintEvent* evt) override;
    void Popup(QWidget* focus = nullptr);



public:
};


class AMSMaterialsSetting : public DPIDialog
{
public:
    AMSMaterialsSetting(QWidget *parent = nullptr);
    ~AMSMaterialsSetting();
    void create();

    void paintEvent(QPaintEvent *evt) override;
    void input_min_finish();
    void input_max_finish();
    void update();
    void Show(bool show = true); // not override — QDialog uses show()/hide()
    void Popup(QString filament = QString(), QString sn = QString(),
               QString temp_min = QString(), QString temp_max = QString(),
               QString k = QString(), QString n = QString());

    void post_select_event(int index);
    void TryRefreshPAProfiles();
    void set_color(QColor color);
    void set_empty_color(QColor color);
    void set_colors(std::vector<QColor> colors);
    void set_ctype(int ctype);

    void on_picker_color();
    MachineObject* obj{ nullptr };
    int            ams_id { 0 };        /* 0 ~ 3 */
    int            slot_id { 0 };        /* 0 ~ 3 */

    std::string    ams_filament_id;
    std::string    ams_setting_id;

    bool           m_is_third;
    QString        m_brand_filament;
    QString        m_brand_sn;
    QString        m_brand_tmp;
    QColor         m_brand_colour;
    std::string    m_filament_type;
    ColorPickerPopup m_color_picker_popup;
    ColorPicker *       m_clr_picker;
    Label*                 m_clr_name;
    std::vector<PACalibResult>  m_pa_profile_items;

    struct FilamentInfos {
        std::string filament_id;
        std::string setting_id;
    };

protected:
    void create_panel_normal(QWidget* parent);
    void create_panel_kn(QWidget* parent);
    void on_dpi_changed(const QRect &suggested_rect) override;
    void on_select_nozzle_id(int idx);
    void on_select_filament(int idx);
    void on_select_cali_result(int idx);
    void on_select_nozzle_pos_id(int idx);
    void on_select_ok();
    void on_select_reset();
    void on_select_close();
    void on_clr_picker();
    bool is_virtual_tray();
    void update_widgets();

    void update_pa_profile_items();
    void update_filament_editing(bool is_printing);
    void update_nozzle_combo(MachineObject* obj);
    int  get_nozzle_combo_id_code() const;
    int  get_nozzle_sel_by_sn(MachineObject* obj, const std::string& sn);
    int  get_cali_index_by_ams_slot(MachineObject* obj, int ams_id, int slot_id);

    void get_filaments_info(const MachineObject*                     obj,
                            const std::string&                       nozzle_diameter_str,
                            QStringList&                           filament_items,
                            std::map<std::string, FilamentInfos>&    map_filament_items,
                            std::unordered_map<QString, QString>&  query_filament_vendors,
                            std::unordered_map<QString, QString>&  query_filament_types);

    Preset* get_filament_by_id(const std::string& filament_id, bool is_system);

protected:
    StateColor          m_btn_bg_green;
    StateColor          m_btn_bg_gray;
    QWidget *           m_panel_normal{nullptr};
    QWidget *           m_panel_SN{nullptr};
    QLabel *            m_sn_number{nullptr};
    QLabel *            warning_text{nullptr};
    //QWidget *           m_panel_body;
    QLabel *            m_title_filament{nullptr};
    QLabel *            m_title_nozzle_type{nullptr};
    QLabel *            m_title_pa_profile{nullptr};
    QLabel *            m_title_colour{nullptr};
    QLabel *            m_title_temperature{nullptr};
    TextInput *         m_input_nozzle_min;
    TextInput*          m_input_nozzle_max;
    ScalableBitmap *    degree;
    QLabel *            bitmap_max_degree{nullptr};
    QLabel *            bitmap_min_degree{nullptr};
    Button *            m_button_reset;
    Button *            m_button_confirm;
    Label*              m_tip_readonly;
    Button *            m_button_close;
    // QColorDialog used on demand

    QWidget *           m_panel_kn{nullptr};
    QLabel*             m_ratio_text{nullptr};
    QLabel *            m_wiki_ctrl{nullptr};
    QLabel*             m_k_param{nullptr};
    TextInput*          m_input_k_val;
    QLabel*             m_n_param{nullptr};
    TextInput*          m_input_n_val;
    int                 m_filament_selection;

    int m_pa_cali_select_id = 0;
    bool m_pa_data_pending{false};

    QComboBox *m_comboBox_filament{nullptr};
    ComboBox * m_comboBox_nozzle_type;
    ComboBox * m_comboBox_cali_result;
    TextInput*       m_readonly_filament;

    std::map<std::string, FilamentInfos> map_filament_items;
};

QEvent::Type getSelectedColorEventType();

}} // namespace Slic3r::GUI

#endif
