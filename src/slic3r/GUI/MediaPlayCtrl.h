//
//  MediaPlayCtrl.h
//  libslic3r_gui
//
//  Created by cmguo on 2021/12/7.
//

#ifndef MediaPlayCtrl_h
#define MediaPlayCtrl_h
#include <QWidget>
#include <QString>
#include <QLabel>

#define USE_WX_MEDIA_CTRL_2 0

#include "wxMediaCtrl3.h"


#include <boost/thread.hpp>
#include <boost/thread/condition_variable.hpp>

#include <chrono>
#include <deque>
#include <functional>
#include <set>
#include <memory>
#include <vector>
#include <cstddef>
#include <string>
#include <atomic>

class Button;
class Label;

namespace Slic3r {

class MachineObject;
class FileTransferObject;

namespace GUI {

class MediaPlayCtrl : public QWidget
{
public:
    MediaPlayCtrl(QWidget *parent, QWidget *media_ctrl, const QPoint &pos = QPoint(), const QSize &size = QSize());

    ~MediaPlayCtrl();

    void SetMachineObject(MachineObject * obj);

    bool IsStreaming() const;

    void ToggleStream();

    void msw_rescale();

    void jump_to_play();

    void RequestFileSystemUrl(std::function<void(std::string url)> cb, bool lan_mode=true);

    using ImageResultCb = std::function<void(int ec, int resp_ec, std::string json, std::vector<std::byte> data)>;
    void SetDeviceImageUrl(std::string url);

protected:
    void onStateChanged(QEvent & event);

    void Play();

    void Stop(QString const &msg = {}, QString const &msg2 = {});

    void TogglePlay();

    void SetStatus(QString const &msg, bool hyperlink = true);

private:
    void load();

    void start_device_image_flow();

    void on_show_hide(QShowEvent & evt);

    void media_proc();

    static bool start_stream_service(bool *need_install = nullptr);

    static bool get_stream_url(std::string *url = nullptr);

private:
    static constexpr int MEDIASTATE_IDLE = (int) 3;
    static constexpr int MEDIASTATE_INITIALIZING = (int) 4;
    static constexpr int MEDIASTATE_LOADING = (int) 5;
    static constexpr int MEDIASTATE_BUFFERING = (int) 6;

    // token
    std::shared_ptr<int> m_token = std::make_shared<int>(0);

    QWidget * m_media_ctrl;
    int m_last_state = MEDIASTATE_IDLE;
    std::string m_machine;
    int m_lan_proto = 0;
    std::string m_lan_ip;
    std::string m_lan_user;
    std::string m_lan_passwd;
    std::string m_dev_ver;
    std::string m_tutk_state;
    bool m_camera_exists = false;
    bool m_support_liveview_preview = false;
    bool m_lan_mode = false;
    int m_remote_proto = 0;
    bool m_device_busy = false;
    bool m_disable_lan = false;
    QString m_url;

    std::deque<QString> m_tasks;
    boost::mutex m_mutex;
    boost::condition_variable m_cond;
    boost::thread m_thread;

    bool m_streaming = false;
    bool m_user_triggered = false;
    int m_failed_retry = 0;
    int m_failed_code = 0;
    std::vector<double> m_stat;
    std::set<int> m_last_failed_codes;
    QDateTime    m_last_user_play;
    QDateTime    m_next_retry;
    std::chrono::system_clock::time_point m_play_timer;
    int           m_print_idle = 0;
    int           m_load_duration = 0;

    std::shared_ptr<int> m_image_token = std::make_shared<int>(0);
    std::chrono::steady_clock::time_point m_image_last_success_time;
    std::string m_image_last_machine;

    ::Button *m_button_play;
    ::Label * m_label_stat;
    ::Label * m_label_status;

    std::shared_ptr<FileTransferObject> m_image_transfer;
};

}}

#endif /* MediaPlayCtrl_h */
