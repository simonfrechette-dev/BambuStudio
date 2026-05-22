#ifndef slic3r_ConfigWizard_hpp_
#define slic3r_ConfigWizard_hpp_

// Phase 4 TODO: full Qt port of ConfigWizard dialog

#include <memory>
#include <QString>
#include "GUI_Utils.hpp"

namespace Slic3r {

class PresetBundle;
class PresetUpdater;

namespace GUI {

class ConfigWizard : public DPIDialog
{
    Q_OBJECT
public:
    enum RunReason { RR_DATA_EMPTY, RR_DATA_LEGACY, RR_DATA_INCOMPAT, RR_USER };
    enum StartPage  { SP_WELCOME, SP_PRINTERS, SP_FILAMENTS, SP_MATERIALS };

    ConfigWizard(QWidget *parent);
    ConfigWizard(ConfigWizard &&) = delete;
    ConfigWizard(const ConfigWizard &) = delete;
    ConfigWizard &operator=(ConfigWizard &&) = delete;
    ConfigWizard &operator=(const ConfigWizard &) = delete;
    ~ConfigWizard() override;

    bool run(RunReason reason, StartPage start_page = SP_WELCOME);

    static const QString &name(const bool from_menu = false);

protected:
    void on_dpi_changed(const QRect &) override {}
    void on_sys_color_changed() override {}

private:
    struct priv;
    std::unique_ptr<priv> p;
};

} // namespace GUI
} // namespace Slic3r

#endif // slic3r_ConfigWizard_hpp_
