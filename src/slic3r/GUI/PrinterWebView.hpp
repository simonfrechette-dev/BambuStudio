#ifndef slic3r_PrinterWebView_hpp_
#define slic3r_PrinterWebView_hpp_

#include <QWidget>
#include <QString>

namespace Slic3r {
namespace GUI {

class PrinterWebView : public QWidget {
    Q_OBJECT
public:
    PrinterWebView(QWidget *parent = nullptr);
    virtual ~PrinterWebView();

    void load_url(const QString& url);
    void UpdateState();
};

} // GUI
} // Slic3r

#endif
