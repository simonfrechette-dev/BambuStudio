// Qt6 stub for PrintHostDialogs.cpp
#include "PrintHostDialogs.hpp"
#include "../Utils/PrintHost.hpp"

namespace Slic3r { namespace GUI {

PrintHostSendDialog::PrintHostSendDialog(const boost::filesystem::path&, PrintHostPostUploadActions, const QStringList&)
    : GUI::MsgDialog(nullptr, QString(), QString()) {}
boost::filesystem::path PrintHostSendDialog::filename() const { return {}; }
PrintHostPostUploadAction PrintHostSendDialog::post_action() const { return {}; }
std::string PrintHostSendDialog::group() const { return {}; }
void PrintHostSendDialog::accept() {}
void PrintHostSendDialog::reject() {}

PrintHostQueueDialog::PrintHostQueueDialog(QWidget* parent) : DPIDialog(parent) {}
void PrintHostQueueDialog::append_job(const PrintHostJob&) {}
void PrintHostQueueDialog::get_active_jobs(std::vector<std::pair<std::string,std::string>>&) {}
void PrintHostQueueDialog::on_dpi_changed(const QRect&) {}
void PrintHostQueueDialog::on_sys_color_changed() {}

void PrintHostQueueDialog::save_user_data(int) {}

}} // namespace Slic3r::GUI
