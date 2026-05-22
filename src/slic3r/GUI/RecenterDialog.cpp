#include "RecenterDialog.hpp"
#include "I18N.hpp"
#include "QtExtensions.hpp"
#include <QPainter>
#include <QVBoxLayout>
#include <QHBoxLayout>

namespace Slic3r { namespace GUI {

RecenterDialog::RecenterDialog(QWidget *parent, int, const QString &title,
                               const QPoint &, const QSize &, long)
    : DPIDialog(parent)
{
    setWindowTitle(title.isEmpty() ? _L("Re-center") : title);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet("background-color: white;");

    init_bitmap();

    auto *outer = new QVBoxLayout(this);
    outer->setContentsMargins(20, 20, 20, 20);
    outer->setSpacing(15);

    m_bitmap_home = new QLabel(this);
    if (!m_home_bmp.bmp().isNull())
        m_bitmap_home->setPixmap(m_home_bmp.bmp());
    m_bitmap_home->setAlignment(Qt::AlignCenter);
    outer->addWidget(m_bitmap_home);

    hint1 = _L("This operation will re-center the hotend to the starting point.");
    hint2 = _L("Please confirm before continuing.");

    m_staticText_hint = new QLabel(hint1 + "\n" + hint2, this);
    m_staticText_hint->setWordWrap(true);
    m_staticText_hint->setAlignment(Qt::AlignCenter);
    outer->addWidget(m_staticText_hint);

    auto *btn_row = new QHBoxLayout;
    btn_row->addStretch(1);

    m_button_close = new Button(this, _L("Cancel"));
    connect(m_button_close, &Button::clicked, this, &QDialog::reject);
    btn_row->addWidget(m_button_close);

    m_button_confirm = new Button(this, _L("Confirm"));
    m_button_confirm->SetValue(true);
    connect(m_button_confirm, &Button::clicked, this, &QDialog::accept);
    btn_row->addWidget(m_button_confirm);

    outer->addLayout(btn_row);
    adjustSize();
    setFixedSize(sizeHint());
}

RecenterDialog::~RecenterDialog() = default;

void RecenterDialog::init_bitmap()
{
    m_home_bmp = ScalableBitmap(this, "homing", 48);
}

void RecenterDialog::OnPaint(QPaintEvent &) {}
void RecenterDialog::render(QPainter &) {}
void RecenterDialog::on_button_confirm(QEvent &) { accept(); }
void RecenterDialog::on_button_close(QEvent &)   { reject(); }
void RecenterDialog::on_dpi_changed(const QRect &) {}

}} // namespace Slic3r::GUI
#include "RecenterDialog.hpp"

// All implementations are stubs pending Qt port.
