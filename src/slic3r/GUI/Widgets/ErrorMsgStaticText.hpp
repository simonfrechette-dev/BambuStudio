#ifndef _WX_ERRORMSGSTATTEXT_H_
#define _WX_ERRORMSGSTATTEXT_H_

#include <QWidget>
#include <QString>

class ErrorMsgStaticText : public QWidget
{
    Q_OBJECT
public:
    QString m_msg;

    ErrorMsgStaticText(QWidget *parent = nullptr, int id = -1,
                       const QPoint &pos = {}, const QSize &size = {});

    void SetLabel(const QString &msg) { m_msg = msg; update(); }

protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // _WX_ERRORMSGSTATTEXT_H_
