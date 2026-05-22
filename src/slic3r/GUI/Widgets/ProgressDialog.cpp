#include "ProgressDialog.hpp"

namespace Slic3r { namespace GUI {

ProgressDialog::ProgressDialog() : QDialog(nullptr) {}

ProgressDialog::ProgressDialog(const QString &title, const QString &message,
                                int maximum, QWidget *parent,
                                int style, bool /*adaptive*/)
    : QDialog(parent)
{
    init(title, message, maximum, parent, style);
}

ProgressDialog::~ProgressDialog() = default;

bool ProgressDialog::init(const QString &title, const QString &message,
                           int maximum, QWidget * /*parent*/, int /*style*/)
{
    setWindowTitle(title);
    m_maximum = maximum;

    auto *layout = new QVBoxLayout(this);
    m_msg_label  = new QLabel(message, this);
    layout->addWidget(m_msg_label);

    m_gauge = new QProgressBar(this);
    m_gauge->setRange(0, maximum);
    m_gauge->setValue(0);
    layout->addWidget(m_gauge);

    m_cancel_btn = new QPushButton(tr("Cancel"), this);
    layout->addWidget(m_cancel_btn, 0, Qt::AlignRight);

    connect(m_cancel_btn, &QPushButton::clicked, this, [this]() {
        m_cancelled = true;
        emit cancelled();
    });

    setLayout(layout);
    return true;
}

bool ProgressDialog::Update(int value, const QString &newmsg, bool * /*skip*/)
{
    if (m_gauge)     m_gauge->setValue(std::clamp(value, 0, m_maximum));
    if (m_msg_label && !newmsg.isEmpty()) m_msg_label->setText(newmsg);
    QApplication::processEvents();
    return !m_cancelled;
}

bool ProgressDialog::Pulse(const QString &newmsg, bool * /*skip*/)
{
    if (m_gauge)     m_gauge->setValue((m_gauge->value() + 1) % (m_maximum + 1));
    if (m_msg_label && !newmsg.isEmpty()) m_msg_label->setText(newmsg);
    QApplication::processEvents();
    return !m_cancelled;
}

bool ProgressDialog::WasCanceled()  const { return m_cancelled; }
bool ProgressDialog::WasCancelled() const { return m_cancelled; }
bool ProgressDialog::WasSkipped()   const { return false; }

int     ProgressDialog::GetValue()   const { return m_gauge ? m_gauge->value() : 0; }
int     ProgressDialog::GetRange()   const { return m_maximum; }
QString ProgressDialog::GetMessage() const { return m_msg_label ? m_msg_label->text() : QString{}; }

void ProgressDialog::SetRange(int maximum)
{
    m_maximum = maximum;
    if (m_gauge) m_gauge->setRange(0, maximum);
}

void ProgressDialog::paintEvent(QPaintEvent *event) { QDialog::paintEvent(event); }

}} // namespace Slic3r::GUI
