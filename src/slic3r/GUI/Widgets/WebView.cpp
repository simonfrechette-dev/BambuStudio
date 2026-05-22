// Phase 7 TODO: implement WebView with QWebEngineView
#include "WebView.hpp"
#include <QWebEngineView>

QWebEngineView *WebView::CreateWebView(QWidget *parent, QString const &url)
{
    auto *view = new QWebEngineView(parent);
    view->load(QUrl(url));
    return view;
}

void WebView::LoadUrl(QWebEngineView *webView, QString const &url)
{
    if (webView) webView->load(QUrl(url));
}

bool WebView::RunScript(QWebEngineView *webView, QString const &msg)
{
    if (!webView) return false;
    webView->page()->runJavaScript(msg);
    return true;
}

void WebView::RecreateAll() {}

