#ifndef slic3r_WebDialytipDialog_hpp_
#define slic3r_WebDialytipDialog_hpp_
#include <QWidget>
#include <QString>


#if wxUSE_WEBVIEW_IE
#endif
#if wxUSE_WEBVIEW_EDGE
#endif


#include "GUI_App.hpp"

namespace Slic3r { namespace GUI {

class DownPluginFrame : public QDialog
{
public:
    DownPluginFrame(GUI_App *pGUI);
    virtual ~DownPluginFrame();


    // Web Function
    void     load_url(QString &url);

    void UpdateState();
    void OnIdle(QEvent &evt);
    // void OnClose(QCloseEvent &evt);

    void OnNavigationRequest(QEvent &evt);
    void OnNavigationComplete(QEvent &evt);
    void OnDocumentLoaded(QEvent &evt);
    void OnNewWindow(QEvent &evt);
    void OnError(QEvent &evt);
    void OnTitleChanged(QEvent &evt);
    void OnFullScreenChanged(QEvent &evt);
    void OnScriptMessage(QEvent &evt);

    void OnScriptResponseMessage(QEvent &evt);
    void RunScript(const QString &javascript);

    // install plugin
    int DownloadPlugin();
    int InstallPlugin();
    int ShowPluginStatus(int status, int percent, bool &cancel);

private:
    GUI_App * m_MainPtr;
    AppConfig m_appconfig_new;

    QWidget *m_browser;

#if wxUSE_WEBVIEW_IE
    QAction *m_script_object_el;
    QAction *m_script_date_el;
    QAction *m_script_array_el;
#endif
    // Last executed JavaScript snippet, for convenience.
    QString m_javascript;
    QString m_response_js;

    // DECLARE_EVENT_TABLE()
};

}} // namespace Slic3r::GUI

#endif /* slic3r_Tab_hpp_ */
