#ifndef slic3r_AmsWidgets_hpp_
#define slic3r_AmsWidgets_hpp_




#include <map>
#include <QAbstractTableModel>
#include <QVariant>
#include <QString>
#include <vector>
#include <memory>
#include "Event.hpp"
#include "libslic3r/ProjectTask.hpp"
#include "wxExtensions.hpp"
#include "slic3r/GUI/DeviceManager.hpp"

namespace Slic3r {
namespace GUI {

class TrayListModel : public QAbstractTableModel
{
public:
	enum
	{
		Col_TrayTitle,
		Col_TrayColor,
		Col_TrayMeterial,
		Col_TrayWeight,
		Col_TrayDiameter,
		Col_TrayTime,
		Col_TraySN,
		Col_TrayManufacturer,
		Col_TraySaturability,
		Col_TrayTransmittance,
		Col_TraySmooth,
		Col_Max,
	};

	TrayListModel();

	virtual unsigned int GetColumnCount() const { // override
		return Col_Max;
	}

	virtual QString GetColumnType(unsigned int col) const { // override
		return "string";
	}
        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;        bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
        int rowCount(const QModelIndex& = {}) const override;
        int columnCount(const QModelIndex& = {}) const override;
void update(MachineObject* obj);
	void clear_data();

private:
	QStringList m_titleColValues;
	QStringList m_colorColValues;
	QStringList m_meterialColValues;
	QStringList m_weightColValues;
	QStringList m_diameterColValues;
	QStringList m_timeColValues;
	QStringList m_snColValues;
	QStringList m_manufacturerColValues;
	QStringList m_saturabilityColValues;
	QStringList m_transmittanceColValues;
	QStringList m_smoothColValues;

};

} // GUI
} // Slic3r

#endif /* slic3r_Tab_hpp_ */
