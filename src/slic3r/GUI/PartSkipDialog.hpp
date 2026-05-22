
#include "Widgets/Label.hpp"
#include "Widgets/CheckBox.hpp"
#include "Widgets/Button.hpp"
#include "Widgets/AnimaController.hpp"
#include "DeviceManager.hpp"
#include "PartSkipCommon.hpp"
#include "Printer/PrinterFileSystem.h"
#include "I18N.hpp"
#include "GUI_Utils.hpp"


namespace Slic3r { namespace GUI {

class SkipPartCanvas;

enum URL_STATE {
    URL_TCP,
    URL_TUTK,
};

class PartSkipConfirmDialog : public DPIDialog
{
private:
protected:
    Label  *m_msg_label;
    Label  *m_tip_label;
    Button *m_apply_button;

public:
    PartSkipConfirmDialog(QWidget *parent);
    ~PartSkipConfirmDialog();

    void    on_dpi_changed(const QRect &suggested_rect);
    Button *GetConfirmButton();
    void    SetMsgLabel(QString msg);
    void    SetTipLabel(QString msg);
    bool    Show(bool show);
};

class PartSkipDialog : public DPIDialog
{
public:
    PartSkipDialog(QWidget *parent);
    ~PartSkipDialog();
    void on_dpi_changed(const QRect &suggested_rect);
    bool Show(bool show);

    void UpdatePartsStateFromPrinter(MachineObject *obj_);
    void SetSimplebookPage(int page);
    void InitSchedule(MachineObject *obj_);
    void InitDialogUI();
    int  GetAllSkippedPartsNum();

    MachineObject *m_obj{nullptr};

    QStackedWidget *m_simplebook;
    QWidget      *m_book_third_panel;
    QWidget      *m_book_second_panel;
    QWidget      *m_book_first_panel;

    SkipPartCanvas   *m_canvas;
    Button           *m_zoom_in_btn;
    Button           *m_zoom_out_btn;
    Button           *m_switch_drag_btn;
    CheckBox         *m_all_checkbox;
    Button           *m_percent_label;
    Label            *m_all_label;
    QWidget          *m_line;
    QWidget          *m_line_top;
    QScrollArea *m_list_view;

    QWidget *m_dlg_placeholder;
    Label   *m_cnt_label;
    Label   *m_tot_label;

    Button *m_apply_btn;

    Label          *m_loading_label;
    Label          *m_retry_label;
    ScalableBitmap *m_retry_icon;
    QLabel *m_retry_bitmap;

    QBoxLayout *m_sizer;
    QBoxLayout *m_dlg_sizer;
    QBoxLayout *m_dlg_content_sizer;
    QBoxLayout *m_dlg_btn_sizer;
    QBoxLayout *m_canvas_sizer;
    QBoxLayout *m_canvas_btn_sizer;
    QBoxLayout *m_list_sizer;
    QBoxLayout *m_scroll_sizer;
    QBoxLayout *m_book_first_sizer;
    QBoxLayout *m_book_second_sizer;
    QBoxLayout *m_book_second_btn_sizer;
    Button     *m_second_retry_btn;
    AnimaIcon  *m_loading_icon;

private:
    int  m_plate_idx{-1};
    int  m_zoom_percent{100};
    bool m_is_drag{false};
    bool m_print_lock{true};
    bool m_enable_apply_btn{false};
    bool is_model_support_partskip{false};

    std::map<uint32_t, PartState>   m_parts_state;
    std::map<uint32_t, std::string> m_parts_name;
    std::vector<int>                m_partskip_ids;

    enum URL_STATE m_url_state = URL_STATE::URL_TCP;

    PartsInfo GetPartsInfo();
    bool      is_drag_mode();

    boost::shared_ptr<PrinterFileSystem> m_file_sys;
    bool                                 m_file_sys_result{false};
    std::string                          m_timestamp;
    std::string                          m_tmp_path;
    std::vector<string>                  m_local_paths;
    std::vector<string>                  m_target_paths;
    std::string                          create_tmp_path();

    bool is_local_file_existed(const std::vector<string> &local_paths);

    void DownloadPartsFile();
    void OnFileSystemEvent(QEvent &event);
    void OnFileSystemResult(QEvent &event);
    void fetchUrl(boost::weak_ptr<PrinterFileSystem> wfs);

    void OnZoomIn(QEvent &event);
    void OnZoomOut(QEvent &event);
    void OnSwitchDrag(QEvent &event);
    void OnZoomPercent(QEvent &event);
    void UpdatePartsStateFromCanvas(QEvent &event);

    void UpdateZoomPercent();
    void UpdateCountLabel();
    void UpdateDialogUI();
    void UpdateApplyButtonStatus();
    bool IsAllChecked();
    bool IsAllCancled();

    void OnRetryButton(QEvent &event);
    void OnAllCheckbox(QEvent &event);
    void OnApplyDialog(QEvent &event);
};

}} // namespace Slic3r::GUI