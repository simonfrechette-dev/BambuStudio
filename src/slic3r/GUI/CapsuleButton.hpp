#ifndef CAPSULE_BUTTON_HPP
#define CAPSULE_BUTTON_HPP

#include "QtExtensions.hpp"
#include "Widgets/Label.hpp"

#include <QWidget>
#include <QString>

namespace Slic3r { namespace GUI {

class CapsuleButton : public QWidget
{
    Q_OBJECT
public:
    CapsuleButton(QWidget *parent, const QString &label, bool selected);
    void Select(bool selected);
    bool IsSelected() const { return m_selected; }
signals:
    void clicked();
protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
private:
    void UpdateStatus();
    Label     *m_label    = nullptr;
    BBLPixmap  m_tag_on;
    BBLPixmap  m_tag_off;
    bool       m_hovered  = false;
    bool       m_selected = false;
};

}} // namespace Slic3r::GUI

#endif // CAPSULE_BUTTON_HPP
