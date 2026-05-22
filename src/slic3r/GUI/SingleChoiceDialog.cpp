#include "SingleChoiceDialog.hpp"
#include "I18N.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

namespace Slic3r { namespace GUI {

SingleChoiceDialog::SingleChoiceDialog(const QString &message, const QString &caption,
                                       const QStringList &choices, int initialSelection,
                                       QWidget *parent)
    : DPIDialog(parent)
{
    setWindowTitle(caption);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    auto *vbox = new QVBoxLayout(this);
    vbox->setContentsMargins(20, 20, 20, 20);
    vbox->setSpacing(15);

    auto *lbl = new QLabel(message, this);
    lbl->setWordWrap(true);
    vbox->addWidget(lbl);

    type_comboBox = new ComboBox(this);
    for (const auto &choice : choices)
        type_comboBox->Append(choice);
    if (initialSelection >= 0 && initialSelection < choices.size())
        type_comboBox->SetSelection(initialSelection);
    vbox->addWidget(type_comboBox);

    auto *btn_row = new QHBoxLayout;
    btn_row->addStretch(1);

    m_button_cancel = new Button(this, _L("Cancel"));
    connect(m_button_cancel, &Button::clicked, this, &QDialog::reject);
    btn_row->addWidget(m_button_cancel);

    m_button_ok = new Button(this, _L("OK"));
    m_button_ok->SetValue(true);
    connect(m_button_ok, &Button::clicked, this, &QDialog::accept);
    btn_row->addWidget(m_button_ok);

    vbox->addLayout(btn_row);
    adjustSize();
}

SingleChoiceDialog::~SingleChoiceDialog() = default;

int SingleChoiceDialog::GetSingleChoiceIndex()
{
    return type_comboBox ? type_comboBox->GetSelection() : 0;
}

void SingleChoiceDialog::on_dpi_changed(const QRect &) {}

}} // namespace Slic3r::GUI
