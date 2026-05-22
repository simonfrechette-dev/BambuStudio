#include "WipeTowerDialog.hpp"
#include "GUI_App.hpp"
#include "I18N.hpp"
#include "libslic3r/FlushVolCalc.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QGroupBox>
#include <cmath>
#include <boost/log/trivial.hpp>

// ---------------------------------------------------------------------------
// Free functions
// ---------------------------------------------------------------------------

bool is_flush_config_modified() { return false; }

void open_flushing_dialog(QObject *parent, QEvent *)
{
    auto *w = qobject_cast<QWidget *>(parent);
    WipingDialog dlg(w);
    dlg.exec();
}

// ---------------------------------------------------------------------------
// WipingDialog
// ---------------------------------------------------------------------------

WipingDialog::WipingDialog(QWidget *parent, const int max_flush_volume)
    : QDialog(parent)
    , m_max_flush_volume(max_flush_volume)
{
    setWindowTitle(_L("Flushing volumes"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    auto *vbox = new QVBoxLayout(this);
    vbox->setContentsMargins(20, 20, 20, 20);
    vbox->setSpacing(12);

    auto *intro = new QLabel(
        _L("Set the flushing volumes for each filament transition. "
           "Higher values produce cleaner transitions at the cost of more wasted material."),
        this);
    intro->setWordWrap(true);
    vbox->addWidget(intro);

    // Buttons
    auto *btn_row = new QHBoxLayout;
    btn_row->addStretch(1);

    auto *cancel_btn = new QPushButton(_L("Cancel"), this);
    connect(cancel_btn, &QPushButton::clicked, this, &QDialog::reject);
    btn_row->addWidget(cancel_btn);

    auto *ok_btn = new QPushButton(_L("OK"), this);
    connect(ok_btn, &QPushButton::clicked, this, [this] {
        m_submit_flag = true;
        accept();
    });
    btn_row->addWidget(ok_btn);

    vbox->addLayout(btn_row);
    adjustSize();
}

// static
WipingDialog::VolumeMatrix WipingDialog::CalcFlushingVolumes(int /*extruder_id*/)
{
    return VolumeMatrix{};
}

std::vector<double> WipingDialog::GetFlattenMatrix() const
{
    std::vector<double> flat;
    for (const auto &row : m_raw_matrixs)
        for (double v : row)
            flat.push_back(v);
    return flat;
}

std::vector<double> WipingDialog::GetMultipliers() const
{
    return m_flush_multipliers;
}

int WipingDialog::CalcFlushingVolume(const QColor & /*from*/, const QColor & /*to*/,
                                      int min_flush_volume, int /*nozzle_flush_dataset*/)
{
    return min_flush_volume;
}

QString WipingDialog::BuildTableObjStr()  { return {}; }
QString WipingDialog::BuildTextObjStr(bool) { return {}; }
void WipingDialog::StoreFlushData(int, const std::vector<std::vector<double>> &,
                                   const std::vector<double> &) {}
