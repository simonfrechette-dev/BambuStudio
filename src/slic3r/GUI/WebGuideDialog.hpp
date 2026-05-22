#ifndef slic3r_WebGuideDialog_hpp_
#define slic3r_WebGuideDialog_hpp_
#include <QWidget>
#include <QString>

#include <atomic>

#if wxUSE_WEBVIEW_IE
#endif
#if wxUSE_WEBVIEW_EDGE
#endif


#include "GUI_App.hpp"
#include "libslic3r/PresetBundle.hpp"
#include "slic3r/Utils/PresetUpdater.hpp"

#include <nlohmann/json.hpp>

namespace Slic3r { namespace GUI {

class GuideFrame : public DPIDialog
{
public:
    GuideFrame(GUI_App *pGUI, long style = 0);
    virtual ~GuideFrame();

    enum GuidePage {
        BBL_WELCOME,
        BBL_REGION,
        BBL_MODELS,
        BBL_FILAMENTS,
        BBL_FILAMENT_ONLY,
        BBL_MODELS_ONLY
    }m_page;

    //Web Function
    void load_url(QString &url);
    QString SetStartPage(GuidePage startpage=BBL_WELCOME, bool load = true);

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

    //Logic
    bool IsFirstUse();

    //Model - Machine - Filaments
    int LoadProfileData();
    int SaveProfileData();
    int LoadProfileFamily(std::string strVendor, std::string strFilePath);
    int SaveProfile();
    int GetFilamentInfo( std::string VendorDirectory,json & pFilaList, std::string filepath, std::string &sVendor, std::string &sType);


    bool apply_config(AppConfig *app_config, PresetBundle *preset_bundle, const PresetUpdater *updater, bool& apply_keeped_changes);
    bool run(bool& config_applied);

    void        StrReplace(std::string &strBase, std::string strSrc, std::string strDes);
    std::string w2s(QString sSrc);
    void        GetStardardFilePath(std::string &FilePath);
    //bool LoadFile(std::string jPath, std::string & sContent);

    // install plugin
    int DownloadPlugin();
    int InstallPlugin();
    int ShowPluginStatus(int status, int percent, bool &cancel);

    void on_dpi_changed(const QRect &suggested_rect) {}

private:
    GUI_App *m_MainPtr;
    AppConfig m_appconfig_new;

    QWidget *m_browser;
    QPushButton * m_TestBtn;

    QString m_SectionName;

    bool bbl_bundle_rsrc;
    boost::filesystem::path vendor_dir;
    boost::filesystem::path rsrc_vendor_dir;

    //First Load
    bool bFirstComplete{false};
    std::atomic_bool m_destroy{false};
    boost::thread* m_load_task{ nullptr };

    // User Config
    bool m_GuideFinish;
    std::string m_PrivacyUse;
    std::string m_Region;

    bool InstallNetplugin;
    bool network_plugin_ready {false};

#if wxUSE_WEBVIEW_IE
    QAction *m_script_object_el;
    QAction *m_script_date_el;
    QAction *m_script_array_el;
#endif
    // Last executed JavaScript snippet, for convenience.
    QString m_javascript;
    QString m_response_js;

    QString m_bbl_user_agent;
    std::string m_editing_filament_id;
};

}} // namespace Slic3r::GUI

#endif /* slic3r_Tab_hpp_ */
