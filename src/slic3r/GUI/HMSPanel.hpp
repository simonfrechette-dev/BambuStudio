#ifndef slic3r_HMSPanel_hpp_
#define slic3r_HMSPanel_hpp_
#include <QWidget>
#include <QLabel>
#include <QTextBrowser>
#include <QScrollArea>
#include <QBoxLayout>

#include <slic3r/GUI/Widgets/Button.hpp>
#include <slic3r/GUI/DeviceManager.hpp>
#include <slic3r/GUI/Widgets/ScrolledWindow.hpp>
#include <slic3r/GUI/StatusPanel.hpp>

#include "DeviceCore/DevHMS.h"

namespace Slic3r {
namespace GUI {

class HMSNotifyItem : public QWidget
{
    DevHMSItem &   m_hms_item;
    std::string m_url;
    std::string dev_id;
    std::string long_error_code;

    QWidget *       m_panel_hms;
    QLabel *m_bitmap_notify;
    QLabel *m_bitmap_arrow;
    QLabel *  m_hms_content;
    QTextBrowser *  m_html;
    QWidget *       m_staticline;

    QPixmap m_img_notify_lv1;
    QPixmap m_img_notify_lv2;
    QPixmap m_img_notify_lv3;
    QPixmap m_img_arrow;

    void          init_bitmaps();
    QPixmap &    get_notify_bitmap();

public:
     HMSNotifyItem(const std::string& dev_id, QWidget *parent, DevHMSItem& item);
    ~HMSNotifyItem();

     void msw_rescale() {}
};


class HMSPanel : public QWidget
{
protected:
    QScrollArea *m_scrolledWindow;
    QBoxLayout *      m_top_sizer;

    int last_status;

    void append_hms_panel(const std::string& dev_id, DevHMSItem &item);
    void delete_hms_panels();


public:
    HMSPanel(QWidget *parent, int id = -1, const QPoint &pos = QPoint(), const QSize &size = QSize(), long style = 0);
    ~HMSPanel();

    void msw_rescale() {}

    void setVisible(bool show) override;

    void update(MachineObject *obj_);

    void show_status(int status);

    void clear_hms_tag();

    MachineObject *obj { nullptr };
    std::map<std::string, DevHMSItem>    temp_hms_list;
};


}
}

#endif
