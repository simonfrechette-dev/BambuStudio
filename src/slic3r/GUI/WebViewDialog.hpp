#ifndef slic3r_WebViewDialog_hpp_
#define slic3r_WebViewDialog_hpp_

// Qt6 port of WebViewDialog.hpp
// Original wx version backed up to WebViewDialog.hpp.wx-backup

#include <QWidget>
#include <QDialog>
#include <QTimer>
#include <QString>
#include <functional>
#include <string>

class QWebEngineView;
class QPushButton;
class QLineEdit;
class QVBoxLayout;
class QHBoxLayout;
class QMenu;
class QAction;
class QLabel;

namespace Slic3r {

class NetworkAgent;

namespace GUI {

class WebViewPanel : public QWidget
{
    Q_OBJECT

public:
    explicit WebViewPanel(QWidget *parent = nullptr);
    virtual ~WebViewPanel();

    void load_url(const QString& url);

    void UpdateState();
    void ResetWholePage();

    // Login
    void SendLoginInfo();
    void ShowNetpluginTip();

    // MW
    void SetMakerworldModelID(std::string ModelID);
    void OpenMakerworldSearchPage(std::string KeyWord);
    void SetPrintHistoryTaskID(int TaskID);

    // DisconnectPage
    QString MakeDisconnectUrl(std::string MenuName);

    // LeftMenu
    std::string m_contentname;
    bool        m_leftfirst{false};
    void CheckMenuNewTag();
    void ShowMenuNewTag(std::string menuname, std::string show);
    void SetLeftMenuShow(std::string menuname, int show);
    void SetLeftMenuWidth(int nWidth);
    void SwitchWebContent(std::string modelname, int refresh = 0);
    void SwitchLeftMenu(std::string strMenu);

    // Recent File
    void SendRecentList(int images);

    // Online
    bool    m_onlinefirst{false};
    QString m_online_type;
    QString m_online_LastUrl;
    std::string m_online_last_url;

    void SendDesignStaffpick(bool on);
    void get_design_staffpick(int offset, int limit, std::function<void(std::string)> callback);
    void get_user_mw_4u_config(std::function<void(std::string)> callback);
    void get_4u_staffpick(int seed, int limit, std::function<void(std::string)> callback);
    void OpenModelDetail(std::string id, NetworkAgent *agent);
    void UpdateMakerworldLoginStatus();
    void SetMakerworldPageLoginStatus(bool login, const QString& ticket = {});
    void get_wiki_search_result(std::string keyword);
    void get_academy_list();

    // Makerlab
    bool    m_MakerLabFirst{false};
    QString m_MakerLab_LastUrl;
    void SendMakerlabList();
    void get_makerlab_list(std::function<void(std::string)> callback);
    void SetMakerlabUrl(std::string url);
    void OpenOneMakerlab(std::string url);
    void OpenMakerlab3mf(std::string Base64Buf, std::string FileName);
    bool SaveBase64ToLocal(std::string Base64Buf, std::string FileName, std::string FileTail,
                           QString& download_path, QString& download_file);
    void SaveMakerlabStl(int SequenceID, std::string Base64Buf, std::string FileName);
    void UpdateMakerlabStatus();

    // wiki
    bool    m_WikiFirst{false};
    QString m_Wiki_LastUrl;

    // Common UI
    void SetWebviewShow(const QString& name, bool show);
    void SetOnlineToolbarVisible(bool visible);
    void UpdateOnlineToolbarState();
    std::string GetStudioLanguage();

    // PrintHistory
    std::string m_TaskInfo;
    bool        m_printhistoryfirst{false};
    QString     m_print_history_LastUrl;
    void ShowUserPrintTask(bool bShow, bool bForce = false);

    bool GetJumpUrl(bool login, const QString& ticket, const QString& targeturl, QString& finalurl);

    void RunScript(const QString& javascript);
    void RunScriptLeft(const QString& javascript);

    void update_mode();

    QTimer* m_LoginUpdateTimer{nullptr};

private slots:
    void OnFreshLoginStatus();

private:
    std::string m_Region;
    int         m_loginstatus{0};
    bool m_isPerformingBack{false};
    bool m_online_history_cleared{false};
    bool m_makerlab_history_cleared{false};
    bool m_has_pending_staff_pick{false};

    QVBoxLayout*    topsizer{nullptr};
    QHBoxLayout*    bSizer_toolbar{nullptr};
    QHBoxLayout*    m_home_web{nullptr};

    QWebEngineView* m_browser{nullptr};
    QWebEngineView* m_browserLeft{nullptr};
    QWebEngineView* m_browserMW{nullptr};
    QWebEngineView* m_browserPH{nullptr};
    QWebEngineView* m_browserML{nullptr};
    QWebEngineView* m_browserWiki{nullptr};

    QPushButton*    m_button_back{nullptr};
    QPushButton*    m_button_forward{nullptr};
    QPushButton*    m_button_stop{nullptr};
    QPushButton*    m_button_reload{nullptr};
    QLineEdit*      m_url{nullptr};
    QPushButton*    m_button_tools{nullptr};

    QWidget*        m_online_toolbar_panel{nullptr};
    QHBoxLayout*    m_online_toolbar_sizer{nullptr};
    QPushButton*    m_online_back_btn{nullptr};
    QPushButton*    m_online_refresh_btn{nullptr};
    QPushButton*    m_online_open_browser_btn{nullptr};
    QWidget*        m_online_container{nullptr};
    QHBoxLayout*    m_online_container_sizer{nullptr};
    int             m_online_toolbar_icon_px{16};

    QMenu*          m_tools_menu{nullptr};
    QAction*        m_tools_handle_navigation{nullptr};
    QAction*        m_tools_handle_new_window{nullptr};
    QAction*        m_edit_cut{nullptr};
    QAction*        m_edit_copy{nullptr};
    QAction*        m_edit_paste{nullptr};
    QAction*        m_edit_undo{nullptr};
    QAction*        m_edit_redo{nullptr};
    QAction*        m_edit_mode{nullptr};
    QAction*        m_scroll_line_up{nullptr};
    QAction*        m_scroll_line_down{nullptr};
    QAction*        m_scroll_page_up{nullptr};
    QAction*        m_scroll_page_down{nullptr};
    QAction*        m_script_string{nullptr};
    QAction*        m_script_integer{nullptr};
    QAction*        m_script_double{nullptr};
    QAction*        m_script_bool{nullptr};
    QAction*        m_script_object{nullptr};
    QAction*        m_script_array{nullptr};
    QAction*        m_script_dom{nullptr};
    QAction*        m_script_undefined{nullptr};
    QAction*        m_script_null{nullptr};
    QAction*        m_script_date{nullptr};
    QAction*        m_script_message{nullptr};
    QAction*        m_script_custom{nullptr};
    QAction*        m_selection_clear{nullptr};
    QAction*        m_selection_delete{nullptr};
    QAction*        m_context_menu{nullptr};
    QAction*        m_dev_tools{nullptr};

    QWidget*        m_info{nullptr};
    QLabel*         m_info_text{nullptr};

    long    m_zoomFactor{100};
    QString m_javascript;
    QString m_response_js;
};

class SourceViewDialog : public QDialog
{
    Q_OBJECT
public:
    SourceViewDialog(QWidget* parent, const QString& source);
};

} // namespace GUI
} // namespace Slic3r

#endif /* slic3r_Tab_hpp_ */
