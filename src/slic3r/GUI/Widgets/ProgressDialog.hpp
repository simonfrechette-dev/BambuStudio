#ifndef slic3r_GUI_PROGRESSDIALOG_hpp_
#define slic3r_GUI_PROGRESSDIALOG_hpp_

#include <QDialog>
#include <QProgressBar>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QString>

namespace Slic3r { namespace GUI {

class ProgressDialog : public QDialog
{
    Q_OBJECT
public:
    ProgressDialog();
    ProgressDialog(const QString &title, const QString &message,
                   int maximum = 100, QWidget *parent = nullptr,
                   int style = 0, bool adaptive = false);

    virtual ~ProgressDialog();

    bool init(const QString &title, const QString &message,
              int maximum = 100, QWidget *parent = nullptr, int style = 0);

    virtual bool Update(int value, const QString &newmsg = {},
                        bool *skip = nullptr);
    virtual bool Pulse(const QString &newmsg = {}, bool *skip = nullptr);
    bool WasCanceled() const;

    virtual void Resume() {}

    virtual int     GetValue() const;
    virtual int     GetRange() const;
    virtual QString GetMessage() const;
    virtual void    SetRange(int maximum);

    virtual bool WasCancelled() const;
    virtual bool WasSkipped() const;

    virtual void OnCancel() {}

signals:
    void cancelled();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QProgressBar *m_gauge       = nullptr;
    QLabel       *m_msg_label   = nullptr;
    QPushButton  *m_cancel_btn  = nullptr;
    bool          m_cancelled   = false;
    int           m_maximum     = 100;
};

}} // namespace Slic3r::GUI

#endif // !slic3r_GUI_PROGRESSDIALOG_hpp_
