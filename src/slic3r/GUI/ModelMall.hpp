#ifndef slic3r_ModelMall_hpp_
#define slic3r_ModelMall_hpp_
#include <QWidget>
#include <QString>

#include "I18N.hpp"


#if wxUSE_WEBVIEW_IE
#endif
#if wxUSE_WEBVIEW_EDGE
#endif

#include "Widgets/WebView.hpp"

#include <curl/curl.h>
#include "wxExtensions.hpp"
#include "Plater.hpp"
#include "Widgets/StepCtrl.hpp"
#include "Widgets/Button.hpp"


#define MODEL_MALL_PAGE_SIZE QSize(FromDIP(1400 * 0.85), FromDIP(1040 * 0.75))
#define MODEL_MALL_PAGE_CONTROL_SIZE QSize(FromDIP(1400 * 0.85), FromDIP(40 * 0.75))
#define MODEL_MALL_PAGE_WEB_SIZE QSize(FromDIP(1400 * 0.85), FromDIP(1000 * 0.75))

namespace Slic3r { namespace GUI {

    class ModelMallDialog : public DPIFrame
    {
    public:
        ModelMallDialog(Plater* plater = nullptr);
        ~ModelMallDialog();

        void OnScriptMessage(QEvent& evt);
        void on_dpi_changed(const QRect& suggested_rect) override;
        void on_show(QShowEvent& event);
        void on_back(QMouseEvent& evt);
        void on_forward(QMouseEvent& evt);
        void go_to_url(QString url);
        void show_control(bool show);
        void go_to_mall(QString url);
        void go_to_publish(QString url);
        void on_refresh(QMouseEvent& evt);
    public:
        QWidget* m_web_control_panel{nullptr};
        QWidget* m_browser{nullptr};
        QString m_url;
    };

}} // namespace Slic3r::GUI

#endif
