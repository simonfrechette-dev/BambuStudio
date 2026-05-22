#ifndef slic3r_GUI_WebView_hpp_
#define slic3r_GUI_WebView_hpp_

#include <QString>

// Phase 7 TODO: implement with QWebEngineView
class QWebEngineView;
class QWidget;

class WebView
{
public:
    static QWebEngineView *CreateWebView(QWidget *parent, QString const &url);

    static void LoadUrl(QWebEngineView *webView, QString const &url);

    static bool RunScript(QWebEngineView *webView, QString const &msg);

    static void RecreateAll();

    static QString BuildEdgeUserDataPath();
};

#endif // !slic3r_GUI_WebView_hpp_
