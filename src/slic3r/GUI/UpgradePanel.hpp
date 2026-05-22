#ifndef slic3r_UpgradePanel_hpp_
#define slic3r_UpgradePanel_hpp_

#include <QWidget>
#include <QLabel>
#include <QString>
#include <slic3r/GUI/Widgets/Button.hpp>
#include "Widgets/ProgressBar.hpp"
#include <slic3r/GUI/DeviceManager.hpp>
#include <slic3r/GUI/Widgets/ScrolledWindow.hpp>
#include <slic3r/GUI/StatusPanel.hpp>
#include "ReleaseNote.hpp"

namespace Slic3r {
namespace GUI {

// Previous definitions
class uiDeviceUpdateVersion;

class ExtensionPanel : public QWidget
{
public:
    QLabel* m_staticText_ext;
    QLabel* m_staticText_ext_val;
    QLabel* m_staticText_ext_ver;
    QLabel* m_staticText_ext_ver_val;
    QLabel* m_staticText_ext_sn_val;
    ScalableBitmap upgrade_green_icon;
    QLabel* m_ext_new_version_img;

    ExtensionPanel(QWidget* parent,
        int      id = -1,
        const QPoint& pos = QPoint(),
        const QSize& size = QSize(),
        long            style = 0,
        const QString& name = QString());
    ~ExtensionPanel();
    void msw_rescale();
};

class AmsPanel : public QWidget
{
public:
    QLabel *m_staticText_ams_model_id;
    QLabel *m_staticText_ams;
    QLabel *m_staticText_ams_sn_val;
    QLabel *m_staticText_ams_ver_val;
    QLabel *m_staticText_beta_version;
    QLabel *m_ams_new_version_img;
    ScalableBitmap upgrade_green_icon;

    AmsPanel(QWidget *      parent,
                     int      id    = -1,
                     const QPoint & pos   = QPoint(),
                     const QSize &  size  = QSize(),
                     long            style = 0,
                     const QString &name  = QString());
    ~AmsPanel();

    void msw_rescale();
};

class ExtraAmsPanel : public AmsPanel
{
public:
    ExtraAmsPanel(QWidget* parent,
        int      id = -1,
        const QPoint& pos = QPoint(),
        const QSize& size = QSize(),
        long            style = 0,
        const QString& name = QString());
};



class MachineInfoPanel : public QWidget
{
protected:
    QWidget *       m_panel_caption;
    QLabel *m_upgrade_status_img;
    QLabel *  m_caption_text;
    QLabel *m_printer_img;
    QLabel *  m_staticText_model_id;
    QLabel *  m_staticText_model_id_val;
    QLabel *  m_staticText_sn;
    QLabel *  m_staticText_sn_val;
    QLabel *m_ota_new_version_img;
    QLabel *  m_staticText_ver;
    QLabel *  m_staticText_ver_val;
    QLabel *  m_staticText_beta_version;
    QFrame *  m_staticline;
    QLabel *m_ams_img;
    QFrame*   m_staticline2;
    ExtraAmsPanel*  m_extra_ams_panel;
    QLabel* m_extra_ams_img;
    QLabel* m_ext_img;
    ExtensionPanel* m_ext_panel;

    QGridLayout*   m_ams_info_sizer;

    /* ams info */
    bool           m_last_ams_show = true;
    QBoxLayout*    m_ams_sizer;

    /* extension info */
    bool           m_last_ext_show = true;
    QBoxLayout*    m_ext_sizer;

    /* extra_ams info */
    bool           m_last_extra_ams_show = true;
    QBoxLayout*    m_extra_ams_sizer;

    /* air_pump info*/
    QBoxLayout*            m_air_pump_sizer = nullptr;
    QLabel*        m_air_pump_img   = nullptr;
    QFrame*          m_air_pump_line_above = nullptr;
    uiDeviceUpdateVersion* m_air_pump_version = nullptr;

    /* rotary attachment*/
    QBoxLayout            *m_rotary_sizer      = nullptr;
    QLabel        *m_rotary_img        = nullptr;
    QFrame          *m_rotary_line_above = nullptr;
    uiDeviceUpdateVersion *m_rotary_version    = nullptr;

    /* cutting module info*/
    QBoxLayout*            m_cutting_sizer = nullptr;
    QLabel*        m_cutting_img = nullptr;
    QFrame*          m_cutting_line_above = nullptr;
    uiDeviceUpdateVersion* m_cutting_version = nullptr;

    /* laser info*/
    QBoxLayout*            m_laser_sizer = nullptr;
    QLabel*        m_lazer_img = nullptr;
    QFrame*          m_laser_line_above = nullptr;
    uiDeviceUpdateVersion* m_laser_version = nullptr;

    /* fire extinguish*/
    QBoxLayout* m_extinguish_sizer = nullptr;
    QLabel* m_extinguish_img = nullptr;
    QFrame* m_extinguish_line_above = nullptr;;
    uiDeviceUpdateVersion* m_extinguish_version = nullptr;

    /*amshub*/
    QBoxLayout*            m_amshub_sizer = nullptr;
    QLabel*        m_amshub_img = nullptr;
    QFrame*          m_amshub_line_above = nullptr;
    uiDeviceUpdateVersion* m_amshub_version = nullptr;

    /* filament track switch */
    QBoxLayout*            m_filatrack_sizer = nullptr;
    QLabel*        m_filatrack_img = nullptr;
    QFrame*          m_filatrack_line_above = nullptr;
    uiDeviceUpdateVersion* m_filatrack_version = nullptr;

    /* nozzle_rack*/
    QFrame * m_nozzle_rack_line_above{nullptr};
    QLabel *m_nozzle_rack_img = nullptr;
    QBoxLayout   *m_nozzle_rack_sizer{nullptr};
    QLabel *m_nozzle_rack_text{nullptr};

     /* exhaust fan */
    QBoxLayout            *m_exhaustfan_sizer   = nullptr;
    QLabel        *m_exhaustfan_img     = nullptr;
    QFrame          *m_exhaustfan_line_above = nullptr;
    uiDeviceUpdateVersion *m_exhaustfan_version    = nullptr;

    /* upgrade widgets */
    QBoxLayout*     m_upgrading_sizer;
    QLabel *  m_staticText_upgrading_info;
    ProgressBar *   m_upgrade_progress;
    QLabel *  m_staticText_upgrading_percent;
    QLabel *m_upgrade_retry_img;
    QLabel *  m_staticText_release_note;
    Button *        m_button_upgrade_firmware;
    Button *        m_nozzle_rack_update_btn;

    QWidget* create_caption_panel(QWidget *parent);
    std::unordered_map<std::string, AmsPanel*>             m_amspanel_list;
    std::vector<ExtraAmsPanel*>    m_extra_ams_panel_list;

    ScalableBitmap m_img_ext;
    ScalableBitmap m_img_monitor_ams;
    ScalableBitmap m_img_extra_ams;
    ScalableBitmap m_img_printer;
    ScalableBitmap m_img_air_pump;
    ScalableBitmap m_img_cutting;
    ScalableBitmap m_img_laser;
    ScalableBitmap m_img_extinguish;
    ScalableBitmap m_img_rotary;
    ScalableBitmap m_img_filatrack;
    ScalableBitmap upgrade_gray_icon;
    ScalableBitmap upgrade_green_icon;
    ScalableBitmap upgrade_yellow_icon;
    ScalableBitmap m_img_nozzle_rack;
    ScalableBitmap m_img_amshub;
    ScalableBitmap m_img_exhaustfan;

    int last_status = -1;
    std::string last_status_str = "";

    std::string m_last_laser_product_name = "";

    SecondaryCheckDialog* confirm_dlg = nullptr;

    void upgrade_firmware_internal();
    void on_show_release_note(QMouseEvent &event);
    void confirm_upgrade(MachineObject* obj = nullptr);

public:
    MachineInfoPanel(QWidget* parent, int id = -1, const QPoint& pos = QPoint(), const QSize& size = QSize(), long style = 0, const QString& name = QString());
    ~MachineInfoPanel();

    void on_sys_color_changed();
    void update_printer_imgs(MachineObject* obj);
    void update_amshub_imgs(MachineObject *obj);
    void init_bitmaps();
    void rescale_bitmaps();

    Button* get_btn() {
        return m_button_upgrade_firmware;
    }

    void msw_rescale();
    void update(MachineObject *obj);
    void update_version_text(MachineObject *obj);
    void update_ams_ext(MachineObject *obj);
    void show_status(int status, std::string upgrade_status_str = "");
    void show_ams(bool show = false, bool force_update = false);
    void show_ext(bool show = false, bool force_update = false);
    void show_extra_ams(bool show = false, bool force_update = false);

    void on_upgrade_firmware(QEvent &event);
    void on_consisitency_upgrade_firmware(QEvent &event);

    MachineObject *m_obj{nullptr};
    FirmwareInfo  m_ota_info;
    FirmwareInfo  m_ams_info;

    bool is_upgrading = false;

    enum PanelType {
        ptUndef,
        ptPushPanel,
        ptOtaPanel,
        ptAmsPanel,
    }panel_type;

private:
    void createAirPumpWidgets(QBoxLayout* main_left_sizer);
    void createCuttingWidgets(QBoxLayout* main_left_sizer);
    void createLaserWidgets(QBoxLayout* main_left_sizer);
    void createExtinguishWidgets(QBoxLayout* main_left_sizer);
    void createFilaTrackSwitchWidgets(QBoxLayout* main_left_sizer);
    void createNozzleRackWidgets(QBoxLayout* main_left_sizer);
    void createRotaryWidgets(QBoxLayout *main_left_sizer);
    void createExhaustFan(QBoxLayout *main_left_sizer);
    void createAmshubWidgets(QBoxLayout *main_left_sizer);

    void update_air_pump(MachineObject* obj);
    void update_cut(MachineObject* obj);
    void update_laszer(MachineObject* obj);
    void update_extinguish(MachineObject* obj);
    void update_rotary(MachineObject* obj);
    void update_filatrack(MachineObject* obj);
    void update_amshub(MachineObject *obj);
    void update_nozzle_rack(MachineObject *obj);
    void update_exhaustfan(MachineObject *obj);
    void on_nozzle_rack_update(QEvent &event);

    void show_air_pump(bool show = true);
    void show_cut(bool show = true);
    void show_laszer(bool show = true);
    void show_rotary(bool show = true);
    void show_extinguish(bool show = true);
    void show_filatrack(bool show = true);
    void show_amshub(bool show = true);
    void show_nozzle_rack(bool show = true);
    void show_exhaustfan(bool show = true);
};

//enum UpgradeMode {
//    umPushUpgrading,
//    umSelectOtaVerUpgrading,
//    umSelectAmsVerUpgrading,
//};
//static UpgradeMode upgrade_mode;

class UpgradePanel : public QWidget
{
protected:
    QScrollArea* m_scrolledWindow;
    QBoxLayout* m_machine_list_sizer;
    MachineInfoPanel *m_push_upgrade_panel{nullptr};

    //enable_select_firmware only in debug mode
    bool enable_select_firmware = false;
    bool m_need_update = false;
    //hint of force upgrade or consistency upgrade
    DevFirmwareUpgradeState last_forced_hint_status = DevFirmwareUpgradeState::DC;
    DevFirmwareUpgradeState last_consistency_hint_status = DevFirmwareUpgradeState::DC;
    int last_status;
    bool m_show_forced_hint = true;
    bool m_show_consistency_hint = true;
    SecondaryCheckDialog* force_dlg{ nullptr };
    SecondaryCheckDialog* consistency_dlg{ nullptr };

public:
    UpgradePanel(QWidget *parent, int id = -1, const QPoint &pos = QPoint(), const QSize &size = QSize(), long style = 0);
    ~UpgradePanel();
    void clean_push_upgrade_panel();
    void msw_rescale();
    void setVisible(bool show) override;

    void refresh_version_and_firmware(MachineObject* obj);
    void update(MachineObject *obj);
    void show_status(int status);
    void on_sys_color_changed();

    MachineObject *m_obj { nullptr };
};

}
}

#endif
