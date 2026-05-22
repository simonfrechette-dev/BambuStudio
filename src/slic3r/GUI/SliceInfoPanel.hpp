#ifndef slic3r_SliceInfoPanel_hpp_
#define slic3r_SliceInfoPanel_hpp_

#include "slic3r/GUI/MonitorBasePanel.h"
#include "libslic3r/ProjectTask.hpp"
#include "DeviceManager.hpp"
#include "GUI.hpp"
#include <QWidget>
#include <QPixmap>
#include <QImage>
#include <QNetworkReply>
#include <QScrollArea>
#include <QLabel>
#include <QBoxLayout>
#include <QGridLayout>
#include "Widgets/PopupWindow.hpp"

namespace Slic3r {
namespace GUI {

class SliceInfoPopup : public PopupWindow
{
public:
    SliceInfoPopup(QWidget *parent, QPixmap bmp = QPixmap(), BBLSliceInfo* info = nullptr);
    ~SliceInfoPopup() = default;

    virtual void Popup(QWidget *focus = nullptr) override;
    virtual void OnDismiss();
    virtual bool ProcessLeftDown(QMouseEvent *event);
    virtual bool Show(bool show = true);

private:
    QScrollArea *m_panel;

    void OnMouse(QMouseEvent *event);
    void OnSize(QResizeEvent *event);
    void OnSetFocus(QFocusEvent *event);
    void OnKillFocus(QFocusEvent *event);
};

class SliceInfoPanel : public QWidget
{
public:
    QNetworkReply *web_reply = nullptr;

private:
    QImage          m_thumbnail_img;

    QBoxLayout *    m_item_top_sizer;
    QLabel*         m_bmp_item_thumbnail;
    QLabel*         m_bmp_item_prediction;
    QLabel*         m_bmp_item_print;
    QLabel*         m_text_item_prediction;
    QLabel*         m_bmp_item_cost;
    QLabel*         m_text_item_cost;
    QGridLayout*    m_filament_info_sizer;
    QLabel*         m_text_plate_index;

public:
    SliceInfoPanel(QWidget *parent,
                 QPixmap  &prediction,
                 QPixmap  &cost,
                 QPixmap  &print);
    ~SliceInfoPanel();

    void SetImages(QPixmap &prediction, QPixmap &cost, QPixmap &printing);

    void on_subtask_print();
    void on_thumbnail_enter(QMouseEvent *event);
    void on_thumbnail_leave(QMouseEvent *event);

    void on_mouse_enter(QMouseEvent *event);
    void on_mouse_leave(QMouseEvent *event);

    void on_webrequest_state(QNetworkReply *reply);
    void update(BBLSliceInfo* info);
    void msw_rescale();
};


}
}
#endif
