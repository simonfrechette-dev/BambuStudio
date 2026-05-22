// Phase 4 TODO: Qt port of DeviceErrorDialog
#pragma once

#include <unordered_set>
#include <map>
#include <atomic>
#include <vector>
#include <string>

#include "GUI_Utils.hpp"
#include "Widgets/StateColor.hpp"
#include <nlohmann/json.hpp>

class Label;
class Button;

namespace Slic3r {

class MachineObject;

namespace GUI {

class DeviceErrorDialog : public DPIDialog
{
    Q_OBJECT
public:
    enum ActionButton : int {
        RESUME_PRINTING = 2,
        RESUME_PRINTING_DEFECTS = 3,
        RESUME_PRINTING_PROBELM_SOLVED = 4,
        STOP_PRINTING = 5,
        CHECK_ASSISTANT = 6,
        FILAMENT_EXTRUDED = 7,
        RETRY_FILAMENT_EXTRUDED = 8,
        CONTINUE = 9,
        LOAD_VIRTUAL_TRAY = 10,
        OK_BUTTON = 11,
        FILAMENT_LOAD_RESUME = 12,
        JUMP_TO_LIVEVIEW,
        NO_REMINDER_NEXT_TIME = 23,
        REFRESH_NOZZLE = 24,
        IGNORE_NO_REMINDER_NEXT_TIME = 25,
        IGNORE_RESUME = 27,
        PROBLEM_SOLVED_RESUME = 28,
        TURN_OFF_FIRE_ALARM = 29,
        RETRY_PROBLEM_SOLVED = 34,
        STOP_DRYING = 35,
        CANCLE = 37,
        REMOVE_CLOSE_BTN = 39,
        PROCEED = 41,
        OK_JUMP_RACK = 49,
        ABORT = 51,
        DISABLE_PURIFICATION = 54,
        DONT_REMIND_NEXT_TIME = 57,
        DBL_CHECK_CANCEL = 10000,
        DBL_CHECK_DONE = 10001,
        DBL_CHECK_RETRY = 10002,
        DBL_CHECK_RESUME = 10003,
        DBL_CHECK_OK = 10004,
    };
    nlohmann::json m_action_json;

public:
    DeviceErrorDialog(MachineObject* obj, QWidget* parent = nullptr);
    ~DeviceErrorDialog() override;

    QString show_error_code(int error_code);
    void    set_action_json(const nlohmann::json &action_json) { m_action_json = action_json; }
    void    update_contents(const QString& title, const QString& text, const QString& error_code,
                            const QString& image_url, const std::vector<int>& btns);

protected:
    void init_button_list();
    void init_button(ActionButton style, const QString& button_text);
    QString parse_error_level(int error_code);
    std::vector<int> convert_to_pseudo_buttons(std::string error_str);
    void on_button_click(ActionButton btn_id);

private:
    MachineObject* m_obj = nullptr;
    int m_error_code = 0;
    std::unordered_set<Button*> m_used_button;
    std::atomic<bool> m_request_cancelled{false};
    QString m_local_img_url;
    Label* m_error_msg_label  = nullptr;
    Label* m_error_code_label = nullptr;
    std::map<int, Button*> m_button_list;
    StateColor btn_bg_white;
};

}} // namespace Slic3r::GUI
