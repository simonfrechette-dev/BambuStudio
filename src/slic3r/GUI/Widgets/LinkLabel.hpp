#ifndef slic3r_GUI_LinkLabel_hpp_
#define slic3r_GUI_LinkLabel_hpp_

#include "Label.hpp"
#include <QWidget>
#include <QString>

class LinkLabel : public QWidget
{
    Q_OBJECT
public:
    LinkLabel(QWidget *parent, const QString &text,
              const std::string &url, long style = 0,
              const QSize &size = {});

    void setLinkUrl(const QString &url);
    void setLabel(const QString &label);
    bool setLinkLabelFColour(const QColor &colour);
    bool setLinkLabelBColour(const QColor &colour);

    Label *getLabel() { return m_txt; }

signals:
    void linkClicked(const QString &url);

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    QString m_url;
    Label  *m_txt{nullptr};
};

#endif // !slic3r_GUI_LinkLabel_hpp_
