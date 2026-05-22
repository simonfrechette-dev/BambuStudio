#include "UpdateDialogs.hpp"
#include "I18N.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <boost/format.hpp>

namespace Slic3r { namespace GUI {

// ---------------------------------------------------------------------------
// MsgUpdateSlic3r
// ---------------------------------------------------------------------------

MsgUpdateSlic3r::MsgUpdateSlic3r(const Semver &ver_current, const Semver &ver_online)
    : MsgDialog(nullptr,
                _L("New version available"),
                _L("New version of BambuStudio is available"),
                MsgOK | MsgCANCEL | MsgICON_INFO)
    , cbox(nullptr)
{
    QString msg = QString(_L("Current version: %1\nNew version: %2"))
        .arg(QString::fromStdString(ver_current.to_string()))
        .arg(QString::fromStdString(ver_online.to_string()));
    auto *lbl = new QLabel(msg, this);
    lbl->setWordWrap(true);
    content_sizer->addWidget(lbl);

    cbox = new QCheckBox(_L("Don't check for updates again"), this);
    cbox->setChecked(false);
    content_sizer->addWidget(cbox);

    finalize();
}

MsgUpdateSlic3r::~MsgUpdateSlic3r() = default;

bool MsgUpdateSlic3r::disable_version_check() const
{
    return cbox && cbox->isChecked();
}

void MsgUpdateSlic3r::on_hyperlink(QEvent &) {}

// ---------------------------------------------------------------------------
// MsgUpdateConfig
// ---------------------------------------------------------------------------

MsgUpdateConfig::MsgUpdateConfig(const std::vector<Update> &updates, bool /*force_before_wizard*/)
    : DPIDialog(nullptr)
{
    setWindowTitle(_L("Configuration update"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    auto *vbox = new QVBoxLayout(this);
    vbox->setContentsMargins(20, 20, 20, 20);
    vbox->setSpacing(10);

    auto *lbl = new QLabel(_L("The following configuration updates are available:"), this);
    lbl->setWordWrap(true);
    vbox->addWidget(lbl);

    for (const auto &u : updates) {
        QString line = QString("%1  v%2")
            .arg(QString::fromStdString(u.vendor))
            .arg(QString::fromStdString(u.version.to_string()));
        if (!u.comment.empty())
            line += "  — " + QString::fromStdString(u.comment);
        auto *row = new QLabel(line, this);
        vbox->addWidget(row);
    }

    auto *btn_row = new QHBoxLayout;
    btn_row->addStretch(1);
    auto *ok_btn = new QPushButton(_L("OK"), this);
    connect(ok_btn, &QPushButton::clicked, this, &QDialog::accept);
    btn_row->addWidget(ok_btn);
    vbox->addLayout(btn_row);
    adjustSize();
}

MsgUpdateConfig::~MsgUpdateConfig() = default;
void MsgUpdateConfig::on_dpi_changed(const QRect &) {}

// ---------------------------------------------------------------------------
// MsgUpdateForced
// ---------------------------------------------------------------------------

MsgUpdateForced::MsgUpdateForced(const std::vector<Update> &updates)
    : MsgDialog(nullptr,
                _L("Configuration incompatible"),
                _L("The following installed configuration bundles are incompatible with this version of BambuStudio:"),
                MsgOK | MsgICON_WARN)
{
    for (const auto &u : updates) {
        QString line = QString("%1  v%2")
            .arg(QString::fromStdString(u.vendor))
            .arg(QString::fromStdString(u.version.to_string()));
        auto *row = new QLabel(line, this);
        content_sizer->addWidget(row);
    }
    finalize();
}

MsgUpdateForced::~MsgUpdateForced() = default;

// ---------------------------------------------------------------------------
// MsgDataIncompatible
// ---------------------------------------------------------------------------

MsgDataIncompatible::MsgDataIncompatible(const std::unordered_map<std::string, QString> &incompats)
    : MsgDialog(nullptr,
                _L("Data incompatible"),
                _L("Incompatible configuration data:"),
                MsgOK | MsgICON_WARN)
{
    for (const auto &kv : incompats) {
        QString line = QString::fromStdString(kv.first) + ": " + kv.second;
        auto *row = new QLabel(line, this);
        row->setWordWrap(true);
        content_sizer->addWidget(row);
    }
    finalize();
}

MsgDataIncompatible::~MsgDataIncompatible() = default;

// ---------------------------------------------------------------------------
// MsgNoUpdates
// ---------------------------------------------------------------------------

MsgNoUpdates::MsgNoUpdates()
    : MsgDialog(nullptr,
                _L("No updates available"),
                _L("No updates available"),
                MsgOK | MsgICON_INFO)
{
    auto *lbl = new QLabel(_L("No configuration updates are available."), this);
    lbl->setWordWrap(true);
    content_sizer->addWidget(lbl);
    finalize();
}

MsgNoUpdates::~MsgNoUpdates() = default;

}} // namespace Slic3r::GUI
#include "UpdateDialogs.hpp"

// All implementations are stubs pending Qt port.
