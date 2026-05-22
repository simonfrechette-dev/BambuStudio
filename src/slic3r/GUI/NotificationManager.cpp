// Qt6 stub for NotificationManager.cpp
// Original wx-based implementation replaced with empty stub.
#include "NotificationManager.hpp"

// All implementations are stubs pending Qt port.

namespace Slic3r { namespace GUI {

void NotificationManager::progress_indicator_set_range(int) {}
void NotificationManager::progress_indicator_set_cancel_callback(std::function<void()>) {}
void NotificationManager::progress_indicator_set_progress(int) {}
void NotificationManager::progress_indicator_set_status_text(const char*) {}
int  NotificationManager::progress_indicator_get_range() const { return 0; }

void NotificationManager::bbl_close_plugin_install_notification() { /* TODO: Qt port stub */ }

}} // namespace Slic3r::GUI
