#ifndef slic3r_PrintHostSendDialog_hpp_
#define slic3r_PrintHostSendDialog_hpp_

#include <set>
#include <QDialog>
#include <QString>
#include <QStringList>
#include <string>
#include <boost/filesystem/path.hpp>


#include "GUI_Utils.hpp"
#include "MsgDialog.hpp"
#include "../Utils/PrintHost.hpp"

class QPushButton;
class QLineEdit;
class QComboBox;
class QComboBox;
class QListView;

namespace Slic3r {

namespace GUI {

class PrintHostSendDialog : public GUI::MsgDialog
{
public:
    PrintHostSendDialog(const boost::filesystem::path &path, PrintHostPostUploadActions post_actions, const QStringList& groups);
    boost::filesystem::path filename() const;
    PrintHostPostUploadAction post_action() const;
    std::string group() const;

    void accept() override;
    void reject() override;
private:
    QLineEdit *txt_filename;
    QComboBox *combo_groups;
    PrintHostPostUploadAction post_upload_action;
    QString    m_valid_suffix;
};


class PrintHostQueueDialog : public DPIDialog
{
public:
    class Event : public QEvent {
    public:
        size_t job_id{0};
        int progress{0};
        QString error;
        Event(QEvent::Type t, size_t jid) : QEvent(t), job_id(jid) {}
        Event(QEvent::Type t, size_t jid, int p) : QEvent(t), job_id(jid), progress(p) {}
        Event(QEvent::Type t, size_t jid, QString e) : QEvent(t), job_id(jid), error(std::move(e)) {}
    };

    PrintHostQueueDialog(QWidget *parent);

    void append_job(const PrintHostJob &job);
    void get_active_jobs(std::vector<std::pair<std::string, std::string>>& ret);

    void setVisible(bool show) override {
        if(!show)
            save_user_data(UDT_SIZE | UDT_POSITION | UDT_COLS);
        DPIDialog::setVisible(show);
    }
protected:
    void on_dpi_changed(const QRect &suggested_rect) override;
    void on_sys_color_changed() override;

private:
    enum Column {
        COL_ID,
        COL_PROGRESS,
        COL_STATUS,
        COL_HOST,
        COL_SIZE,
        COL_FILENAME,
        COL_ERRORMSG
    };

    enum JobState {
        ST_NEW,
        ST_PROGRESS,
        ST_ERROR,
        ST_CANCELLING,
        ST_CANCELLED,
        ST_COMPLETED,
    };

    enum { HEIGHT = 60, WIDTH = 30, SPACING = 5 };

    enum UserDataType{
        UDT_SIZE = 1,
        UDT_POSITION = 2,
        UDT_COLS = 4
    };

    QPushButton *btn_cancel;
    QPushButton *btn_error;
    QListView *job_list;
    // Note: EventGuard prevents delivery of progress evts to a freed PrintHostQueueDialog
    EventGuard on_progress_evt;
    EventGuard on_error_evt;
    EventGuard on_cancel_evt;

    JobState get_state(int idx);
    void set_state(int idx, JobState);
    void on_list_select();
    void on_progress(Event&);
    void on_error(Event&);
    void on_cancel(Event&);
    // This vector keep adress and filename of uploads. It is used when checking for running uploads during exit.
    std::vector<std::pair<std::string, std::string>> upload_names;
    void save_user_data(int);
    bool load_user_data(int, std::vector<int>&);
};


}}

#endif
