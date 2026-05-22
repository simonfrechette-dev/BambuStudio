#ifndef slic3r_MultiSendMachineModel_hpp_
#define slic3r_MultiSendMachineModel_hpp_

#include "DeviceManager.hpp"

namespace Slic3r { 
namespace GUI {

class MultiSendMachineModel : public QAbstractItemModel
{
public:
    MultiSendMachineModel();
    ~MultiSendMachineModel();

    void Init();

    QModelIndex AddMachine(MachineObject* obj);

private:
};

} // namespace GUI
} // namespace Slic3r

#endif
