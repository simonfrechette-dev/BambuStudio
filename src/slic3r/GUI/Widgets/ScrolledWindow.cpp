#include "ScrolledWindow.hpp"
#include <QScrollBar>
#include <QVBoxLayout>

ScrolledWindow::ScrolledWindow(QWidget *parent,
                               int /*marginWidth*/,
                               int /*scrollbarWidth*/,
                               int /*tipLength*/)
    : QScrollArea(parent)
    , m_panel(new QWidget(this))
    , m_bothDirections(true)
{
    setWidget(m_panel);
    setWidgetResizable(true);
    setFrameShape(QFrame::NoFrame);
    m_panel->setAutoFillBackground(true);
}

void ScrolledWindow::SetBackgroundColour(QColor color)
{
    QPalette p = m_panel->palette();
    p.setColor(QPalette::Window, color);
    m_panel->setPalette(p);
    m_panel->update();
}

void ScrolledWindow::SetVirtualSize(int x, int y)
{
    m_panel->setMinimumSize(x, y);
}

void ScrolledWindow::SetVirtualSize(QSize size)
{
    m_panel->setMinimumSize(size);
}

void ScrolledWindow::SetScrollbars(int pixelsPerUnitX, int pixelsPerUnitY,
                                    int noUnitsX, int noUnitsY,
                                    int xPos, int yPos, bool /*noRefresh*/)
{
    const int vw = pixelsPerUnitX * noUnitsX;
    const int vh = pixelsPerUnitY * noUnitsY;
    if (vw > 0 || vh > 0)
        SetVirtualSize(vw, vh);
    if (xPos != 0)
        horizontalScrollBar()->setValue(xPos * pixelsPerUnitX);
    if (yPos != 0)
        verticalScrollBar()->setValue(yPos * pixelsPerUnitY);
}
