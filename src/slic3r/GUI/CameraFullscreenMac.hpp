#pragma once

#ifdef __APPLE__

class QWidget;

namespace Slic3r { namespace GUI {

using CameraFullscreenEscapeCallback = void (*)(void *);

void *install_camera_fullscreen_escape_monitor(CameraFullscreenEscapeCallback callback, void *context);
void  uninstall_camera_fullscreen_escape_monitor(void *monitor);
void *enter_camera_fullscreen_presentation(QWidget *window);
void  apply_camera_fullscreen_frame(QWidget *window);
void  restore_camera_fullscreen_presentation(void *presentation);

}} // namespace Slic3r::GUI

#endif
