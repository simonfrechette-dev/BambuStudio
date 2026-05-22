// StepMeshDialog.cpp — Qt6 port
#include "StepMeshDialog.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QDoubleValidator>
#include <QLineEdit>
#include <QLabel>

#include "GUI_App.hpp"
#include "I18N.hpp"
#include "Widgets/Label.hpp"

// Custom QEvent type for mesh-done notification
static const QEvent::Type STEP_MESH_DONE_EVT =
    static_cast<QEvent::Type>(QEvent::registerEventType());

StepMeshDialog::StepMeshDialog(QWidget *parent, Slic3r::Step &file,
                               double linear_init, double angle_init)
    : Slic3r::GUI::DPIDialog(parent)
    , m_file(file)
    , m_last_linear(linear_init)
    , m_last_angle(angle_init)
{
    setWindowTitle(_L("Import STEP Mesh Settings"));
    setMinimumWidth(420);

    auto *main_layout = new QVBoxLayout(this);
    main_layout->setContentsMargins(20, 20, 20, 16);
    main_layout->setSpacing(12);

    // Title
    auto *title = new Label(this, Label::Head_16);
    title->setText(_L("Mesh Quality Settings"));
    main_layout->addWidget(title);

    // Description
    auto *desc = new Label(this, Label::Body_12);
    desc->setWordWrap(true);
    desc->setText(_L("Adjust tessellation tolerances for the STEP file mesh. "
                     "Smaller values produce finer meshes with more triangles."));
    main_layout->addWidget(desc);

    // Parameters form
    auto *form = new QFormLayout();
    form->setSpacing(8);

    // Linear deflection
    auto *linear_edit = new QLineEdit(this);
    linear_edit->setValidator(new QDoubleValidator(0.0001, 10.0, 6, linear_edit));
    linear_edit->setText(QString::number(linear_init));
    m_linear_last = linear_edit->text();
    connect(linear_edit, &QLineEdit::textChanged, this, [this, linear_edit](const QString &v) {
        m_linear_last = v;
        update_mesh_number_text();
    });
    form->addRow(_L("Linear deflection:"), linear_edit);

    // Angular deflection
    auto *angle_edit = new QLineEdit(this);
    angle_edit->setValidator(new QDoubleValidator(0.01, 90.0, 4, angle_edit));
    angle_edit->setText(QString::number(angle_init));
    m_angle_last = angle_edit->text();
    connect(angle_edit, &QLineEdit::textChanged, this, [this, angle_edit](const QString &v) {
        m_angle_last = v;
        update_mesh_number_text();
    });
    form->addRow(_L("Angular deflection (°):"), angle_edit);

    main_layout->addLayout(form);

    // Split compound checkbox
    m_split_compound_checkbox = new QCheckBox(_L("Split compound shapes"), this);
    m_split_compound_checkbox->setChecked(true);
    main_layout->addWidget(m_split_compound_checkbox);

    // Preview mesh checkbox (enable/disable preview)
    m_checkbox = new QCheckBox(_L("Preview mesh count"), this);
    m_checkbox->setChecked(false);
    main_layout->addWidget(m_checkbox);

    // Triangle count label
    mesh_face_number_text = new QLabel("", this);
    mesh_face_number_text->setStyleSheet("color: #6B7280;");
    main_layout->addWidget(mesh_face_number_text);

    main_layout->addStretch(1);

    // Buttons
    auto *btn_layout = new QHBoxLayout();
    btn_layout->addStretch(1);

    m_button_cancel = new Button(this, _L("Cancel"));
    btn_layout->addWidget(m_button_cancel);

    m_button_ok = new Button(this, _L("Import"));
    m_button_ok->SetValue(true);
    btn_layout->addWidget(m_button_ok);

    main_layout->addLayout(btn_layout);

    connect(m_button_ok,     &Button::clicked, this, &QDialog::accept);
    connect(m_button_cancel, &Button::clicked, this, &QDialog::reject);
}

StepMeshDialog::~StepMeshDialog()
{
    stop_task();
}

void StepMeshDialog::on_dpi_changed(const QRect & /*suggested_rect*/) {}

bool StepMeshDialog::validate_number_range(const QString &value, double min, double max)
{
    bool ok = false;
    double v = value.toDouble(&ok);
    return ok && v >= min && v <= max;
}

void StepMeshDialog::update_mesh_number_text()
{
    if (!m_checkbox || !m_checkbox->isChecked()) return;
    bool ok1, ok2;
    double lin = m_linear_last.toDouble(&ok1);
    double ang = m_angle_last.toDouble(&ok2);
    if (ok1 && ok2) {
        unsigned int n = m_file.get_triangle_num(lin, ang);
        mesh_face_number_text->setText(
            _L("Estimated triangles: ") + QString::number(n));
    }
}

void StepMeshDialog::on_task_done(QEvent & /*event*/) {}

void StepMeshDialog::stop_task()
{
    if (m_task) {
        m_file.m_stop_mesh = true;
        if (m_task->joinable()) m_task->join();
        delete m_task;
        m_task = nullptr;
    }
}
