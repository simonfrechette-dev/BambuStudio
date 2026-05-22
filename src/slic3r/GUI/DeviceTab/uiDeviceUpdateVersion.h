//**********************************************************/
/* File: uiDeviceUpdateVersion.h
*  Description: The panel with firmware info
* 
*  \n class uiDeviceUpdateVersion
//**********************************************************/

#pragma once
#include <QWidget>
#include <QString>
#include "slic3r/GUI/wxExtensions.hpp"
#include "slic3r/GUI/DeviceManager.hpp"

// Previous defintions
class QLabel;
class QLabel;

namespace Slic3r::GUI
{
// @Class uiDeviceUpdateVersion
// @Note  The panel with firmware info
class uiDeviceUpdateVersion : public QWidget
{
public:
    uiDeviceUpdateVersion(QWidget* parent,
                          int id = -1,
                          const QPoint& pos = QPoint(),
                          const QSize& size = QSize(),
                          long style = 0);
    ~uiDeviceUpdateVersion() = default;

public:
    void  UpdateInfo(const DevFirmwareVersionInfo& info);

private:
    void  CreateWidgets();

    void  SetName(const QString& str) { m_dev_name->setText(str); };
    void  SetSerial(const QString& str) { m_dev_snl->setText(str); };
    void  SetVersion(const QString& cur_version, const QString& latest_version);

private:
    QLabel*   m_dev_name;
    QLabel*   m_dev_snl;
    QLabel*   m_dev_version;
    QLabel* m_dev_upgrade_indicator;
};
};// end of namespace Slic3r::GUI