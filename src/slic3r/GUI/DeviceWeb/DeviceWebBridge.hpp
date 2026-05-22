#ifndef DEVICEWEBBRIDGE_HPP
#define DEVICEWEBBRIDGE_HPP

#include <nlohmann/json.hpp>
#include <atomic>
#include <chrono>
#include <optional>
#include <QString>

#include "slic3r/GUI/Widgets/WebView.hpp"
#include "DeviceWebModel.hpp"
#include "DeviceWebManager.hpp"

namespace Slic3r { namespace GUI {

static constexpr const char* DEVICE_WEB_BRIDGE_VERSION = "1.0";

class DeviceWebBridge
{
private:
    std::atomic<std::uint64_t> m_seq{0};
    QWebEngineView*    m_web{nullptr};
    DeviceWebManager*  m_vm_mgr{nullptr};

    static inline std::uint64_t TimeNowMs() {
        return static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count());
    }

    template<typename T>
    void SendMsg(const Header& head, T&& params)
    {
        nlohmann::json j;
        j["event"] = "device";
        j["head"]  = head;
        j["body"]  = std::forward<T>(params);
        std::string script = "window.__cppPush(" + j.dump() + ");";
        WebView::RunScript(m_web, QString::fromStdString(script));
    }

    void InitBridge();
    void OnWebLoaded();
    void OnWebNav();
    void OnWebMsg();
    bool ValidateJson(const nlohmann::json& j);
    bool ValidateHeader(const Header& head);
    void DispatchWebCommand(const nlohmann::json& header, const nlohmann::json& body);

public:
    DeviceWebBridge(QWebEngineView* webView);
    ~DeviceWebBridge();

    void SetManager(DeviceWebManager* mgr) { m_vm_mgr = mgr; }

    template<typename T>
    void ReportMsg(T&& params) {
        Header head{DEVICE_WEB_BRIDGE_VERSION, MsgType::Report, m_seq++, TimeNowMs()};
        SendMsg(head, std::forward<T>(params));
    }

    template<typename T>
    void ResponseMsg(Header head, T&& params) {
        head.type = MsgType::Response;
        head.ts   = TimeNowMs();
        SendMsg(head, std::forward<T>(params));
    }
};

}} // namespace Slic3r::GUI

#endif // DEVICEWEBBRIDGE_HPP
