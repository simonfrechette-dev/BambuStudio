// Qt6 stub for WebUserLoginDialog.cpp
// Original wx implementation backed up to WebUserLoginDialog.cpp.wx-backup

#include "WebUserLoginDialog.hpp"
#include <QWebEngineView>
#include <QTimer>

namespace Slic3r { namespace GUI {

int ZUserLogin::web_sequence_id = 0;

ZUserLogin::ZUserLogin(QWidget* parent)
    : QDialog(parent)
{}

ZUserLogin::~ZUserLogin() {}

void ZUserLogin::load_url(const QString& /*url*/) {}
std::string ZUserLogin::w2s(const QString& sSrc) { return sSrc.toStdString(); }
void ZUserLogin::UpdateState() {}
void ZUserLogin::RunScript(const QString& /*javascript*/) {}
bool ZUserLogin::ShowErrorPage() { return false; }
bool ZUserLogin::run() { return exec() == QDialog::Accepted; }
std::string ZUserLogin::GetStudioLanguage() { return "en"; }
void ZUserLogin::OnTimer() {}

}} // namespace Slic3r::GUI
