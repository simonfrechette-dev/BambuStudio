#pragma once
#include <QScrollArea>
#include <QWidget>
#include <QColor>

/**
 * wxScrolled<QWidget> equivalent.
 * Wraps QScrollArea but exposes the same public API used by BambuStudio.
 * Content is placed inside getPanel() (the scroll area's viewport widget).
 */
class ScrolledWindow : public QScrollArea
{
    Q_OBJECT
public:
    explicit ScrolledWindow(QWidget *parent,
                            int marginWidth    = 0,
                            int scrollbarWidth = 4,
                            int tipLength      = 0);

    // Returns the inner content widget — add child widgets here.
    QWidget *GetPanel() { return m_panel; }

    void SetTipColor(QColor) {}
    void SetMarginColor(QColor) {}
    void SetScrollbarColor(QColor) {}
    void SetScrollbarTip(int) {}

    void Refresh() { update(); }
    void SetBackgroundColour(QColor color);

    void SetVirtualSize(int x, int y);
    void SetVirtualSize(QSize size);

    bool IsBothDirections() const { return m_bothDirections; }

    // Compatibility shim: unit-based virtual size setter (like wxScrolled::SetScrollbars).
    void SetScrollbars(int pixelsPerUnitX, int pixelsPerUnitY,
                       int noUnitsX,       int noUnitsY,
                       int xPos = 0,       int yPos = 0,
                       bool /*noRefresh*/ = false);

private:
    QWidget *m_panel;
    bool     m_bothDirections;
};
