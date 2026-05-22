// Qt6 port of StatusPanel.cpp
// Phase 1: StatusBasePanel skeleton + PrintingTaskPanel
// Phase 4: AMS + Filament groups

#define TEMP_THRESHOLD_ALLOW_E_CTRL 170.0f

#include "StatusPanel.hpp"
#include "I18N.hpp"
#include "Widgets/Label.hpp"
#include "Widgets/Button.hpp"
#include "Widgets/StateColor.hpp"
#include "GUI_App.hpp"
#include "DeviceCore/DevManager.h"
#include "DeviceCore/DevExtruderSystem.h"
#include "DeviceCore/DevConfigUtil.h"
#include "DeviceCore/DevNozzleSystem.h"
#include "DeviceCore/DevBed.h"
#include "DeviceCore/DevChamber.h"
#include "DeviceCore/DevFan.h"
#include "DeviceCore/DevLamp.h"
#include "DeviceCore/DevInfo.h"
#include "DeviceCore/DevAxis.h"
#include "DeviceTab/wgtDeviceNozzleRack.h"
#include "Widgets/FanControl.hpp"
#include "MediaPlayCtrl.h"
#include "PartSkipDialog.hpp"
#include "CalibrationWizardPage.hpp"
#include "DeviceCore/DevStatus.h"
#include "DeviceCore/DevPrintTaskInfo.h"
#include "DeviceCore/DevConfig.h"
#include "DeviceCore/DevPrintOptions.h"
#include "DeviceCore/DevStorage.h"
#include "Calibration.hpp"
#include "ExtrusionCalibration.hpp"
#include "PrintOptionsDialog.hpp"
#include "SafetyOptionsDialog.hpp"
#include "RecenterDialog.hpp"
#include "AMSSetting.hpp"
#include "AMSMaterialsSetting.hpp"
#include "AmsMappingPopup.hpp"
#include "ReleaseNote.hpp"
#include "MsgDialog.hpp"
#include "slic3r/Utils/BBLUtil.hpp"
#include "libslic3r/Utils.hpp"

#include <QNetworkAccessManager>
#include <QTimer>
#include <QNetworkRequest>
#include <QUrl>
#include <QDesktopServices>

#include <QPainter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QWidget>
#include <QStackedWidget>
#include <QMouseEvent>

// Helper: forward mouse-press events to a callback (used by filament expand panel)
namespace {
class PanelClickFilter : public QObject {
    std::function<void(QMouseEvent&)> m_fn;
public:
    explicit PanelClickFilter(QObject *parent, std::function<void(QMouseEvent&)> fn)
        : QObject(parent), m_fn(std::move(fn)) {}
    bool eventFilter(QObject *, QEvent *ev) override {
        if (ev->type() == QEvent::MouseButtonPress) {
            m_fn(*static_cast<QMouseEvent*>(ev));
            return true;
        }
        return false;
    }
};

// Average luminance of non-transparent pixels; used to decide thumbnail bg rendering
static int get_brightness_value(const QImage &image) {
    QImage gray = image.convertToFormat(QImage::Format_Grayscale8);
    int w = gray.width(), h = gray.height();
    long total = 0;
    int  count = 0;
    for (int y = 0; y < h; y += 2) {
        for (int x = 0; x < w; x += 2) {
            unsigned char alpha = qAlpha(image.pixel(x, y));
            if (alpha != 0) {
                total += QColor(gray.pixel(x, y)).red();
                ++count;
            }
        }
    }
    return (total <= 0 || count <= 0) ? 0 : (int)(total / count);
}
} // anonymous namespace

namespace Slic3r {
namespace GUI {

// =========================================================
// File-scope constants (exact wx equivalents)
// =========================================================

static const int PAGE_TITLE_HEIGHT      = 36;
static const int PAGE_TITLE_LEFT_MARGIN = 17;
static const int PAGE_SPACING           = 10;
static const int PAGE_MIN_WIDTH         = 574;
static const int PROGRESSBAR_HEIGHT     = 8;

static const QSize TASK_THUMBNAIL_SIZE       = QSize(120, 120);
static const QSize TASK_BUTTON_SIZE          = QSize(48, 24);
static const QSize TASK_BUTTON_SIZE2         = QSize(-1, 24);
static const QSize SWITCHING_STATUS_BTN_SIZE = QSize(25, 26);

static const QColor STATUS_PANEL_BG     = QColor(238, 238, 238);
static const QColor STATUS_TITLE_BG     = QColor(248, 248, 248);
static const QColor STATIC_BOX_LINE_COL = QColor(238, 238, 238);
static const QColor BUTTON_NORMAL1_COL  = QColor(238, 238, 238);
static const QColor BUTTON_NORMAL2_COL  = QColor(206, 206, 206);
static const QColor BUTTON_PRESS_COL    = QColor(172, 172, 172);
static const QColor BUTTON_HOVER_COL    = QColor(0, 174, 66);
static const QColor DISCONNECT_TEXT_COL = QColor(171, 172, 172);
static const QColor NORMAL_TEXT_COL     = QColor(48, 58, 60);
static const QColor PAGE_TITLE_FONT_COL = QColor(0x26, 0x2E, 0x30);
static const QColor STAGE_TEXT_COL      = QColor(0x6B, 0x6B, 0x6B);

// AMS brand colours reused by score/market buttons
static const QColor AMS_CONTROL_BRAND_COLOUR   = QColor(0, 174, 66);
static const QColor AMS_CONTROL_DISABLE_COLOUR = QColor(180, 180, 180);
static const QColor AMS_CONTROL_WHITE_COLOUR   = QColor(255, 255, 255);

static const QString NA_STR         = _L("N/A");
static const QString TEMP_BLANK_STR = "_";

static const QColor NORMAL_FAN_TEXT_COL   = QColor(107, 107, 107);
static const QColor TEXT_LIGHT_FONT_COL   = QColor(107, 107, 107);
static const int    bed_temp_range[2]     = {20, 120};
static const int    default_champer_temp_min = 20;
static const int    default_champer_temp_max = 60;

static const QSize  Z_BUTTON_SIZE              = QSize(44, 40);
static const QSize  MISC_BUTTON_PANEL_SIZE     = QSize(136, 55);
static const QSize  MISC_BUTTON_1FAN_SIZE      = QSize(132, 51);
static const QSize  MISC_BUTTON_2FAN_SIZE      = QSize(66, 51);
static const QSize  TEMP_CTRL_MIN_SIZE_ONE_ICO = QSize(125, 52);
static const QSize  TEMP_CTRL_MIN_SIZE_TWO_ICO = QSize(145, 48);
static const QSize  AXIS_MIN_SIZE              = QSize(258, 258);
static const int    TEMP_THRESHOLD_VAL         = 2;

// =========================================================
// RectTextPanel
// =========================================================

RectTextPanel::RectTextPanel(QWidget *parent)
    : QWidget(parent)
{
}

void RectTextPanel::setText(const QString t)
{
    text = t;
    update();
}

void RectTextPanel::Rescale()
{
    update();
}

void RectTextPanel::OnPaint(QPaintEvent &event)
{
    QPainter dc(this);
    dc.setRenderHint(QPainter::Antialiasing);
    QRect  r  = rect();
    QColor bg(0x1B, 0x88, 0x44);
    dc.setBrush(bg);
    dc.setPen(Qt::NoPen);
    dc.drawRoundedRect(r, 4, 4);
    dc.setPen(Qt::white);
    dc.setFont(Label::Body_10);
    dc.drawText(r, Qt::AlignCenter, text);
    event.accept();
}

void RectTextPanel::paintEvent(QPaintEvent *event)
{
    OnPaint(*event);
}

// =========================================================
// ExtruderImage
// =========================================================

ExtruderImage::ExtruderImage(QWidget *parent, int id, int nozzle_num, const QPoint &pos, const QSize &size)
    : QWidget(parent)
    , m_nozzle_num(nozzle_num)
    , m_pipe_filled_load(nullptr)
    , m_pipe_filled_unload(nullptr)
    , m_pipe_empty_load(nullptr)
    , m_pipe_empty_unload(nullptr)
    , m_pipe_filled_load_unselected(nullptr)
    , m_pipe_filled_unload_unselected(nullptr)
    , m_pipe_empty_load_unselected(nullptr)
    , m_pipe_empty_unload_unselected(nullptr)
    , m_left_extruder_active_filled(nullptr)
    , m_left_extruder_active_empty(nullptr)
    , m_left_extruder_unactive_filled(nullptr)
    , m_left_extruder_unactive_empty(nullptr)
    , m_right_extruder_active_filled(nullptr)
    , m_right_extruder_active_empty(nullptr)
    , m_right_extruder_unactive_filled(nullptr)
    , m_right_extruder_unactive_empty(nullptr)
    , m_extruder_single_nozzle_empty_load(nullptr)
    , m_extruder_single_nozzle_empty_unload(nullptr)
    , m_extruder_single_nozzle_filled_load(nullptr)
    , m_extruder_single_nozzle_filled_unload(nullptr)
{
    if (!size.isEmpty()) resize(size);
    if (!pos.isNull()) move(pos);
    setFixedSize(45, 112);
}

ExtruderImage::~ExtruderImage()
{
    delete m_pipe_filled_load;
    delete m_pipe_filled_unload;
    delete m_pipe_empty_load;
    delete m_pipe_empty_unload;
    delete m_pipe_filled_load_unselected;
    delete m_pipe_filled_unload_unselected;
    delete m_pipe_empty_load_unselected;
    delete m_pipe_empty_unload_unselected;
    delete m_left_extruder_active_filled;
    delete m_left_extruder_active_empty;
    delete m_left_extruder_unactive_filled;
    delete m_left_extruder_unactive_empty;
    delete m_right_extruder_active_filled;
    delete m_right_extruder_active_empty;
    delete m_right_extruder_unactive_filled;
    delete m_right_extruder_unactive_empty;
    delete m_extruder_single_nozzle_empty_load;
    delete m_extruder_single_nozzle_empty_unload;
    delete m_extruder_single_nozzle_filled_load;
    delete m_extruder_single_nozzle_filled_unload;
}

void ExtruderImage::update(int nozzle_num, int /*nozzle_id*/)
{
    setExtruderCount(nozzle_num);
    QWidget::update();
}

void ExtruderImage::update(ExtruderState single_state)
{
    m_single_ext_state = single_state;
    QWidget::update();
}

void ExtruderImage::update(ExtruderState right_state, ExtruderState left_state)
{
    m_right_ext_state = right_state;
    m_left_ext_state  = left_state;
    QWidget::update();
}

void ExtruderImage::msw_rescale()
{
    QWidget::update();
}

void ExtruderImage::setExtruderCount(int nozzle_num)
{
    m_nozzle_num = nozzle_num;
}

void ExtruderImage::setExtruderUsed(std::string loc)
{
    current_nozzle_loc = loc;
}

void ExtruderImage::paintEvent(QPaintEvent &evt)
{
    QPainter dc(this);
    doRender(dc);
    evt.accept();
}

void ExtruderImage::render(QPainter &dc)
{
    doRender(dc);
}

void ExtruderImage::doRender(QPainter &dc)
{
    // Phase 3 will implement the full rendering
    dc.fillRect(rect(), Qt::white);
}

// =========================================================
// ExtruderSwithingStatus
// =========================================================

ExtruderSwithingStatus::ExtruderSwithingStatus(QWidget *parent)
    : QWidget(parent)
{
    m_switching_status_label = new Label(this);
    m_switching_status_label->setFont(Label::Body_13);
    if (parent) {
        m_switching_status_label->setStyleSheet(
            QString("background-color: %1;").arg(parent->palette().window().color().name()));
    }

    StateColor e_ctrl_bg(
        std::make_pair(BUTTON_PRESS_COL,   (int)StateColor::Pressed),
        std::make_pair(BUTTON_NORMAL1_COL, (int)StateColor::Normal));
    StateColor e_ctrl_bd(
        std::make_pair(BUTTON_HOVER_COL,   (int)StateColor::Hovered),
        std::make_pair(BUTTON_NORMAL1_COL, (int)StateColor::Normal));

    m_button_quit = new Button(this, _CTX(L_CONTEXT("Quit", "Quit_Switching"), "Quit_Switching"), "", 0);
    m_button_quit->setFont(Label::Body_13);
    connect(m_button_quit, &Button::clicked, this, [this]() {
        QEvent e(QEvent::None);
        on_quit(e);
    });
    m_button_quit->setFixedSize(SWITCHING_STATUS_BTN_SIZE);
    m_button_quit->SetBackgroundColor(e_ctrl_bg);
    m_button_quit->SetBorderColor(e_ctrl_bd);
    m_button_quit->SetBorderWidth(2);

    m_button_retry = new Button(this, _L("Retry"), "", 0);
    m_button_retry->setFont(Label::Body_13);
    connect(m_button_retry, &Button::clicked, this, [this]() {
        QEvent e(QEvent::None);
        on_retry(e);
    });
    m_button_retry->setFixedSize(SWITCHING_STATUS_BTN_SIZE);
    m_button_retry->SetBackgroundColor(e_ctrl_bg);
    m_button_retry->SetBorderColor(e_ctrl_bd);
    m_button_retry->SetBorderWidth(2);

    auto *btn_sizer = new QHBoxLayout;
    btn_sizer->setContentsMargins(0, 0, 0, 0);
    btn_sizer->setSpacing(0);
    btn_sizer->addWidget(m_button_quit,  0, Qt::AlignVCenter);
    btn_sizer->addSpacing(10);
    btn_sizer->addWidget(m_button_retry, 0, Qt::AlignVCenter);

    auto *main_sizer = new QVBoxLayout(this);
    main_sizer->setContentsMargins(0, 0, 0, 0);
    main_sizer->setSpacing(0);
    main_sizer->addSpacing(10);
    main_sizer->addWidget(m_switching_status_label, 0, Qt::AlignHCenter);
    main_sizer->addSpacing(10);
    main_sizer->addLayout(btn_sizer);
}

void ExtruderSwithingStatus::updateBy(MachineObject *obj_in)
{
    m_obj = obj_in;
    if (!m_obj) {
        hide();
    } else {
        if ((time(nullptr) - m_last_ctrl_time) > HOLD_TIME_6SEC) {
            updateBy(reinterpret_cast<Slic3r::MachineObject*>(obj_in)->GetExtderSystem());
        }
    }
}

bool ExtruderSwithingStatus::has_content_shown() const
{
    return isVisible() && (m_button_quit->isVisible() || m_button_retry->isVisible());
}

void ExtruderSwithingStatus::msw_rescale()
{
    QWidget::update();
}

void ExtruderSwithingStatus::updateBy(const DevExtderSystem *ext_system)
{
    setVisible(ext_system->GetTotalExtderCount() > 1);
    if (!isVisible()) return;
    // Phase 3 will complete this
}

void ExtruderSwithingStatus::showQuitBtn(bool show)
{
    m_button_quit->setVisible(show);
}

void ExtruderSwithingStatus::showRetryBtn(bool show)
{
    m_button_retry->setVisible(show);
}

void ExtruderSwithingStatus::on_quit(QEvent & /*event*/)
{
    // Phase 3 implementation
}

void ExtruderSwithingStatus::on_retry(QEvent & /*event*/)
{
    // Phase 3 implementation
}

// =========================================================
// PrintingTaskPanel
// =========================================================

PrintingTaskPanel::PrintingTaskPanel(QWidget *parent, PrintingTaskType type)
    : QWidget(parent)
    , m_type(type)
    , m_star_count(0)
    , m_question_button(nullptr)
{
    create_panel(this);
}

PrintingTaskPanel::~PrintingTaskPanel() {}

void PrintingTaskPanel::create_panel(QWidget *parent)
{
    auto *sizer = new QVBoxLayout;
    sizer->setContentsMargins(0, 0, 0, 0);
    sizer->setSpacing(0);

    // ---- Title bar ----
    m_panel_printing_title = new QWidget(parent);
    m_panel_printing_title->setFixedHeight(PAGE_TITLE_HEIGHT);
    m_panel_printing_title->setStyleSheet(
        QString("background-color: %1;").arg(STATUS_TITLE_BG.name()));

    auto *bSizer_printing_title = new QHBoxLayout(m_panel_printing_title);
    bSizer_printing_title->setContentsMargins(0, 0, 0, 0);
    bSizer_printing_title->setSpacing(0);

    m_staticText_printing = new QLabel(_L("Printing Progress"), m_panel_printing_title);
    m_staticText_printing->setStyleSheet(
        QString("color: %1; background: transparent;").arg(PAGE_TITLE_FONT_COL.name()));

    bSizer_printing_title->addSpacing(PAGE_TITLE_LEFT_MARGIN);
    bSizer_printing_title->addWidget(m_staticText_printing, 0, Qt::AlignVCenter);
    bSizer_printing_title->addStretch(1);

    // ---- Thumbnail ----
    m_bitmap_thumbnail = new QLabel(parent);
    m_bitmap_thumbnail->setFixedSize(TASK_THUMBNAIL_SIZE);
    m_bitmap_thumbnail->setAlignment(Qt::AlignCenter);

    // ---- Task info vertical sizer ----
    auto *bSizer_subtask_info = new QVBoxLayout;
    bSizer_subtask_info->setContentsMargins(0, 0, 0, 0);
    bSizer_subtask_info->setSpacing(0);

    // -- Task name row --
    auto *task_name_panel      = new QWidget(parent);
    auto *bSizer_task_name_hor = new QHBoxLayout(task_name_panel);
    bSizer_task_name_hor->setContentsMargins(0, 0, 10, 0);
    bSizer_task_name_hor->setSpacing(0);

    m_staticText_subtask_value = new QLabel(_L("N/A"), task_name_panel);
    m_staticText_subtask_value->setMaximumWidth(600);
    m_staticText_subtask_value->setFont(Label::Body_13);
    m_staticText_subtask_value->setStyleSheet("color: rgb(44,44,46);");
    m_staticText_subtask_value->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_staticText_subtask_value->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    m_bitmap_static_use_time = new QLabel(task_name_panel);
    m_bitmap_static_use_time->setFixedSize(16, 16);

    m_staticText_consumption_of_time = new QLabel("0m", task_name_panel);
    m_staticText_consumption_of_time->setFont(Label::Body_12);
    m_staticText_consumption_of_time->setStyleSheet("color: #686868;");

    m_bitmap_static_use_weight = new QLabel(task_name_panel);
    m_bitmap_static_use_weight->setFixedSize(16, 16);

    m_staticText_consumption_of_weight = new QLabel("0g", task_name_panel);
    m_staticText_consumption_of_weight->setFont(Label::Body_12);
    m_staticText_consumption_of_weight->setStyleSheet("color: #686868;");

    bSizer_task_name_hor->addWidget(m_staticText_subtask_value,         1);
    bSizer_task_name_hor->addWidget(m_bitmap_static_use_time,           0, Qt::AlignVCenter);
    bSizer_task_name_hor->addSpacing(3);
    bSizer_task_name_hor->addWidget(m_staticText_consumption_of_time,   0, Qt::AlignVCenter);
    bSizer_task_name_hor->addSpacing(10);
    bSizer_task_name_hor->addWidget(m_bitmap_static_use_weight,         0, Qt::AlignVCenter);
    bSizer_task_name_hor->addSpacing(3);
    bSizer_task_name_hor->addWidget(m_staticText_consumption_of_weight, 0, Qt::AlignVCenter);

    auto *bSizer_task_name = new QVBoxLayout;
    bSizer_task_name->setContentsMargins(0, 0, 0, 0);
    bSizer_task_name->setSpacing(0);
    bSizer_task_name->addWidget(task_name_panel);

    // -- Profile label --
    m_staticText_profile_value = new QLabel(parent);
    m_staticText_profile_value->setFont(Label::Body_11);
    m_staticText_profile_value->setStyleSheet("color: #6B6B6B;");
    m_staticText_profile_value->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_staticText_profile_value->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    // ---- Progress area ----
    auto *progress_lr_panel = new QWidget(parent);
    progress_lr_panel->setStyleSheet("background: white;");
    progress_lr_panel->setMaximumWidth(720);

    m_gauge_progress = new ProgressBar(progress_lr_panel, 100);
    m_gauge_progress->SetValue(0);
    m_gauge_progress->SetHeight(PROGRESSBAR_HEIGHT);

    StateColor white_bg(
        std::make_pair(QColor(255, 255, 255), (int)StateColor::Disabled),
        std::make_pair(QColor(255, 255, 255), (int)StateColor::Pressed),
        std::make_pair(QColor(255, 255, 255), (int)StateColor::Hovered),
        std::make_pair(QColor(255, 255, 255), (int)StateColor::Enabled),
        std::make_pair(QColor(255, 255, 255), (int)StateColor::Normal));

    // AnimaIcon animation list for pausing/stopping
    std::vector<std::string> anim_list{"ams_rfid_1", "ams_rfid_2", "ams_rfid_3", "ams_rfid_4"};

    m_pausing_icon = new AnimaIcon(progress_lr_panel, 0, anim_list, "refresh_printer", 100);
    m_pausing_icon->setMinimumSize(20, 20);
    m_pausing_icon->setToolTip(_L("Pausing"));
    m_pausing_icon->hide();

    m_stopping_icon = new AnimaIcon(progress_lr_panel, 0, anim_list, "refresh_printer", 100);
    m_stopping_icon->setMinimumSize(20, 20);
    m_stopping_icon->setToolTip(_L("Stopping"));
    m_stopping_icon->hide();

    m_button_partskip = new Button(progress_lr_panel, QString(), "print_control_partskip_disable", 20);
    m_button_partskip->setEnabled(false);
    m_button_partskip->hide();
    m_button_partskip->SetBackgroundColor(white_bg);
    m_button_partskip->SetIcon("print_control_partskip_disable");
    m_button_partskip->SetBorderColor(QColor(255, 255, 255));
    m_button_partskip->setFont(Label::Body_12);
    m_button_partskip->SetCornerRadius(0);
    m_button_partskip->setToolTip(_L("Parts Skip"));

    m_button_pause_resume = new ScalableButton(progress_lr_panel, 0, "print_control_pause");

    m_button_abort = new ScalableButton(progress_lr_panel, 0, "print_control_stop");
    m_button_abort->setToolTip(_L("Stop"));

    // -- Percent text panel --
    auto *penel_text  = new QWidget(progress_lr_panel);
    penel_text->setStyleSheet("background: white;");
    auto *bSizer_text = new QHBoxLayout(penel_text);
    bSizer_text->setContentsMargins(0, 0, 0, 0);
    bSizer_text->setSpacing(0);

    auto *sizer_percent = new QVBoxLayout;
    sizer_percent->setContentsMargins(0, 0, 0, 0);
    sizer_percent->setSpacing(0);
    sizer_percent->addStretch(1);

    auto *sizer_percent_icon = new QVBoxLayout;
    sizer_percent_icon->setContentsMargins(0, 0, 0, 0);
    sizer_percent_icon->setSpacing(0);
    sizer_percent_icon->addStretch(1);

    m_staticText_progress_percent = new QLabel("0", penel_text);
    m_staticText_progress_percent->setFont(Label::Head_18);
    m_staticText_progress_percent->setMaximumHeight(20);
    m_staticText_progress_percent->setStyleSheet("color: rgb(0,174,66);");

    m_staticText_progress_percent_icon = new QLabel("%", penel_text);
    m_staticText_progress_percent_icon->setFont(Label::Body_11);
    m_staticText_progress_percent_icon->setMaximumHeight(13);
    m_staticText_progress_percent_icon->setStyleSheet("color: rgb(0,174,66);");

    sizer_percent->addWidget(m_staticText_progress_percent);
    sizer_percent_icon->addWidget(m_staticText_progress_percent_icon);

    m_staticText_layers = new QLabel(_L("Layer: N/A"), penel_text);
    m_staticText_layers->setFont(Label::Body_12);
    m_staticText_layers->setStyleSheet("color: rgb(107,107,107);");
    m_staticText_layers->hide();

    m_staticText_progress_left = new QLabel(NA_STR, penel_text);
    m_staticText_progress_left->setFont(Label::Body_12);
    m_staticText_progress_left->setStyleSheet("color: rgb(107,107,107);");

    bSizer_text->addLayout(sizer_percent);
    bSizer_text->addLayout(sizer_percent_icon);
    bSizer_text->addStretch(1);
    bSizer_text->addWidget(m_staticText_layers,        0, Qt::AlignVCenter);
    bSizer_text->addSpacing(20);
    bSizer_text->addWidget(m_staticText_progress_left, 0, Qt::AlignVCenter);

    // -- Finish time panel --
    auto *penel_finish_time  = new QWidget(progress_lr_panel);
    penel_finish_time->setStyleSheet("background: white;");
    auto *bSizer_finish_time = new QHBoxLayout(penel_finish_time);
    bSizer_finish_time->setContentsMargins(0, 0, 0, 0);
    bSizer_finish_time->setSpacing(0);

    // Printing stage sub-panel
    m_printing_stage_panel = new QWidget(penel_finish_time);
    auto *printingstage_vertical_sizer   = new QVBoxLayout(m_printing_stage_panel);
    auto *printingstage_horizontal_sizer = new QHBoxLayout;
    printingstage_vertical_sizer->setContentsMargins(0, 0, 0, 0);
    printingstage_vertical_sizer->setSpacing(0);
    printingstage_horizontal_sizer->setContentsMargins(0, 0, 0, 0);
    printingstage_horizontal_sizer->setSpacing(0);

    m_printing_stage_underline = new QWidget(m_printing_stage_panel);
    m_printing_stage_underline->setFixedHeight(1);
    m_printing_stage_underline->setStyleSheet("background: rgb(146,146,146);");
    m_printing_stage_underline->hide();

    m_printing_stage_value = new QLabel(QString(), m_printing_stage_panel);
    m_printing_stage_value->setMaximumWidth(800);
    m_printing_stage_value->setFont(Label::Body_11);
    m_printing_stage_value->setStyleSheet(
        QString("color: %1;").arg(STAGE_TEXT_COL.name()));
    m_printing_stage_value->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    m_question_button = new ScalableButton(m_printing_stage_panel, 0, "thermal_question");
    m_question_button->setToolTip(_L("Click to view thermal preconditioning explanation"));
    m_question_button->setStyleSheet("background: white;");
    m_question_button->hide();

    printingstage_horizontal_sizer->addWidget(m_printing_stage_value, 0, Qt::AlignLeft | Qt::AlignVCenter);
    printingstage_horizontal_sizer->addSpacing(5);
    printingstage_horizontal_sizer->addWidget(m_question_button,      0, Qt::AlignVCenter);
    printingstage_vertical_sizer->addLayout(printingstage_horizontal_sizer);
    printingstage_vertical_sizer->addWidget(m_printing_stage_underline);

    m_staticText_finish_time = new Label(penel_finish_time);
    m_staticText_finish_time->setText(_L("Estimated finish time: ") + "N/A");
    m_staticText_finish_time->setFont(Label::Body_14);
    m_staticText_finish_time->setStyleSheet("color: rgb(107,107,107);");
    m_staticText_finish_time->setToolTip(
        _L("The estimated printing time for \nmulti-color models may be inaccurate."));

    m_staticText_finish_day = new RectTextPanel(penel_finish_time);
    m_staticText_finish_day->setMinimumSize(20, 20);
    m_staticText_finish_day->hide();

    bSizer_finish_time->addWidget(m_printing_stage_panel,   0, Qt::AlignLeft | Qt::AlignVCenter);
    bSizer_finish_time->addStretch(1);
    bSizer_finish_time->addWidget(m_staticText_finish_time, 0, Qt::AlignVCenter);
    bSizer_finish_time->addSpacing(10);
    bSizer_finish_time->addWidget(m_staticText_finish_day,  0, Qt::AlignVCenter);

    // -- Assemble progress_lr_panel layouts --
    auto *progress_lr_sizer    = new QHBoxLayout(progress_lr_panel);
    auto *progress_left_sizer  = new QVBoxLayout;
    auto *progress_right_sizer = new QHBoxLayout;
    progress_lr_sizer->setContentsMargins(0, 0, 0, 0);
    progress_lr_sizer->setSpacing(0);
    progress_left_sizer->setContentsMargins(0, 0, 0, 0);
    progress_left_sizer->setSpacing(0);
    progress_right_sizer->setContentsMargins(0, 0, 0, 0);
    progress_right_sizer->setSpacing(0);

    progress_left_sizer->addWidget(penel_text);
    progress_left_sizer->addSpacing(10);
    progress_left_sizer->addWidget(m_gauge_progress);
    progress_left_sizer->addSpacing(10);
    progress_left_sizer->addWidget(penel_finish_time);

    progress_right_sizer->addSpacing(18);
    progress_right_sizer->addWidget(m_button_partskip,     0, Qt::AlignVCenter);
    progress_right_sizer->addSpacing(18);
    progress_right_sizer->addWidget(m_pausing_icon,        0, Qt::AlignVCenter);
    progress_right_sizer->addWidget(m_button_pause_resume, 0, Qt::AlignVCenter);
    progress_right_sizer->addSpacing(18);
    progress_right_sizer->addWidget(m_stopping_icon,       0, Qt::AlignVCenter);
    progress_right_sizer->addWidget(m_button_abort,        0, Qt::AlignVCenter);
    progress_right_sizer->addSpacing(18);

    progress_lr_sizer->addLayout(progress_left_sizer,  1);
    progress_lr_sizer->addLayout(progress_right_sizer, 0);

    // ---- Assemble bSizer_subtask_info ----
    bSizer_subtask_info->addSpacing(14);
    bSizer_subtask_info->addLayout(bSizer_task_name);
    bSizer_subtask_info->addSpacing(5);
    bSizer_subtask_info->addWidget(m_staticText_profile_value);
    bSizer_subtask_info->addSpacing(5);
    bSizer_subtask_info->addWidget(progress_lr_panel);

    // ---- m_printing_sizer: thumbnail + info ----
    m_printing_sizer = new QHBoxLayout;
    m_printing_sizer->setContentsMargins(0, 0, 0, 0);
    m_printing_sizer->setSpacing(0);

    m_printing_sizer->addSpacing(12);
    m_printing_sizer->addWidget(m_bitmap_thumbnail, 0, Qt::AlignVCenter);
    m_printing_sizer->addSpacing(8 + 12);
    m_printing_sizer->addLayout(bSizer_subtask_info, 1);

    // ---- Static separator line (hidden) ----
    m_staticline = new QWidget(parent);
    m_staticline->setStyleSheet(
        QString("background: %1;").arg(STATIC_BOX_LINE_COL.name()));
    m_staticline->setFixedHeight(1);
    m_staticline->hide();

    // ---- Error text panel ----
    m_panel_error_txt = new QWidget(parent);
    m_panel_error_txt->setStyleSheet("background: white;");
    auto *static_text_sizer = new QHBoxLayout(m_panel_error_txt);
    static_text_sizer->setContentsMargins(0, 0, 0, 0);
    static_text_sizer->setSpacing(0);

    m_error_text = new Label(m_panel_error_txt, QString(), Label::LB_AUTO_WRAP);
    m_error_text->setStyleSheet("color: red;");
    static_text_sizer->addSpacing(PAGE_TITLE_LEFT_MARGIN);
    static_text_sizer->addWidget(m_error_text, 1);
    static_text_sizer->addSpacing(10);

    StateColor clean_bg(
        std::make_pair(QColor(255, 255, 255), (int)StateColor::Disabled),
        std::make_pair(QColor(206, 206, 206), (int)StateColor::Pressed),
        std::make_pair(QColor(238, 238, 238), (int)StateColor::Hovered),
        std::make_pair(QColor(255, 255, 255), (int)StateColor::Enabled),
        std::make_pair(QColor(255, 255, 255), (int)StateColor::Normal));
    StateColor clean_bd(
        std::make_pair(QColor(144, 144, 144), (int)StateColor::Disabled),
        std::make_pair(QColor(38,  46,  48),  (int)StateColor::Enabled));
    StateColor clean_text(
        std::make_pair(QColor(144, 144, 144), (int)StateColor::Disabled),
        std::make_pair(QColor(38,  46,  48),  (int)StateColor::Enabled));

    m_button_clean = new Button(m_panel_error_txt, _L("Clear"));
    m_button_clean->SetBackgroundColor(clean_bg);
    m_button_clean->SetBorderColor(clean_bd);
    m_button_clean->SetTextColor(clean_text);
    m_button_clean->setFont(Label::Body_10);
    m_button_clean->setMinimumSize(TASK_BUTTON_SIZE2);

    static_text_sizer->addWidget(m_button_clean, 0, Qt::AlignVCenter);
    static_text_sizer->addSpacing(5);

    m_panel_error_txt->hide();

    // ---- Score static line (hidden) ----
    m_score_staticline = new QWidget(parent);
    m_score_staticline->setStyleSheet(
        QString("background: %1;").arg(STATIC_BOX_LINE_COL.name()));
    m_score_staticline->setFixedHeight(1);
    m_score_staticline->hide();

    // ---- Request failed panel ----
    m_request_failed_panel = new QWidget(parent);
    m_request_failed_panel->setStyleSheet("background: white;");
    auto *static_request_failed_sizer = new QHBoxLayout(m_request_failed_panel);
    static_request_failed_sizer->setContentsMargins(10, 10, 10, 10);
    static_request_failed_sizer->setSpacing(0);

    m_request_failed_info = new QLabel(
        _L("You have completed printing the mall model, \nbut the synchronization of rating information has failed."),
        m_request_failed_panel);
    m_request_failed_info->setStyleSheet("color: red;");
    m_request_failed_info->setFont(Label::Body_10);
    static_request_failed_sizer->addWidget(m_request_failed_info, 0);

    StateColor btn_bg_green(
        std::make_pair(AMS_CONTROL_DISABLE_COLOUR, (int)StateColor::Disabled),
        std::make_pair(QColor(27,  136, 68),        (int)StateColor::Pressed),
        std::make_pair(QColor(61,  203, 115),       (int)StateColor::Hovered),
        std::make_pair(AMS_CONTROL_BRAND_COLOUR,    (int)StateColor::Normal));
    StateColor btn_bd_green(
        std::make_pair(AMS_CONTROL_WHITE_COLOUR,  (int)StateColor::Disabled),
        std::make_pair(AMS_CONTROL_BRAND_COLOUR,  (int)StateColor::Enabled));

    m_button_market_retry = new Button(m_request_failed_panel, _L("Retry"));
    m_button_market_retry->SetBackgroundColor(btn_bg_green);
    m_button_market_retry->SetBorderColor(btn_bd_green);
    m_button_market_retry->SetTextColor(QColor("#FFFFFE"));
    m_button_market_retry->setFixedSize(128, 26);
    m_button_market_retry->setMinimumHeight(26);
    m_button_market_retry->SetCornerRadius(13);
    static_request_failed_sizer->addStretch(1);
    static_request_failed_sizer->addWidget(m_button_market_retry);

    m_request_failed_panel->hide();

    // ---- Score subtask info panel ----
    m_score_subtask_info = new QWidget(parent);
    m_score_subtask_info->setStyleSheet("background: white;");
    auto *static_score_sizer = new QVBoxLayout(m_score_subtask_info);
    static_score_sizer->setContentsMargins(10, 10, 10, 10);
    static_score_sizer->setSpacing(0);

    auto *static_score_text = new QLabel(
        _L("How do you like this printing file?"), m_score_subtask_info);
    static_score_sizer->addWidget(static_score_text, 1);

    m_has_rated_prompt = new QLabel(
        _L("(The model has already been rated. Your rating will overwrite the previous rating.)"),
        m_score_subtask_info);
    m_has_rated_prompt->setStyleSheet("color: black;");
    m_has_rated_prompt->setFont(Label::Body_10);
    m_has_rated_prompt->hide();
    static_score_sizer->addWidget(m_has_rated_prompt, 1);

    auto *static_score_star_sizer = new QHBoxLayout;
    static_score_star_sizer->setContentsMargins(0, 0, 0, 0);
    static_score_star_sizer->setSpacing(0);

    m_score_star.resize(5);
    for (int i = 0; i < (int)m_score_star.size(); ++i) {
        m_score_star[i] = new ScalableButton(m_score_subtask_info, 0, "score_star_dark",
                                             QString(), QSize(26, 26), QPoint(), 0, true, 26);
        m_score_star[i]->setFixedSize(26, 26);
        int captured_i = i;
        connect(m_score_star[i], &ScalableButton::clicked, this, [this, captured_i]() {
            for (int j = 0; j < (int)m_score_star.size(); ++j) {
                ScalableBitmap light_star(nullptr, "score_star_light", 26);
                m_score_star[j]->SetBitmap_(light_star);
                if (j == captured_i) {
                    m_star_count = j + 1;
                    break;
                }
            }
            for (int k = m_star_count; k < (int)m_score_star.size(); ++k) {
                ScalableBitmap dark_star(nullptr, "score_star_dark", 26);
                m_score_star[k]->SetBitmap_(dark_star);
            }
            m_star_count_dirty = true;
            m_button_market_scoring->setEnabled(true);
        });
        static_score_star_sizer->addSpacing(5);
        static_score_star_sizer->addWidget(m_score_star[i], 1);
    }

    m_button_market_scoring = new Button(m_score_subtask_info, _L("Rate"));
    m_button_market_scoring->SetBackgroundColor(btn_bg_green);
    m_button_market_scoring->SetBorderColor(btn_bd_green);
    m_button_market_scoring->SetTextColor(QColor("#FFFFFE"));
    m_button_market_scoring->setFixedSize(128, 26);
    m_button_market_scoring->setMinimumHeight(26);
    m_button_market_scoring->SetCornerRadius(13);
    m_button_market_scoring->setEnabled(false);

    static_score_star_sizer->addStretch(1);
    static_score_star_sizer->addSpacing(10);
    static_score_star_sizer->addWidget(m_button_market_scoring);
    static_score_sizer->addLayout(static_score_star_sizer);

    m_score_subtask_info->hide();

    // ---- Assemble top-level sizer ----
    sizer->addWidget(m_panel_printing_title);
    sizer->addSpacing(12);
    sizer->addLayout(m_printing_sizer);
    sizer->addSpacing(15);
    sizer->addWidget(m_staticline);
    sizer->addWidget(m_panel_error_txt);
    sizer->addSpacing(12);
    sizer->addSpacing(15);
    sizer->addWidget(m_score_staticline);
    sizer->addWidget(m_request_failed_panel);
    sizer->addWidget(m_score_subtask_info);
    sizer->addSpacing(12);

    if (m_type == CALIBRATION) {
        m_panel_printing_title->hide();
        m_bitmap_thumbnail->hide();
        task_name_panel->hide();
        m_staticText_profile_value->hide();
    }

    parent->setLayout(sizer);
    parent->setMinimumWidth(PAGE_MIN_WIDTH);
}

void PrintingTaskPanel::init_bitmaps()
{
    m_thumbnail_placeholder = ScalableBitmap(this, "monitor_placeholder", 120);
    m_bitmap_use_time       = ScalableBitmap(this, "print_info_time",    16);
    m_bitmap_use_weight     = ScalableBitmap(this, "print_info_weight",  16);
    m_bitmap_thumbnail->setPixmap(m_thumbnail_placeholder.bmp());
    m_bitmap_static_use_time->setPixmap(m_bitmap_use_time.bmp());
    m_bitmap_static_use_weight->setPixmap(m_bitmap_use_weight.bmp());
}

void PrintingTaskPanel::init_scaled_buttons()
{
    m_button_clean->setMinimumSize(QSize(48, 24));
    m_button_clean->SetCornerRadius(12);
}

void PrintingTaskPanel::error_info_reset()
{
    if (m_panel_error_txt->isVisible()) {
        m_staticline->hide();
        m_panel_error_txt->hide();
        layout()->activate();
    }
}

void PrintingTaskPanel::show_error_msg(QString msg)
{
    m_error_text->setText(msg);
    m_staticline->show();
    m_panel_error_txt->show();
    layout()->activate();
}

void PrintingTaskPanel::reset_printing_value()
{
    m_staticText_progress_percent->setText("0");
    m_staticText_progress_left->setText(NA_STR);
    m_gauge_progress->SetValue(0);
    m_staticText_finish_time->setText(_L("Estimated finish time: ") + "N/A");
    m_staticText_layers->hide();
    m_staticText_finish_day->hide();
}

void PrintingTaskPanel::msw_rescale()
{
    m_pausing_icon->Rescale();
    m_stopping_icon->Rescale();
    m_panel_printing_title->setFixedHeight(PAGE_TITLE_HEIGHT);
    m_gauge_progress->SetHeight(PROGRESSBAR_HEIGHT);
    m_gauge_progress->Rescale();
    m_staticText_finish_day->Rescale();
    m_button_pause_resume->msw_rescale();
    m_button_abort->msw_rescale();
    m_bitmap_thumbnail->setFixedSize(TASK_THUMBNAIL_SIZE);

    for (int i = 0; i < (int)m_score_star.size(); ++i) {
        if (i < m_star_count) {
            ScalableBitmap light_star(nullptr, "score_star_light", 26);
            m_score_star[i]->SetBitmap_(light_star);
        } else {
            ScalableBitmap dark_star(nullptr, "score_star_dark", 26);
            m_score_star[i]->SetBitmap_(dark_star);
        }
    }
    m_button_market_scoring->Rescale();
    m_button_clean->Rescale();
    m_button_market_retry->Rescale();
}

void PrintingTaskPanel::enable_partskip_button(MachineObject * /*obj*/, bool enable)
{
    m_button_partskip->setEnabled(enable);
    if (enable) {
        m_button_partskip->show();
        m_button_partskip->SetIcon("print_control_partskip");
    } else {
        m_button_partskip->hide();
    }
}

void PrintingTaskPanel::update_pausing_state(bool enter)
{
    if (enter) {
        m_pausing_icon->show();
        m_pausing_icon->Play();
        m_button_pause_resume->hide();
    } else {
        m_pausing_icon->Stop();
        m_pausing_icon->hide();
        m_button_pause_resume->show();
    }
}

void PrintingTaskPanel::update_stopping_state(bool enter)
{
    if (enter) {
        m_stopping_icon->show();
        m_stopping_icon->Play();
        m_button_abort->hide();
    } else {
        m_stopping_icon->Stop();
        m_stopping_icon->hide();
        m_button_abort->show();
    }
}

void PrintingTaskPanel::enable_pause_resume_button(bool enable, std::string type)
{
    m_button_pause_resume->setEnabled(enable);
    if (!enable) {
        if (type == "pause_disable") {
            m_button_pause_resume->SetBitmap_("print_control_pause_disable");
        } else if (type == "resume_disable") {
            m_button_pause_resume->SetBitmap_("print_control_resume_disable");
        }
    } else {
        if (type == "resume") {
            m_button_pause_resume->SetBitmap_("print_control_resume");
            m_button_pause_resume->setToolTip(_L("Resume"));
        } else if (type == "pause") {
            m_button_pause_resume->SetBitmap_("print_control_pause");
            m_button_pause_resume->setToolTip(_L("Pause"));
        }
    }
}

void PrintingTaskPanel::enable_abort_button(bool enable)
{
    m_button_abort->setEnabled(enable);
}

void PrintingTaskPanel::update_subtask_name(QString name)
{
    m_staticText_subtask_value->setText(name);
}

void PrintingTaskPanel::update_stage_value(QString stage, int /*val*/)
{
    m_printing_stage_value->setText(stage);
}

void PrintingTaskPanel::update_stage_value_with_machine(QString stage, int val,
                                                        MachineObject * /*obj*/)
{
    update_stage_value(stage, val);
}

void PrintingTaskPanel::on_stage_clicked(QMouseEvent & /*event*/)
{
    // Phase 6 will implement
}

void PrintingTaskPanel::update_progress_percent(QString percent, QString icon)
{
    m_staticText_progress_percent->setText(percent);
    m_staticText_progress_percent_icon->setText(icon);
}

void PrintingTaskPanel::update_left_time(QString time)
{
    m_staticText_progress_left->setText(time);
}

void PrintingTaskPanel::update_finish_time(QString finish_time)
{
    m_staticText_finish_time->setText(_L("Estimated finish time: ") + finish_time);
}

void PrintingTaskPanel::update_left_time(int mc_left_time)
{
    // Phase 6 will add full dhms formatting
    m_staticText_progress_left->setText(QString::number(mc_left_time) + "s");
}

void PrintingTaskPanel::update_layers_num(bool show, QString num)
{
    if (show && !num.isEmpty()) {
        m_staticText_layers->setText(num);
        m_staticText_layers->show();
    } else {
        m_staticText_layers->hide();
    }
}

void PrintingTaskPanel::show_priting_use_info(bool show, QString time, QString weight)
{
    m_bitmap_static_use_time->setVisible(show);
    m_staticText_consumption_of_time->setVisible(show);
    m_bitmap_static_use_weight->setVisible(show);
    m_staticText_consumption_of_weight->setVisible(show);
    if (show) {
        m_staticText_consumption_of_time->setText(time);
        m_staticText_consumption_of_weight->setText(weight);
    }
}

void PrintingTaskPanel::show_profile_info(bool show, QString profile)
{
    m_staticText_profile_value->setVisible(show);
    if (show) m_staticText_profile_value->setText(profile);
}

void PrintingTaskPanel::set_thumbnail_img(const QPixmap &bmp, const std::string &bmp_name)
{
    m_thumbnail_bmp_display      = bmp;
    m_thumbnail_bmp_display_name = bmp_name;
    m_bitmap_thumbnail->setPixmap(
        bmp.scaled(TASK_THUMBNAIL_SIZE, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    m_bitmap_thumbnail->update();
}

void PrintingTaskPanel::set_plate_index(int plate_idx)
{
    m_plate_index = plate_idx;
    m_bitmap_thumbnail->update();
}

void PrintingTaskPanel::market_scoring_show(bool show)
{
    m_score_staticline->setVisible(show);
    m_score_subtask_info->setVisible(show);
    layout()->activate();
}

bool PrintingTaskPanel::is_market_scoring_show()
{
    return m_score_subtask_info->isVisible();
}

void PrintingTaskPanel::set_has_reted_text(bool has_rated)
{
    m_has_rated_prompt->setVisible(has_rated);
    layout()->activate();
}

void PrintingTaskPanel::set_star_count(int star_count)
{
    m_star_count = star_count;
    for (int i = 0; i < (int)m_score_star.size(); ++i) {
        if (i < m_star_count) {
            ScalableBitmap light_star(nullptr, "score_star_light", 26);
            m_score_star[i]->SetBitmap_(light_star);
        } else {
            ScalableBitmap dark_star(nullptr, "score_star_dark", 26);
            m_score_star[i]->SetBitmap_(dark_star);
        }
    }
}

void PrintingTaskPanel::paint(QPaintEvent & /*event*/)
{
    // Phase 7 will implement thumbnail painting with background grid
}

// =========================================================
// StatusBasePanel
// =========================================================

StatusBasePanel::StatusBasePanel(QWidget *parent, int /*id*/, const QPoint & /*pos*/,
                                 const QSize & /*size*/, long /*style*/,
                                 const QString & /*name*/)
    : QScrollArea(parent)
    , m_media_play_ctrl(nullptr)
    , m_camera_fullscreen_frame(nullptr)
    , m_camera_media_sizer(nullptr)
    , m_setting_button(nullptr)
    , m_camera_fullscreen_button(nullptr)
    // Members created by machine-control stub — init to nullptr
    , m_switch_lamp(nullptr)
    , m_switch_speed(nullptr)
    , m_bpButton_xy(nullptr)
    , m_bpButton_z_10(nullptr)
    , m_bpButton_z_1(nullptr)
    , m_bpButton_z_down_1(nullptr)
    , m_bpButton_z_down_10(nullptr)
    , m_bpButton_e_10(nullptr)
    , m_bpButton_e_down_10(nullptr)
    , m_nozzle_btn_panel(nullptr)
    , m_tempCtrl_nozzle(nullptr)
    , m_tempCtrl_nozzle_deputy(nullptr)
    , m_tempCtrl_bed(nullptr)
    , m_tempCtrl_chamber(nullptr)
    , m_switch_fan(nullptr)
    , m_switch_nozzle_fan(nullptr)
    , m_switch_printing_fan(nullptr)
    , m_switch_cham_fan(nullptr)
    , m_ams_rack_switch(nullptr)
    , m_ams_control(nullptr)
    , m_ams_control_box(nullptr)
    , m_panel_nozzle_rack(nullptr)
    , m_extruder_switching_status(nullptr)
    , m_filament_step(nullptr)
    , m_button_retry(nullptr)
    , m_fila_change_abort(nullptr)
    , m_filament_load_box(nullptr)
{
    Slic3r::DeviceManager *dev = Slic3r::GUI::wxGetApp().getDeviceManager();
    if (dev) obj = reinterpret_cast<MachineObject*>(dev->get_selected_machine());

    init_bitmaps();

    // Create body widget FIRST so widget() returns it inside create_monitoring_page()
    auto *body = new QWidget;
    setWidget(body);
    setWidgetResizable(true);
    body->setStyleSheet("background-color: #EEEEEE;");

    auto *bSizer_status = new QVBoxLayout(body);
    bSizer_status->setContentsMargins(0, 0, 0, 0);
    bSizer_status->setSpacing(0);

    // Top separator
    auto *m_panel_separotor_top = new QWidget(body);
    m_panel_separotor_top->setFixedHeight(PAGE_SPACING);
    m_panel_separotor_top->setStyleSheet("background: #EEEEEE;");
    bSizer_status->addWidget(m_panel_separotor_top);

    auto *bSizer_status_below = new QHBoxLayout;
    bSizer_status_below->setContentsMargins(0, 0, 0, 0);
    bSizer_status_below->setSpacing(0);

    // Left separator
    auto *m_panel_separotor_left = new QWidget(body);
    m_panel_separotor_left->setMinimumWidth(PAGE_SPACING);
    m_panel_separotor_left->setStyleSheet("background: #EEEEEE;");
    bSizer_status_below->addWidget(m_panel_separotor_left, 0);

    // Left column layout
    auto *bSizer_left = new QVBoxLayout;
    bSizer_left->setContentsMargins(0, 0, 0, 0);
    bSizer_left->setSpacing(0);

    // create_monitoring_page() parents its items to widget() == body
    auto *m_monitoring_sizer = create_monitoring_page();
    bSizer_left->addLayout(m_monitoring_sizer, 1);

    // Separator added to monitoring sizer (matches wx: m_monitoring_sizer->Add(sep))
    auto *m_panel_separotor1 = new QWidget(body);
    m_panel_separotor1->setFixedHeight(PAGE_SPACING);
    m_panel_separotor1->setStyleSheet("background: #EEEEEE;");
    m_monitoring_sizer->addWidget(m_panel_separotor1);

    // Project task panel added to monitoring sizer (matches wx: m_monitoring_sizer->Add(task))
    m_project_task_panel = new PrintingTaskPanel(body, PrintingTaskType::PRINGINT);
    m_project_task_panel->init_bitmaps();
    m_monitoring_sizer->addWidget(m_project_task_panel);

    bSizer_status_below->addLayout(bSizer_left, 1);

    // Middle separator
    auto *m_panel_separator_middle = new QWidget(body);
    m_panel_separator_middle->setMinimumWidth(PAGE_SPACING);
    m_panel_separator_middle->setStyleSheet("background: #EEEEEE;");
    bSizer_status_below->addWidget(m_panel_separator_middle, 0);

    // Machine control panel
    m_machine_ctrl_panel = new QWidget(body);
    m_machine_ctrl_panel->setStyleSheet("background: white;");
    auto *m_machine_control = create_machine_control_page(m_machine_ctrl_panel);
    m_machine_ctrl_panel->setLayout(m_machine_control);
    bSizer_status_below->addWidget(m_machine_ctrl_panel, 0);

    // Right separator
    m_panel_separator_right = new QWidget(body);
    m_panel_separator_right->setFixedWidth(PAGE_SPACING);
    m_panel_separator_right->setStyleSheet("background: #EEEEEE;");
    bSizer_status_below->addWidget(m_panel_separator_right, 0);

    bSizer_status->addLayout(bSizer_status_below, 1);

    // Bottom separator
    m_panel_separotor_bottom = new QWidget(body);
    m_panel_separotor_bottom->setFixedHeight(PAGE_SPACING);
    m_panel_separotor_bottom->setStyleSheet("background: #EEEEEE;");
    bSizer_status->addWidget(m_panel_separotor_bottom);
}

StatusBasePanel::~StatusBasePanel()
{
    close_camera_fullscreen();
    delete m_media_play_ctrl;
}

void StatusBasePanel::init_bitmaps()
{
    m_bitmap_item_prediction = create_scaled_bitmap("monitor_item_prediction", nullptr, 16);
    m_bitmap_item_cost       = create_scaled_bitmap("monitor_item_cost",        nullptr, 16);
    m_bitmap_item_print      = create_scaled_bitmap("monitor_item_print",       nullptr, 18);
    m_bitmap_axis_home       = ScalableBitmap(this, "monitor_axis_home",        32);
    m_bitmap_lamp_on         = ScalableBitmap(this, "monitor_lamp_on",          24);
    m_bitmap_lamp_off        = ScalableBitmap(this, "monitor_lamp_off",         24);
    m_bitmap_fan_on          = ScalableBitmap(this, "monitor_fan_on",           22);
    m_bitmap_fan_off         = ScalableBitmap(this, "monitor_fan_off",          22);
    m_bitmap_speed           = ScalableBitmap(this, "monitor_speed",            24);
    m_bitmap_speed_active    = ScalableBitmap(this, "monitor_speed_active",     24);
    m_thumbnail_brokenimg    = ScalableBitmap(this, "monitor_brokenimg",       120);
    m_thumbnail_sdcard       = ScalableBitmap(this, "monitor_sdcard_thumbnail", 120);
}

// Phase 1 stub: camera title + black placeholder.  Phase 5 adds real camera UI.
QBoxLayout *StatusBasePanel::create_monitoring_page()
{
    QWidget *w   = widget(); // body widget — set before this call
    auto    *lay = new QVBoxLayout;
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(0);

    // ---- Camera title bar ----
    m_panel_monitoring_title = new QWidget(w);
    m_panel_monitoring_title->setFixedHeight(PAGE_TITLE_HEIGHT);
    m_panel_monitoring_title->setStyleSheet(
        QString("background-color: %1;").arg(STATUS_TITLE_BG.name()));

    auto *title_lay = new QHBoxLayout(m_panel_monitoring_title);
    title_lay->setContentsMargins(0, 0, 0, 0);
    title_lay->setSpacing(0);

    m_staticText_monitoring = new Label(m_panel_monitoring_title, _L("Camera"));
    m_staticText_monitoring->setFont(Label::Body_13);
    m_staticText_monitoring->setStyleSheet(
        QString("color: %1;").arg(PAGE_TITLE_FONT_COL.name()));
    title_lay->addSpacing(PAGE_TITLE_LEFT_MARGIN);
    title_lay->addWidget(m_staticText_monitoring, 0, Qt::AlignVCenter);
    title_lay->addSpacing(13);
    title_lay->addStretch(1);

    // Timelapse text + switch (hidden by default)
    m_staticText_timelapse = new QLabel(_L("Timelapse"), m_panel_monitoring_title);
    m_staticText_timelapse->hide();
    title_lay->addWidget(m_staticText_timelapse, 0, Qt::AlignVCenter);

    m_mqtt_source = new QLabel(QString("MqttSource"), m_panel_monitoring_title);
    m_mqtt_source->hide();
    title_lay->addWidget(m_mqtt_source, 0, Qt::AlignVCenter);

    m_bmToggleBtn_timelapse = new SwitchButton(m_panel_monitoring_title);
    m_bmToggleBtn_timelapse->setMinimumWidth(40);
    m_bmToggleBtn_timelapse->hide();
    title_lay->addWidget(m_bmToggleBtn_timelapse, 0, Qt::AlignVCenter);

    // Status icon labels (sdcard, timelapse, recording, vcamera)
    m_bitmap_sdcard_img = new QLabel(m_panel_monitoring_title);
    m_bitmap_sdcard_img->setFixedSize(38, 24);
    m_bitmap_sdcard_img->setToolTip(_L("Storage"));
    title_lay->addWidget(m_bitmap_sdcard_img, 0, Qt::AlignVCenter);

    m_bitmap_timelapse_img = new QLabel(m_panel_monitoring_title);
    m_bitmap_timelapse_img->setFixedSize(38, 24);
    m_bitmap_timelapse_img->setToolTip(_L("Timelapse"));
    m_bitmap_timelapse_img->hide();
    title_lay->addWidget(m_bitmap_timelapse_img, 0, Qt::AlignVCenter);

    m_bitmap_recording_img = new QLabel(m_panel_monitoring_title);
    m_bitmap_recording_img->setFixedSize(38, 24);
    m_bitmap_recording_img->setToolTip(_L("Video"));
    title_lay->addWidget(m_bitmap_recording_img, 0, Qt::AlignVCenter);

    m_bitmap_vcamera_img = new QLabel(m_panel_monitoring_title);
    m_bitmap_vcamera_img->setFixedSize(38, 24);
    m_bitmap_vcamera_img->setToolTip(_L("Go Live"));
    m_bitmap_vcamera_img->hide();
    title_lay->addWidget(m_bitmap_vcamera_img, 0, Qt::AlignVCenter);

    // Fullscreen + settings camera buttons
    m_camera_fullscreen_button = new CameraItem(
        m_panel_monitoring_title, "camera_fullscreen", "camera_fullscreen_hover");
    m_camera_fullscreen_button->setMinimumSize(38, 24);
    m_camera_fullscreen_button->setStyleSheet(
        QString("background-color: %1;").arg(STATUS_TITLE_BG.name()));
    m_camera_fullscreen_button->setToolTip(_L("Enter Camera Full Screen"));
    title_lay->addWidget(m_camera_fullscreen_button, 0, Qt::AlignVCenter);

    m_setting_button = new CameraItem(
        m_panel_monitoring_title, "camera_setting", "camera_setting_hover");
    m_setting_button->setMinimumSize(38, 24);
    m_setting_button->setStyleSheet(
        QString("background-color: %1;").arg(STATUS_TITLE_BG.name()));
    m_setting_button->setToolTip(_L("Camera Setting"));
    title_lay->addWidget(m_setting_button, 0, Qt::AlignVCenter);

    title_lay->addSpacing(13);

    // Connect fullscreen toggle via event filter (CameraItem has no clicked signal)
    {
        auto *cf = new PanelClickFilter(m_camera_fullscreen_button,
            [this](QMouseEvent &) { toggle_camera_fullscreen(); });
        m_camera_fullscreen_button->installEventFilter(cf);
    }

    lay->addWidget(m_panel_monitoring_title);

    // ---- Camera view (wxMediaCtrl3 equivalent) ----
    m_media_ctrl = new QWidget(w);
    m_media_ctrl->setStyleSheet("background: black;");
    m_media_ctrl->setMinimumSize(PAGE_MIN_WIDTH, 288);

    // ---- Media play control bar ----
    // Slic3r::GUI::MediaPlayCtrl vs global-scope forward decl — reinterpret_cast like MachineObject
    auto *play_ctrl = new Slic3r::GUI::MediaPlayCtrl(w, m_media_ctrl);
    m_media_play_ctrl = reinterpret_cast<::MediaPlayCtrl *>(play_ctrl);

    lay->addWidget(m_media_ctrl, 1);
    lay->addWidget(play_ctrl, 0);

    m_camera_media_sizer = lay;

    return lay;
}

QBoxLayout *StatusBasePanel::create_machine_control_page(QWidget *parent)
{
    auto *bSizer_right = new QVBoxLayout;
    bSizer_right->setContentsMargins(0, 0, 0, 0);
    bSizer_right->setSpacing(0);

    // ---- Control title bar ----
    m_panel_control_title = new QWidget(parent);
    m_panel_control_title->setFixedHeight(PAGE_TITLE_HEIGHT);
    m_panel_control_title->setStyleSheet(
        QString("background-color: %1;").arg(STATUS_TITLE_BG.name()));

    auto *title_lay = new QHBoxLayout(m_panel_control_title);
    title_lay->setContentsMargins(0, 0, 0, 0);
    title_lay->setSpacing(0);

    m_staticText_control = new Label(m_panel_control_title, _L("Control"));
    m_staticText_control->setFont(Label::Body_13);
    m_staticText_control->setStyleSheet(
        QString("color: %1;").arg(PAGE_TITLE_FONT_COL.name()));

    StateColor btn_bg_green(
        std::make_pair(AMS_CONTROL_DISABLE_COLOUR, (int)StateColor::Disabled),
        std::make_pair(QColor(27, 136, 68),         (int)StateColor::Pressed),
        std::make_pair(QColor(61, 203, 115),         (int)StateColor::Hovered),
        std::make_pair(AMS_CONTROL_BRAND_COLOUR,     (int)StateColor::Normal));
    StateColor btn_bd_green(
        std::make_pair(AMS_CONTROL_WHITE_COLOUR, (int)StateColor::Disabled),
        std::make_pair(AMS_CONTROL_BRAND_COLOUR, (int)StateColor::Enabled));
    StateColor btn_text_white(
        std::make_pair(QColor("#FFFFFE"), (int)StateColor::Normal));

    m_parts_btn = new Button(m_panel_control_title, _L("Printer Parts"));
    m_parts_btn->SetBackgroundColor(btn_bg_green);
    m_parts_btn->SetBorderColor(btn_bd_green);
    m_parts_btn->SetTextColor(btn_text_white);
    m_parts_btn->setMinimumSize(QSize(0, 26));

    m_options_btn = new Button(m_panel_control_title, _L("Print Options"));
    m_options_btn->SetBackgroundColor(btn_bg_green);
    m_options_btn->SetBorderColor(btn_bd_green);
    m_options_btn->SetTextColor(btn_text_white);
    m_options_btn->setMinimumSize(QSize(0, 26));
    m_options_btn->hide();

    m_safety_btn = new Button(m_panel_control_title, _L("Safety Options"));
    m_safety_btn->SetBackgroundColor(btn_bg_green);
    m_safety_btn->SetBorderColor(btn_bd_green);
    m_safety_btn->SetTextColor(btn_text_white);
    m_safety_btn->setMinimumSize(QSize(0, 26));
    m_safety_btn->hide();

    m_calibration_btn = new Button(m_panel_control_title, _L("Calibration"));
    m_calibration_btn->SetBackgroundColor(btn_bg_green);
    m_calibration_btn->SetBorderColor(btn_bd_green);
    m_calibration_btn->SetTextColor(btn_text_white);
    m_calibration_btn->setMinimumSize(QSize(0, 26));

    title_lay->addSpacing(PAGE_TITLE_LEFT_MARGIN);
    title_lay->addWidget(m_staticText_control, 0, Qt::AlignVCenter);
    title_lay->addStretch(1);
    title_lay->addWidget(m_parts_btn, 0, Qt::AlignVCenter);
    title_lay->addSpacing(10);
    title_lay->addWidget(m_options_btn, 0, Qt::AlignVCenter);
    title_lay->addSpacing(10);
    title_lay->addWidget(m_safety_btn, 0, Qt::AlignVCenter);
    title_lay->addSpacing(10);
    title_lay->addWidget(m_calibration_btn, 0, Qt::AlignVCenter);
    title_lay->addSpacing(10);

    bSizer_right->addWidget(m_panel_control_title);

    // ---- Content: temp+axis + ams + filament ----
    auto *bSizer_control = new QVBoxLayout;
    bSizer_control->setContentsMargins(8, 0, 8, 0);
    bSizer_control->setSpacing(0);

    auto *temp_axis_ctrl_sizer  = create_temp_axis_group(parent);
    auto *m_filament_load_sizer = create_filament_group(parent);

    auto *ams_rack_sizer = new QHBoxLayout;
    ams_rack_sizer->setContentsMargins(0, 0, 0, 0);
    ams_rack_sizer->setSpacing(0);
    ams_rack_sizer->addWidget(create_ams_group(parent));
    // m_panel_nozzle_rack stays nullptr until Phase 4

    m_ams_rack_switch = new SwitchBoard(parent, _L("Filament"), _L("Hotends"),
                                        QSize(126, 26));
    m_ams_rack_switch->updateState("left");
    m_ams_rack_switch->hide();
    // Signal connected in Phase 8

    bSizer_control->addSpacing(8);
    bSizer_control->addLayout(temp_axis_ctrl_sizer);
    bSizer_control->addWidget(m_ams_rack_switch, 0, Qt::AlignHCenter);
    bSizer_control->addSpacing(6);
    bSizer_control->addLayout(ams_rack_sizer);
    bSizer_control->addSpacing(6);
    bSizer_control->addLayout(m_filament_load_sizer);
    bSizer_control->addSpacing(4);

    bSizer_right->addLayout(bSizer_control, 1);

    return bSizer_right;
}

QBoxLayout *StatusBasePanel::create_temp_axis_group(QWidget *parent)
{
    auto *sizer = new QVBoxLayout;
    sizer->setContentsMargins(0, 0, 0, 0);
    sizer->setSpacing(0);

    auto *box = new StaticBox(parent);
    StateColor box_bg(std::make_pair(QColor(255, 255, 255), (int)StateColor::Normal));
    StateColor box_bd(std::make_pair(STATUS_PANEL_BG, (int)StateColor::Normal));
    box->SetBackgroundColor(box_bg);
    box->SetBorderColor(box_bd);
    box->SetCornerRadius(5);
    box->setFixedWidth(586);

    auto *content_sizer = new QHBoxLayout(box);
    content_sizer->setContentsMargins(5, 5, 5, 5);
    content_sizer->setSpacing(0);

    auto *temp_ctrl = create_temp_control(box);
    content_sizer->addLayout(temp_ctrl);

    m_temp_temp_line = new QWidget(box);
    m_temp_temp_line->setFixedWidth(1);
    m_temp_temp_line->setStyleSheet(
        QString("background-color: %1;").arg(STATIC_BOX_LINE_COL.name()));
    content_sizer->addWidget(m_temp_temp_line);

    auto *axis_sizer = create_axis_control(box);
    auto *bed_panel  = create_bed_control(box);
    auto *axis_bed   = new QVBoxLayout;
    axis_bed->setContentsMargins(0, 0, 0, 0);
    axis_bed->setSpacing(0);
    axis_bed->addLayout(axis_sizer);
    axis_bed->addWidget(bed_panel, 0, Qt::AlignHCenter);
    content_sizer->addLayout(axis_bed, 1);

    m_temp_extruder_line = new QWidget(box);
    m_temp_extruder_line->setFixedWidth(1);
    m_temp_extruder_line->setStyleSheet(
        QString("background-color: %1;").arg(STATIC_BOX_LINE_COL.name()));
    content_sizer->addWidget(m_temp_extruder_line);

    auto *extruder_sizer = create_extruder_control(box);
    content_sizer->addLayout(extruder_sizer);

    sizer->addWidget(box);
    return sizer;
}

QBoxLayout *StatusBasePanel::create_temp_control(QWidget *parent)
{
    auto *sizer = new QVBoxLayout;
    sizer->setContentsMargins(0, 0, 0, 0);
    sizer->setSpacing(0);

    StateColor tempinput_text(
        std::make_pair(DISCONNECT_TEXT_COL, (int)StateColor::Disabled),
        std::make_pair(NORMAL_TEXT_COL,     (int)StateColor::Normal));
    StateColor tempinput_border(
        std::make_pair(QColor(255, 255, 255), (int)StateColor::Disabled),
        std::make_pair(BUTTON_HOVER_COL,      (int)StateColor::Focused),
        std::make_pair(BUTTON_HOVER_COL,      (int)StateColor::Hovered),
        std::make_pair(QColor(255, 255, 255), (int)StateColor::Normal));

    m_tempCtrl_nozzle_deputy = new TempInput(parent, 0, TEMP_BLANK_STR,
        TempInputType::TEMP_OF_DEPUTY_NOZZLE_TYPE, TEMP_BLANK_STR,
        "monitor_nozzle_temp", "monitor_nozzle_temp_active");
    m_tempCtrl_nozzle_deputy->SetMinSize(TEMP_CTRL_MIN_SIZE_TWO_ICO);
    m_tempCtrl_nozzle_deputy->AddTemp(0);
    m_tempCtrl_nozzle_deputy->SetMinTemp(20);
    m_tempCtrl_nozzle_deputy->SetMaxTemp(300);
    m_tempCtrl_nozzle_deputy->SetBorderWidth(2);
    m_tempCtrl_nozzle_deputy->SetTextColor(tempinput_text);
    m_tempCtrl_nozzle_deputy->SetBorderColor(tempinput_border);
    sizer->addWidget(m_tempCtrl_nozzle_deputy);
    m_tempCtrl_nozzle_deputy->hide();

    m_tempCtrl_nozzle = new TempInput(parent, 0, TEMP_BLANK_STR,
        TempInputType::TEMP_OF_MAIN_NOZZLE_TYPE, TEMP_BLANK_STR,
        "monitor_nozzle_temp", "monitor_nozzle_temp_active");
    m_tempCtrl_nozzle->SetMinSize(TEMP_CTRL_MIN_SIZE_TWO_ICO);
    m_tempCtrl_nozzle->AddTemp(0);
    m_tempCtrl_nozzle->SetMinTemp(20);
    m_tempCtrl_nozzle->SetMaxTemp(300);
    m_tempCtrl_nozzle->SetBorderWidth(2);
    m_tempCtrl_nozzle->SetTextColor(tempinput_text);
    m_tempCtrl_nozzle->SetBorderColor(tempinput_border);
    sizer->addWidget(m_tempCtrl_nozzle);

    m_line_nozzle = new StaticLine(parent);
    m_line_nozzle->SetLineColour(STATIC_BOX_LINE_COL);
    sizer->addWidget(m_line_nozzle);

    m_tempCtrl_bed = new TempInput(parent, 0, TEMP_BLANK_STR,
        TempInputType::TEMP_OF_NORMAL_TYPE, TEMP_BLANK_STR,
        "monitor_bed_temp", "monitor_bed_temp_active");
    m_tempCtrl_bed->SetMinSize(TEMP_CTRL_MIN_SIZE_ONE_ICO);
    m_tempCtrl_bed->AddTemp(0);
    m_tempCtrl_bed->SetMinTemp(bed_temp_range[0]);
    m_tempCtrl_bed->SetMaxTemp(bed_temp_range[1]);
    m_tempCtrl_bed->SetBorderWidth(2);
    m_tempCtrl_bed->SetTextColor(tempinput_text);
    m_tempCtrl_bed->SetBorderColor(tempinput_border);
    sizer->addWidget(m_tempCtrl_bed);

    auto *line_bed = new StaticLine(parent);
    line_bed->SetLineColour(STATIC_BOX_LINE_COL);
    sizer->addWidget(line_bed);

    m_tempCtrl_chamber = new TempInput(parent, 0, TEMP_BLANK_STR,
        TempInputType::TEMP_OF_NORMAL_TYPE, TEMP_BLANK_STR,
        "monitor_frame_temp", "monitor_frame_temp_active");
    m_tempCtrl_chamber->SetMinSize(TEMP_CTRL_MIN_SIZE_ONE_ICO);
    m_tempCtrl_chamber->AddTemp(0);
    m_tempCtrl_chamber->SetReadOnly(true);
    m_tempCtrl_chamber->SetMinTemp(default_champer_temp_min);
    m_tempCtrl_chamber->SetMaxTemp(default_champer_temp_max);
    m_tempCtrl_chamber->SetBorderWidth(2);
    m_tempCtrl_chamber->SetTextColor(tempinput_text);
    m_tempCtrl_chamber->SetBorderColor(tempinput_border);
    sizer->addWidget(m_tempCtrl_chamber);

    m_misc_ctrl_sizer = create_misc_control(parent);
    sizer->addLayout(m_misc_ctrl_sizer);

    return sizer;
}

QBoxLayout *StatusBasePanel::create_misc_control(QWidget *parent)
{
    auto *sizer = new QVBoxLayout;
    sizer->setContentsMargins(0, 0, 0, 0);
    sizer->setSpacing(0);

    // Horizontal row: speed | vertical divider | lamp
    auto *line_sizer = new QHBoxLayout;
    line_sizer->setContentsMargins(0, 0, 0, 0);
    line_sizer->setSpacing(0);

    StateColor misc_text(
        std::make_pair(DISCONNECT_TEXT_COL, (int)StateColor::Disabled),
        std::make_pair(NORMAL_TEXT_COL,     (int)StateColor::Normal));

    m_switch_speed = new ImageSwitchButton(parent, m_bitmap_speed_active, m_bitmap_speed);
    m_switch_speed->SetLabels(_L("100%"), _L("100%"));
    m_switch_speed->setMinimumSize(MISC_BUTTON_2FAN_SIZE);
    m_switch_speed->setMaximumSize(MISC_BUTTON_2FAN_SIZE);
    m_switch_speed->SetPadding(3);
    m_switch_speed->SetBorderWidth(2);
    m_switch_speed->setFont(Label::Head_13);
    m_switch_speed->SetTextColor(misc_text);
    m_switch_speed->SetValue(false);
    line_sizer->addWidget(m_switch_speed, 1, Qt::AlignVCenter);

    auto *vline = new StaticLine(parent, true);
    vline->SetLineColour(STATIC_BOX_LINE_COL);
    line_sizer->addWidget(vline);

    m_switch_lamp = new ImageSwitchButton(parent, m_bitmap_lamp_on, m_bitmap_lamp_off);
    m_switch_lamp->SetLabels(_L("Lamp"), _L("Lamp"));
    m_switch_lamp->setMinimumSize(MISC_BUTTON_2FAN_SIZE);
    m_switch_lamp->setMaximumSize(MISC_BUTTON_2FAN_SIZE);
    m_switch_lamp->SetPadding(3);
    m_switch_lamp->SetBorderWidth(2);
    m_switch_lamp->setFont(Label::Head_13);
    m_switch_lamp->SetTextColor(misc_text);
    m_switch_lamp->SetValue(false);
    line_sizer->addWidget(m_switch_lamp, 1, Qt::AlignVCenter);

    // Horizontal separator above fan
    auto *hline1 = new StaticLine(parent);
    hline1->SetLineColour(STATIC_BOX_LINE_COL);
    sizer->addWidget(hline1);

    // Fan panel (StaticBox container with FanSwitchButton inside)
    m_fan_panel = new StaticBox(parent);
    m_fan_panel->setMinimumSize(MISC_BUTTON_PANEL_SIZE);
    m_fan_panel->setMaximumSize(MISC_BUTTON_PANEL_SIZE);
    StateColor fan_bg(std::make_pair(QColor(255, 255, 255), (int)StateColor::Normal));
    m_fan_panel->SetBackgroundColor(fan_bg);
    m_fan_panel->SetBorderWidth(0);
    m_fan_panel->SetCornerRadius(0);

    auto *fan_line_sizer = new QHBoxLayout(m_fan_panel);
    fan_line_sizer->setContentsMargins(2, 2, 2, 2);
    fan_line_sizer->setSpacing(0);

    StateColor fan_text(
        std::make_pair(DISCONNECT_TEXT_COL, (int)StateColor::Disabled),
        std::make_pair(NORMAL_FAN_TEXT_COL, (int)StateColor::Normal));

    m_switch_fan = new FanSwitchButton(m_fan_panel, m_bitmap_fan_on, m_bitmap_fan_off);
    m_switch_fan->SetValue(false);
    m_switch_fan->setMinimumSize(MISC_BUTTON_1FAN_SIZE);
    m_switch_fan->setMaximumSize(MISC_BUTTON_1FAN_SIZE);
    m_switch_fan->SetPadding(1);
    m_switch_fan->SetBorderWidth(0);
    m_switch_fan->SetCornerRadius(0);
    m_switch_fan->setFont(Label::Body_10);
    m_switch_fan->UseTextFan();
    m_switch_fan->SetTextColor(fan_text);
    fan_line_sizer->addWidget(m_switch_fan, 1);

    sizer->addWidget(m_fan_panel);

    // Horizontal separator below fan
    auto *hline2 = new StaticLine(parent);
    hline2->SetLineColour(STATIC_BOX_LINE_COL);
    sizer->addWidget(hline2);

    sizer->addLayout(line_sizer);

    return sizer;
}

QBoxLayout *StatusBasePanel::create_axis_control(QWidget *parent)
{
    auto *sizer = new QVBoxLayout;
    sizer->setContentsMargins(0, 0, 0, 0);
    sizer->setSpacing(0);

    StateColor axis_text(
        std::make_pair(DISCONNECT_TEXT_COL, (int)StateColor::Disabled),
        std::make_pair(NORMAL_TEXT_COL,     (int)StateColor::Normal));

    m_bpButton_xy = new AxisCtrlButton(parent, m_bitmap_axis_home);
    m_bpButton_xy->SetTextColor(axis_text);
    m_bpButton_xy->SetMinSize(AXIS_MIN_SIZE);

    sizer->addStretch();
    sizer->addWidget(m_bpButton_xy, 0, Qt::AlignHCenter);
    sizer->addStretch();

    return sizer;
}

QWidget *StatusBasePanel::create_bed_control(QWidget *parent)
{
    auto *panel = new QWidget(parent);
    panel->setStyleSheet("background: white;");

    auto *bSizer_z_ctrl = new QHBoxLayout(panel);
    bSizer_z_ctrl->setContentsMargins(0, 0, 0, 0);
    bSizer_z_ctrl->setSpacing(0);

    StateColor z10_bg(
        std::make_pair(BUTTON_PRESS_COL,   (int)StateColor::Pressed),
        std::make_pair(BUTTON_NORMAL1_COL, (int)StateColor::Normal));
    StateColor z10_bd(
        std::make_pair(BUTTON_HOVER_COL,   (int)StateColor::Hovered),
        std::make_pair(BUTTON_NORMAL1_COL, (int)StateColor::Normal));
    StateColor z1_bg(
        std::make_pair(BUTTON_PRESS_COL,   (int)StateColor::Pressed),
        std::make_pair(BUTTON_NORMAL2_COL, (int)StateColor::Normal));
    StateColor z1_bd(
        std::make_pair(BUTTON_HOVER_COL,   (int)StateColor::Hovered),
        std::make_pair(BUTTON_NORMAL2_COL, (int)StateColor::Normal));
    StateColor z_text(
        std::make_pair(DISCONNECT_TEXT_COL, (int)StateColor::Disabled),
        std::make_pair(NORMAL_TEXT_COL,     (int)StateColor::Normal));

    m_bpButton_z_10 = new Button(panel, "10", "monitor_bed_up", 15);
    m_bpButton_z_10->setFont(Label::Body_12);
    m_bpButton_z_10->SetBorderWidth(0);
    m_bpButton_z_10->SetBackgroundColor(z10_bg);
    m_bpButton_z_10->SetBorderColor(z10_bd);
    m_bpButton_z_10->SetTextColor(z_text);
    m_bpButton_z_10->setMinimumSize(Z_BUTTON_SIZE);

    m_bpButton_z_1 = new Button(panel, " 1", "monitor_bed_up", 15);
    m_bpButton_z_1->setFont(Label::Body_12);
    m_bpButton_z_1->SetBorderWidth(0);
    m_bpButton_z_1->SetBackgroundColor(z1_bg);
    m_bpButton_z_1->SetBorderColor(z1_bd);
    m_bpButton_z_1->SetTextColor(z_text);
    m_bpButton_z_1->setMinimumSize(Z_BUTTON_SIZE);

    m_staticText_z_tip = new QLabel(_L("Bed"), panel);
    m_staticText_z_tip->setFont(Label::Body_12);
    m_staticText_z_tip->setStyleSheet(
        QString("color: %1;").arg(TEXT_LIGHT_FONT_COL.name()));

    m_bpButton_z_down_1 = new Button(panel, " 1", "monitor_bed_down", 15);
    m_bpButton_z_down_1->setFont(Label::Body_12);
    m_bpButton_z_down_1->SetBorderWidth(0);
    m_bpButton_z_down_1->SetBackgroundColor(z1_bg);
    m_bpButton_z_down_1->SetBorderColor(z1_bd);
    m_bpButton_z_down_1->SetTextColor(z_text);
    m_bpButton_z_down_1->setMinimumSize(Z_BUTTON_SIZE);

    m_bpButton_z_down_10 = new Button(panel, "10", "monitor_bed_down", 15);
    m_bpButton_z_down_10->setFont(Label::Body_12);
    m_bpButton_z_down_10->SetBorderWidth(0);
    m_bpButton_z_down_10->SetBackgroundColor(z10_bg);
    m_bpButton_z_down_10->SetBorderColor(z10_bd);
    m_bpButton_z_down_10->SetTextColor(z_text);
    m_bpButton_z_down_10->setMinimumSize(Z_BUTTON_SIZE);

    bSizer_z_ctrl->addWidget(m_bpButton_z_10);
    bSizer_z_ctrl->addSpacing(2);
    bSizer_z_ctrl->addWidget(m_bpButton_z_1);
    bSizer_z_ctrl->addWidget(m_staticText_z_tip, 0, Qt::AlignVCenter);
    bSizer_z_ctrl->addWidget(m_bpButton_z_down_1);
    bSizer_z_ctrl->addSpacing(2);
    bSizer_z_ctrl->addWidget(m_bpButton_z_down_10);

    return panel;
}

QBoxLayout *StatusBasePanel::create_extruder_control(QWidget *parent)
{
    auto *sizer = new QVBoxLayout;
    sizer->setContentsMargins(0, 0, 0, 0);
    sizer->setSpacing(0);

    auto *panel = new QWidget(parent);
    panel->setStyleSheet("background: white;");
    panel->setFixedWidth(143);

    StateColor e_ctrl_bg(
        std::make_pair(BUTTON_PRESS_COL,   (int)StateColor::Pressed),
        std::make_pair(BUTTON_NORMAL1_COL, (int)StateColor::Normal));
    StateColor e_ctrl_bd(
        std::make_pair(BUTTON_HOVER_COL,   (int)StateColor::Hovered),
        std::make_pair(BUTTON_NORMAL1_COL, (int)StateColor::Normal));

    m_nozzle_btn_panel = new SwitchBoard(panel, _L("Left"), _L("Right"),
                                         QSize(126, 26));
    m_nozzle_btn_panel->SetAutoDisableWhenSwitch();

    m_bpButton_e_10 = new Button(panel, "", "monitor_extruder_up", 22);
    m_bpButton_e_10->SetBorderWidth(2);
    m_bpButton_e_10->SetBackgroundColor(e_ctrl_bg);
    m_bpButton_e_10->SetBorderColor(e_ctrl_bd);
    m_bpButton_e_10->setMinimumSize(QSize(40, 40));

    m_extruder_book = new QStackedWidget(panel);
    m_extruder_book->setFixedSize(QSize(45, 112));
    m_extruder_book->insertWidget(0, new QWidget(panel));
    for (int nozzle_num = 1; nozzle_num <= 2; nozzle_num++) {
        auto *extruder_img = new ExtruderImage(m_extruder_book, 0, nozzle_num);
        m_extruder_book->insertWidget(nozzle_num, extruder_img);
        m_extruderImage.push_back(extruder_img);
    }
    m_extruder_book->setCurrentIndex(0);

    m_bpButton_e_down_10 = new Button(panel, "", "monitor_extruder_down", 22);
    m_bpButton_e_down_10->SetBorderWidth(2);
    m_bpButton_e_down_10->SetBackgroundColor(e_ctrl_bg);
    m_bpButton_e_down_10->SetBorderColor(e_ctrl_bd);
    m_bpButton_e_down_10->setMinimumSize(QSize(40, 40));

    m_extruder_switching_status = new ExtruderSwithingStatus(panel);
    m_extruder_switching_status->setStyleSheet(
        QString("color: %1;").arg(TEXT_LIGHT_FONT_COL.name()));

    m_extruder_label = new Label(panel, _L("Extruder"));
    m_extruder_label->setFont(Label::Body_13);
    m_extruder_label->setStyleSheet(
        QString("color: %1;").arg(TEXT_LIGHT_FONT_COL.name()));

    auto *bSizer_e_ctrl = new QVBoxLayout(panel);
    bSizer_e_ctrl->setContentsMargins(0, 0, 0, 0);
    bSizer_e_ctrl->setSpacing(0);
    bSizer_e_ctrl->addSpacing(15);
    bSizer_e_ctrl->addWidget(m_nozzle_btn_panel, 0, Qt::AlignHCenter);
    bSizer_e_ctrl->addSpacing(15);
    bSizer_e_ctrl->addWidget(m_bpButton_e_10, 0, Qt::AlignHCenter);
    bSizer_e_ctrl->addSpacing(7);
    bSizer_e_ctrl->addWidget(m_extruder_book, 0, Qt::AlignHCenter);
    bSizer_e_ctrl->addSpacing(7);
    bSizer_e_ctrl->addWidget(m_bpButton_e_down_10, 0, Qt::AlignHCenter);
    bSizer_e_ctrl->addStretch(1);
    bSizer_e_ctrl->addWidget(m_extruder_switching_status, 0, Qt::AlignHCenter);
    bSizer_e_ctrl->addSpacing(10);
    bSizer_e_ctrl->addWidget(m_extruder_label, 0, Qt::AlignHCenter);

    sizer->addWidget(panel, 1);
    return sizer;
}

StaticBox *StatusBasePanel::create_ams_group(QWidget *parent)
{
    StateColor box_bg(std::make_pair(QColor(255, 255, 255), (int)StateColor::Normal));
    StateColor box_bd(std::make_pair(STATUS_PANEL_BG, (int)StateColor::Normal));

    m_ams_control_box = new StaticBox(parent);
    m_ams_control_box->SetBackgroundColor(box_bg);
    m_ams_control_box->SetBorderColor(box_bd);
    m_ams_control_box->SetCornerRadius(5);
    m_ams_control_box->setMinimumWidth(586);
    m_ams_control_box->setMaximumWidth(586);

    m_ams_control = new AMSControl(m_ams_control_box);

    auto *sizer_box = new QVBoxLayout;
    sizer_box->setContentsMargins(3, 3, 3, 3);
    sizer_box->addWidget(m_ams_control, 0, Qt::AlignHCenter);
    m_ams_control_box->setLayout(sizer_box);

    return m_ams_control_box;
}

QBoxLayout *StatusBasePanel::create_settings_group(QWidget * /*parent*/)
{
    return new QVBoxLayout;
}

QBoxLayout *StatusBasePanel::create_filament_group(QWidget *parent)
{
    auto *sizer = new QVBoxLayout;
    sizer->setContentsMargins(0, 0, 0, 0);
    sizer->setSpacing(0);

    // ---- Scale panel (fold/expand header) ----
    m_scale_panel = new QWidget(parent);
    m_scale_panel->setMinimumSize(586, 40);
    m_scale_panel->setMaximumSize(586, 40);
    m_scale_panel->setStyleSheet("background-color: white;");

    auto *sizer_scale_panel = new QHBoxLayout(m_scale_panel);
    sizer_scale_panel->setContentsMargins(20, 0, 0, 0);
    sizer_scale_panel->setSpacing(0);

    auto *m_title_filament_loading = new Label(m_scale_panel, _L("Filament loading..."));
    m_title_filament_loading->setStyleSheet("background-color: white; color: rgb(27,136,68);");
    m_title_filament_loading->setFont(Label::Body_14);

    m_img_filament_loading = new QLabel(m_scale_panel);
    m_img_filament_loading->setFixedSize(24, 24);
    m_img_filament_loading->setPixmap(
        create_scaled_bitmap("filament_load_fold", nullptr, 24).scaled(24, 24, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    sizer_scale_panel->addWidget(m_title_filament_loading, 0, Qt::AlignVCenter);
    sizer_scale_panel->addWidget(m_img_filament_loading, 0, Qt::AlignVCenter);

    m_scale_panel->hide();

    // Install click filter on both the panel and the label
    auto *click_filter = new PanelClickFilter(m_scale_panel, [this](QMouseEvent &e) {
        expand_filament_loading(e);
    });
    m_scale_panel->installEventFilter(click_filter);
    m_title_filament_loading->installEventFilter(click_filter);

    // ---- Load box ----
    StateColor box_bg(std::make_pair(QColor(255, 255, 255), (int)StateColor::Normal));
    StateColor box_bd(std::make_pair(STATUS_PANEL_BG, (int)StateColor::Normal));

    m_filament_load_box = new StaticBox(parent);
    m_filament_load_box->SetBackgroundColor(box_bg);
    m_filament_load_box->SetBorderColor(box_bd);
    m_filament_load_box->SetCornerRadius(5);
    m_filament_load_box->setMinimumWidth(586);
    m_filament_load_box->setMaximumWidth(586);
    m_filament_load_box->setStyleSheet("background-color: white;");

    auto *sizer_box = new QVBoxLayout;
    sizer_box->setContentsMargins(0, 5, 0, 5);
    sizer_box->setSpacing(0);

    m_filament_step = new FilamentLoad(m_filament_load_box);
    m_filament_step->set_background_color(Qt::white);

    m_filament_load_img = new QLabel(m_filament_load_box);
    m_filament_load_img->setStyleSheet("background-color: white;");

    auto *steps_sizer = new QHBoxLayout;
    steps_sizer->setContentsMargins(0, 0, 0, 0);
    steps_sizer->setSpacing(0);
    steps_sizer->addWidget(m_filament_step, 0, Qt::AlignLeft);
    steps_sizer->addWidget(m_filament_load_img, 0, Qt::AlignTop);
    steps_sizer->addStretch(1);

    // Button colours matching original wx
    StateColor btn_bd_white(
        std::make_pair(QColor(255, 255, 254), (int)StateColor::Disabled),
        std::make_pair(QColor(38, 46, 48),    (int)StateColor::Enabled));
    StateColor btn_text_white(
        std::make_pair(QColor(255, 255, 254), (int)StateColor::Disabled),
        std::make_pair(QColor(38, 46, 48),    (int)StateColor::Enabled));
    StateColor btn_bg_white(
        std::make_pair(AMS_CONTROL_DISABLE_COLOUR,    (int)StateColor::Disabled),
        std::make_pair(AMS_CONTROL_DISABLE_COLOUR,    (int)StateColor::Pressed),
        std::make_pair(QColor(238, 238, 238),          (int)StateColor::Hovered),
        std::make_pair(AMS_CONTROL_WHITE_COLOUR,       (int)StateColor::Normal));

    auto *fila_change_sizer = new QHBoxLayout;
    fila_change_sizer->setContentsMargins(28, 0, 0, 0);
    fila_change_sizer->setSpacing(0);

    m_button_retry = new Button(m_filament_load_box, _L("Retry"));
    m_button_retry->setFont(Label::Body_13);
    m_button_retry->SetBorderColor(btn_bd_white);
    m_button_retry->SetTextColor(btn_text_white);
    m_button_retry->setMinimumSize(QSize(80, 31));
    m_button_retry->SetBackgroundColor(btn_bg_white);
    connect(m_button_retry, &Button::clicked, this, [this]() {
        BOOST_LOG_TRIVIAL(info) << "on_ams_retry";
        if (obj) { reinterpret_cast<Slic3r::MachineObject*>(obj)->command_ams_control("resume"); }
    });

    m_fila_change_abort = new Button(m_filament_load_box, _L("Stop"));
    m_fila_change_abort->setFont(Label::Body_13);
    m_fila_change_abort->SetBorderColor(btn_bd_white);
    m_fila_change_abort->SetTextColor(btn_text_white);
    m_fila_change_abort->setMinimumSize(QSize(80, 31));
    m_fila_change_abort->SetBackgroundColor(btn_bg_white);
    m_fila_change_abort->hide();
    connect(m_fila_change_abort, &Button::clicked, this, [this]() {
        BOOST_LOG_TRIVIAL(info) << "on_ams_abort";
        if (obj) { reinterpret_cast<Slic3r::MachineObject*>(obj)->command_ams_control("abort"); }
    });

    fila_change_sizer->addWidget(m_button_retry);
    fila_change_sizer->addSpacing(14);
    fila_change_sizer->addWidget(m_fila_change_abort);

    sizer_box->addLayout(steps_sizer);
    sizer_box->addLayout(fila_change_sizer);
    m_filament_load_box->setLayout(sizer_box);
    m_filament_load_box->hide();

    sizer->addSpacing(5);
    sizer->addWidget(m_scale_panel, 0, Qt::AlignHCenter);
    sizer->addWidget(m_filament_load_box, 0, Qt::AlignHCenter);
    return sizer;
}

// ---- Misc StatusBasePanel ----

void StatusBasePanel::reset_temp_misc_control()
{
    // Reset temperature inputs to blank state
    m_tempCtrl_nozzle->SetIconNormal();
    m_tempCtrl_nozzle->SetLabel(TEMP_BLANK_STR);
    m_tempCtrl_nozzle->GetTextCtrl()->setText(TEMP_BLANK_STR);

    m_tempCtrl_nozzle_deputy->SetIconNormal();
    m_tempCtrl_nozzle_deputy->SetLabel(TEMP_BLANK_STR);
    m_tempCtrl_nozzle_deputy->GetTextCtrl()->setText(TEMP_BLANK_STR);

    m_tempCtrl_bed->SetIconNormal();
    m_tempCtrl_bed->SetLabel(TEMP_BLANK_STR);
    m_tempCtrl_bed->GetTextCtrl()->setText(TEMP_BLANK_STR);

    m_tempCtrl_chamber->SetIconNormal();
    m_tempCtrl_chamber->SetLabel(TEMP_BLANK_STR);
    m_tempCtrl_chamber->GetTextCtrl()->setText(TEMP_BLANK_STR);

    m_tempCtrl_nozzle->Enable(true);
    m_tempCtrl_nozzle_deputy->Enable(true);
    m_tempCtrl_bed->Enable(true);
    m_tempCtrl_chamber->Enable(true);

    // Reset misc controls
    m_switch_speed->SetLabels(_L("100%"), _L("100%"));
    m_switch_speed->SetValue(false);
    m_switch_lamp->SetLabels(_L("Lamp"), _L("Lamp"));
    m_switch_lamp->SetValue(false);
}

void StatusBasePanel::show_ams_group(bool show)
{
    if (!m_ams_control) return;

    if (m_ams_control->isVisible() != show) {
        m_ams_control->setVisible(show);
        m_ams_control->updateGeometry();
    }

    // If switch is visible and left-side (AMS) is selected, don't hide the box yet
    if (show && m_ams_rack_switch && m_ams_rack_switch->isVisible() && m_ams_rack_switch->switch_left)
        return;

    if (m_ams_control_box->isVisible() != show) {
        m_ams_control_box->setVisible(show);
        if (parentWidget()) parentWidget()->updateGeometry();
    }
}

void StatusBasePanel::show_filament_load_group(bool show)
{
    if (!m_scale_panel || m_scale_panel->isVisible() == show) return;

    m_scale_panel->setVisible(show);
    if (!show) {
        // Reset fold icon when hiding
        m_img_filament_loading->setPixmap(
            create_scaled_bitmap("filament_load_fold", nullptr, 24)
                .scaled(24, 24, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        // Also collapse the load box
        if (m_filament_load_box->isVisible())
            m_filament_load_box->setVisible(false);
    }

    if (obj && m_filament_step) {
        auto *real_obj = reinterpret_cast<Slic3r::MachineObject*>(obj);
        auto  ext_sys  = real_obj->GetExtderSystem();
        auto  cur_ext  = ext_sys ? ext_sys->GetCurrentExtder() : std::optional<DevExtder>{};
        bool  has_fil  = cur_ext.has_value() ? cur_ext->HasFilamentInExt() : false;
        m_filament_step->SetupSteps(obj, has_fil);
    }

    if (parentWidget()) parentWidget()->updateGeometry();
}

void StatusBasePanel::expand_filament_loading(QMouseEvent & /*e*/)
{
    bool tag_show = !m_filament_load_box->isVisible();

    if (tag_show) {
        m_img_filament_loading->setPixmap(
            create_scaled_bitmap("filament_load_expand", nullptr, 24)
                .scaled(24, 24, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        m_img_filament_loading->setPixmap(
            create_scaled_bitmap("filament_load_fold", nullptr, 24)
                .scaled(24, 24, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

    if (obj) {
        auto *real_obj = reinterpret_cast<Slic3r::MachineObject*>(obj);
        static int load_img_size = 215;
        auto *ext_sys  = real_obj->GetExtderSystem();
        int cur_ext_id = (ext_sys && ext_sys->GetTotalExtderCount() > 1)
                         ? ext_sys->GetCurrentExtderId() : 0;
        bool has_nozzle_rack =
            real_obj->GetNozzleSystem() &&
            real_obj->GetNozzleSystem()->GetNozzleRack() &&
            real_obj->GetNozzleSystem()->GetNozzleRack()->IsSupported();
        std::string img_name = DevPrinterConfigUtil::get_filament_load_img(
            real_obj->printer_type, cur_ext_id, has_nozzle_rack);
        if (!img_name.empty()) {
            try {
                m_filament_load_img->setPixmap(
                    create_scaled_bitmap(img_name, nullptr, load_img_size)
                        .scaled(load_img_size, load_img_size, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            } catch (...) {
                BOOST_LOG_TRIVIAL(error) << "Failed to load filament image";
            }
        }
        bool support_abort = real_obj->is_support_fila_change_abort
                          || DevPrinterConfigUtil::support_ams_fila_change_abort(real_obj->printer_type);
        m_fila_change_abort->setVisible(support_abort);
    }

    m_filament_load_box->setVisible(tag_show);
    m_filament_step->setVisible(tag_show);
    if (parentWidget()) parentWidget()->adjustSize();
}

void StatusBasePanel::jump_to_Rack()
{
    auto *real_obj = obj ? reinterpret_cast<Slic3r::MachineObject*>(obj) : nullptr;
    if (real_obj && real_obj->GetNozzleRack() && real_obj->GetNozzleRack()->IsSupported()) {
        if (m_ams_rack_switch) m_ams_rack_switch->updateState("right");
        m_ams_control_box->setVisible(false);
        if (m_panel_nozzle_rack) m_panel_nozzle_rack->setVisible(true);
        if (parentWidget()) parentWidget()->updateGeometry();
    }
}

void StatusBasePanel::on_ams_rack_switch(QEvent & /*event*/)
{
    // switchPos(bool leftSelected) → left=AMS, right=NozzleRack
    // Full signal connection done in Phase 8; body handles state:
    if (!m_ams_rack_switch) return;
    bool left = m_ams_rack_switch->switch_left;

    if (!m_ams_control_box->isVisible() && left) {
        m_ams_control_box->setVisible(true);
        if (m_panel_nozzle_rack) m_panel_nozzle_rack->setVisible(false);
        if (parentWidget()) parentWidget()->updateGeometry();
    } else if (m_panel_nozzle_rack && !m_panel_nozzle_rack->isVisible() && !left) {
        m_ams_control_box->setVisible(false);
        m_panel_nozzle_rack->setVisible(true);
        if (parentWidget()) parentWidget()->updateGeometry();
    }
}

bool StatusBasePanel::can_show_camera_fullscreen() const
{
    return m_media_ctrl != nullptr && isVisible();
}

bool StatusBasePanel::is_camera_fullscreen() const
{
    return m_camera_fullscreen_frame != nullptr;
}

void StatusBasePanel::toggle_camera_fullscreen()
{
    if (is_camera_fullscreen()) {
        close_camera_fullscreen();
    } else {
        show_camera_fullscreen();
    }
}

void StatusBasePanel::show_camera_fullscreen()
{
    // Phase 5 will implement
}

void StatusBasePanel::close_camera_fullscreen()
{
    if (!m_camera_fullscreen_frame) return;
    // Phase 5 will implement full teardown
    m_camera_fullscreen_frame = nullptr;
}

void StatusBasePanel::on_camera_fullscreen(QMouseEvent & /*event*/)
{
    toggle_camera_fullscreen();
}

// =========================================================
// StatusPanel
// =========================================================

StatusPanel::StatusPanel(QWidget *parent, int id, const QPoint &pos,
                         const QSize &size, long style, const QString &name)
    : StatusBasePanel(parent, id, pos, size, style, name)
    , m_score_data(nullptr)
{
    m_score_data            = new ScoreData;
    m_score_data->rating_id = -1;

    init_scaled_buttons();
    // m_buttons filled in Phase 2 when machine control panel widgets exist

    obj = nullptr;

    // Default enable states
    m_project_task_panel->enable_partskip_button(nullptr, false);
    m_project_task_panel->enable_pause_resume_button(false, "resume_disable");
    m_project_task_panel->enable_abort_button(false);

    // Connect print task panel action buttons
    connect(m_project_task_panel->get_pause_resume_button(), &ScalableButton::clicked,
            this, [this]() {
                QEvent e(QEvent::None);
                on_subtask_pause_resume(e);
            });
    connect(m_project_task_panel->get_abort_button(), &ScalableButton::clicked,
            this, [this]() {
                QEvent e(QEvent::None);
                on_subtask_abort(e);
            });
    connect(m_project_task_panel->get_market_scoring_button(), &Button::clicked,
            this, [this]() {
                QEvent e(QEvent::None);
                on_market_scoring(e);
            });
    connect(m_project_task_panel->get_market_retry_buttom(), &Button::clicked,
            this, [this]() {
                QEvent e(QEvent::None);
                on_market_retry(e);
            });
    connect(m_project_task_panel->get_clean_button(), &Button::clicked,
            this, [this]() {
                QEvent e(QEvent::None);
                on_print_error_clean(e);
            });
}

StatusPanel::~StatusPanel()
{
    delete m_score_data;
    delete m_print_error_dlg;
    delete abort_dlg;
    delete con_load_dlg;
    delete ctrl_e_hint_dlg;
    delete sdcard_hint_dlg;
    delete axis_go_home_dlg;
    delete m_partskip_dlg;
}

void StatusPanel::init_scaled_buttons()
{
    m_project_task_panel->init_scaled_buttons();
}

void StatusPanel::set_default()
{
    m_project_task_panel->reset_printing_value();
    m_project_task_panel->error_info_reset();
}

void StatusPanel::show_status(int /*status*/) {}

void StatusPanel::set_hold_count(int &count) { count = COMMAND_TIMEOUT; }

void StatusPanel::rescale_camera_icons() {}

void StatusPanel::on_sys_color_changed() {}

void StatusPanel::msw_rescale()
{
    m_project_task_panel->msw_rescale();
}

void StatusPanel::update(MachineObject *obj)
{
    if (!obj) return;
    auto *mo = reinterpret_cast<Slic3r::MachineObject *>(obj);
    if (!mo->is_info_ready()) {
        if (m_nozzle_btn_panel) m_nozzle_btn_panel->setEnabled(false);
        return;
    }

    update_subtask(obj);

    if (mo->is_in_printing() && !mo->can_resume()) {
        show_printing_status(false, true);
    } else {
        show_printing_status();
    }

    // Non-FDM mode: disable lamp switch
    if (!mo->GetInfo()->IsFdmMode()) { m_switch_lamp->setEnabled(false); }

    update_temp_ctrl(obj);
    update_misc_ctrl(obj);

    update_ams(obj);
    update_cali(obj);

    update_rack(obj);

    // Update extrusion calibration dialog
    if (m_extrusion_cali_dlg) {
        auto *ec = reinterpret_cast<Slic3r::GUI::ExtrusionCalibration *>(m_extrusion_cali_dlg);
        ec->update_machine_obj(obj);
        ec->update();
    }

    // Update calibration dialog
    if (calibration_dlg) {
        auto *cd = reinterpret_cast<Slic3r::GUI::CalibrationDialog *>(calibration_dlg);
        cd->update_machine_obj(obj);
        cd->update_cali(obj);
    }

    std::string current_printer_type = mo->printer_type;
    bool        supports_safety      = DevPrinterConfigUtil::support_safety_options(current_printer_type);

    DevConfig *config = mo->GetConfig();

    if (config->SupportFirstLayerInspect() || config->SupportAIMonitor() ||
        mo->GetPrintOptions()->GetDetectionOption(PrintOptionEnum::Buildplate_Mark_Detection)->is_support_detect ||
        mo->GetPrintOptions()->GetDetectionOption(PrintOptionEnum::Auto_Recovery_Detection)->is_support_detect) {
        m_options_btn->show();
        if (print_options_dlg) {
            auto *pod = reinterpret_cast<Slic3r::GUI::PrintOptionsDialog *>(print_options_dlg);
            pod->update_machine_obj(obj);
            pod->update_options(obj);
        }
    } else {
        m_options_btn->hide();
    }

    if (mo->support_door_open_check()) {
        if (supports_safety) {
            m_safety_btn->show();
            if (safety_options_dlg) {
                auto *sod = reinterpret_cast<Slic3r::GUI::SafetyOptionsDialog *>(safety_options_dlg);
                sod->update_machine_obj(obj);
                sod->update_options(obj);
            }
        } else {
            m_safety_btn->hide();
        }
    } else {
        m_safety_btn->hide();
    }

    m_parts_btn->show();

    if (m_panel_control_title) { m_panel_control_title->updateGeometry(); }

    if (!mo->connection_type().empty()) {
        auto iter_connect_type = m_print_connect_types.find(mo->get_dev_id());
        if (iter_connect_type != m_print_connect_types.end()) {
            if (iter_connect_type->second != mo->connection_type()) {
                if (iter_connect_type->second == "lan" && mo->connection_type() == "cloud") {
                    m_print_connect_types[mo->get_dev_id()] = mo->connection_type();
                }
                if (iter_connect_type->second == "cloud" && mo->connection_type() == "lan") {
                    m_print_connect_types[mo->get_dev_id()] = mo->connection_type();
                }
            }
        }
        m_print_connect_types[mo->get_dev_id()] = mo->connection_type();
    }

    update_error_message();

    update_camera_state(obj);
}

void StatusPanel::show_printing_status(bool ctrl_area, bool temp_area)
{
    if (!ctrl_area) {
        m_bpButton_xy->setEnabled(false);
        m_bpButton_z_10->setEnabled(false);
        m_bpButton_z_1->setEnabled(false);
        m_bpButton_z_down_1->setEnabled(false);
        m_bpButton_z_down_10->setEnabled(false);
        m_bpButton_e_10->setEnabled(false);
        m_bpButton_e_down_10->setEnabled(false);

        m_bpButton_z_10->SetIcon("monitor_bed_up_disable");
        m_bpButton_z_1->SetIcon("monitor_bed_up_disable");
        m_bpButton_z_down_1->SetIcon("monitor_bed_down_disable");
        m_bpButton_z_down_10->SetIcon("monitor_bed_down_disable");
        m_bpButton_e_10->SetIcon("monitor_extruder_up_disable");
        m_bpButton_e_down_10->SetIcon("monitor_extrduer_down_disable");

        m_staticText_z_tip->setStyleSheet(QString("color: %1;").arg(DISCONNECT_TEXT_COL.name()));
        m_extruder_label->setStyleSheet(QString("color: %1;").arg(DISCONNECT_TEXT_COL.name()));
    } else {
        m_bpButton_xy->setEnabled(true);
        m_bpButton_z_10->setEnabled(true);
        m_bpButton_z_1->setEnabled(true);
        m_bpButton_z_down_1->setEnabled(true);
        m_bpButton_z_down_10->setEnabled(true);
        m_bpButton_e_10->setEnabled(true);
        m_bpButton_e_down_10->setEnabled(true);

        m_bpButton_z_10->SetIcon("monitor_bed_up");
        m_bpButton_z_1->SetIcon("monitor_bed_up");
        m_bpButton_z_down_1->SetIcon("monitor_bed_down");
        m_bpButton_z_down_10->SetIcon("monitor_bed_down");
        m_bpButton_e_10->SetIcon("monitor_extruder_up");
        m_bpButton_e_down_10->SetIcon("monitor_extrduer_down");

        m_staticText_z_tip->setStyleSheet(QString("color: %1;").arg(TEXT_LIGHT_FONT_COL.name()));
        m_extruder_label->setStyleSheet(QString("color: %1;").arg(TEXT_LIGHT_FONT_COL.name()));
    }

    if (!temp_area) {
        m_tempCtrl_nozzle->Enable(false);
        m_tempCtrl_nozzle_deputy->Enable(false);
        m_tempCtrl_bed->Enable(false);
        m_tempCtrl_chamber->Enable(false);
        m_switch_speed->setEnabled(false);
        m_switch_speed->SetValue(false);
        m_switch_lamp->setEnabled(false);
        m_switch_fan->setEnabled(false);
    } else {
        m_tempCtrl_nozzle->Enable(true);
        m_tempCtrl_nozzle_deputy->Enable(true);
        m_tempCtrl_bed->Enable(true);
        m_tempCtrl_chamber->Enable(true);
        m_switch_speed->setEnabled(true);
        m_switch_speed->SetValue(true);
        m_switch_lamp->setEnabled(true);
        m_switch_fan->setEnabled(true);
    }
}

void StatusPanel::update_left_time(int /*mc_left_time*/) {}

void StatusPanel::update_basic_print_data(bool def)
{
    if (def) {
        if (!obj) return;
        auto *mo = reinterpret_cast<Slic3r::MachineObject*>(obj);
        if (!mo->slice_info) return;
        QString prediction = QString::fromStdString(get_bbl_time_dhms(mo->slice_info->prediction));
        QString weight     = QString::asprintf("%.2fg", mo->slice_info->weight);
        m_project_task_panel->show_priting_use_info(true, prediction, weight);
    } else {
        m_project_task_panel->show_priting_use_info(false, "0m", "0g");
    }
}

void StatusPanel::update_model_info()
{
    auto get_subtask_fn = [this](BBLModelTask *subtask) {
        QTimer::singleShot(0, this, [this, subtask]() {
            auto *mo = reinterpret_cast<Slic3r::MachineObject*>(obj);
            if (obj && mo->subtask_id_ == subtask->task_id) { mo->set_modeltask(subtask); }
        });
    };

    if (wxGetApp().getAgent() && obj) {
        auto *mo = reinterpret_cast<Slic3r::MachineObject*>(obj);
        BBLSubTask *curr_task = mo->get_subtask();
        if (curr_task) {
            BBLModelTask *curr_model_task = mo->get_modeltask();
            if (!curr_model_task && !request_model_info_flag) {
                curr_model_task          = new BBLModelTask();
                curr_model_task->task_id = curr_task->task_id;
                request_model_info_flag  = true;
                if (!curr_model_task->task_id.empty() && curr_model_task->task_id.compare("0") != 0) {
                    wxGetApp().getAgent()->get_subtask(curr_model_task, get_subtask_fn);
                }
            }
        }
    }
}

void StatusPanel::update_subtask(MachineObject *obj)
{
    if (!obj) return;
    auto *mo = reinterpret_cast<Slic3r::MachineObject *>(obj);

    if (m_current_print_mode != PRINGINT) {
        if (calib_bitmap == nullptr) {
            m_calib_mode = get_obj_calibration_mode(obj, m_calib_method, cali_stage);
            if (m_calib_mode == CalibMode::Calib_None) {
                m_current_print_mode = PRINGINT;
            } else {
                m_current_print_mode = CALIBRATION;
                auto get_bitmap = [](const QString &png_path, int width, int height) -> QPixmap {
                    QPixmap pm(png_path);
                    return pm.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                };
                QString png_path;
                int width  = m_project_task_panel->get_bitmap_thumbnail()->size().width();
                int height = m_project_task_panel->get_bitmap_thumbnail()->size().height();
                if (m_calib_method == CalibrationMethod::CALI_METHOD_AUTO ||
                    m_calib_method == CalibrationMethod::CALI_METHOD_NEW_AUTO) {
                    std::string image_name = mo->get_auto_pa_cali_thumbnail_img_str();
                    if (m_calib_mode == CalibMode::Calib_PA_Line) {
                        if (mo->is_multi_extruders()) {
                            int cur_ext_id = mo->GetExtderSystem()->GetCurrentExtderId();
                            if (cur_ext_id == 0) {
                                image_name += "_right";
                            } else {
                                image_name += "_left";
                            }
                        }
                        png_path = QString::fromStdString(Slic3r::resources_dir()) + "/images/" +
                                   QString::fromStdString(image_name) + ".png";
                    } else if (m_calib_mode == CalibMode::Calib_Flow_Rate) {
                        png_path = QString::fromStdString(Slic3r::resources_dir()) +
                                   "/images/flow_rate_calibration_auto.png";
                    }
                } else if (m_calib_method == CalibrationMethod::CALI_METHOD_MANUAL) {
                    if (m_calib_mode == CalibMode::Calib_PA_Line) {
                        if (cali_stage == 0) {
                            png_path = QString::fromStdString(Slic3r::resources_dir()) +
                                       "/images/fd_calibration_manual.png";
                        } else if (cali_stage == 1) {
                            png_path = QString::fromStdString(Slic3r::resources_dir()) +
                                       "/images/fd_pattern_manual_device.png";
                        }
                    }
                }
                if (!png_path.isEmpty()) {
                    calib_bitmap  = new QPixmap;
                    *calib_bitmap = get_bitmap(png_path, width, height);
                }
            }
        }
        if (calib_bitmap != nullptr) m_project_task_panel->set_thumbnail_img(*calib_bitmap, "");
    }

    m_project_task_panel->show_layers_num(mo->is_support_layer_num);

    update_model_info();
    update_partskip_button(obj);
    update_printer_parts_options(obj);

    if (mo->is_system_printing() || mo->is_in_calibration()) {
        reset_printing_values();
    } else if (mo->is_in_printing() || mo->print_status == "FINISH") {
        m_project_task_panel->update_subtask_name(from_u8(mo->subtask_name));

        if (mo->get_modeltask() && mo->get_modeltask()->design_id > 0) {
            m_project_task_panel->show_profile_info(true,
                QString::fromUtf8(mo->get_modeltask()->profile_name.c_str()));
        } else {
            m_project_task_panel->show_profile_info(false);
        }

        // update thumbnail
        if (mo->is_sdcard_printing()) {
            update_basic_print_data(false);
            update_sdcard_subtask(obj);
        } else {
            update_basic_print_data(true);
            update_cloud_subtask(obj);
        }

        update_partskip_subtask(obj);

        if (mo->is_in_prepare() || mo->print_status == "SLICING") {
            m_project_task_panel->market_scoring_show(false);
            m_project_task_panel->get_request_failed_panel()->hide();
            m_project_task_panel->enable_partskip_button(nullptr, false);
            m_project_task_panel->enable_abort_button(false);
            m_project_task_panel->enable_pause_resume_button(false, "pause_disable");
            QString prepare_text;
            bool    show_percent = true;

            if (mo->is_in_prepare()) {
                prepare_text = _L("Downloading...");
            } else if (mo->print_status == "SLICING") {
                if (mo->queue_number <= 0) {
                    prepare_text = _L("Cloud Slicing...");
                } else {
                    prepare_text = _L("In Cloud Slicing Queue, there are %1 tasks ahead.")
                                       .arg(mo->queue_number);
                    show_percent = false;
                }
            } else {
                prepare_text = _L("Downloading...");
            }

            if (mo->gcode_file_prepare_percent >= 0 && mo->gcode_file_prepare_percent <= 100 && show_percent)
                prepare_text += QString("(%1%)").arg(mo->gcode_file_prepare_percent);

            m_project_task_panel->update_stage_value_with_machine(
                mo->get_curr_stage().isEmpty() ? prepare_text : mo->get_curr_stage(), 0, obj);
            m_project_task_panel->update_progress_percent(NA_STR, QString());
            m_project_task_panel->update_left_time(NA_STR);
            m_project_task_panel->update_layers_num(true, QString(_L("Layer: %1")).arg(NA_STR));
            m_project_task_panel->update_subtask_name(from_u8(mo->subtask_name));

            if (mo->get_modeltask() && mo->get_modeltask()->design_id > 0) {
                m_project_task_panel->show_profile_info(true,
                    QString::fromUtf8(mo->get_modeltask()->profile_name.c_str()));
            } else {
                m_project_task_panel->show_profile_info(false);
            }
            update_basic_print_data(false);
        } else {
            if (mo->can_resume()) {
                m_project_task_panel->enable_pause_resume_button(true, "resume");
            } else {
                m_project_task_panel->enable_pause_resume_button(true, "pause");
            }

            m_project_task_panel->enable_partskip_button(obj, true);
            m_project_task_panel->update_left_time(mo->mc_left_time);

            if (mo->subtask_) {
                m_project_task_panel->update_stage_value_with_machine(
                    mo->get_curr_stage(), mo->subtask_->task_progress, obj);
                m_project_task_panel->update_progress_percent(
                    QString("%1").arg(mo->subtask_->task_progress), "%");
                m_project_task_panel->update_layers_num(true,
                    QString(_L("Layer: %1/%2")).arg(mo->curr_layer).arg(mo->total_layers));
            } else {
                m_project_task_panel->update_stage_value_with_machine(mo->get_curr_stage(), 0, obj);
                m_project_task_panel->update_progress_percent(NA_STR, QString());
                m_project_task_panel->update_layers_num(true, QString(_L("Layer: %1")).arg(NA_STR));
            }

            if (mo->is_printing_finished()) {
                mo->update_model_task();
                m_project_task_panel->enable_abort_button(false);
                m_project_task_panel->enable_partskip_button(nullptr, false);
                m_project_task_panel->enable_pause_resume_button(false, "resume_disable");
                // is makeworld subtask
                if (wxGetApp().has_model_mall() && mo->is_makeworld_subtask()) {
                    // has model mall rating result
                    if (mo->rating_info && mo->rating_info->request_successful) {
                        m_project_task_panel->get_request_failed_panel()->hide();
                        BOOST_LOG_TRIVIAL(info) << "model mall result request successful";
                        if (!m_project_task_panel->get_star_count_dirty()) {
                            if (mo->rating_info->start_count > 0) {
                                m_project_task_panel->set_star_count(mo->rating_info->start_count);
                                m_project_task_panel->set_star_count_dirty(true);
                                BOOST_LOG_TRIVIAL(info) << "Initialize scores";
                                m_project_task_panel->get_market_scoring_button()->setEnabled(true);
                                m_project_task_panel->set_has_reted_text(true);
                            } else {
                                m_project_task_panel->set_star_count(0);
                                m_project_task_panel->set_star_count_dirty(false);
                                m_project_task_panel->get_market_scoring_button()->setEnabled(false);
                                m_project_task_panel->set_has_reted_text(false);
                            }
                        }
                        update_market_scoring(true);
                    } else if (mo->rating_info && !mo->rating_info->request_successful) {
                        BOOST_LOG_TRIVIAL(info) << "model mall result request failed";
                        if (403 != mo->rating_info->http_code) {
                            BOOST_LOG_TRIVIAL(info) << "Request need retry";
                            m_project_task_panel->get_market_retry_buttom()->setEnabled(
                                !mo->get_model_mall_result_need_retry);
                            m_project_task_panel->get_request_failed_panel()->show();
                        } else {
                            BOOST_LOG_TRIVIAL(info) << "Request rejected";
                        }
                    }
                } else {
                    update_market_scoring(false);
                }
            } else { // model printing is not finished, hide scoring page
                m_project_task_panel->enable_abort_button(true);
                m_project_task_panel->market_scoring_show(false);
                m_project_task_panel->get_request_failed_panel()->hide();
            }
        }
    } else {
        reset_printing_values();
    }

    auto job_state = mo->GetStatus()->GetJobState();
    m_project_task_panel->update_pausing_state(
        job_state.has_value() && *job_state == DevJobState::JobStatePausing);
    m_project_task_panel->update_stopping_state(
        job_state.has_value() && *job_state == DevJobState::JobStateStoppping);
}

void StatusPanel::update_partskip_subtask(MachineObject *obj)
{
    if (!obj) return;
    auto *mo = reinterpret_cast<Slic3r::MachineObject *>(obj);
    if (!mo->subtask_) return;

    auto partskip_button = m_project_task_panel->get_partskip_button();
    if (partskip_button) {
        int part_cnt = 0;
        if (m_project_task_panel->get_part_skipped_dirty() > 0) {
            m_project_task_panel->set_part_skipped_dirty(m_project_task_panel->get_part_skipped_dirty() - 1);
            part_cnt = m_project_task_panel->get_part_skipped_count();
            BOOST_LOG_TRIVIAL(info) << "part skip: stop recv printer dirty data.";
        } else {
            part_cnt = (int)mo->m_partskip_ids.size();
            BOOST_LOG_TRIVIAL(info) << "part skip: recv printer normal data.";
        }
        if (part_cnt > 0)
            partskip_button->setText(QString(_L("(%1)")).arg(part_cnt));
        else
            partskip_button->setText("");
    }

    if (m_partskip_dlg) {
        auto *pdlg = reinterpret_cast<Slic3r::GUI::PartSkipDialog *>(m_partskip_dlg);
        if (pdlg->isVisible()) { pdlg->UpdatePartsStateFromPrinter(obj); }
    }
}

void StatusPanel::update_cloud_subtask(MachineObject *obj)
{
    if (!obj) return;
    auto *mo = reinterpret_cast<Slic3r::MachineObject *>(obj);
    if (!mo->subtask_) return;

    if (is_task_changed(obj)) {
        mo->set_modeltask(nullptr);
        mo->free_slice_info();
        reset_printing_values();
        BOOST_LOG_TRIVIAL(info) << "monitor: change to sub task id = " << mo->subtask_->task_id;
        if (web_request && !web_request->isFinished()) {
            BOOST_LOG_TRIVIAL(info) << "web_request: cancelled";
            web_request->abort();
            web_request = nullptr;
        }
        m_start_loading_thumbnail = true;
    }

    if (m_start_loading_thumbnail) {
        update_calib_bitmap();
        if (mo->slice_info) {
            m_request_url = QString::fromStdString(mo->slice_info->thumbnail_url);
            if (!m_request_url.isEmpty()) {
                auto it = img_list.find(m_request_url);
                if (it != img_list.end()) {
                    if (m_current_print_mode != PrintingTaskType::CALIBRATION ||
                        (m_calib_mode == CalibMode::Calib_Flow_Rate &&
                         m_calib_method == CalibrationMethod::CALI_METHOD_MANUAL)) {
                        QImage &img = it->second;
                        QPixmap pm  = QPixmap::fromImage(img.scaled(
                            m_project_task_panel->get_bitmap_thumbnail()->size().width(),
                            m_project_task_panel->get_bitmap_thumbnail()->size().height(),
                            Qt::KeepAspectRatio, Qt::SmoothTransformation));
                        m_project_task_panel->set_thumbnail_img(pm, "");
                        m_project_task_panel->set_brightness_value(get_brightness_value(img));
                    }
                    if (this->obj) {
                        m_project_task_panel->set_plate_index(mo->m_plate_index);
                    } else {
                        m_project_task_panel->set_plate_index(-1);
                    }
                    task_thumbnail_state = ThumbnailState::TASK_THUMBNAIL;
                    BOOST_LOG_TRIVIAL(trace) << "web_request: use cache image";
                } else {
                    if (!m_net_manager) {
                        m_net_manager = new QNetworkAccessManager(this);
                        connect(m_net_manager, &QNetworkAccessManager::finished,
                                this, &StatusPanel::on_webrequest_state);
                    }
                    web_request = m_net_manager->get(QNetworkRequest(QUrl(m_request_url)));
                    BOOST_LOG_TRIVIAL(trace) << "monitor: start request thumbnail, url = "
                                             << m_request_url.toStdString();
                    m_start_loading_thumbnail = false;
                }
            }
        }
    }
}

void StatusPanel::update_sdcard_subtask(MachineObject *obj)
{
    if (!obj) return;

    if (!m_load_sdcard_thumbnail) {
        update_calib_bitmap();
        if (m_current_print_mode != PrintingTaskType::CALIBRATION) {
            m_project_task_panel->set_thumbnail_img(m_thumbnail_sdcard.bmp(), m_thumbnail_sdcard.name());
        }
        task_thumbnail_state    = ThumbnailState::SDCARD_THUMBNAIL;
        m_load_sdcard_thumbnail = true;
    }
}

void StatusPanel::update_temp_ctrl(MachineObject *obj)
{
    if (!obj) return;
    auto *mo = reinterpret_cast<Slic3r::MachineObject *>(obj);

    DevBed *bed             = mo->GetBed();
    int     bed_cur_temp    = (int)bed->GetBedTemp();
    int     bed_target_temp = (int)bed->GetBedTempTarget();
    m_tempCtrl_bed->SetCurrTemp(bed_cur_temp);

    auto limit = mo->get_bed_temperature_limit();
    if (mo->bed_temp_range.size() > 1) { limit = mo->bed_temp_range[1]; }
    m_tempCtrl_bed->SetMaxTemp(limit);

    if (mo->nozzle_temp_range.size() >= 2) {
        m_tempCtrl_nozzle->SetMinTemp(mo->nozzle_temp_range[0]);
        m_tempCtrl_nozzle->SetMaxTemp(mo->nozzle_temp_range[1]);
        m_tempCtrl_nozzle_deputy->SetMinTemp(mo->nozzle_temp_range[0]);
        m_tempCtrl_nozzle_deputy->SetMaxTemp(mo->nozzle_temp_range[1]);
    }

    // update temperature if not input temp target
    if (m_temp_bed_timeout > 0) {
        m_temp_bed_timeout--;
    } else {
        if (!bed_temp_input) { m_tempCtrl_bed->SetTagTemp(bed_target_temp); }
    }

    if ((bed_target_temp - bed_cur_temp) >= TEMP_THRESHOLD_VAL) {
        m_tempCtrl_bed->SetIconActive();
    } else {
        m_tempCtrl_bed->SetIconNormal();
    }

    bool to_update_layout = false;
    int  nozzle_num       = mo->GetExtderSystem()->GetTotalExtderCount();
    if (nozzle_num == 1) {
        m_tempCtrl_nozzle->SetCurrTemp(mo->GetExtderSystem()->GetNozzleTempCurrent(MAIN_EXTRUDER_ID));
        m_tempCtrl_nozzle->SetCurrType(TEMP_OF_NORMAL_TYPE);
        m_tempCtrl_nozzle_deputy->SetCurrType(TEMP_OF_NORMAL_TYPE);
        m_tempCtrl_nozzle_deputy->SetLabel(TEMP_BLANK_STR);
        m_tempCtrl_nozzle_deputy->hide();

        if (m_tempCtrl_nozzle->minimumSize() != TEMP_CTRL_MIN_SIZE_ONE_ICO) {
            to_update_layout = true;
            m_tempCtrl_nozzle->SetMinSize(TEMP_CTRL_MIN_SIZE_ONE_ICO);
        }
    } else if (nozzle_num == 2) {
        m_tempCtrl_nozzle->SetCurrType(TEMP_OF_MAIN_NOZZLE_TYPE);
        m_tempCtrl_nozzle->SetCurrTemp(mo->GetExtderSystem()->GetNozzleTempCurrent(MAIN_EXTRUDER_ID));
        m_tempCtrl_nozzle->show();

        m_tempCtrl_nozzle_deputy->SetCurrType(TEMP_OF_DEPUTY_NOZZLE_TYPE);
        m_tempCtrl_nozzle_deputy->SetCurrTemp(mo->GetExtderSystem()->GetNozzleTempCurrent(DEPUTY_EXTRUDER_ID));
        m_tempCtrl_nozzle_deputy->show();

        if (m_tempCtrl_nozzle->minimumSize() != TEMP_CTRL_MIN_SIZE_TWO_ICO) {
            to_update_layout = true;
            m_tempCtrl_nozzle->SetMinSize(TEMP_CTRL_MIN_SIZE_TWO_ICO);
        }
    }

    if (m_temp_nozzle_timeout > 0) {
        m_temp_nozzle_timeout--;
    } else {
        if (!nozzle_temp_input) {
            auto main_extder = mo->GetExtderSystem()->GetExtderById(MAIN_EXTRUDER_ID);
            if (main_extder) {
                m_tempCtrl_nozzle->SetTagTemp(main_extder->GetTargetTemp());
                m_tempCtrl_nozzle->SetCurrTemp((int)main_extder->GetCurrentTemp());
                if (main_extder->GetTargetTemp() - main_extder->GetCurrentTemp() > TEMP_THRESHOLD_VAL) {
                    m_tempCtrl_nozzle->SetIconActive();
                } else {
                    m_tempCtrl_nozzle->SetIconNormal();
                }
            }
        }
    }

    if (m_temp_nozzle_deputy_timeout > 0) {
        m_temp_nozzle_deputy_timeout--;
    } else {
        if (!nozzle_temp_input && nozzle_num >= 2) {
            auto deputy_extder = mo->GetExtderSystem()->GetExtderById(DEPUTY_EXTRUDER_ID);
            if (deputy_extder) {
                m_tempCtrl_nozzle_deputy->SetTagTemp(deputy_extder->GetTargetTemp());
                m_tempCtrl_nozzle_deputy->SetCurrTemp((int)deputy_extder->GetCurrentTemp());
                if (deputy_extder->GetTargetTemp() - deputy_extder->GetCurrentTemp() > TEMP_THRESHOLD_VAL) {
                    m_tempCtrl_nozzle_deputy->SetIconActive();
                } else {
                    m_tempCtrl_nozzle_deputy->SetIconNormal();
                }
            }
        }
    }

    // support current temp for chamber
    const auto &chamber = mo->GetChamber();
    if (chamber->SupportChamberTempDisplay()) {
        m_tempCtrl_chamber->SetCurrTemp((int)chamber->GetChamberTemp());
    } else {
        m_tempCtrl_chamber->SetCurrTemp(TEMP_BLANK_STR);
    }

    // support edit chamber temp
    if (chamber->SupportChamberEdit()) {
        m_tempCtrl_chamber->SetReadOnly(false);
        m_tempCtrl_chamber->setEnabled(true);
        m_tempCtrl_chamber->SetMinTemp(chamber->GetChamberTempEditMin());
        m_tempCtrl_chamber->SetMaxTemp(chamber->GetChamberTempEditMax());
        m_tempCtrl_chamber->AddTemp(0);
        m_tempCtrl_chamber->GetTextCtrl()->setCursor(Qt::IBeamCursor);

        if (m_temp_chamber_timeout > 0) {
            m_temp_chamber_timeout--;
        } else {
            if (!cham_temp_input) { m_tempCtrl_chamber->SetTagTemp((int)chamber->GetChamberTempTarget()); }
        }
    } else {
        m_tempCtrl_chamber->SetReadOnly(true);
        m_tempCtrl_chamber->SetTagTemp(TEMP_BLANK_STR);
        m_tempCtrl_chamber->GetTextCtrl()->setCursor(Qt::ArrowCursor);
    }

    if ((chamber->GetChamberTempTarget() - chamber->GetChamberTemp()) >= TEMP_THRESHOLD_VAL) {
        m_tempCtrl_chamber->SetIconActive();
    } else {
        m_tempCtrl_chamber->SetIconNormal();
    }

    if (to_update_layout) { m_tempCtrl_nozzle->updateGeometry(); }
}

void StatusPanel::update_misc_ctrl(MachineObject *obj)
{
    auto get_extder_shown_state = [](bool ext_has_filament) -> ExtruderState {
        return ext_has_filament ? ExtruderState::FILLED_LOAD : ExtruderState::EMPTY_LOAD;
    };

    if (!obj) return;
    auto *mo = reinterpret_cast<Slic3r::MachineObject *>(obj);

    /* extder */
    auto extder_system = mo->GetExtderSystem();
    m_nozzle_num       = extder_system->GetTotalExtderCount();
    int select_index   = m_nozzle_num - 1;

    if (m_nozzle_num >= 2) {
        m_extruder_book->setCurrentIndex(m_nozzle_num);

        /* style */
        m_nozzle_btn_panel->SetLabels(
            _L(DevPrinterConfigUtil::get_toolhead_display_name(mo->printer_type,
                DEPUTY_EXTRUDER_ID, ToolHeadComponent::Extruder, ToolHeadNameCase::TitleCase, true)),
            _L(DevPrinterConfigUtil::get_toolhead_display_name(mo->printer_type,
                MAIN_EXTRUDER_ID, ToolHeadComponent::Extruder, ToolHeadNameCase::TitleCase, true)));
        m_nozzle_btn_panel->show();
        m_extruderImage[select_index]->setExtruderCount(m_nozzle_num);

        if (mo->GetExtderSystem()->GetTotalExtderSize() > 1) {
            m_extruderImage[select_index]->update(
                get_extder_shown_state(mo->GetExtderSystem()->HasFilamentInExt(0)),
                get_extder_shown_state(mo->GetExtderSystem()->HasFilamentInExt(1)));
        }

        /* current */
        if (mo->GetExtderSystem()->GetCurrentExtderId() == 0xf) {
            m_extruderImage[select_index]->setExtruderUsed("");
            m_nozzle_btn_panel->updateState("");
        } else if (mo->GetExtderSystem()->GetCurrentExtderId() == MAIN_EXTRUDER_ID) {
            m_extruderImage[select_index]->setExtruderUsed("right");
            m_nozzle_btn_panel->updateState("right");
        } else if (mo->GetExtderSystem()->GetCurrentExtderId() == DEPUTY_EXTRUDER_ID) {
            m_extruderImage[select_index]->setExtruderUsed("left");
            m_nozzle_btn_panel->updateState("left");
        }

        m_nozzle_btn_panel->SetClientData(mo);

        /* enable status */
        if ((mo->is_in_printing() && !mo->is_in_printing_pause()) ||
            mo->ams_status_main == AMS_STATUS_MAIN_FILAMENT_CHANGE ||
            mo->targ_nozzle_id_from_pc != INVALID_EXTRUDER_ID) {
            m_nozzle_btn_panel->Disable();
        } else {
            m_nozzle_btn_panel->Enable();
        }
    } else {
        m_nozzle_btn_panel->hide();
        m_extruder_book->setCurrentIndex(m_nozzle_num);
        m_extruderImage[select_index]->setExtruderCount(m_nozzle_num);

        if (extder_system->GetTotalExtderSize() > 0) {
            ExtruderState shown_state = get_extder_shown_state(extder_system->HasFilamentInExt(0));
            m_extruderImage[select_index]->update(shown_state);
        }
    }

    /* switch extder */
    m_extruder_switching_status->updateBy(obj);
    m_extruder_label->setVisible(!m_extruder_switching_status->has_content_shown());

    /* other */
    if (mo->GetAxis()->IsArchCoreXY()) {
        m_staticText_z_tip->setText(_L("Bed"));
    } else {
        m_staticText_z_tip->setText("Z");
    }

    // update extruder icon
    update_extruder_status(obj);

    if (mo->GetInfo()->IsFdmMode()) {
        if (!m_fan_panel->isVisible()) m_fan_panel->show();
        if (m_fan_control_popup) { m_fan_control_popup->update_fan_data(obj); }
    } else {
        if (m_fan_panel->isVisible()) { m_fan_panel->hide(); }
        if (m_fan_control_popup && m_fan_control_popup->isVisible()) m_fan_control_popup->hide();
    }

    mo->is_series_o() ? m_switch_fan->UseTextAirCondition() : m_switch_fan->UseTextFan();

    /* lamp */
    bool light_on = mo->GetLamp()->IsChamberLightOn();
    if (m_switch_lamp_timeout > 0)
        m_switch_lamp_timeout--;
    else {
        m_switch_lamp->SetValue(light_on);
    }

    if (speed_lvl_timeout > 0)
        speed_lvl_timeout--;
    else {
        this->speed_lvl  = (int)mo->GetPrintingSpeedLevel();
        QString text_speed = QString("%1%").arg(mo->printing_speed_mag);
        m_switch_speed->SetLabels(text_speed, text_speed);
    }
}

void StatusPanel::update_ams(MachineObject *obj)
{
    auto *mo = reinterpret_cast<Slic3r::MachineObject *>(obj);

    // update obj in sub dlg
    if (m_ams_setting_dlg && reinterpret_cast<Slic3r::GUI::AMSSetting *>(m_ams_setting_dlg)->isVisible()) {
        reinterpret_cast<Slic3r::GUI::AMSSetting *>(m_ams_setting_dlg)->UpdateByObj(obj);
    }
    if (m_filament_setting_dlg) {
        reinterpret_cast<Slic3r::GUI::AMSMaterialsSetting *>(m_filament_setting_dlg)->obj = obj;
        if (reinterpret_cast<Slic3r::GUI::AMSMaterialsSetting *>(m_filament_setting_dlg)->isVisible()) {
            reinterpret_cast<Slic3r::GUI::AMSMaterialsSetting *>(m_filament_setting_dlg)->TryRefreshPAProfiles();
        }
    }

    if (obj && mo->GetCalib()->IsVersionExpired() && mo->is_security_control_ready()) {
        mo->GetCalib()->SyncCalibVersion();

        PACalibExtruderInfo cali_info;
        cali_info.nozzle_diameter        = mo->GetExtderSystem()->GetNozzleDiameter(0);
        cali_info.use_extruder_id        = false;
        cali_info.use_nozzle_volume_type = false;
        CalibUtils::emit_get_PA_calib_infos(cali_info);

        BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << " calibration: sync calib version for device "
                                << BBLCrossTalk::Crosstalk_DevName(mo->get_dev_name());
    } else if (obj) {
        BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << " calibration: not sync calib version, IsVersionExpired="
                                << mo->GetCalib()->IsVersionExpired()
                                << " is_security_control_ready=" << mo->is_security_control_ready();
    }

    if (obj && mo->is_security_control_ready()) { mo->check_ams_filament_valid(); }

    DevAmsType ams_mode = DevAmsType::AMS;
    if ((mo->is_enable_np || mo->is_enable_ams_np) && mo->GetFilaSystem()->GetAmsList().size() > 0) {
        ams_mode = mo->GetFilaSystem()->GetAmsList().begin()->second->GetAmsType();
    } else if (mo->get_printer_ams_type() == "f1") {
        ams_mode = DevAmsType::AMS_LITE; // STUDIO-14066
    }

    if (!obj || !mo->is_connected()) {
        last_tray_exist_bits  = -1;
        last_ams_exist_bits   = -1;
        last_tray_is_bbl_bits = -1;
        last_read_done_bits   = -1;
        last_reading_bits     = -1;
        last_ams_version      = -1;
        BOOST_LOG_TRIVIAL(trace) << "machine object" << BBLCrossTalk::Crosstalk_DevName(mo->get_dev_name())
                                 << " was disconnected, set show_ams_group is false";

        m_ams_control->SetAmsModel(DevAmsType::EXT_SPOOL, ams_mode);
        show_ams_group(false);
        show_filament_load_group(false);
        m_ams_control->show_auto_refill(false);
    } else {
        m_ams_control->SetAmsModel(ams_mode, ams_mode);
        m_filament_step->SetAmsModel(ams_mode, ams_mode);
        show_ams_group(true);
        // show_filament_load_group(true);

        if (mo->GetFilaSystem()->GetAmsList().empty() || mo->ams_exist_bits == 0) {
            m_ams_control->show_auto_refill(false);
        } else {
            m_ams_control->show_auto_refill(true);
        }
    }

    if (m_filament_setting_dlg)
        reinterpret_cast<Slic3r::GUI::AMSMaterialsSetting *>(m_filament_setting_dlg)->update();

    std::vector<AMSinfo> ams_info;
    const auto          &ams_list = mo->GetFilaSystem()->GetAmsList();
    for (auto ams = ams_list.begin(); ams != ams_list.end(); ams++) {
        AMSinfo info;
        info.ams_id = ams->first;
        if (ams->second->IsExist() &&
            info.parse_ams_info(obj, ams->second, mo->GetFilaSystem()->IsDetectRemainEnabled(), mo->is_support_ams_humidity)) {
            ams_info.push_back(info);
        }
    }

    std::vector<AMSinfo> ext_info;
    ext_info.clear();
    for (auto slot : mo->vt_slot) {
        AMSinfo info;
        info.parse_ext_info(obj, slot);
        if (ams_mode == DevAmsType::AMS_LITE) info.ext_type = AMSModelOriginType::LITE_EXT;
        ext_info.push_back(info);
    }

    // Qt stub: UpdateAms only takes series_name (full params not yet ported)
    m_ams_control->UpdateAms(mo->get_printer_series_str());
    m_ams_control->UpdateAmsDryControl(obj);

    last_tray_exist_bits  = mo->tray_exist_bits;
    last_ams_exist_bits   = mo->ams_exist_bits;
    last_tray_is_bbl_bits = mo->tray_is_bbl_bits;
    last_read_done_bits   = mo->tray_read_done_bits;
    last_reading_bits     = mo->tray_reading_bits;
    last_ams_version      = mo->ams_version;

    /*right*/
    if (auto ext = mo->GetExtderSystem()->GetExtderById(MAIN_EXTRUDER_ID); ext.has_value()) {
        if (ext->HasFilamentInExt()) {
            m_ams_control->SetAmsStep(ext->GetSlotNow().ams_id, ext->GetSlotNow().slot_id, MAIN_EXTRUDER_ID,
                                      AMSPassRoadType::AMS_ROAD_TYPE_LOAD, AMSPassRoadSTEP::AMS_ROAD_STEP_COMBO_LOAD_STEP2);
        } else {
            m_ams_control->SetAmsStep(ext->GetSlotNow().ams_id, ext->GetSlotNow().slot_id, MAIN_EXTRUDER_ID,
                                      AMSPassRoadType::AMS_ROAD_TYPE_UNLOAD, AMSPassRoadSTEP::AMS_ROAD_STEP_NONE);
        }
        m_ams_control->SetExtruder(ext->HasFilamentInExt(), MAIN_EXTRUDER_ID, ext->GetSlotNow().ams_id, ext->GetSlotNow().slot_id);
    }

    /*left*/
    if (auto ext = mo->GetExtderSystem()->GetExtderById(DEPUTY_EXTRUDER_ID); ext.has_value()) {
        if (ext->HasFilamentInExt()) {
            m_ams_control->SetAmsStep(ext->GetSlotNow().ams_id, ext->GetSlotNow().slot_id, DEPUTY_EXTRUDER_ID,
                                      AMSPassRoadType::AMS_ROAD_TYPE_LOAD, AMSPassRoadSTEP::AMS_ROAD_STEP_COMBO_LOAD_STEP2);
        } else {
            m_ams_control->SetAmsStep(ext->GetSlotNow().ams_id, ext->GetSlotNow().slot_id, DEPUTY_EXTRUDER_ID,
                                      AMSPassRoadType::AMS_ROAD_TYPE_UNLOAD, AMSPassRoadSTEP::AMS_ROAD_STEP_NONE);
        }
        m_ams_control->SetExtruder(ext->HasFilamentInExt(), DEPUTY_EXTRUDER_ID, ext->GetSlotNow().ams_id, ext->GetSlotNow().slot_id);
    }

    update_filament_loading_panel(obj);

    const auto &amslist = mo->GetFilaSystem()->GetAmsList();
    for (auto ams_it = amslist.begin(); ams_it != amslist.end(); ams_it++) {
        std::string ams_id = ams_it->first;
        try {
            int ams_id_int = atoi(ams_id.c_str());
            for (auto tray_it = ams_it->second->GetTrays().begin(); tray_it != ams_it->second->GetTrays().end(); tray_it++) {
                std::string tray_id     = tray_it->first;
                int         tray_id_int = atoi(tray_id.c_str());
                // new protocol
                if (ams_id_int < 128) {
                    if ((mo->tray_reading_bits & (1 << (ams_id_int * 4 + tray_id_int))) != 0) {
                        m_ams_control->PlayRridLoading(QString::fromStdString(ams_id), QString::fromStdString(tray_id));
                    } else {
                        m_ams_control->StopRridLoading(QString::fromStdString(ams_id), QString::fromStdString(tray_id));
                    }
                } else {
                    int check_flag = (1 << (16 + ams_id_int - 128));
                    if ((mo->tray_reading_bits & check_flag) != 0) {
                        m_ams_control->PlayRridLoading(QString::fromStdString(ams_id), QString::fromStdString(tray_id));
                    } else {
                        m_ams_control->StopRridLoading(QString::fromStdString(ams_id), QString::fromStdString(tray_id));
                    }
                }
            }
        } catch (...) {}
    }

    // GetCurentAms/GetCurrentCan not yet ported to Qt — skip update_ams_control_state call
}

void StatusPanel::update_filament_loading_panel(MachineObject *obj)
{
    if (!obj) {
        show_filament_load_group(false);
        return;
    }
    auto *mo = reinterpret_cast<Slic3r::MachineObject *>(obj);

    bool ams_loading_state = false;
    auto ams_status_sub    = mo->ams_status_sub;

    if (mo->is_enable_np && mo->ams_status_main != AMS_STATUS_MAIN_COLD_PULL) {
        ams_loading_state = mo->GetExtderSystem()->IsBusyLoading();
    } else if (mo->ams_status_main == AMS_STATUS_MAIN_FILAMENT_CHANGE) {
        ams_loading_state = true;
    }

    if (ams_loading_state) {
        update_load_with_temp();

        const std::string &cur_ams_id  = mo->GetExtderSystem()->GetCurrentAmsId();
        const std::string &cur_tray_id = mo->GetExtderSystem()->GetCurrentSlotId();
        if (!cur_ams_id.empty() && !cur_tray_id.empty()) {
            m_filament_step->updateID(std::atoi(cur_ams_id.c_str()), std::atoi(cur_tray_id.c_str()));
        }

        auto loading_ext         = mo->GetExtderSystem()->GetLoadingExtder();
        auto tar                 = loading_ext ? loading_ext->GetSlotTarget() : DevAmsSlotInfo();
        bool busy_for_vt_loading = (tar.ams_id == std::to_string(VIRTUAL_TRAY_MAIN_ID) ||
                                    tar.ams_id == std::to_string(VIRTUAL_TRAY_DEPUTY_ID)) &&
                                   !mo->is_target_slot_unload();
        if (busy_for_vt_loading) {
            if (ams_status_sub == 0x02) {
                m_filament_step->SetFilamentStep(obj, DevFilamentStep::STEP_HEAT_NOZZLE, STEP_TYPE_VT_LOAD);
            } else if (ams_status_sub == 0x05) {
                m_filament_step->SetFilamentStep(obj, DevFilamentStep::STEP_PUSH_NEW_FILAMENT, STEP_TYPE_VT_LOAD);
            } else if (ams_status_sub == 0x06) {
                m_filament_step->SetFilamentStep(obj, DevFilamentStep::STEP_CONFIRM_EXTRUDED, STEP_TYPE_VT_LOAD);
            } else if (ams_status_sub == 0x07) {
                m_filament_step->SetFilamentStep(obj, DevFilamentStep::STEP_PURGE_OLD_FILAMENT, STEP_TYPE_VT_LOAD);
            } else {
                m_filament_step->SetFilamentStep(obj, DevFilamentStep::STEP_IDLE, STEP_TYPE_VT_LOAD);
                ams_loading_state = false;
            }
        } else {
            if (ams_status_sub == 0x02) {
                if (!mo->is_target_slot_unload()) {
                    m_filament_step->SetFilamentStep(obj, DevFilamentStep::STEP_HEAT_NOZZLE, STEP_TYPE_LOAD);
                } else {
                    m_filament_step->SetFilamentStep(obj, DevFilamentStep::STEP_HEAT_NOZZLE, STEP_TYPE_UNLOAD);
                }
            } else if (ams_status_sub == 0x03) {
                if (!mo->is_target_slot_unload()) {
                    m_filament_step->SetFilamentStep(obj, DevFilamentStep::STEP_CUT_FILAMENT, STEP_TYPE_LOAD);
                } else {
                    m_filament_step->SetFilamentStep(obj, DevFilamentStep::STEP_CUT_FILAMENT, STEP_TYPE_UNLOAD);
                }
            } else if (ams_status_sub == 0x04) {
                if (!mo->is_target_slot_unload()) {
                    m_filament_step->SetFilamentStep(obj, DevFilamentStep::STEP_PULL_CURR_FILAMENT, STEP_TYPE_LOAD);
                } else {
                    m_filament_step->SetFilamentStep(obj, DevFilamentStep::STEP_PULL_CURR_FILAMENT, STEP_TYPE_UNLOAD);
                }
            } else if (ams_status_sub == 0x05) {
                if (!mo->is_target_slot_unload()) {
                    if (m_is_load_with_temp) {
                        m_filament_step->SetFilamentStep(obj, DevFilamentStep::STEP_CUT_FILAMENT, STEP_TYPE_LOAD);
                    } else {
                        m_filament_step->SetFilamentStep(obj, DevFilamentStep::STEP_PUSH_NEW_FILAMENT, STEP_TYPE_LOAD);
                    }
                } else {
                    m_filament_step->SetFilamentStep(obj, DevFilamentStep::STEP_PUSH_NEW_FILAMENT, STEP_TYPE_UNLOAD);
                }
            } else if (ams_status_sub == 0x06) {
                if (!mo->is_target_slot_unload()) {
                    m_filament_step->SetFilamentStep(obj, DevFilamentStep::STEP_PUSH_NEW_FILAMENT, STEP_TYPE_LOAD);
                } else {
                    m_filament_step->SetFilamentStep(obj, DevFilamentStep::STEP_PUSH_NEW_FILAMENT, STEP_TYPE_UNLOAD);
                }
            } else if (ams_status_sub == 0x07) {
                if (!mo->is_target_slot_unload()) {
                    if (m_is_load_with_temp) {
                        m_filament_step->SetFilamentStep(obj, DevFilamentStep::STEP_PULL_CURR_FILAMENT, STEP_TYPE_LOAD);
                    } else {
                        m_filament_step->SetFilamentStep(obj, DevFilamentStep::STEP_PURGE_OLD_FILAMENT, STEP_TYPE_LOAD);
                    }
                } else {
                    m_filament_step->SetFilamentStep(obj, DevFilamentStep::STEP_PURGE_OLD_FILAMENT, STEP_TYPE_UNLOAD);
                }
            } else if (ams_status_sub == 0x08) {
                if (!mo->is_target_slot_unload()) {
                    m_filament_step->SetFilamentStep(obj, DevFilamentStep::STEP_CHECK_POSITION, STEP_TYPE_LOAD);
                } else {
                    m_filament_step->SetFilamentStep(obj, DevFilamentStep::STEP_CHECK_POSITION, STEP_TYPE_UNLOAD);
                }
            } else if (ams_status_sub == 0x09) {
                // just wait
            } else if (ams_status_sub == 0x0B) {
                if (!mo->is_target_slot_unload()) {
                    m_filament_step->SetFilamentStep(obj, DevFilamentStep::STEP_CHECK_POSITION, STEP_TYPE_LOAD);
                } else {
                    m_filament_step->SetFilamentStep(obj, DevFilamentStep::STEP_CHECK_POSITION, STEP_TYPE_UNLOAD);
                }
            } else {
                m_filament_step->SetFilamentStep(obj, DevFilamentStep::STEP_IDLE, STEP_TYPE_UNLOAD);
                ams_loading_state = false;
            }
        }
    } else if (mo->ams_status_main == AMS_STATUS_MAIN_ASSIST) {
        m_filament_step->SetFilamentStep(obj, DevFilamentStep::STEP_IDLE, STEP_TYPE_LOAD);
        ams_loading_state = false;
    } else {
        m_filament_step->SetFilamentStep(obj, DevFilamentStep::STEP_IDLE, STEP_TYPE_LOAD);
        ams_loading_state = false;
    }

    show_filament_load_group(ams_loading_state);
}

void StatusPanel::update_extruder_status(MachineObject * /*obj*/) {}

static void sGetSwitchInfo(MachineObject     *obj,
                           const std::string &ams_id,
                           const std::string &slot_id,
                           QString           &load_error_info,
                           QString           &unload_error_info)
{
    load_error_info.clear();
    unload_error_info.clear();

    if (!obj) {
        load_error_info   = "Please select a printer";
        unload_error_info = "Please select a printer";
        return;
    }
    auto *mo = reinterpret_cast<Slic3r::MachineObject *>(obj);

    if (mo->is_in_printing() && !mo->can_resume()) {
        const auto &err_info = _L("The printer is busy on other print job");
        load_error_info      = err_info;
        unload_error_info    = err_info;
        return;
    }

    if (mo->can_resume() && !devPrinterUtil::IsVirtualSlot(ams_id)) {
        const auto &err_info = _L("When printing is paused, filament loading and unloading are only supported for external slots.");
        load_error_info      = err_info;
        unload_error_info    = err_info;
        return;
    }

    bool in_switch_filament = false;
    if (mo->is_enable_np && mo->GetExtderSystem()->IsBusyLoading()) {
        in_switch_filament = true;
    } else if (mo->ams_status_main == AMS_STATUS_MAIN_FILAMENT_CHANGE) {
        in_switch_filament = true;
    }

    if (in_switch_filament) {
        const auto &err_info = _L("Current extruder is busy changing filament");
        load_error_info      = err_info;
        unload_error_info    = err_info;
        return;
    }

    auto tray_item = mo->get_tray(ams_id, slot_id);
    if (!tray_item) {
        const auto &err_info = _L("Choose an AMS slot then press \"Load\" or \"Unload\" button to automatically load or unload filaments.");
        load_error_info      = err_info;
        unload_error_info    = err_info;
        return;
    }

    if (mo->GetFilaSwitch()->IsInstalled()) {
        if (devPrinterUtil::IsVirtualSlot(ams_id)) {
            const auto &err_info = _L("\"Load\" or \"Unload\" is not supported for external spool while using Filament Track Switch.");
            load_error_info      = err_info;
            unload_error_info    = err_info;
            return;
        }

        if (!mo->GetFilaSwitch()->IsReady()) {
            const auto &err_info = _L("The Filament Track Switch has not been setup. Please setup on printer.");
            load_error_info      = err_info;
            unload_error_info    = err_info;
            return;
        }
    }

    for (auto ext : mo->GetExtderSystem()->GetExtruders()) {
        if (mo->GetFilaSwitch()->IsInstalled()) { continue; }
        if (ext.GetSlotNow().ams_id == ams_id && ext.GetSlotNow().slot_id == slot_id && ext.HasFilamentInExt()) {
            load_error_info = _L("Current slot has alread been loaded");
        }
    }
    if (!devPrinterUtil::IsVirtualSlot(ams_id) && !tray_item->is_exists) {
        load_error_info = _L("The selected slot is empty.");
    }

    auto ams_item = mo->GetFilaSystem()->GetAmsById(ams_id);
    if (ams_item) {
        if (auto extder_id_opt = ams_item->GetCurrentExtruderId(); extder_id_opt.has_value()) {
            auto extder = mo->GetExtderSystem()->GetExtderById(extder_id_opt.value());
            if (extder && !extder->HasFilamentInExt() ||
                (extder->GetSlotNow().ams_id != ams_id || extder->GetSlotNow().slot_id != slot_id)) {
                unload_error_info = _L("The selected slot is not loaded in the extruder.");
            }
        } else {
            if (mo->GetFilaSwitch()->IsInstalled()) {
                unload_error_info = _L("The selected slot is not loaded in the extruder.");
            } else {
                unload_error_info = _L("No extruder found for the selected slot.");
            }
        }
    }
}

void StatusPanel::update_ams_control_state(std::string ams_id, std::string slot_id)
{
    QString load_error_info, unload_error_info;
    sGetSwitchInfo(obj, ams_id, slot_id, load_error_info, unload_error_info);
    m_ams_control->EnableLoadFilamentBtn(load_error_info.isEmpty(), ams_id, slot_id, load_error_info);
    m_ams_control->EnableUnLoadFilamentBtn(unload_error_info.isEmpty(), ams_id, slot_id, unload_error_info);
}

void StatusPanel::update_rack(MachineObject *obj)
{
    auto *mo = reinterpret_cast<Slic3r::MachineObject *>(obj);
    if (mo && mo->GetNozzleSystem()->GetNozzleRack()->IsSupported()) {
        m_ams_rack_switch->show();
        m_panel_nozzle_rack->UpdateRackInfo(mo->GetNozzleSystem()->GetNozzleRack());
    } else {
        m_ams_rack_switch->hide();
        m_panel_nozzle_rack->hide();
    }
}

void StatusPanel::update_cali(MachineObject *obj)
{
    if (!obj) return;
    auto *mo = reinterpret_cast<Slic3r::MachineObject *>(obj);

    // Disable calibration button in 2D (non-FDM) mode
    if (!mo->GetInfo()->IsFdmMode()) {
        m_calibration_btn->setToolTip(_L("Printer 2D mode does not support 3D calibration"));
        m_calibration_btn->setText(_L("Calibration"));
        m_calibration_btn->setEnabled(false);
        return;
    } else if (!m_calibration_btn->isEnabled()) {
        m_calibration_btn->setToolTip(QString());
        m_calibration_btn->setEnabled(true);
    }

    if (mo->is_calibration_running()) {
        m_calibration_btn->setText(_L("Calibrating"));
        if (calibration_dlg && reinterpret_cast<Slic3r::GUI::CalibrationDialog *>(calibration_dlg)->isVisible()) {
            m_calibration_btn->setEnabled(false);
        } else {
            m_calibration_btn->setEnabled(true);
        }
    } else {
        // IDLE
        m_calibration_btn->setText(_L("Calibration"));
        // Disable during printing
        if (mo->is_in_printing()) {
            m_calibration_btn->setEnabled(false);
        } else {
            m_calibration_btn->setEnabled(true);
        }
    }
}

void StatusPanel::update_calib_bitmap()
{
    m_current_print_mode = PrintingTaskType::NOT_CLEAR;
    if (calib_bitmap != nullptr) {
        delete calib_bitmap;
        calib_bitmap = nullptr;
    }
}

void StatusPanel::update_market_scoring(bool show)
{
    if (m_project_task_panel->is_market_scoring_show() != show) {
        m_project_task_panel->market_scoring_show(show);
    }
}

void StatusPanel::reset_printing_values()
{
    m_project_task_panel->update_pausing_state(false);
    m_project_task_panel->update_stopping_state(false);
    m_project_task_panel->enable_partskip_button(nullptr, false);
    m_project_task_panel->enable_pause_resume_button(false, "pause_disable");
    m_project_task_panel->enable_abort_button(false);
    m_project_task_panel->reset_printing_value();
    m_project_task_panel->update_subtask_name(NA_STR);
    m_project_task_panel->show_profile_info(false);
    m_project_task_panel->update_stage_value_with_machine(QString(), 0, obj);
    m_project_task_panel->update_progress_percent(NA_STR, QString());
    m_project_task_panel->market_scoring_show(false);
    m_project_task_panel->get_request_failed_panel()->hide();
    update_basic_print_data(false);
    m_project_task_panel->update_left_time(NA_STR);
    m_project_task_panel->update_finish_time(NA_STR);
    m_project_task_panel->update_layers_num(true, QString(_L("Layer: %1")).arg(NA_STR));
    update_calib_bitmap();

    task_thumbnail_state      = ThumbnailState::PLACE_HOLDER;
    m_start_loading_thumbnail = false;
    m_load_sdcard_thumbnail   = false;
    skip_print_error          = 0;
}

void StatusPanel::on_webrequest_state(QNetworkReply *reply)
{
    BOOST_LOG_TRIVIAL(trace) << "monitor: monitor_panel web request finished, error = " << reply->error();
    if (reply->error() == QNetworkReply::NoError) {
        if (m_current_print_mode != PrintingTaskType::CALIBRATION ||
            (m_calib_mode == CalibMode::Calib_Flow_Rate &&
             m_calib_method == CalibrationMethod::CALI_METHOD_MANUAL)) {
            QImage img;
            img.loadFromData(reply->readAll());
            img_list.insert(std::make_pair(m_request_url, img));
            QPixmap pm = QPixmap::fromImage(img.scaled(
                m_project_task_panel->get_bitmap_thumbnail()->size().width(),
                m_project_task_panel->get_bitmap_thumbnail()->size().height(),
                Qt::KeepAspectRatio, Qt::SmoothTransformation));
            m_project_task_panel->set_thumbnail_img(pm, "");
            m_project_task_panel->set_brightness_value(get_brightness_value(img));
        }
        if (obj) {
            auto *mo = reinterpret_cast<Slic3r::MachineObject *>(obj);
            m_project_task_panel->set_plate_index(mo->m_plate_index);
        } else {
            m_project_task_panel->set_plate_index(-1);
        }
        task_thumbnail_state = ThumbnailState::TASK_THUMBNAIL;
    } else if (reply->error() != QNetworkReply::OperationCanceledError) {
        m_project_task_panel->set_thumbnail_img(m_thumbnail_brokenimg.bmp(), m_thumbnail_brokenimg.name());
        m_project_task_panel->set_plate_index(-1);
        task_thumbnail_state = ThumbnailState::BROKEN_IMG;
    }
    reply->deleteLater();
    web_request = nullptr;
}

bool StatusPanel::is_task_changed(MachineObject *obj)
{
    if (!obj) return false;
    auto *mo = reinterpret_cast<Slic3r::MachineObject *>(obj);

    if (last_subtask != mo->subtask_ || last_profile_id != mo->profile_id_ || last_task_id != mo->task_id_) {
        last_subtask            = mo->subtask_;
        last_profile_id         = mo->profile_id_;
        last_task_id            = mo->task_id_;
        request_model_info_flag = false;
        m_project_task_panel->set_star_count_dirty(false);
        return true;
    }
    return false;
}

void StatusPanel::update_camera_state(MachineObject *obj)
{
    if (!obj) return;
    auto *mo = reinterpret_cast<Slic3r::MachineObject *>(obj);

    // sdcard
    auto sdcard_state = mo->GetStorage()->get_sdcard_state();
    if (m_last_sdcard != (int)sdcard_state) {
        if (sdcard_state == DevStorage::NO_SDCARD) {
            m_bitmap_sdcard_img->setPixmap(m_bitmap_sdcard_state_no.bmp());
            m_bitmap_sdcard_img->setToolTip(_L("No Storage"));
        } else if (sdcard_state == DevStorage::HAS_SDCARD_NORMAL) {
            m_bitmap_sdcard_img->setPixmap(m_bitmap_sdcard_state_normal.bmp());
            m_bitmap_sdcard_img->setToolTip(_L("Storage"));
        } else if (sdcard_state == DevStorage::HAS_SDCARD_ABNORMAL) {
            m_bitmap_sdcard_img->setPixmap(m_bitmap_sdcard_state_abnormal.bmp());
            m_bitmap_sdcard_img->setToolTip(_L("Storage Abnormal"));
        } else {
            m_bitmap_sdcard_img->setPixmap(m_bitmap_sdcard_state_normal.bmp());
            m_bitmap_sdcard_img->setToolTip(_L("Storage"));
        }
        m_last_sdcard = (int)sdcard_state;
    }

    // recording
    if (m_last_recording != (mo->is_recording() ? 1 : 0)) {
        if (mo->is_recording()) {
            m_bitmap_recording_img->setPixmap(m_bitmap_recording_on.bmp());
        } else {
            m_bitmap_recording_img->setPixmap(m_bitmap_recording_off.bmp());
        }
        m_last_recording = mo->is_recording() ? 1 : 0;
    }

    if (!m_bitmap_recording_img->isVisible()) {
        m_bitmap_recording_img->show();
    }

    // timelapse
    if (mo->is_support_timelapse) {
        if (m_last_timelapse != (mo->is_timelapse() ? 1 : 0)) {
            if (mo->is_timelapse()) {
                m_bitmap_timelapse_img->setPixmap(m_bitmap_timelapse_on.bmp());
            } else {
                m_bitmap_timelapse_img->setPixmap(m_bitmap_timelapse_off.bmp());
            }
            m_last_timelapse = mo->is_timelapse() ? 1 : 0;
        }

        if (!m_bitmap_timelapse_img->isVisible()) {
            m_bitmap_timelapse_img->show();
        }
    } else {
        if (m_bitmap_timelapse_img->isVisible()) {
            m_bitmap_timelapse_img->hide();
        }
    }

    // vcamera
    auto *play_ctrl_real = reinterpret_cast<Slic3r::GUI::MediaPlayCtrl *>(m_media_play_ctrl);
    if (mo->virtual_camera) {
        if (m_last_vcamera != (play_ctrl_real->IsStreaming() ? 1 : 0)) {
            if (play_ctrl_real->IsStreaming()) {
                m_bitmap_vcamera_img->setPixmap(m_bitmap_vcamera_on.bmp());
            } else {
                m_bitmap_vcamera_img->setPixmap(m_bitmap_vcamera_off.bmp());
            }
            m_last_vcamera = play_ctrl_real->IsStreaming() ? 1 : 0;
        }

        if (!m_bitmap_vcamera_img->isVisible()) {
            m_bitmap_vcamera_img->show();
        }
    } else {
        if (m_bitmap_vcamera_img->isVisible()) {
            m_bitmap_vcamera_img->hide();
        }
    }

    // camera popup — sync_vcamera_state if visible
    // TODO: m_camera_popup->update(show_vcamera) — CameraPopup::update() not yet ported
}

void StatusPanel::update_partskip_button(MachineObject *obj)
{
    if (!obj) return;
    auto *mo = reinterpret_cast<Slic3r::MachineObject*>(obj);
    auto *partskip_button = m_project_task_panel->get_partskip_button();
    if (mo->is_support_partskip) {
        partskip_button->show();
    } else {
        partskip_button->hide();
    }
    BOOST_LOG_TRIVIAL(info) << "part skip: is_support_partskip: " << mo->is_support_partskip;
}

void StatusPanel::update_printer_parts_options(MachineObject * /*obj*/) {}

QString StatusPanel::getTrayName(const std::string /*amsID*/, const std::string /*slotID*/)
{
    return QString();
}

void StatusPanel::update_error_message()
{
    if (!obj) return;
    auto *mo = reinterpret_cast<Slic3r::MachineObject *>(obj);

    static int last_error = -1;

    if (mo->print_error <= 0) {
        m_project_task_panel->error_info_reset();
        if (m_print_error_dlg) {
            delete m_print_error_dlg;
            m_print_error_dlg = nullptr;
        }
    } else if (mo->print_error != last_error) {
        if (m_print_error_dlg) {
            delete m_print_error_dlg;
            m_print_error_dlg = nullptr;
        }

        m_print_error_dlg  = new DeviceErrorDialog(mo, this);
        QString error_msg  = m_print_error_dlg->show_error_code(mo->print_error);
        BOOST_LOG_TRIVIAL(info) << "print error: device error code = " << mo->print_error;

        if (!error_msg.isEmpty()) { m_project_task_panel->show_error_msg(error_msg); }
    }

    last_error = mo->print_error;
}

void StatusPanel::create_tasklist_info() {}

void StatusPanel::show_task_list_info(bool /*show*/) {}

void StatusPanel::update_tasklist_info() {}

void StatusPanel::on_market_scoring(QEvent & /*event*/) {}

void StatusPanel::on_market_retry(QEvent & /*event*/)
{
    if (obj) {
        auto *mo = reinterpret_cast<Slic3r::MachineObject*>(obj);
        mo->get_model_mall_result_need_retry = true;
    } else {
        BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << " retry failed";
    }
}

void StatusPanel::on_subtask_partskip(QEvent & /*event*/) {}

void StatusPanel::on_subtask_pause_resume(QEvent & /*event*/)
{
    if (obj) {
        auto *mo = reinterpret_cast<Slic3r::MachineObject*>(obj);
        if (mo->can_resume()) {
            BOOST_LOG_TRIVIAL(info) << "monitor: resume current print task dev_id =" << BBLCrossTalk::Crosstalk_DevId(mo->get_dev_id());
            mo->command_task_resume();
        } else {
            BOOST_LOG_TRIVIAL(info) << "monitor: pause current print task dev_id =" << BBLCrossTalk::Crosstalk_DevId(mo->get_dev_id());
            mo->command_task_pause();
        }
    }
}

void StatusPanel::on_subtask_abort(QEvent & /*event*/)
{
    auto *dlg = reinterpret_cast<Slic3r::GUI::SecondaryCheckDialog*>(abort_dlg);
    if (abort_dlg == nullptr) {
        abort_dlg = reinterpret_cast<::SecondaryCheckDialog*>(
            new Slic3r::GUI::SecondaryCheckDialog(this->parentWidget(), -1, _L("Cancel print")));
        dlg = reinterpret_cast<Slic3r::GUI::SecondaryCheckDialog*>(abort_dlg);
        connect(dlg->m_button_ok, &Button::clicked, this, [this]() {
            if (obj) {
                auto *mo = reinterpret_cast<Slic3r::MachineObject*>(obj);
                BOOST_LOG_TRIVIAL(info) << "monitor: stop current print task dev_id =" << BBLCrossTalk::Crosstalk_DevId(mo->get_dev_id());
                mo->command_task_abort();
            }
        });
    }
    dlg->update_text(_L("Are you sure you want to stop this print?"));
    dlg->m_button_cancel->SetBackgroundColor(dlg->btn_bg_green);
    dlg->m_button_cancel->SetBorderColorNormal(QColor(Qt::white));
    dlg->m_button_cancel->SetTextColorNormal(QColor("#FFFFFE"));
    dlg->m_button_cancel->setText(_L("No"));
    dlg->m_button_ok->SetBackgroundColor(dlg->btn_bg_white);
    dlg->m_button_ok->SetBorderColorNormal(QColor(38, 46, 48));
    dlg->m_button_ok->SetTextColorNormal(QColor(Qt::black));
    dlg->m_button_ok->setText(_L("Yes"));
    dlg->on_show();
    dlg->raise();
}

void StatusPanel::on_print_error_clean(QEvent & /*event*/)
{
    error_info_reset();
    auto *mo = reinterpret_cast<Slic3r::MachineObject*>(obj);
    skip_print_error = mo->print_error;
    char buf[32];
    ::snprintf(buf, sizeof(buf), "%08X", skip_print_error);
    BOOST_LOG_TRIVIAL(info) << "skip_print_error: " << buf;
    before_error_code = mo->print_error;
}

void StatusPanel::error_info_reset()
{
    m_project_task_panel->error_info_reset();
}

void StatusPanel::show_recenter_dialog()
{
    RecenterDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        if (obj) {
            auto *mo = reinterpret_cast<Slic3r::MachineObject *>(obj);
            mo->GetAxis()->Ctrl_GoHome();
        }
    }
}

bool StatusPanel::check_axis_z_at_home(MachineObject *obj)
{
    if (obj) {
        auto *mo = reinterpret_cast<Slic3r::MachineObject*>(obj);
        if (!mo->GetAxis()->IsAxisAtHomeZ()) {
            BOOST_LOG_TRIVIAL(info) << "axis z is not at home";
            show_recenter_dialog();
            return false;
        }
        return true;
    }
    return false;
}

void StatusPanel::on_axis_ctrl_xy(QEvent & /*event*/)
{
    // Connection uses buttonClicked(int) lambda in constructor — this overload unused
}

void StatusPanel::on_axis_ctrl_z_up_10(QEvent & /*event*/)
{
    if (obj) {
        auto *mo = reinterpret_cast<Slic3r::MachineObject*>(obj);
        mo->GetAxis()->Ctrl_Axis("Z", 1.0, -10.0f, 900);
        if (!check_axis_z_at_home(obj)) return;
    }
}

void StatusPanel::on_axis_ctrl_z_up_1(QEvent & /*event*/)
{
    if (obj) {
        auto *mo = reinterpret_cast<Slic3r::MachineObject*>(obj);
        mo->GetAxis()->Ctrl_Axis("Z", 1.0, -1.0f, 900);
        if (!check_axis_z_at_home(obj)) return;
    }
}

void StatusPanel::on_axis_ctrl_z_down_1(QEvent & /*event*/)
{
    if (obj) {
        auto *mo = reinterpret_cast<Slic3r::MachineObject*>(obj);
        mo->GetAxis()->Ctrl_Axis("Z", 1.0, 1.0f, 900);
        if (!check_axis_z_at_home(obj)) return;
    }
}

void StatusPanel::on_axis_ctrl_z_down_10(QEvent & /*event*/)
{
    if (obj) {
        auto *mo = reinterpret_cast<Slic3r::MachineObject*>(obj);
        mo->GetAxis()->Ctrl_Axis("Z", 1.0, 10.0f, 900);
        if (!check_axis_z_at_home(obj)) return;
    }
}

void StatusPanel::on_axis_ctrl_e_up_10(QEvent & /*event*/)
{
    if (obj) {
        auto *mo = reinterpret_cast<Slic3r::MachineObject*>(obj);
        auto ext = mo->GetExtderSystem()->GetCurrentExtder();
        if (ext && ext->GetCurrentTemp() >= TEMP_THRESHOLD_ALLOW_E_CTRL) {
            if (mo->is_enable_np) {
                mo->command_extruder_control(ext->GetExtId(), -10.0f);
            } else {
                mo->GetAxis()->Ctrl_Axis("E", 1.0, -10.0f, 900);
            }
        } else {
            axis_ctrl_e_hint(true);
        }
    }
}

void StatusPanel::on_axis_ctrl_e_down_10(QEvent & /*event*/)
{
    if (obj) {
        auto *mo = reinterpret_cast<Slic3r::MachineObject*>(obj);
        auto ext = mo->GetExtderSystem()->GetCurrentExtder();
        if (ext && ext->GetCurrentTemp() >= TEMP_THRESHOLD_ALLOW_E_CTRL) {
            if (mo->is_enable_np) {
                mo->command_extruder_control(ext->GetExtId(), 10.0f);
            } else {
                mo->GetAxis()->Ctrl_Axis("E", 1.0, 10.0f, 900);
            }
        } else {
            axis_ctrl_e_hint(false);
        }
    }
}

void StatusPanel::axis_ctrl_e_hint(bool /*up_down*/)
{
    if (ctrl_e_hint_dlg == nullptr) {
        ctrl_e_hint_dlg = new Slic3r::GUI::MessageDialog(
                this,
                _L("Please heat the nozzle to above 170 degree before loading or unloading filament."),
                _L("Warning"),
                MsgOK);
    }
    ctrl_e_hint_dlg->exec();
}

void StatusPanel::on_nozzle_selected(QEvent & /*event*/) {}

void StatusPanel::on_bed_temp_kill_focus(QFocusEvent & /*event*/) {}

void StatusPanel::on_bed_temp_set_focus(QFocusEvent & /*event*/) {}

void StatusPanel::on_set_bed_temp()
{
    if (!obj) return;
    auto *mo = reinterpret_cast<Slic3r::MachineObject*>(obj);
    QString str = m_tempCtrl_bed->GetTextCtrl()->text();
    try {
        bool ok;
        long bed_temp = str.toLong(&ok);
        if (ok) {
            set_hold_count(m_temp_bed_timeout);
            int limit = mo->get_bed_temperature_limit();
            if (mo->bed_temp_range.size() > 1) { limit = mo->bed_temp_range[1]; }
            if (bed_temp >= limit) {
                BOOST_LOG_TRIVIAL(info) << "can not set over limit = " << limit << ", set temp = " << bed_temp;
                bed_temp = limit;
                m_tempCtrl_bed->SetTagTemp(QString::number(bed_temp));
                m_tempCtrl_bed->Warning(false);
            }
            mo->command_set_bed(bed_temp);
        }
    } catch (...) {
        ;
    }
}

void StatusPanel::on_nozzle_temp_kill_focus(QFocusEvent & /*event*/) {}

void StatusPanel::on_nozzle_temp_set_focus(QFocusEvent & /*event*/) {}

void StatusPanel::on_set_nozzle_temp(int nozzle_id)
{
    if (!obj) return;
    auto *mo = reinterpret_cast<Slic3r::MachineObject*>(obj);

    try {
        const auto& extder = mo->GetExtderSystem()->GetExtderById(nozzle_id);
        if (!extder) return;

        if (!extder->HasNozzleInstalled()) {
            Slic3r::GUI::MessageDialog msg_dlg(
                this,
                _L("Right extruder hotend not detected. Cannot set nozzle temperature."),
                _L("Warning"), MsgICON_WARN | MsgOK);
            msg_dlg.exec();
            return;
        }

        if (nozzle_id == MAIN_EXTRUDER_ID) {
            QString str = m_tempCtrl_nozzle->GetTextCtrl()->text();
            bool ok;
            long nozzle_temp = str.toLong(&ok);
            if (ok) {
                set_hold_count(m_temp_nozzle_timeout);
                if (nozzle_temp > m_tempCtrl_nozzle->get_max_temp()) {
                    nozzle_temp = m_tempCtrl_nozzle->get_max_temp();
                    m_tempCtrl_nozzle->SetTagTemp(QString::number(nozzle_temp));
                    m_tempCtrl_nozzle->Warning(false);
                }
                if (m_tempCtrl_nozzle->GetCurrType() == TempInputType::TEMP_OF_NORMAL_TYPE) {
                    mo->command_set_nozzle(nozzle_temp);
                } else {
                    mo->command_set_nozzle_new(MAIN_EXTRUDER_ID, nozzle_temp);
                }
            }
        }

        if (nozzle_id == DEPUTY_EXTRUDER_ID) {
            QString str = m_tempCtrl_nozzle_deputy->GetTextCtrl()->text();
            bool ok;
            long nozzle_temp = str.toLong(&ok);
            if (ok) {
                set_hold_count(m_temp_nozzle_deputy_timeout);
                if (nozzle_temp > m_tempCtrl_nozzle_deputy->get_max_temp()) {
                    nozzle_temp = m_tempCtrl_nozzle_deputy->get_max_temp();
                    m_tempCtrl_nozzle_deputy->SetTagTemp(QString::number(nozzle_temp));
                    m_tempCtrl_nozzle_deputy->Warning(false);
                }
                mo->command_set_nozzle_new(DEPUTY_EXTRUDER_ID, nozzle_temp);
            }
        }
    } catch (...) {
        ;
    }
}

void StatusPanel::on_set_chamber_temp()
{
    if (!obj) return;
    auto *mo = reinterpret_cast<Slic3r::MachineObject*>(obj);
    QString str = m_tempCtrl_chamber->GetTextCtrl()->text();
    try {
        bool ok;
        long chamber_temp = str.toLong(&ok);
        if (ok) {
            set_hold_count(m_temp_chamber_timeout);
            if (chamber_temp > m_tempCtrl_chamber->get_max_temp()) {
                chamber_temp = m_tempCtrl_chamber->get_max_temp();
                m_tempCtrl_chamber->SetTagTemp(QString::number(chamber_temp));
                m_tempCtrl_chamber->Warning(false);
            }
            if (mo->is_in_printing() && mo->GetFan()->GetSupportAirduct() && mo->GetFan()->is_at_cooling_mode()) {
                Slic3r::GUI::MessageDialog dlg(
                    this,
                    _L("Chamber temperature cannot be changed in cooling mode while printing."),
                    QString(), MsgICON_WARN | MsgOK);
                dlg.exec();
                return;
            } else if (!mo->GetFan()->is_at_heating_mode() && chamber_temp >= mo->GetConfig()->GetChamberTempSwitchHeat()) {
                Slic3r::GUI::MessageDialog dlg(
                    this,
                    _L("If the chamber temperature exceeds 40\u2103, the system will automatically switch to heating mode. "
                       "Please confirm whether to switch."),
                    QString(), MsgICON_WARN | MsgOK | MsgCANCEL);
                if (dlg.exec() != QDialog::Accepted) return;
            }
            mo->GetChamber()->CtrlSetChamberTemp(chamber_temp);
        }
    } catch (...) {
        ;
    }
}

void StatusPanel::on_ams_load(QEvent & /*event*/)
{
    BOOST_LOG_TRIVIAL(info) << "on_ams_load";
    on_ams_load_curr();
}

void StatusPanel::update_load_with_temp()
{
    if (!obj) return;
    auto *real_obj = reinterpret_cast<Slic3r::MachineObject*>(obj);
    if (!real_obj->is_filament_at_extruder()) {
        m_is_load_with_temp = true;
    } else {
        m_is_load_with_temp = false;
    }
}

void StatusPanel::on_ams_load_curr()
{
    if (obj) {
        auto *real_obj = reinterpret_cast<Slic3r::MachineObject*>(obj);
        std::string curr_ams_id = m_ams_control->GetCurentAms();
        std::string curr_can_id = m_ams_control->GetCurrentCan(curr_ams_id);

        // FeedDirectionDialog not yet ported — proceed without explicit extruder selection
        std::optional<int> extruder_id = std::nullopt;

        update_load_with_temp();
        // virtual tray
        if (curr_ams_id.compare(std::to_string(VIRTUAL_TRAY_MAIN_ID)) == 0 ||
            curr_ams_id.compare(std::to_string(VIRTUAL_TRAY_DEPUTY_ID)) == 0) {
            int vt_slot_idx = 0;
            if (curr_ams_id.compare(std::to_string(VIRTUAL_TRAY_DEPUTY_ID)) == 0) { vt_slot_idx = 1; }

            int         old_temp  = -1;
            int         new_temp  = -1;
            DevAmsTray *curr_tray = &real_obj->vt_slot[vt_slot_idx];

            if (!curr_tray) return;

            try {
                if (!curr_tray->nozzle_temp_max.empty() && !curr_tray->nozzle_temp_min.empty())
                    old_temp = (atoi(curr_tray->nozzle_temp_min.c_str()) + atoi(curr_tray->nozzle_temp_max.c_str())) / 2;
                if (!curr_tray->nozzle_temp_max.empty() && !curr_tray->nozzle_temp_min.empty())
                    new_temp = (atoi(curr_tray->nozzle_temp_min.c_str()) + atoi(curr_tray->nozzle_temp_max.c_str())) / 2;
            } catch (...) { ; }

            if (real_obj->is_enable_np || real_obj->is_enable_ams_np) {
                try {
                    if (!curr_ams_id.empty() && !curr_can_id.empty()) {
                        real_obj->command_ams_change_filament(true, curr_ams_id, "0", old_temp, new_temp, extruder_id);
                    }
                } catch (...) {}
            } else {
                real_obj->command_ams_change_filament(true, "254", "0", old_temp, new_temp, extruder_id);
            }
            return;
        }

        std::map<std::string, DevAms *>::iterator it = real_obj->GetFilaSystem()->GetAmsList().find(curr_ams_id);
        if (it == real_obj->GetFilaSystem()->GetAmsList().end()) {
            BOOST_LOG_TRIVIAL(trace) << "ams: find " << curr_ams_id << " failed";
            return;
        }
        auto tray_it = it->second->GetTrays().find(curr_can_id);
        if (tray_it == it->second->GetTrays().end()) {
            BOOST_LOG_TRIVIAL(trace) << "ams: find " << curr_can_id << " failed";
            return;
        }
        DevAmsTray *curr_tray = real_obj->get_curr_tray();
        DevAmsTray *targ_tray = real_obj->get_ams_tray(curr_ams_id, curr_can_id);

        int old_temp = -1;
        int new_temp = -1;

        if (curr_tray && targ_tray) {
            try {
                if (!curr_tray->nozzle_temp_max.empty() && !curr_tray->nozzle_temp_min.empty())
                    old_temp = (atoi(curr_tray->nozzle_temp_min.c_str()) + atoi(curr_tray->nozzle_temp_max.c_str())) / 2;
                if (!targ_tray->nozzle_temp_max.empty() && !targ_tray->nozzle_temp_min.empty())
                    new_temp = (atoi(targ_tray->nozzle_temp_min.c_str()) + atoi(targ_tray->nozzle_temp_max.c_str())) / 2;
            } catch (...) { ; }
        }

        if (real_obj->is_enable_np) {
            try {
                if (!curr_ams_id.empty() && !curr_can_id.empty()) {
                    real_obj->command_ams_change_filament(true, curr_ams_id, curr_can_id, old_temp, new_temp, extruder_id);
                }
            } catch (...) {}
        } else {
            real_obj->command_ams_change_filament(true, curr_ams_id, curr_can_id, old_temp, new_temp, extruder_id);
        }
    }
}

void StatusPanel::on_ams_load_vams(QEvent & /*event*/)
{
    BOOST_LOG_TRIVIAL(info) << "on_ams_load_vams_tray";
    m_ams_control->SwitchAms(std::to_string(VIRTUAL_TRAY_MAIN_ID));
    on_ams_load_curr();
}

void StatusPanel::on_ams_switch(QEvent & /*event*/)
{
    if (obj) {
        auto *real_obj = reinterpret_cast<Slic3r::MachineObject*>(obj);
        if (auto ext = real_obj->GetExtderSystem()->GetExtderById(MAIN_EXTRUDER_ID); ext.has_value()) {
            if (ext->HasFilamentInExt()) {
                m_ams_control->SetAmsStep(ext->GetSlotNow().ams_id, ext->GetSlotNow().slot_id, MAIN_EXTRUDER_ID,
                    AMSPassRoadType::AMS_ROAD_TYPE_LOAD, AMSPassRoadSTEP::AMS_ROAD_STEP_COMBO_LOAD_STEP2);
            } else {
                m_ams_control->SetAmsStep(ext->GetSlotNow().ams_id, ext->GetSlotNow().slot_id, MAIN_EXTRUDER_ID,
                    AMSPassRoadType::AMS_ROAD_TYPE_UNLOAD, AMSPassRoadSTEP::AMS_ROAD_STEP_NONE);
            }
            m_ams_control->SetExtruder(ext->HasFilamentInExt(), MAIN_EXTRUDER_ID, ext->GetSlotNow().ams_id, ext->GetSlotNow().slot_id);
        }
        if (auto ext = real_obj->GetExtderSystem()->GetExtderById(DEPUTY_EXTRUDER_ID); ext.has_value()) {
            if (ext->HasFilamentInExt()) {
                m_ams_control->SetAmsStep(ext->GetSlotNow().ams_id, ext->GetSlotNow().slot_id, DEPUTY_EXTRUDER_ID,
                    AMSPassRoadType::AMS_ROAD_TYPE_LOAD, AMSPassRoadSTEP::AMS_ROAD_STEP_COMBO_LOAD_STEP2);
            } else {
                m_ams_control->SetAmsStep(ext->GetSlotNow().ams_id, ext->GetSlotNow().slot_id, DEPUTY_EXTRUDER_ID,
                    AMSPassRoadType::AMS_ROAD_TYPE_UNLOAD, AMSPassRoadSTEP::AMS_ROAD_STEP_NONE);
            }
            m_ams_control->SetExtruder(ext->HasFilamentInExt(), DEPUTY_EXTRUDER_ID, ext->GetSlotNow().ams_id, ext->GetSlotNow().slot_id);
        }
    }
}

void StatusPanel::on_ams_unload(QEvent & /*event*/)
{
    if (obj) {
        auto *real_obj = reinterpret_cast<Slic3r::MachineObject*>(obj);
        std::string curr_ams_id = m_ams_control->GetCurentAms();
        std::string curr_can_id = m_ams_control->GetCurrentCan(curr_ams_id);
        if (real_obj->is_enable_np) {
            try {
                for (auto ext : real_obj->GetExtderSystem()->GetExtruders()) {
                    if (ext.GetSlotNow().ams_id == curr_ams_id && ext.GetSlotNow().slot_id == curr_can_id) {
                        real_obj->command_ams_change_filament(false, curr_ams_id, "255");
                    }
                }
            } catch (...) {}
        } else {
            real_obj->command_ams_change_filament(false, curr_ams_id, "255");
        }
    }
}

void StatusPanel::on_ams_filament_backup(QEvent & /*event*/)
{
    if (obj) {
        auto *real_obj = reinterpret_cast<Slic3r::MachineObject*>(obj);
        auto *dlg = new Slic3r::GUI::AmsReplaceMaterialDialog(this);
        dlg->update_machine_obj(obj);
        dlg->exec();
    }
}

void StatusPanel::on_ams_setting_click(QEvent & /*event*/)
{
    if (obj) {
        auto *real_obj = reinterpret_cast<Slic3r::MachineObject*>(obj);
        if (!m_ams_setting_dlg) {
            m_ams_setting_dlg = reinterpret_cast<::AMSSetting*>(new Slic3r::GUI::AMSSetting(this));
        }
        reinterpret_cast<Slic3r::GUI::AMSSetting*>(m_ams_setting_dlg)->UpdateByObj(obj);
        reinterpret_cast<Slic3r::GUI::AMSSetting*>(m_ams_setting_dlg)->show();
    }
}

void StatusPanel::on_filament_edit(QEvent & /*event*/)
{
    if (!m_filament_setting_dlg) {
        m_filament_setting_dlg = reinterpret_cast<::AMSMaterialsSetting*>(new Slic3r::GUI::AMSMaterialsSetting(this));
    }
    if (obj) {
        auto *real_obj = reinterpret_cast<Slic3r::MachineObject*>(obj);
        auto *dlg = reinterpret_cast<Slic3r::GUI::AMSMaterialsSetting*>(m_filament_setting_dlg);
        dlg->obj = obj;

        std::string curr_ams_id = m_ams_control->GetCurentAms();
        std::string curr_can_id = m_ams_control->GetCurrentCan(curr_ams_id);
        int ams_id  = curr_ams_id.empty() ? 0 : atoi(curr_ams_id.c_str());
        int slot_id = curr_can_id.empty() ? 0 : atoi(curr_can_id.c_str());

        try {
            dlg->ams_id  = ams_id;
            dlg->slot_id = slot_id;

            std::string sn_number;
            std::string filament;
            std::string temp_max;
            std::string temp_min;
            QString     k_val;
            QString     n_val;

            auto tray = real_obj->GetFilaSystem()->GetAmsTray(curr_ams_id, curr_can_id);
            if (tray) {
                k_val         = QString::asprintf("%.3f", tray->k);
                n_val         = QString::asprintf("%.3f", tray->n);
                QColor color  = Slic3r::DevAmsTray::decode_color(tray->color);

                std::vector<QColor> cols;
                for (auto col : tray->cols) { cols.push_back(Slic3r::DevAmsTray::decode_color(col)); }
                dlg->set_ctype(tray->ctype);
                dlg->ams_filament_id = tray->setting_id;

                if (dlg->ams_filament_id.empty()) {
                    dlg->set_empty_color(color);
                } else {
                    dlg->set_color(color);
                    dlg->set_colors(cols);
                }

                dlg->m_is_third = !Slic3r::DevFilaSystem::IsBBL_Filament(tray->tag_uid);
                if (!dlg->m_is_third) {
                    sn_number = tray->uuid;
                    filament  = tray->sub_brands;
                    temp_max  = tray->nozzle_temp_max;
                    temp_min  = tray->nozzle_temp_min;
                }
            }

            dlg->Popup(QString::fromStdString(filament), QString::fromStdString(sn_number),
                       QString::fromStdString(temp_min), QString::fromStdString(temp_max),
                       k_val, n_val);
        } catch (...) { ; }
    }
}

void StatusPanel::on_ext_spool_edit(QEvent & /*event*/)
{
    if (!m_filament_setting_dlg) {
        m_filament_setting_dlg = reinterpret_cast<::AMSMaterialsSetting*>(new Slic3r::GUI::AMSMaterialsSetting(this));
    }
    if (obj) {
        auto *real_obj = reinterpret_cast<Slic3r::MachineObject*>(obj);
        auto *dlg = reinterpret_cast<Slic3r::GUI::AMSMaterialsSetting*>(m_filament_setting_dlg);
        dlg->obj = obj;

        std::string curr_ams_id = m_ams_control->GetCurentAms();
        int ams_id      = curr_ams_id.empty() ? VIRTUAL_TRAY_MAIN_ID : atoi(curr_ams_id.c_str());
        int nozzle_index = (ams_id == VIRTUAL_TRAY_MAIN_ID) ? 0 : 1;
        dlg->ams_id  = ams_id;
        dlg->slot_id = 0;

        try {
            std::string sn_number;
            std::string filament;
            std::string temp_max;
            std::string temp_min;
            QString     k_val = QString::asprintf("%.3f", real_obj->vt_slot[nozzle_index].k);
            QString     n_val = QString::asprintf("%.3f", real_obj->vt_slot[nozzle_index].n);
            QColor color      = Slic3r::DevAmsTray::decode_color(real_obj->vt_slot[nozzle_index].color);
            dlg->ams_filament_id = real_obj->vt_slot[nozzle_index].setting_id;

            std::vector<QColor> cols;
            for (auto col : real_obj->vt_slot[nozzle_index].cols) { cols.push_back(Slic3r::DevAmsTray::decode_color(col)); }
            dlg->set_ctype(real_obj->vt_slot[nozzle_index].ctype);

            if (dlg->ams_filament_id.empty()) {
                dlg->set_empty_color(color);
            } else {
                dlg->set_color(color);
                dlg->set_colors(cols);
            }

            dlg->m_is_third = !Slic3r::DevFilaSystem::IsBBL_Filament(real_obj->vt_slot[nozzle_index].tag_uid);
            if (!dlg->m_is_third) {
                sn_number = real_obj->vt_slot[nozzle_index].uuid;
                filament  = real_obj->vt_slot[nozzle_index].sub_brands;
                temp_max  = real_obj->vt_slot[nozzle_index].nozzle_temp_max;
                temp_min  = real_obj->vt_slot[nozzle_index].nozzle_temp_min;
            }

            dlg->Popup(QString::fromStdString(filament), QString::fromStdString(sn_number),
                       QString::fromStdString(temp_min), QString::fromStdString(temp_max),
                       k_val, n_val);
        } catch (...) { ; }
    }
}

void StatusPanel::on_filament_extrusion_cali(QEvent & /*event*/)
{
    if (!m_extrusion_cali_dlg) {
        m_extrusion_cali_dlg = reinterpret_cast<::ExtrusionCalibration*>(new Slic3r::GUI::ExtrusionCalibration(this));
    }
    if (obj) {
        auto *real_obj = reinterpret_cast<Slic3r::MachineObject*>(obj);
        auto *dlg = reinterpret_cast<Slic3r::GUI::ExtrusionCalibration*>(m_extrusion_cali_dlg);
        dlg->obj = obj;
        std::string ams_id  = m_ams_control->GetCurentAms();
        std::string tray_id = m_ams_control->GetCurrentCan(ams_id);
        if (tray_id.empty() && ams_id.compare(std::to_string(VIRTUAL_TRAY_MAIN_ID)) != 0) {
            Slic3r::GUI::MessageDialog msg_dlg(nullptr,
                _L("Please select an AMS slot before calibration"),
                QString(), MsgICON_WARN | MsgOK);
            msg_dlg.exec();
            return;
        }

        int ams_id_int  = 0;
        int tray_id_int = 0;

        if (devPrinterUtil::IsVirtualSlot(ams_id)) {
            dlg->ams_filament_id = "";
        } else {
            ams_id_int  = atoi(ams_id.c_str());
            tray_id_int = atoi(tray_id.c_str());
            if (auto tray = real_obj->GetFilaSystem()->GetAmsTray(ams_id, tray_id)) {
                if (Slic3r::DevFilaSystem::IsBBL_Filament(tray->tag_uid))
                    dlg->ams_filament_id = tray->setting_id;
                else
                    dlg->ams_filament_id = "";
            }
        }

        try {
            dlg->m_ams_id  = ams_id_int;
            dlg->m_slot_id = tray_id_int;
            dlg->Popup();
        } catch (...) { ; }
    }
}

void StatusPanel::on_ams_refresh_rfid(QEvent & /*event*/)
{
    if (obj) {
        auto *real_obj = reinterpret_cast<Slic3r::MachineObject*>(obj);
        std::string curr_ams_id = m_ams_control->GetCurentAms();
        std::string curr_can_id = m_ams_control->GetCurrentCan(curr_ams_id);

        int ams_id_int = curr_ams_id.empty() ? -1 : atoi(curr_ams_id.c_str());
        if (ams_id_int < 0 || ams_id_int > VIRTUAL_TRAY_MAIN_ID) { return; }
        if (ams_id_int == VIRTUAL_TRAY_MAIN_ID) { return; }

        std::map<std::string, DevAms *>::iterator it = real_obj->GetFilaSystem()->GetAmsList().find(curr_ams_id);
        if (it == real_obj->GetFilaSystem()->GetAmsList().end()) {
            BOOST_LOG_TRIVIAL(trace) << "ams: find " << curr_ams_id << " failed";
            return;
        }
        auto slot_it = it->second->GetTrays().find(curr_can_id);
        if (slot_it == it->second->GetTrays().end()) {
            BOOST_LOG_TRIVIAL(trace) << "ams: find " << curr_can_id << " failed";
            return;
        }

        bool has_filament_at_extruder = false;
        bool use_new_command          = false;

        if (real_obj->is_enable_np || real_obj->is_enable_ams_np) {
            use_new_command = true;
            auto current_extruder_id = it->second->GetCurrentExtruderId();
            if (current_extruder_id.has_value()) {
                has_filament_at_extruder = real_obj->GetExtderSystem()->HasFilamentInExt(current_extruder_id.value());
            }
        } else {
            has_filament_at_extruder = real_obj->is_filament_at_extruder();
        }

        if (has_filament_at_extruder) {
            Slic3r::GUI::MessageDialog msg_dlg(nullptr,
                _L("Cannot read filament info: the filament is loaded to the toolhead, please unload the filament and try again."),
                QString(), MsgICON_WARN | MsgOK);
            msg_dlg.exec();
            return;
        }

        try {
            if (!use_new_command) {
                int tray_index = atoi(curr_ams_id.c_str()) * 4 + atoi(slot_it->second->id.c_str());
                real_obj->command_ams_refresh_rfid(std::to_string(tray_index));
            }
            if (use_new_command) {
                real_obj->command_ams_refresh_rfid2(atoi(curr_ams_id.c_str()), atoi(curr_can_id.c_str()));
            }
        } catch (...) { ; }
    }
}

void StatusPanel::on_ams_selected(QEvent & /*event*/)
{
    if (obj) {
        auto *real_obj = reinterpret_cast<Slic3r::MachineObject*>(obj);
        std::string curr_ams_id = m_ams_control->GetCurentAms();
        std::string curr_can_id = m_ams_control->GetCurrentCan(curr_ams_id);

        if (curr_ams_id.compare(std::to_string(VIRTUAL_TRAY_MAIN_ID)) == 0) {
            return;
        } else {
            std::map<std::string, DevAms *>::iterator it = real_obj->GetFilaSystem()->GetAmsList().find(curr_ams_id);
            if (it == real_obj->GetFilaSystem()->GetAmsList().end()) {
                BOOST_LOG_TRIVIAL(trace) << "ams: find " << curr_ams_id << " failed";
                return;
            }
            auto tray_it = it->second->GetTrays().find(curr_can_id);
            if (tray_it == it->second->GetTrays().end()) {
                BOOST_LOG_TRIVIAL(trace) << "ams: find " << curr_can_id << " failed";
                return;
            }
            try {
                int tray_index = atoi(curr_ams_id.c_str()) * 4 + atoi(tray_it->second->id.c_str());
                real_obj->command_ams_select_tray(std::to_string(tray_index));
            } catch (...) { ; }
        }
    }
}

void StatusPanel::on_ams_guide(QEvent & /*event*/)
{
    // Use default AMS wiki URL (m_ams_model not accessible from Qt AMSControl)
    QDesktopServices::openUrl(QUrl("https://wiki.bambulab.com/en/software/bambu-studio/use-ams-on-bambu-studio"));
}

void StatusPanel::on_ams_retry(QEvent & /*event*/)
{
    BOOST_LOG_TRIVIAL(info) << "on_ams_retry";
    if (obj) {
        auto *real_obj = reinterpret_cast<Slic3r::MachineObject*>(obj);
        real_obj->command_ams_control("resume");
    }
}

void StatusPanel::on_fan_changed(QEvent & /*event*/)
{
    set_hold_count(this->m_switch_cham_fan_timeout);
}

void StatusPanel::on_cham_temp_kill_focus(QFocusEvent & /*event*/) {}

void StatusPanel::on_cham_temp_set_focus(QFocusEvent & /*event*/) {}

void StatusPanel::on_switch_speed(QEvent & /*event*/) {}

void StatusPanel::on_lamp_switch(QEvent & /*event*/)
{
    if (!obj) return;
    auto *real_obj = reinterpret_cast<Slic3r::MachineObject*>(obj);
    bool value = m_switch_lamp->GetValue();
    if (value) {
        m_switch_lamp->SetValue(true);
        set_hold_count(this->m_switch_lamp_timeout);
        real_obj->GetLamp()->CtrlSetChamberLight(DevLamp::LIGHT_EFFECT_ON);
    } else {
        if (real_obj->GetLamp()->HasLampCloseRecheck()) {
            Slic3r::GUI::MessageDialog msg_dlg(nullptr,
                _L("Turning off the lights during the task will cause the failure of AI monitoring, like spaghetti detection. Please choose carefully."),
                QString(), MsgICON_WARN | MsgOK | MsgCANCEL);
            msg_dlg.SetButtonLabel(MsgID_OK,     _L("Keep it On"));
            msg_dlg.SetButtonLabel(MsgID_CANCEL, _L("Still turn it Off"));
            if (msg_dlg.exec() != QDialog::Rejected) { return; }
        }
        m_switch_lamp->SetValue(false);
        set_hold_count(this->m_switch_lamp_timeout);
        real_obj->GetLamp()->CtrlSetChamberLight(DevLamp::LIGHT_EFFECT_OFF);
    }
}

void StatusPanel::on_printing_fan_switch(QEvent & /*event*/) {}

void StatusPanel::on_nozzle_fan_switch(QEvent & /*event*/)
{
    if (m_fan_control_popup) {
        m_fan_control_popup->deleteLater();
        m_fan_control_popup = nullptr;
    }
    if (!obj) { return; }
    auto *real_obj = reinterpret_cast<Slic3r::MachineObject*>(obj);
    if (real_obj->GetFan()->GetAirDuctData().modes.empty()) {
        real_obj->GetFan()->converse_to_duct(true, real_obj->GetFan()->GetSupportAuxFanData(), real_obj->GetFan()->GetSupportChamberFan());
    }
    m_fan_control_popup = new FanControlPopupNew(this, obj, real_obj->GetFan()->GetAirDuctData());
    QPoint pos = m_switch_fan->mapToGlobal(QPoint(0, m_switch_fan->height()));
    pos.rx() -= 150;
    pos.ry() -= 20;
    m_fan_control_popup->move(pos);
    m_fan_control_popup->exec();
}

void StatusPanel::on_thumbnail_enter(QMouseEvent & /*event*/) {}

void StatusPanel::on_thumbnail_leave(QMouseEvent & /*event*/) {}

void StatusPanel::refresh_thumbnail_webrequest(QMouseEvent & /*event*/)
{
    if (!obj) return;
    if (task_thumbnail_state != ThumbnailState::BROKEN_IMG) return;
    auto *mo = reinterpret_cast<Slic3r::MachineObject*>(obj);
    if (mo->slice_info) {
        m_request_url = QString::fromStdString(mo->slice_info->thumbnail_url);
        if (!m_request_url.isEmpty()) {
            if (!m_net_manager) {
                m_net_manager = new QNetworkAccessManager(this);
                connect(m_net_manager, &QNetworkAccessManager::finished,
                        this, &StatusPanel::on_webrequest_state);
            }
            web_request = m_net_manager->get(QNetworkRequest(QUrl(m_request_url)));
            BOOST_LOG_TRIVIAL(trace) << "monitor: retry webrequest for broken thumbnail, url = " << m_request_url.toStdString();
        }
    }
}

void StatusPanel::on_switch_vcamera(QEvent & /*event*/) {}

void StatusPanel::on_camera_enter(QMouseEvent & /*event*/) {}

void StatusPanel::on_camera_leave(QMouseEvent & /*event*/) {}

void StatusPanel::on_show_parts_options(QEvent & /*event*/)
{
    if (obj) {
        auto *real_obj = reinterpret_cast<Slic3r::MachineObject*>(obj);
        if (print_parts_dlg == nullptr) {
            print_parts_dlg = reinterpret_cast<::PrinterPartsDialog*>(new Slic3r::GUI::PrinterPartsDialog(this));
        }
        auto *dlg = reinterpret_cast<Slic3r::GUI::PrinterPartsDialog*>(print_parts_dlg);
        dlg->update_machine_obj(obj);
        dlg->exec();
    }
}

void StatusPanel::on_show_print_options(QEvent & /*event*/)
{
    if (obj) {
        auto *real_obj = reinterpret_cast<Slic3r::MachineObject*>(obj);
        if (print_options_dlg == nullptr) {
            print_options_dlg = reinterpret_cast<::PrintOptionsDialog*>(new Slic3r::GUI::PrintOptionsDialog(this));
        }
        auto *dlg = reinterpret_cast<Slic3r::GUI::PrintOptionsDialog*>(print_options_dlg);
        dlg->update_machine_obj(obj);
        dlg->update_options(obj);
        dlg->exec();
    }
}

void StatusPanel::on_show_safety_options(QEvent & /*event*/)
{
    if (obj) {
        auto *real_obj = reinterpret_cast<Slic3r::MachineObject*>(obj);
        if (DevPrinterConfigUtil::support_safety_options(real_obj->printer_type)) {
            if (safety_options_dlg == nullptr) {
                safety_options_dlg = reinterpret_cast<::SafetyOptionsDialog*>(new Slic3r::GUI::SafetyOptionsDialog(this));
            }
            auto *dlg = reinterpret_cast<Slic3r::GUI::SafetyOptionsDialog*>(safety_options_dlg);
            dlg->update_machine_obj(obj);
            dlg->update_options(obj);
            dlg->exec();
        }
    }
}

void StatusPanel::on_start_calibration(QEvent & /*event*/)
{
    if (obj) {
        auto *real_obj = reinterpret_cast<Slic3r::MachineObject*>(obj);
        if (calibration_dlg == nullptr) {
            calibration_dlg = reinterpret_cast<::CalibrationDialog*>(new Slic3r::GUI::CalibrationDialog());
        }
        auto *dlg = reinterpret_cast<Slic3r::GUI::CalibrationDialog*>(calibration_dlg);
        dlg->update_machine_obj(obj);
        dlg->update_cali(obj);
        dlg->exec();
    }
}

bool StatusPanel::is_stage_list_info_changed(MachineObject * /*obj*/) { return false; }

} // namespace GUI
} // namespace Slic3r
