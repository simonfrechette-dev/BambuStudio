#pragma once
#include <QScrollBar>
#include <QColor>

// Thin wrapper kept for API compatibility. Qt's QScrollBar handles
// all rendering natively; this class adds the colour-hint API.
class MyScrollbar : public QScrollBar
{
    Q_OBJECT
public:
    explicit MyScrollbar(Qt::Orientation orientation, QWidget *parent = nullptr)
        : QScrollBar(orientation, parent) {}

    void SetViewStart(int) {}
    void SetTipColor(QColor) {}
    void SetMarginColor(QColor) {}
    void SetScrollbarColor(QColor) {}
    void SetScrollbarTip(int) {}
    void SetVirtualDim(int /*pixelsPerUnit*/, int /*noUnits*/) {}
};
