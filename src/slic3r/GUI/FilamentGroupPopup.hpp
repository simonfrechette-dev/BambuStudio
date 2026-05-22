#ifndef FILAMENT_GROUP_HOVER_HPP
#define FILAMENT_GROUP_HOVER_HPP
#include <QWidget>
#include <QString>

#include "libslic3r/PrintConfig.hpp"
#include "Widgets/PopupWindow.hpp"
#include "Widgets/Label.hpp"
#include "Widgets/SwitchButton.hpp"
#include "Widgets/StaticBox.hpp"

namespace Slic3r { namespace GUI {

class PartPlate;
class Plater;


bool play_dual_extruder_slice_video();
bool play_dual_extruder_print_tpu_video();
bool open_filament_group_wiki();

class FilamentGroupPopup : public PopupWindow
{
public:
    FilamentGroupPopup(QWidget *parent, const std::vector<FilamentMapMode>& available_modes = {});
    void tryPopup(Plater* plater,PartPlate* plate, bool slice_all);
    void tryClose();

    FilamentMapMode GetSelectedMode() const { return m_mode; }
private:
    void OnPaint(QPaintEvent&event);
    void StartTimer();
    void ResetTimer();

    void OnRadioBtn(int idx);
    void OnLeaveWindow(QMouseEvent &);
    void OnEnterWindow(QMouseEvent &);
    void OnTimer(QTimerEvent &event);
    void Dismiss();

    void CreateBmps();
    void RecreateUIElements();
    void UpdateNozzleLabels();
    void Init(const std::vector<FilamentMapMode>& available_modes);
    void UpdateButtonStatus(int hover_idx = -1);
    void DrawRoundedCorner(int radius);
private:
    FilamentMapMode GetFilamentMapMode() const;
    void SetFilamentMapMode(const FilamentMapMode mode);

    // smart filament
    void MakeSmartFilamentSection(QLayout *top_sizer, int horizontal_margin, int vertical_padding);
    void UpdateSmartFilamentSection();
    void OnSmartFilamentToggle(QEvent &event);

private:
    std::vector<FilamentMapMode> m_all_modes;
    std::vector<FilamentMapMode> m_available_modes;

    bool m_connected{ false };
    bool m_active{ false };
    bool m_support_quality_mode{ false };

    bool m_sync_plate{ false };
    bool m_slice_all{ false };
    bool m_fila_switch_ready{ false };
    FilamentMapMode m_mode;
    QTimer        *m_timer;

    std::vector<QPushButton*> radio_btns;
    std::vector<Label *>   button_labels;
    std::vector<Label *>   button_desps;
    std::vector<Label *>   detail_infos;
    std::vector<QLayout *> button_sizers;
    std::vector<QLayout *> label_sizers;
    std::vector<QLayoutItem *> mode_spacer; // vertical space between each mode

    QPixmap checked_bmp;
    QPixmap unchecked_bmp;
    QPixmap disabled_bmp;
    QPixmap checked_hover_bmp;
    QPixmap unchecked_hover_bmp;
    QPixmap global_tag_bmp;

    QLabel *wiki_link;
    QLabel *video_link;

    // Smart filament assign section
    StaticBox    *m_smart_filament_panel{nullptr};
    QLayoutItem  *m_smart_filament_spacer{nullptr};
    SwitchButton *m_smart_filament_switch{nullptr};

    PartPlate* partplate_ref{ nullptr };
    Plater* plater_ref{ nullptr };
};
}} // namespace Slic3r::GUI
#endif
