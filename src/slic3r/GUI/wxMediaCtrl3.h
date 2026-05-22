//  wxMediaCtrl3.h - Qt6 port stub
#ifndef wxMediaCtrl3_h
#define wxMediaCtrl3_h

#include <QWidget>
#include <QString>
#include <atomic>
#include "../Utils/FrameBuffer.hpp"

namespace Slic3r { namespace GUI {
inline const QEvent::Type EVT_MEDIA_CTRL_STAT = static_cast<QEvent::Type>(QEvent::registerEventType());
} }

void wxMediaCtrl_OnSize(QWidget* ctrl, QSize const& videoSize, int width, int height);

#endif
