#pragma once

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QBoxLayout>
#include <QPixmap>
#include <string>


namespace Slic3r {

class MachineObject;
namespace GUI {

class ThermalPreconditioningDialog : public QDialog
{
    Q_OBJECT
public:
    ThermalPreconditioningDialog(QWidget *parent, std::string dev_id, bool is_show_remain_time);
    ~ThermalPreconditioningDialog();

    void update_thermal_remaining_time();

private:
    void create_ui();
    void on_ok_clicked();
    void on_timer();

    std::string  m_dev_id;
    QTimer      *m_refresh_timer;
    QLabel      *m_remaining_time_label;
    QLabel      *m_explanation_label;
    QPushButton *m_ok_button;
    QLabel      *m_title_bitmap;
};

} // namespace GUI
} // namespace Slic3r