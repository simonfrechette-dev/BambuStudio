#ifndef slic3r_GUI_StepCtrlBase_hpp_
#define slic3r_GUI_StepCtrlBase_hpp_

#include "StaticBox.hpp"
#include "../QtExtensions.hpp"
#include <QFont>
#include <QString>
#include <vector>

class StepCtrlBase : public StaticBox
{
    Q_OBJECT
protected:
    QFont      font_tip;
    StateColor clr_bar;
    StateColor clr_step;
    StateColor clr_text;
    StateColor clr_tip;
    int radius    = 7;
    int bar_width = 4;

    std::vector<QString> steps;
    std::vector<QString> tips;
    QString hint;
    int step = -1;

public:
    explicit StepCtrlBase(QWidget *parent = nullptr);
    ~StepCtrlBase() override;

    void SetHint(const QString &h);
    bool SetTipFont(const QFont &font);

    int  AppendItem(const QString &item, const QString &tip = {});
    void DeleteAllItems();
    unsigned int GetCount() const;
    int  GetSelection() const;
    void SelectItem(int item);
    void Idle();
    QString GetItemText(unsigned int item) const;
    int     GetItemUseText(const QString &txt) const;
    void    SetItemText(unsigned int item, const QString &value);

signals:
    void stepChanging(int item);
    void stepChanged(int item);

private:
    bool sendStepCtrlEvent(bool changing = false);
};

class StepCtrl : public StepCtrlBase
{
    Q_OBJECT
    ScalableBitmap bmp_thumb;
public:
    explicit StepCtrl(QWidget *parent = nullptr);
    void Rescale();

protected:
    void doRender(QPainter &painter) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QPoint drag_offset;
    bool   dragging = false;
};

class StepIndicator : public StepCtrlBase
{
    Q_OBJECT
    ScalableBitmap bmp_ok;
public:
    explicit StepIndicator(QWidget *parent = nullptr);
    void Rescale();
    void SelectNext();

protected:
    void doRender(QPainter &painter) override;
};

class FilamentStepIndicator : public StepCtrlBase
{
    Q_OBJECT
    ScalableBitmap bmp_ok;
    QString        m_slot_information;
public:
    explicit FilamentStepIndicator(QWidget *parent = nullptr);
    void Rescale();
    void SelectNext();
    void SetSlotInformation(const QString &slot);

protected:
    void doRender(QPainter &painter) override;
};

#endif // !slic3r_GUI_StepCtrlBase_hpp_
