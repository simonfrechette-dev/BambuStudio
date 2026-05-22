#pragma once
#ifndef slic3r_ZWebUserLogin_HEAD_
#define slic3r_ZWebUserLogin_HEAD_

// Qt6 port of WebUserLoginDialog.hpp
// Original wx version backed up to WebUserLoginDialog.hpp.wx-backup

#include <QDialog>
#include <QTimer>
#include <QString>
#include <string>

class QWebEngineView;

namespace Slic3r { namespace GUI {

class ZUserLogin : public QDialog
{
    Q_OBJECT
public:
    ZUserLogin(QWidget* parent = nullptr);
    virtual ~ZUserLogin();

    void load_url(const QString& url);

    std::string w2s(const QString& sSrc);

    void UpdateState();

    void RunScript(const QString& javascript);

    bool m_networkOk{false};
    bool ShowErrorPage();
    bool run();

    std::string GetStudioLanguage();

    static int web_sequence_id;

private slots:
    void OnTimer();

private:
    QTimer*        m_timer{nullptr};
    QString        TargetUrl;
    QWebEngineView* m_browser{nullptr};
    QString        m_javascript;
    QString        m_response_js;
    QString        m_bbl_user_agent;
};

}} // namespace Slic3r::GUI

#endif
