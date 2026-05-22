#ifndef slic3r_PublishDialog_hpp_
#define slic3r_PublishDialog_hpp_
#include <QWidget>
#include <QString>


#include "I18N.hpp"

#include "wxExtensions.hpp"
#include "Plater.hpp"
#include "Widgets/StepCtrl.hpp"
#include "Widgets/ProgressDialog.hpp"
#include "Widgets/Button.hpp"
#include "Widgets/ProgressBar.hpp"


namespace Slic3r {
namespace GUI {

enum PublishStep {
    STEP_SLICING = 0,
    STEP_PACKING,
    STEP_UPLOADING,
    STEP_FILL_INFO,
    STEP_PUBLISH_COUNT,
};

class PublishDialog : public DPIDialog
{
public:
    PublishDialog(Plater* plater = nullptr);

    bool UpdateStatus(QString &msg, int percent = -1, bool yeild = true);
    void Pulse(QString &msg, bool &skip);
    void SetPublishStep(PublishStep step, bool yeild = false, int percent = -1);
    void start_slicing();
    void reset();
    bool was_cancelled() { return m_was_cancelled; }
    void cancel();

protected:
    QWidget*     m_step_panel;
    ::StepIndicator *m_publish_steps;
    QLabel *m_text_note;
    QLabel *m_text_progress;
    ProgressBar  *m_progress;
    Button*       m_btn_cancel;
    QLabel *m_text_errors;
    Plater *      m_plater{nullptr};
    bool          m_was_cancelled { false };

    QBoxLayout* create_publish_step_sizer();
    void on_close(QCloseEvent &event);
    void on_dpi_changed(const QRect &suggested_rect) override;
};

} // GUI
} // Slic3r


#endif  /* slic3r_BedShapeDialog_hpp_ */
