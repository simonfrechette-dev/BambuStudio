// UxProgramTermsDialog.cpp — Qt6 port
#include "UxProgramTermsDialog.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextBrowser>
#include <QPushButton>

#include "GUI_App.hpp"
#include "I18N.hpp"
#include "Widgets/Button.hpp"
#include "Widgets/Label.hpp"

namespace Slic3r { namespace GUI {

UxProgramTermsDialog::UxProgramTermsDialog(QWidget *parent)
    : DPIDialog(parent)
{
    setWindowTitle(_L("User Experience Program Terms"));
    setMinimumSize(580, 480);

    auto *main_layout = new QVBoxLayout(this);
    main_layout->setContentsMargins(24, 24, 24, 20);
    main_layout->setSpacing(12);

    auto *title_lbl = new Label(this, Label::Head_16);
    title_lbl->setText(_L("User Experience Program"));
    main_layout->addWidget(title_lbl);

    // Scrollable terms content area (stub — real version loads from m_host_url)
    auto *text_browser = new QTextBrowser(this);
    text_browser->setReadOnly(true);
    text_browser->setHtml(
        "<p>" +
        _L("By joining the User Experience Improvement Program, you agree to allow BambuLab "
           "to collect anonymised usage data to improve the software and your experience.") +
        "</p>");
    main_layout->addWidget(text_browser, 1);

    // Buttons
    auto *btn_layout = new QHBoxLayout();
    btn_layout->addStretch(1);

    auto *btn_cancel = new Button(this, _L("Decline"));
    btn_layout->addWidget(btn_cancel);

    auto *btn_ok = new Button(this, _L("Accept"));
    btn_ok->SetValue(true);
    btn_layout->addWidget(btn_ok);

    main_layout->addLayout(btn_layout);

    connect(btn_ok,     &Button::clicked, this, &QDialog::accept);
    connect(btn_cancel, &Button::clicked, this, &QDialog::reject);
}

void UxProgramTermsDialog::on_dpi_changed(const QRect & /*suggested_rect*/) {}

}} // namespace Slic3r::GUI
