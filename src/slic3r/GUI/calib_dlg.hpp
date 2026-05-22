#ifndef slic3r_calib_dlg_hpp_
#define slic3r_calib_dlg_hpp_
#include <QWidget>
#include <QString>

#include "wxExtensions.hpp"
#include "GUI_Utils.hpp"
#include "Widgets/RadioBox.hpp"
#include "Widgets/Button.hpp"
#include "Widgets/RoundedRectangle.hpp"
#include "Widgets/Label.hpp"
#include "Widgets/CheckBox.hpp"
#include "Widgets/ComboBox.hpp"
#include "Widgets/TextInput.hpp"
#include "GUI_App.hpp"
#include "libslic3r/Calib.hpp"

namespace Slic3r { namespace GUI {

class PA_Calibration_Dlg : public DPIDialog
{
public:
    PA_Calibration_Dlg(QWidget* parent, int id, Plater* plater);
    ~PA_Calibration_Dlg();
    void on_dpi_changed(const QRect& suggested_rect) override;
	void on_show(QShowEvent& event);
protected:
    void reset_params();
	virtual void on_start(QEvent& event);
	virtual void on_extruder_type_changed(QEvent& event);
	virtual void on_method_changed(QEvent& event);

protected:
	bool m_hasBowdenExtruder{false};
	int  m_bowdenExtruderId{-1};
	Calib_Params m_params;

	QGroupBox* m_rbExtruderType{nullptr};
	QGroupBox* m_rbMethod;
	TextInput* m_tiStartPA;
	TextInput* m_tiEndPA;
	TextInput* m_tiPAStep;
	CheckBox* m_cbPrintNum;
	Button* m_btnStart;

	Plater* m_plater;
};

class Temp_Calibration_Dlg : public DPIDialog
{
public:
    Temp_Calibration_Dlg(QWidget* parent, int id, Plater* plater);
    ~Temp_Calibration_Dlg();
    void on_dpi_changed(const QRect& suggested_rect) override;

protected:
    
    virtual void on_start(QEvent& event);
    virtual void on_filament_type_changed(QEvent& event);
    Calib_Params m_params;

    QGroupBox* m_rbFilamentType;
    TextInput* m_tiStart;
    TextInput* m_tiEnd;
    TextInput* m_tiStep;
    Button* m_btnStart;
    Plater* m_plater;
};

class MaxVolumetricSpeed_Test_Dlg : public DPIDialog
{
public:
    MaxVolumetricSpeed_Test_Dlg(QWidget* parent, int id, Plater* plater);
    ~MaxVolumetricSpeed_Test_Dlg();
    void on_dpi_changed(const QRect& suggested_rect) override;

protected:

    virtual void on_start(QEvent& event);
    Calib_Params m_params;

    TextInput* m_tiStart;
    TextInput* m_tiEnd;
    TextInput* m_tiStep;
    Button* m_btnStart;
    Plater* m_plater;
};

class VFA_Test_Dlg : public DPIDialog {
public:
    VFA_Test_Dlg(QWidget* parent, int id, Plater* plater);
    ~VFA_Test_Dlg();
    void on_dpi_changed(const QRect& suggested_rect) override;

protected:
    virtual void on_start(QEvent& event);
    Calib_Params m_params;

    TextInput* m_tiStart;
    TextInput* m_tiEnd;
    TextInput* m_tiStep;
    Button* m_btnStart;
    Plater* m_plater;
};


class Retraction_Test_Dlg : public DPIDialog
{
public:
    Retraction_Test_Dlg (QWidget* parent, int id, Plater* plater);
    ~Retraction_Test_Dlg ();
    void on_dpi_changed(const QRect& suggested_rect) override;

protected:

    virtual void on_start(QEvent& event);
    Calib_Params m_params;

    TextInput* m_tiStart;
    TextInput* m_tiEnd;
    TextInput* m_tiStep;
    Button* m_btnStart;
    Plater* m_plater;
};

}} // namespace Slic3r::GUI

#endif
