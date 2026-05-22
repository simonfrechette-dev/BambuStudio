#ifndef slic3r_MonitorPage_hpp_
#define slic3r_MonitorPage_hpp_

#include <QWidget>
#include <QBoxLayout>

namespace Slic3r {
namespace GUI {

class MonitorPage : public QWidget
{
private:
    QBoxLayout* m_main_sizer;
    QBoxLayout* m_content_sizer;
public:
    explicit MonitorPage(QWidget *parent = nullptr);
    ~MonitorPage();
    void msw_rescale() {}
};


}
}
#endif
