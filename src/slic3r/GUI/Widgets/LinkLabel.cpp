#include "LinkLabel.hpp"

#include <QVBoxLayout>
#include <QMouseEvent>
#include <QDesktopServices>
#include <QUrl>

LinkLabel::LinkLabel(QWidget *parent, const QString &text,
                     const std::string &url, long style,
                     const QSize &size)
    : QWidget(parent)
    , m_url(QString::fromStdString(url))
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    m_txt = new Label(this, text, Label::LB_HYPERLINK | style);
    layout->addWidget(m_txt);
    setLayout(layout);
    if (!size.isEmpty()) setFixedSize(size);
    setCursor(Qt::PointingHandCursor);
}

void LinkLabel::setLinkUrl(const QString &url) { m_url = url; }

void LinkLabel::setLabel(const QString &label)
{
    if (m_txt) m_txt->setText(label);
}

bool LinkLabel::setLinkLabelFColour(const QColor &colour)
{
    if (!m_txt) return false;
    QPalette p = m_txt->palette();
    p.setColor(QPalette::WindowText, colour);
    m_txt->setPalette(p);
    return true;
}

bool LinkLabel::setLinkLabelBColour(const QColor & /*colour*/) { return true; }

void LinkLabel::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (!m_url.isEmpty())
            QDesktopServices::openUrl(QUrl(m_url));
        emit linkClicked(m_url);
        event->accept();
    } else {
        QWidget::mousePressEvent(event);
    }
}
