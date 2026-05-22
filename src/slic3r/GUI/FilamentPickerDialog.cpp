// FilamentPickerDialog.cpp — Qt6 port (stub implementations)
#include "FilamentPickerDialog.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QTimer>
#include <QPainter>

#include "GUI_App.hpp"
#include "I18N.hpp"
#include "Widgets/Label.hpp"

namespace Slic3r { namespace GUI {

FilamentPickerDialog::FilamentPickerDialog(QWidget *parent,
                                           const QString &fila_id,
                                           const FilamentColor &fila_color,
                                           const std::string &fila_type)
    : DPIDialog(parent)
    , m_cur_filament_color(fila_color)
{
    setWindowTitle(_L("Choose Filament Color"));
    setMinimumSize(420, 500);

    auto *main_layout = new QVBoxLayout(this);
    main_layout->setContentsMargins(16, 16, 16, 12);
    main_layout->setSpacing(8);

    // Preview panel
    main_layout->addLayout(CreatePreviewPanel(fila_color, fila_type));

    // Separator
    auto *sep_layout = new QHBoxLayout();
    sep_layout->addLayout(CreateSeparatorLine());
    main_layout->addLayout(sep_layout);

    // Color grid
    main_layout->addWidget(CreateColorGrid(), 1);

    // "More info" link
    CreateMoreInfoButton();
    if (m_more_btn) main_layout->addWidget(m_more_btn);

    // Button row
    main_layout->addLayout(CreateButtonPanel());

    BindEvents();
    LoadFilamentData(fila_id);
}

FilamentPickerDialog::~FilamentPickerDialog()
{
    CleanupTimers();
}

void FilamentPickerDialog::on_dpi_changed(const QRect & /*r*/) {}

QColor FilamentPickerDialog::GetSelectedColour() const
{
    if (m_cur_filament_color.m_colors.empty())
        return QColor(255, 255, 255);
    return *m_cur_filament_color.m_colors.begin();
}

// ─── private helpers ────────────────────────────────────────────────────────

QBoxLayout *FilamentPickerDialog::CreatePreviewPanel(const FilamentColor &fila_color,
                                                      const std::string &fila_type)
{
    auto *row = new QHBoxLayout();

    // Color swatch
    m_color_demo = new QLabel(this);
    m_color_demo->setFixedSize(48, 48);
    {
        QColor swatch_color = fila_color.m_colors.empty()
            ? QColor(255, 255, 255)
            : *fila_color.m_colors.begin();
        m_color_demo->setStyleSheet(
            QString("background: %1; border-radius: 6px;")
                .arg(swatch_color.name()));
    }
    row->addWidget(m_color_demo);

    auto *info_col = new QVBoxLayout();
    SetupLabelsContent(fila_color, fila_type);
    if (m_label_preview_color) info_col->addWidget(m_label_preview_color);
    if (m_label_preview_idx)   info_col->addWidget(m_label_preview_idx);
    if (m_label_preview_type)  info_col->addWidget(m_label_preview_type);
    row->addLayout(info_col, 1);
    return row;
}

QScrollArea *FilamentPickerDialog::CreateColorGrid()
{
    auto *scroll = new QScrollArea(this);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setWidgetResizable(true);
    auto *content = new QWidget(scroll);
    scroll->setWidget(content);
    // color buttons populated in LoadFilamentData
    return scroll;
}

QBoxLayout *FilamentPickerDialog::CreateSeparatorLine()
{
    auto *row = new QHBoxLayout();
    auto *line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    row->addWidget(line);
    return row;
}

void FilamentPickerDialog::CreateMoreInfoButton()
{
    m_more_btn = new Button(this, _L("More Info"));
}

QBoxLayout *FilamentPickerDialog::CreateButtonPanel()
{
    auto *row = new QHBoxLayout();
    row->addStretch(1);
    m_cancel_btn = new Button(this, _L("Cancel"));
    row->addWidget(m_cancel_btn);
    m_ok_btn = new Button(this, _L("OK"));
    m_ok_btn->SetValue(true);
    row->addWidget(m_ok_btn);
    connect(m_ok_btn,     &Button::clicked, this, &QDialog::accept);
    connect(m_cancel_btn, &Button::clicked, this, &QDialog::reject);
    return row;
}

void FilamentPickerDialog::BindEvents()
{
    // Mouse events for shaped-window dragging handled via event filter if needed
}

void FilamentPickerDialog::CreateColorBitmap(const FilamentColor & /*fila_color*/) {}

QBoxLayout *FilamentPickerDialog::CreateInfoSection()
{
    auto *col = new QVBoxLayout();
    m_label_preview_color = new QLabel(this);
    m_label_preview_idx   = new QLabel(this);
    m_label_preview_type  = new QLabel(this);
    col->addWidget(m_label_preview_color);
    col->addWidget(m_label_preview_idx);
    col->addWidget(m_label_preview_type);
    return col;
}

void FilamentPickerDialog::SetupLabelsContent(const FilamentColor &fila_color,
                                               const std::string &fila_type)
{
    if (!m_label_preview_color) m_label_preview_color = new QLabel(this);
    if (!m_label_preview_idx)   m_label_preview_idx   = new QLabel(this);
    if (!m_label_preview_type)  m_label_preview_type  = new QLabel(this);

    m_label_preview_color->setText(
        fila_color.m_colors.empty() ? _L("Custom") : m_cur_color_name ? *m_cur_color_name : _L("Custom"));
    m_label_preview_type->setText(QString::fromStdString(fila_type));
}

void FilamentPickerDialog::UpdatePreview(const FilamentColorCode & /*filament*/) {}
void FilamentPickerDialog::UpdateCustomColorPreview(const QColor & /*color*/) {}
void FilamentPickerDialog::UpdateButtonStates(QPushButton * /*btn*/) {}
void FilamentPickerDialog::SetWindowShape() {}
void FilamentPickerDialog::CreateShapedBitmap() {}

bool FilamentPickerDialog::LoadFilamentData(const QString & /*fila_id*/)
{
    m_is_data_loaded = false;
    return false;
}

QColor FilamentPickerDialog::GetSingleColorData()
{
    return GetSelectedColour();
}

void FilamentPickerDialog::StartFlashing()
{
    if (!m_flash_timer) {
        m_flash_timer = new QTimer(this);
        connect(m_flash_timer, &QTimer::timeout, this, [this]() {
            ++m_flash_step;
            if (m_flash_step >= 6) {
                m_flash_timer->stop();
                m_flash_step = 0;
            }
            update();
        });
    }
    m_flash_step = 0;
    m_flash_timer->start(80);
}

bool FilamentPickerDialog::IsClickOnTopMostWindow(const QPoint & /*pos*/)
{
    return true;
}

void FilamentPickerDialog::StartClickDetection()
{
    if (!m_click_timer) {
        m_click_timer = new QTimer(this);
        m_click_timer->setSingleShot(true);
        m_click_timer->setInterval(200);
    }
    m_click_timer->start();
}

void FilamentPickerDialog::StopClickDetection()
{
    if (m_click_timer) m_click_timer->stop();
}

void FilamentPickerDialog::CleanupTimers()
{
    if (m_click_timer) { m_click_timer->stop(); }
    if (m_flash_timer) { m_flash_timer->stop(); }
}

void FilamentPickerDialog::OnMouseLeftDown(QMouseEvent & /*e*/) { m_last_mouse_down = true; }
void FilamentPickerDialog::OnMouseMove(QMouseEvent & /*e*/) {}
void FilamentPickerDialog::OnMouseLeftUp(QMouseEvent & /*e*/) { m_last_mouse_down = false; }
void FilamentPickerDialog::OnButtonPaint(QPaintEvent & /*e*/) {}
void FilamentPickerDialog::OnTimerCheck(QTimerEvent & /*e*/) {}
void FilamentPickerDialog::OnFlashTimer(QTimerEvent & /*e*/) {}

}} // namespace Slic3r::GUI
