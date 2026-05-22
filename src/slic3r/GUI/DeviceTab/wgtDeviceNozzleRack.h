//**********************************************************/
/* File: wgtDeviceNozzleRack.h
*  Description: The panel with rack and nozzles
*
*  \n class wgtDeviceNozzleRackArea;
*  \n class wgtDeviceNozzleRackNozzleItem;
*  \n class wgtDeviceNozzleRackToolHead;
*  \n class wgtDeviceNozzleRackPos;
//**********************************************************/

#pragma once
#include <QWidget>
#include <QString>
#include "slic3r/GUI/DeviceCore/DevNozzleRack.h"

#include "slic3r/GUI/Widgets/StaticBox.hpp"
#include "slic3r/GUI/Widgets/AnimaController.hpp"

#include <memory>

// Previous definitions
class Button;
class Label;
class ScalableBitmap;
class ScalableButton;
namespace Slic3r
{
    struct DevNozzle;
    class DevNozzleRack;
namespace GUI
{
    class wgtDeviceNozzleRackArea;
    class wgtDeviceNozzleRackNozzleItem;
    class wgtDeviceNozzleRackToolHead;
    class wgtDeviceNozzleRackPos;
    class wgtDeviceNozzleRackTitle;
    class wgtDeviceNozzleRackUpgradeDlg;
}
};

// Events

namespace Slic3r::GUI
{
class wgtDeviceNozzleRack : public QWidget
{
public:
    wgtDeviceNozzleRack(QWidget* parent,
                        int id = -1,
                        const QPoint& pos = QPoint(),
                        const QSize& size = QSize(),
                        long style = 0);
    ~wgtDeviceNozzleRack() = default;

public:
    void UpdateRackInfo(std::shared_ptr<DevNozzleRack> rack);
    void Rescale();

private:
    void CreateGui();

private:
    std::weak_ptr<DevNozzleRack> m_nozzle_rack;

    // GUI
    wgtDeviceNozzleRackToolHead* m_toolhead_panel{ nullptr };
    wgtDeviceNozzleRackArea* m_rack_area{ nullptr };
};


class wgtDeviceNozzleRackToolHead : public QWidget
{
public:
    wgtDeviceNozzleRackToolHead(QWidget* parent) : QWidget(parent) { CreateGui();}

public:
    void UpdateToolHeadInfo(const DevNozzle& extruder_nozzle);
    void Rescale();

private:
    void CreateGui();

private:
    bool m_extruder_nozzle_exist = false;
    std::string m_filament_color;

    // GUI
    ScalableBitmap* m_extruder_nozzle_normal = nullptr;
    ScalableBitmap* m_extruder_nozzle_empty = nullptr;
    QLabel* m_toolhead_icon;

    Label*  m_nozzle_diamenter_label;
    Label*  m_nozzle_flowtype_label;
};


class wgtDeviceNozzleRackArea : public QWidget
{
public:
    wgtDeviceNozzleRackArea(QWidget* parent) : QWidget(parent) { CreateGui();}

public:
    void UpdateRackInfo(std::weak_ptr<DevNozzleRack> rack);
    void Rescale();

private:
    void CreateGui();
    StaticBox* CreateNozzleBox(const std::vector<int> nozzle_idxes);
    QLayout* CreateRefreshBook(QWidget* parent);

    // updates
    void UpdateNozzleItems(const std::unordered_map<int, wgtDeviceNozzleRackNozzleItem*>& nozzle_items,
        std::shared_ptr<DevNozzleRack> nozzle_rack);

    // events
    void OnBtnHotendsInfos(QEvent& evt);
    void OnBtnReadAll(QEvent& evt);

private:
    std::weak_ptr<DevNozzleRack> m_nozzle_rack;
    DevNozzleRack::RackPos m_rack_pos = DevNozzleRack::RACK_POS_UNKNOWN;
    DevNozzleRack::RackStatus m_rack_status = DevNozzleRack::RACK_STATUS_UNKNOWN;

    // GUI
    QStackedWidget* m_simple_book{ nullptr };
    QWidget* m_panel_content{ nullptr };
    QWidget* m_panel_refresh{ nullptr };

    wgtDeviceNozzleRackTitle* m_title_nozzle_rack;
    QBoxLayout* m_hotends_sizer;
    StaticBox* m_arow_nozzles_box;
    StaticBox* m_brow_nozzles_box;
    std::unordered_map<int, wgtDeviceNozzleRackNozzleItem*> m_nozzle_items;

    wgtDeviceNozzleRackPos* m_rack_pos_panel;

    Button* m_btn_hotends_infos;
    Button* m_btn_read_all;

    /* refresh book */
    Label* m_progress_refresh{ nullptr };
    AnimaIcon* m_refresh_icon{ nullptr };

    wgtDeviceNozzleRackUpgradeDlg* m_rack_upgrade_dlg = nullptr;
};

class wgtDeviceNozzleRackPos : public QWidget
{
public:
    explicit wgtDeviceNozzleRackPos(QWidget* parent) : QWidget(parent) { CreateGui();}

public:
    void UpdateRackPos(const std::shared_ptr<DevNozzleRack>& rack);
    void Rescale();

private:
    void CreateGui();

    void UpdateRackPos(DevNozzleRack::RackPos new_pos,
                       DevNozzleRack::RackStatus new_status,
                       bool is_reading);

    // events
    void OnMoveRackUp(QEvent& evt);
    void OnMoveRackDown(QEvent& evt);
    void OnBtnHomingRack(QEvent& evt);

private:
    std::weak_ptr<DevNozzleRack> m_rack;
    DevNozzleRack::RackPos m_rack_pos = DevNozzleRack::RACK_POS_UNKNOWN;
    DevNozzleRack::RackStatus m_rack_status = DevNozzleRack::RACK_STATUS_UNKNOWN;

    // GUI
    StaticBox* m_rowup_panel;
    ScalableButton* m_btn_rowup;
    Label* m_label_rowup_status{ nullptr };
    Label* m_label_rowup{ nullptr };

    StaticBox* m_rowbottom_panel;
    ScalableButton* m_btn_rowbottom_up;
    Label* m_label_rowbottom_status{ nullptr };
    Label* m_label_rowbottom{ nullptr };

    ScalableButton* m_btn_homing{ nullptr };
};

class wgtDeviceNozzleRackNozzleItem : public StaticBox
{
public:
    enum NOZZLE_STATUS
    {
        NOZZLE_EMPTY,
        NOZZLE_NORMAL,
        NOZZLE_UNKNOWN,
        NOZZLE_ERROR
    };

public:
    wgtDeviceNozzleRackNozzleItem(QWidget* parent, int nozzle_id);

public:
    void Update(const std::shared_ptr<DevNozzleRack> rack, bool on_rack = true); // on_rack is false means extruder nozzle

    int  GetNozzleId() const { return m_nozzle_id; }
    void SetDisplayIdText(const QString& text) { m_nozzle_label_id->setText(text);};

    void EnableSelect();;
    void SetSelected(bool selected);
    bool IsSelected() const { return m_is_selected; }

    bool IsDisabled() const { return m_is_disabled; }
    void SetDisable(bool disabled);

    void Rescale();

private:
    void CreateGui();

    void SetNozzleStatus(NOZZLE_STATUS status, const QString& str1, const QString& str2, const std::string& color);

    void OnBtnNozzleStatus(QMouseEvent& evt);
    void OnItemSelected(QMouseEvent& evt);

private:
    std::weak_ptr<DevNozzleRack> m_rack;

    int           m_nozzle_id; // internal id, from 0 to 5
    std::string   m_filament_color;
    NOZZLE_STATUS m_status      = NOZZLE_STATUS::NOZZLE_EMPTY;

    // select
    bool  m_is_selected = false;
    bool  m_enable_select = false;
    ScalableBitmap* m_nozzle_selected_image{ nullptr };
    QLabel* m_nozzle_selected_bitmap{ nullptr };

    // enable or disable
    bool m_is_disabled = false;

    // Images
    ScalableBitmap* m_nozzle_normal_image{ nullptr };
    ScalableBitmap* m_nozzle_empty_image{ nullptr };
    ScalableBitmap* m_nozzle_unknown_image{ nullptr };
    ScalableBitmap* m_nozzle_error_image{ nullptr };

    // GUI
    QLabel* m_nozzle_icon{ nullptr };
    Label* m_nozzle_label_id { nullptr };
    Label* m_nozzle_label_1{ nullptr };
    QLabel* m_nozzle_status_icon = nullptr;
    Label* m_nozzle_label_2{ nullptr };
};

};// end of namespace Slic3r::GUI