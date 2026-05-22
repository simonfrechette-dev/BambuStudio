// Qt6 port of GUI.cpp helpers
#include "GUI.hpp"
#include "MsgDialog.hpp"
#include "GUI_App.hpp"
#include "I18N.hpp"

namespace Slic3r { namespace GUI {

void show_error(QWidget *parent, const QString &message, bool monospaced_font)
{
    ErrorDialog dlg(parent, message, monospaced_font);
    dlg.exec();
}

void show_error(QWidget *parent, const char *message, bool monospaced_font)
{
    show_error(parent, QString::fromUtf8(message), monospaced_font);
}

void show_error_id(int /*id*/, const std::string &message)
{
    show_error(nullptr, message);
}

void show_info(QWidget *parent, const QString &message, const QString &title)
{
    InfoDialog dlg(parent, title.isEmpty() ? _L("Info") : title, message);
    dlg.exec();
}

void show_info(QWidget *parent, const char *message, const char *title)
{
    show_info(parent,
              QString::fromUtf8(message),
              title ? QString::fromUtf8(title) : QString());
}

}} // namespace Slic3r::GUI
