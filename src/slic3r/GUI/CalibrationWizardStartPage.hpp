#ifndef slic3r_GUI_CalibrationWizardStartPage_hpp_
#define slic3r_GUI_CalibrationWizardStartPage_hpp_

#include "CalibrationWizardPage.hpp"

namespace Slic3r { namespace GUI {



class CalibrationStartPage : public CalibrationWizardPage
{
public:
    CalibrationStartPage(QWidget* parent,
        int id = -1,
        const QPoint& pos = QPoint(),
        const QSize& size = QSize(),
        long style = 0);

protected:
    CalibMode m_cali_mode;

    QBoxLayout*   m_top_sizer{ nullptr };
    QBoxLayout*   m_images_sizer{ nullptr };
    Label*        m_when_title{ nullptr };
    Label*        m_when_content{ nullptr };
    Label*        m_about_title{ nullptr };
    Label*        m_about_content{ nullptr };
    QLabel* m_before_bmp{ nullptr };
    QLabel* m_after_bmp{ nullptr };
    QLabel* m_bmp_intro{ nullptr };
    PAPageHelpPanel* m_help_panel{ nullptr };

    void create_when(QWidget* parent, QString title, QString content);
    void create_about(QWidget* parent, QString title, QString content);
    void create_bitmap(QWidget* parent, const QPixmap& before_img, const QPixmap& after_img);
    void create_bitmap(QWidget* parent, std::string before_img, std::string after_img);
    void create_bitmap(QWidget* parent, std::string img);
};

class CalibrationPAStartPage : public CalibrationStartPage
{
public:
    CalibrationPAStartPage(QWidget* parent,
        int id = -1,
        const QPoint& pos = QPoint(),
        const QSize& size = QSize(),
        long style = 0);

    void create_page(QWidget* parent);

    void on_reset_page();
    void on_device_connected(MachineObject* obj);
    void msw_rescale() override;

    virtual void update(MachineObject *obj) override;
};

class CalibrationFlowRateStartPage : public CalibrationStartPage
{
public:
    CalibrationFlowRateStartPage(QWidget* parent,
        int id = -1,
        const QPoint& pos = QPoint(),
        const QSize& size = QSize(),
        long style = 0);

    void create_page(QWidget* parent);
    void on_reset_page();
    void on_device_connected(MachineObject* obj);
    void msw_rescale() override;

    virtual void update(MachineObject *obj) override;
};

class CalibrationMaxVolumetricSpeedStartPage : public CalibrationStartPage
{
public:
    CalibrationMaxVolumetricSpeedStartPage(QWidget* parent,
        int id = -1,
        const QPoint& pos = QPoint(),
        const QSize& size = QSize(),
        long style = 0);

    void create_page(QWidget* parent);
    void msw_rescale() override;
};

}} // namespace Slic3r::GUI

#endif