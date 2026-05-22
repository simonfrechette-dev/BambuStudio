#ifndef slic3r_BitmapComboBox_hpp_
#define slic3r_BitmapComboBox_hpp_

#include <QComboBox>
#include <QPixmap>
#include <QString>

#include "GUI_Utils.hpp"

// ---------------------------------
// ***  BitmapComboBox  ***
// ---------------------------------
namespace Slic3r {
namespace GUI {

// BitmapComboBox used to presets list on Sidebar and Tabs
class BitmapComboBox : public QComboBox
{
    Q_OBJECT
public:
    BitmapComboBox(QWidget* parent = nullptr);
    ~BitmapComboBox();

    int Append(const QString& item);
    int Append(const QString& item, const QPixmap& bitmap);

    void Rescale() {}
};

    }}
#endif
