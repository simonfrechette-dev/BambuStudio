#ifndef slic3r_MacDarkMode_hpp_
#define slic3r_MacDarkMode_hpp_

#include <QString>

class QObject;

namespace Slic3r {
namespace GUI {

#if __APPLE__
extern bool mac_dark_mode();
extern double mac_max_scaling_factor();
extern void set_miniaturizable(void * window);
void WKWebView_evaluateJavaScript(void * web, QString const & script, void (*callback)(QString const &));
void WKWebView_setTransparentBackground(void * web);
void set_tag_when_enter_full_screen(bool isfullscreen);
void set_title_colour_after_set_title(void * window);
void initGestures(void * view, QObject * handler);
void openFolderForFile(QString const & file);
void StaticGroup_layoutBadge(void * group, void * badge);
#endif


} // namespace GUI
} // namespace Slic3r

#endif // MacDarkMode_h
