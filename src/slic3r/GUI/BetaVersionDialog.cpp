// BetaVersionDialog.cpp — Qt6 port
#include "BetaVersionDialog.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

#include "GUI_App.hpp"
#include "I18N.hpp"

namespace Slic3r { namespace GUI {

BetaVersionDialog::BetaVersionDialog(QWidget *parent)
    : DPIDialog(parent)
{
    setWindowTitle(_L("Software Update"));
    setMinimumWidth(500);

    auto *main_layout = new QVBoxLayout(this);
    main_layout->setContentsMargins(24, 24, 24, 20);
    main_layout->setSpacing(12);

    // Heading label
    m_heading_label = new Label(this, Label::Head_16);
    m_heading_label->setText(_L("New Beta Version Available"));
    main_layout->addWidget(m_heading_label);

    m_version_label = new Label(this, Label::Body_14);
    main_layout->addWidget(m_version_label);

    // Overview text
    m_overview_label = new Label(this, Label::Body_14);
    m_overview_label->setWordWrap(true);
    m_overview_label->setText(_L("This is a beta version. It may contain new features but also unresolved issues."));
    main_layout->addWidget(m_overview_label);

    // Gray detail panel with 3 numbered items
    m_detail_panel = new QWidget(this);
    m_detail_panel->setStyleSheet("background: #F3F4F6; border-radius: 6px;");
    m_detail_sizer = new QVBoxLayout(m_detail_panel);
    m_detail_sizer->setContentsMargins(16, 12, 16, 12);
    m_detail_sizer->setSpacing(10);

    createDetailItem(m_detail_sizer, m_detail_panel, 1,
        _L("Preview new features"),
        _L("Get early access to the latest features before official release."));
    createDetailItem(m_detail_sizer, m_detail_panel, 2,
        _L("Help improve the software"),
        _L("Your feedback on beta versions helps us identify and fix issues faster."));
    createDetailItem(m_detail_sizer, m_detail_panel, 3,
        _L("Potential instability"),
        _L("Beta versions may have bugs or unexpected behavior not present in stable releases."));

    main_layout->addWidget(m_detail_panel);
    main_layout->addStretch(1);

    // Button row
    auto *btn_layout = new QHBoxLayout();
    btn_layout->setSpacing(8);

    m_button_dont_show = new Button(this, _L("Don't show Beta updates"));
    btn_layout->addWidget(m_button_dont_show);
    btn_layout->addStretch(1);

    m_button_skip = new Button(this, _L("Skip"));
    btn_layout->addWidget(m_button_skip);

    m_button_try_now = new Button(this, _L("Try Now"));
    m_button_try_now->SetValue(true); // brand green accent
    btn_layout->addWidget(m_button_try_now);

    main_layout->addLayout(btn_layout);

    connect(m_button_try_now,   &Button::clicked, this, [this]() { done(1); });
    connect(m_button_skip,      &Button::clicked, this, [this]() { done(0); });
    connect(m_button_dont_show, &Button::clicked, this, [this]() { done(0); });
}

BetaVersionDialog::~BetaVersionDialog() {}

void BetaVersionDialog::updateContent(const QString &available_version,
                                      const QString &current_version)
{
    m_heading_label->setText(_L("New Beta Version: ") + available_version);
    m_version_label->setText(_L("Current version: ") + current_version);
}

void BetaVersionDialog::on_dpi_changed(const QRect & /*suggested_rect*/) {}

void BetaVersionDialog::createDetailItem(QLayout *parent_sizer, QWidget *parent_win,
                                         int index,
                                         const QString &title, const QString &body,
                                         const QString & /*bold_segment*/)
{
    auto *row = new QWidget(parent_win);
    auto *row_layout = new QHBoxLayout(row);
    row_layout->setContentsMargins(0, 0, 0, 0);
    row_layout->setSpacing(8);

    auto *num = new Label(row, Label::Body_14);
    num->setText(QString::number(index) + ".");
    num->setFixedWidth(20);
    row_layout->addWidget(num);

    auto *text_col = new QVBoxLayout();
    text_col->setSpacing(2);

    auto *title_lbl = new Label(row, Label::Head_14);
    title_lbl->setText(title);
    text_col->addWidget(title_lbl);
    m_detail_title_labels.push_back(title_lbl);

    auto *body_lbl = new Label(row, Label::Body_12);
    body_lbl->setText(body);
    body_lbl->setWordWrap(true);
    text_col->addWidget(body_lbl);
    m_detail_body_labels.push_back(body_lbl);

    row_layout->addLayout(text_col);
    parent_sizer->addWidget(row);
}

}} // namespace Slic3r::GUI
