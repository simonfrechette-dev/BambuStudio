#ifndef _WIPE_TOWER_DIALOG_H_
#define _WIPE_TOWER_DIALOG_H_

#include <QDialog>
#include <QColor>
#include <QString>
#include <vector>
#include "libslic3r/PrintConfig.hpp"
#include "libslic3r/FlushVolCalc.hpp"


bool is_flush_config_modified();
void open_flushing_dialog(QObject *parent, QEvent *event);

class WipingDialog : public QDialog
{
public:
        using VolumeMatrix = std::vector<std::vector<double>>;

        WipingDialog(QWidget* parent = nullptr, const int max_flush_volume = Slic3r::g_max_flush_volume);
        static VolumeMatrix CalcFlushingVolumes(int extruder_id);
        std::vector<double> GetFlattenMatrix()const;
        std::vector<double> GetMultipliers()const;
        bool GetSubmitFlag() const { return m_submit_flag; }

private:
        static int CalcFlushingVolume(const QColor& from_, const QColor& to_, int min_flush_volume, int nozzle_flush_dataset);
        QString BuildTableObjStr();
        QString BuildTextObjStr(bool multi_language = true);
        void StoreFlushData(int extruder_num, const std::vector<std::vector<double>>& flush_volume_vecs, const std::vector<double>& flush_multipliers);

        int m_max_flush_volume;

        VolumeMatrix m_raw_matrixs;
        std::vector<double> m_flush_multipliers;
        std::vector<size_t> m_physical_indices;
        bool m_submit_flag{ false };
};

#endif  // _WIPE_TOWER_DIALOG_H_
