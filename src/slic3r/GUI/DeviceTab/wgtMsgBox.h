#ifndef WGTMSGBOX_H
#define WGTMSGBOX_H
#include <QWidget>
#include <QString>


#include "slic3r/GUI/Widgets/StaticBox.hpp"

class Label;
namespace Slic3r::GUI
{
class wgtMsgBox : public StaticBox
{
public:
    wgtMsgBox(QWidget* parent);

public:
    Label* GetTextLabel() const { return m_txt_label; }

private:
    Label* m_txt_label;

private:
    void CreateGUI();
    void OnCloseClicked(QEvent& evt);
};
}

#endif // WGTMSGBOX_H