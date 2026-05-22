#ifndef slic3r_Monitor_hpp_
#define slic3r_Monitor_hpp_

// Qt port of Monitor.hpp

#include <map>
#include <vector>
#include <memory>
#include <QWidget>
#include <QLabel>
#include <QPixmap>
#include <QTimer>
#include <QBoxLayout>
#include <QProgressBar>
#include "Event.hpp"
#include "libslic3r/ProjectTask.hpp"
#include "slic3r/GUI/MsgDialog.hpp"
#include "slic3r/GUI/DeviceManager.hpp"
#include "slic3r/GUI/DeviceCore/DevHMS.h"
#include "slic3r/GUI/MonitorBasePanel.h"
#include "slic3r/GUI/StatusPanel.hpp"
class SelectMachinePopup;
class Tabbook;
class AddMachinePanel;
class MediaFilePanel;
namespace Slic3r { namespace GUI { class SideTools; } }
namespace Slic3r { namespace GUI { class UpgradePanel; } }
namespace Slic3r { namespace GUI { class HMSPanel; } }

namespace Slic3r {
namespace GUI {

class MediaFilePanel;

class AddMachinePanel : public QWidget
{
    Q_OBJECT
protected:
    QPushButton*    m_button_add_machine{ nullptr };
    QLabel*         m_staticText_add_machine{ nullptr };
    QLabel*         m_bitmap_empty{ nullptr };

    void on_add_machine();

public:
    explicit AddMachinePanel(QWidget* parent = nullptr);
    ~AddMachinePanel();

    void msw_rescale();
};

class MonitorPanel : public QWidget
{
    Q_OBJECT
private:
    static bool is_hms_list_equal(const std::map<std::string, DevHMSItem> &map1, const std::map<std::string, DevHMSItem> &map2);

    Tabbook*            m_tabpanel{ nullptr };
    QBoxLayout*         m_main_sizer{ nullptr };

    AddMachinePanel*    m_status_add_machine_panel{ nullptr };
    StatusPanel*        m_status_info_panel{ nullptr };
    MediaFilePanel*     m_media_file_panel{ nullptr };
    Slic3r::GUI::UpgradePanel*       m_upgrade_panel{ nullptr };
    Slic3r::GUI::HMSPanel*           m_hms_panel{ nullptr };

    /* status tab live-update widgets */
    QLabel*             m_lbl_printer_name{ nullptr };
    QLabel*             m_lbl_nozzle_temp{ nullptr };
    QLabel*             m_lbl_bed_temp{ nullptr };
    QLabel*             m_lbl_chamber_temp{ nullptr };
    QProgressBar*       m_prog_bar{ nullptr };
    QLabel*             m_lbl_job_name{ nullptr };
    QLabel*             m_lbl_time_remain{ nullptr };
    QLabel*             m_lbl_layer{ nullptr };

    /* side tools */
    Slic3r::GUI::SideTools* m_side_tools{ nullptr };
    QLabel*             m_bitmap_printer_type{ nullptr };
    QLabel*             m_bitmap_arrow{ nullptr };
    QLabel*             m_staticText_printer_name{ nullptr };
    QLabel*             m_bitmap_wifi_signal{ nullptr };
    QBoxLayout*         m_side_tools_sizer{ nullptr };
    SelectMachinePopup* m_select_machine{nullptr};

    /* images */
    QPixmap m_signal_strong_img;
    QPixmap m_signal_middle_img;
    QPixmap m_signal_weak_img;
    QPixmap m_signal_no_img;
    QPixmap m_printer_img;
    QPixmap m_arrow_img;

    int  last_wifi_signal = -1;
    int  last_status = 0;
    bool m_initialized{ false };
    bool update_flag{ false };
    QTimer* m_refresh_timer = nullptr;

    std::map<std::string, DevHMSItem> m_last_hms_list;

public:
    explicit MonitorPanel(QWidget* parent = nullptr);
    ~MonitorPanel();

    enum PrinterTab {
        PT_STATUS  = 0,
        PT_MEDIA   = 1,
        PT_UPDATE  = 2,
        PT_HMS     = 3,
        PT_DEBUG   = 4,
        PT_MAX_NUM = 5
    };

    void init_bitmap();
    void init_timer();
    void init_tabpanel();
    Tabbook* get_tabpanel() { return m_tabpanel; }
    void set_default();
    QWidget* create_side_tools();

    void on_sys_color_changed();
    void msw_rescale();

    StatusPanel* get_status_panel() { return m_status_info_panel; }
    void select_machine(std::string machine_sn);

    /* update apis */
    void update_all();
    void update_hms_tag();
    bool Show(bool show);
    void show_status(int status);

    std::string get_string_from_tab(PrinterTab tab);

    MachineObject* obj{ nullptr };
    std::string last_conn_type = "undedefined";

    void stop_update()  { update_flag = false; }
    void start_update() { update_flag = true; }

    void jump_to_HMS();
    void jump_to_Upgrade();
    void jump_to_LiveView();
    void jump_to_Rack();

public Q_SLOTS:
    void on_timer();
    void on_select_printer();
    void on_printer_clicked();

protected:
    void resizeEvent(QResizeEvent* event) override;
};

} // GUI
} // Slic3r

#endif /* slic3r_Monitor_hpp_ */
