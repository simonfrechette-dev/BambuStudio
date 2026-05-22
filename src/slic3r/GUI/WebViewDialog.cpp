// Qt6 stub for WebViewDialog.cpp
// Original wx implementation backed up to WebViewDialog.cpp.wx-backup
// TODO Phase 4: implement using QWebEngineView

#include "WebViewDialog.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QMenu>
#include <QAction>
#include <QLabel>
#include <QTimer>
#include <QWebEngineView>

namespace Slic3r {
namespace GUI {

WebViewPanel::WebViewPanel(QWidget *parent)
    : QWidget(parent)
{
    auto* lay = new QVBoxLayout(this);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(0);

    // Address/toolbar bar
    auto* toolbar = new QWidget(this);
    toolbar->setFixedHeight(32);
    toolbar->setStyleSheet(QStringLiteral(
        "QWidget { background:#242424; border-bottom:1px solid #444; }"));
    auto* tb_lay = new QHBoxLayout(toolbar);
    tb_lay->setContentsMargins(4, 2, 4, 2);
    tb_lay->setSpacing(4);

    m_button_back = new QPushButton(tr("←"), toolbar);
    m_button_back->setFixedSize(28, 24);
    m_button_back->setStyleSheet(
        "QPushButton { background:#333; color:#ccc; border:none; border-radius:3px; }"
        "QPushButton:hover { background:#555; }");
    m_button_forward = new QPushButton(tr("→"), toolbar);
    m_button_forward->setFixedSize(28, 24);
    m_button_forward->setStyleSheet(m_button_back->styleSheet());
    m_button_reload = new QPushButton(tr("⟳"), toolbar);
    m_button_reload->setFixedSize(28, 24);
    m_button_reload->setStyleSheet(m_button_back->styleSheet());

    m_url = new QLineEdit(toolbar);
    m_url->setStyleSheet(
        "QLineEdit { background:#333; color:#ddd; border:1px solid #555; border-radius:3px;"
        " padding:0 4px; selection-background-color:#555; }");

    tb_lay->addWidget(m_button_back);
    tb_lay->addWidget(m_button_forward);
    tb_lay->addWidget(m_button_reload);
    tb_lay->addWidget(m_url, 1);

    lay->addWidget(toolbar);

    // WebEngine view
    m_browser = new QWebEngineView(this);
    lay->addWidget(m_browser, 1);

    // Wire toolbar to view
    connect(m_button_back,    &QPushButton::clicked, m_browser, &QWebEngineView::back);
    connect(m_button_forward, &QPushButton::clicked, m_browser, &QWebEngineView::forward);
    connect(m_button_reload,  &QPushButton::clicked, m_browser, &QWebEngineView::reload);

    connect(m_url, &QLineEdit::returnPressed, this, [this]() {
        QString url = m_url->text().trimmed();
        if (!url.startsWith("http://") && !url.startsWith("https://"))
            url = "https://" + url;
        m_browser->load(QUrl(url));
    });

    connect(m_browser, &QWebEngineView::urlChanged, this, [this](const QUrl& url) {
        if (m_url) m_url->setText(url.toString());
    });

    // Load Bambu home page
    m_browser->load(QUrl(QStringLiteral("https://bambulab.com/en")));
}

WebViewPanel::~WebViewPanel() {}

void WebViewPanel::load_url(const QString& url) {
    if (m_browser) m_browser->load(QUrl(url));
    if (m_url) m_url->setText(url);
}
void WebViewPanel::UpdateState() {}
void WebViewPanel::ResetWholePage() {}
void WebViewPanel::SendLoginInfo() {}
void WebViewPanel::ShowNetpluginTip() {}
void WebViewPanel::SetMakerworldModelID(std::string /*ModelID*/) {}
void WebViewPanel::OpenMakerworldSearchPage(std::string /*KeyWord*/) {}
void WebViewPanel::SetPrintHistoryTaskID(int /*TaskID*/) {}
QString WebViewPanel::MakeDisconnectUrl(std::string /*MenuName*/) { return {}; }
void WebViewPanel::CheckMenuNewTag() {}
void WebViewPanel::ShowMenuNewTag(std::string /*menuname*/, std::string /*show*/) {}
void WebViewPanel::SetLeftMenuShow(std::string /*menuname*/, int /*show*/) {}
void WebViewPanel::SetLeftMenuWidth(int /*nWidth*/) {}
void WebViewPanel::SwitchWebContent(std::string /*modelname*/, int /*refresh*/) {}
void WebViewPanel::SwitchLeftMenu(std::string /*strMenu*/) {}
void WebViewPanel::SendRecentList(int /*images*/) {}
void WebViewPanel::SendDesignStaffpick(bool /*on*/) {}
void WebViewPanel::get_design_staffpick(int /*offset*/, int /*limit*/, std::function<void(std::string)> /*callback*/) {}
void WebViewPanel::get_user_mw_4u_config(std::function<void(std::string)> /*callback*/) {}
void WebViewPanel::get_4u_staffpick(int /*seed*/, int /*limit*/, std::function<void(std::string)> /*callback*/) {}
void WebViewPanel::OpenModelDetail(std::string /*id*/, NetworkAgent* /*agent*/) {}
void WebViewPanel::UpdateMakerworldLoginStatus() {}
void WebViewPanel::SetMakerworldPageLoginStatus(bool /*login*/, const QString& /*ticket*/) {}
void WebViewPanel::get_wiki_search_result(std::string /*keyword*/) {}
void WebViewPanel::get_academy_list() {}
void WebViewPanel::SendMakerlabList() {}
void WebViewPanel::get_makerlab_list(std::function<void(std::string)> /*callback*/) {}
void WebViewPanel::SetMakerlabUrl(std::string /*url*/) {}
void WebViewPanel::OpenOneMakerlab(std::string /*url*/) {}
void WebViewPanel::OpenMakerlab3mf(std::string /*Base64Buf*/, std::string /*FileName*/) {}
bool WebViewPanel::SaveBase64ToLocal(std::string /*Base64Buf*/, std::string /*FileName*/, std::string /*FileTail*/,
                                     QString& /*download_path*/, QString& /*download_file*/) { return false; }
void WebViewPanel::SaveMakerlabStl(int /*SequenceID*/, std::string /*Base64Buf*/, std::string /*FileName*/) {}
void WebViewPanel::UpdateMakerlabStatus() {}
void WebViewPanel::SetWebviewShow(const QString& /*name*/, bool /*show*/) {}
void WebViewPanel::SetOnlineToolbarVisible(bool /*visible*/) {}
void WebViewPanel::UpdateOnlineToolbarState() {}
std::string WebViewPanel::GetStudioLanguage() { return "en"; }
void WebViewPanel::ShowUserPrintTask(bool /*bShow*/, bool /*bForce*/) {}
bool WebViewPanel::GetJumpUrl(bool /*login*/, const QString& /*ticket*/, const QString& /*targeturl*/, QString& /*finalurl*/) { return false; }
void WebViewPanel::RunScript(const QString& /*javascript*/) {}
void WebViewPanel::RunScriptLeft(const QString& /*javascript*/) {}
void WebViewPanel::update_mode() {}
void WebViewPanel::OnFreshLoginStatus() {}

// SourceViewDialog
SourceViewDialog::SourceViewDialog(QWidget* parent, const QString& /*source*/)
    : QDialog(parent)
{}

} // namespace GUI
} // namespace Slic3r
