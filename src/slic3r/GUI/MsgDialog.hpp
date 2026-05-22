#ifndef slic3r_MsgDialog_hpp_
#define slic3r_MsgDialog_hpp_

// Qt port of MsgDialog.hpp

#include <string>
#include <unordered_map>
#include <functional>
#include "GUI_Utils.hpp"
#include <QFont>
#include <QPixmap>
#include <QBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QLineEdit>
#include <QFrame>
#include <QHash>
#include "Widgets/Button.hpp"
#include "Widgets/CheckBox.hpp"
#include "Widgets/TextInput.hpp"
#include "BBLStatusBar.hpp"
// BBLStatusBarSend.hpp excluded (wx dependency removed)
#include "libslic3r/Semver.hpp"

// Qt dialog button ID constants (mirrors wx button IDs for portability)
enum {
    MsgID_OK        = 5100,
    MsgID_CANCEL    = 5101,
    MsgID_YES       = 5103,
    MsgID_NO        = 5104,
    MsgID_APPLY     = 5105,
    MsgID_CLOSE     = 5106,
    MsgID_HELP      = 5107,
    MsgOK           = 0x0004,
    MsgCANCEL       = 0x0010,
    MsgYES_NO       = 0x0020,
    MsgICON_INFO = 0x0040,
    MsgICON_WARN     = 0x0080,
    MsgICON_ERROR       = 0x0100,
    MsgICON_QUESTION    = 0x0200,
    wxCLOSE        = 0x0800,
};

enum ButtonSizeType{
    ButtonSizeNormal = 0,
    ButtonSizeMiddle = 1,
    ButtonSizeLong   = 2,
};

namespace Slic3r {
namespace GUI {

struct ButtonData
{
    ButtonSizeType  type;
    Button*         button;
};

class MsgButton
{
public:
    QString     id;
    ButtonData* buttondata;
};

using MsgButtonsHash = QHash<QString, MsgButton*>;

// A message / query dialog with a bitmap on the left and any content on the right
// with buttons underneath.
struct MsgDialog : DPIDialog
{
    MsgDialog(MsgDialog &&) = delete;
    MsgDialog(const MsgDialog &) = delete;
    MsgDialog &operator=(MsgDialog &&) = delete;
    MsgDialog &operator=(const MsgDialog &) = delete;
    virtual ~MsgDialog();

    void show_dsa_button(const QString& title = {});
    bool get_checkbox_state();
    virtual void on_dpi_changed(const QRect& suggested_rect);

    void AddButton(int btn_id, const QString& label, bool set_focus = false) { add_button(btn_id, set_focus, label); }
    void SetButtonLabel(int btn_id, const QString& label, bool set_focus = false);

protected:
    enum {
        BORDER = 20,
        LOGO_SPACING = 35,
        LOGO_GAP = 20,
        CONTENT_WIDTH = 242,
        CONTENT_MAX_HEIGHT = 60,
        BTN_SPACING = 20,
        VERT_SPACING = 15,
    };

    MsgDialog(QWidget* parent, const QString& title, const QString& headline, long style = 0, const QPixmap& bitmap = QPixmap(), const QString& forward_str = "");
    Button* add_button(int btn_id, bool set_focus = false, const QString& label = {});
    Button* get_button(int btn_id);
    void apply_style(long style);
    void finalize();

    QFont        boldfont;
    QBoxLayout*  content_sizer{ nullptr };
    QBoxLayout*  btn_sizer{ nullptr };
    QBoxLayout*  m_dsa_sizer{ nullptr };
    QLabel*      logo{ nullptr };
    MsgButtonsHash m_buttons;
    CheckBox*    m_checkbox_dsa{ nullptr };
    QString      m_forward_str;
};


// Generic error dialog
class ErrorDialog : public MsgDialog
{
public:
    ErrorDialog(QWidget* parent, const QString& temp_msg, bool courier_font);
    ErrorDialog(ErrorDialog &&) = delete;
    ErrorDialog(const ErrorDialog &) = delete;
    ErrorDialog &operator=(ErrorDialog &&) = delete;
    ErrorDialog &operator=(const ErrorDialog &) = delete;
    virtual ~ErrorDialog() = default;

private:
    QString msg;
};


// Generic warning dialog
class WarningDialog : public MsgDialog
{
public:
    WarningDialog(QWidget* parent,
                  const QString& message,
                  const QString& caption = {},
                  long style = 0);
    WarningDialog(WarningDialog&&) = delete;
    WarningDialog(const WarningDialog&) = delete;
    WarningDialog &operator=(WarningDialog&&) = delete;
    WarningDialog &operator=(const WarningDialog&) = delete;
    virtual ~WarningDialog() = default;
};

// Post-processing script confirmation
class PostProcessScriptDialog : public MsgDialog
{
    QLineEdit*  m_script_text{ nullptr };
    Button*     m_toggle_details{ nullptr };
    bool        m_details_expanded{ false };

public:
    PostProcessScriptDialog(QWidget* parent, const QString& message, const QString& script_content);
    PostProcessScriptDialog(PostProcessScriptDialog&&) = delete;
    PostProcessScriptDialog(const PostProcessScriptDialog&) = delete;
    PostProcessScriptDialog& operator=(PostProcessScriptDialog&&) = delete;
    PostProcessScriptDialog& operator=(const PostProcessScriptDialog&) = delete;
    ~PostProcessScriptDialog() override = default;
};

// Generic message dialog
class MessageDialog : public MsgDialog
{
public:
    MessageDialog(QWidget* parent,
                  const QString& message,
                  const QString& caption     = {},
                  long           style       = 0,
                  const QString& forward_str = {},
                  const QString& link_text   = {},
                  std::function<void(const QString&)> link_callback = nullptr);
    MessageDialog(QWidget* parent,
                  const QString& message,
                  const QString& caption,
                  long           style,
                  const QString& forward_str,
                  const QString& link_text,
                  std::function<void(const QString&)> link_callback,
                  bool           is_marked_msg);

    MessageDialog(MessageDialog&&) = delete;
    MessageDialog(const MessageDialog&) = delete;
    MessageDialog &operator=(MessageDialog&&) = delete;
    MessageDialog &operator=(const MessageDialog&) = delete;
    virtual ~MessageDialog() = default;
};

// Generic rich message dialog
class RichMessageDialog : public MsgDialog
{
    QCheckBox* m_checkBox{ nullptr };
    QString    m_checkBoxText;
    bool       m_checkBoxValue{ false };

public:
    RichMessageDialog(QWidget* parent,
                      const QString& message,
                      const QString& caption = {},
                      long style = 0);
    RichMessageDialog(RichMessageDialog&&) = delete;
    RichMessageDialog(const RichMessageDialog&) = delete;
    RichMessageDialog &operator=(RichMessageDialog&&) = delete;
    RichMessageDialog &operator=(const RichMessageDialog&) = delete;
    virtual ~RichMessageDialog() = default;

    int exec() override;

    void ShowCheckBox(const QString& checkBoxText, bool checked = false)
    {
        m_checkBoxText  = checkBoxText;
        m_checkBoxValue = checked;
    }

    QString GetCheckBoxText()  const { return m_checkBoxText; }
    bool    IsCheckBoxChecked() const;

    virtual bool SetYesNoLabels(const QString& yes, const QString& no) {
        m_yes = yes; m_no = no; return true;
    }
    virtual bool SetYesNoCancelLabels(const QString& yes, const QString& no, const QString& cancel) {
        m_yes = yes; m_no = no; m_cancel = cancel; return true;
    }
    virtual bool SetOKLabel(const QString& ok) { m_ok = ok; return true; }
    virtual bool SetOKCancelLabels(const QString& ok, const QString& cancel) {
        m_ok = ok; m_cancel = cancel; return true;
    }
    virtual bool SetHelpLabel(const QString& help) { m_help = help; return true; }

    bool HasCustomLabels() const {
        return !(m_ok.isEmpty() && m_cancel.isEmpty() && m_help.isEmpty() &&
                 m_yes.isEmpty() && m_no.isEmpty());
    }

    QString GetYesLabel()    const { return m_yes.isEmpty()    ? tr("Yes")    : m_yes; }
    QString GetNoLabel()     const { return m_no.isEmpty()     ? tr("No")     : m_no; }
    QString GetOKLabel()     const { return m_ok.isEmpty()     ? tr("OK")     : m_ok; }
    QString GetCancelLabel() const { return m_cancel.isEmpty() ? tr("Cancel") : m_cancel; }
    QString GetHelpLabel()   const { return m_help.isEmpty()   ? tr("Help")   : m_help; }

protected:
    void DoSetCustomLabel(QString& var, const QString& label) { var = label; }
    const QString& GetCustomYesLabel()    const { return m_yes; }
    const QString& GetCustomNoLabel()     const { return m_no; }
    const QString& GetCustomOKLabel()     const { return m_ok; }
    const QString& GetCustomHelpLabel()   const { return m_help; }
    const QString& GetCustomCancelLabel() const { return m_cancel; }

private:
    QString m_yes, m_no, m_ok, m_cancel, m_help;
};

// Generic info dialog
class InfoDialog : public MsgDialog
{
public:
    InfoDialog(QWidget* parent, const QString& title, const QString& msg, bool is_marked = false, long style = 0 | 0);
    InfoDialog(InfoDialog&&) = delete;
    InfoDialog(const InfoDialog&) = delete;
    InfoDialog &operator=(InfoDialog&&) = delete;
    InfoDialog &operator=(const InfoDialog&) = delete;
    virtual ~InfoDialog() = default;

private:
    QString msg;
};

class DownloadDialog : public MsgDialog
{
public:
    DownloadDialog(QWidget* parent, const QString& title, const QString& msg, bool is_marked = false, long style = 0 | 0);
    DownloadDialog(DownloadDialog&&) = delete;
    DownloadDialog(const DownloadDialog&) = delete;
    DownloadDialog &operator=(DownloadDialog&&) = delete;
    DownloadDialog &operator=(const DownloadDialog&) = delete;
    virtual ~DownloadDialog() = default;

    void SetExtendedMessage(const QString& extendedMessage);

private:
    QString msg;
};

class DeleteConfirmDialog : public DPIDialog
{
public:
    DeleteConfirmDialog(QWidget* parent, const QString& title, const QString& msg);
    ~DeleteConfirmDialog();
    virtual void on_dpi_changed(const QRect& suggested_rect);

private:
    QString  msg;
    Button*  m_del_btn{ nullptr };
    Button*  m_cancel_btn{ nullptr };
    QLabel*  m_msg_text{ nullptr };
};

class Newer3mfVersionDialog : public DPIDialog
{
public:
    Newer3mfVersionDialog(QWidget* parent, const Semver* file_version, const Semver* cloud_version, const QString& new_keys);
    ~Newer3mfVersionDialog() {}
    virtual void on_dpi_changed(const QRect& suggested_rect) {}

private:
    QBoxLayout* get_msg_sizer();
    QBoxLayout* get_btn_sizer();

    const Semver* m_file_version{ nullptr };
    const Semver* m_cloud_version{ nullptr };
    QString       m_new_keys;
    Button*       m_update_btn{ nullptr };
    Button*       m_later_btn{ nullptr };
    QLabel*       m_msg_text{ nullptr };
};

class NetworkErrorDialog : public DPIDialog
{
public:
    NetworkErrorDialog(QWidget* parent);
    ~NetworkErrorDialog() {}
    virtual void on_dpi_changed(const QRect& suggested_rect) {}

private:
    QLabel* m_text_basic{ nullptr };
    QLabel* m_link_server_state{ nullptr };
    QLabel* m_text_proposal{ nullptr };
    QLabel* m_text_wiki{ nullptr };
    Button* m_button_confirm{ nullptr };

public:
    bool m_show_again{ false };
};

struct FilamentWarningInfo
{
    QString info_msg;
    QString wiki_url;
};

class FilamentWarningDialog : public MsgDialog
{
public:
    FilamentWarningDialog(QWidget* parent, const QString& title, std::vector<FilamentWarningInfo> infos);
    FilamentWarningDialog(FilamentWarningDialog&&) = delete;
    FilamentWarningDialog(const FilamentWarningDialog&) = delete;
    FilamentWarningDialog &operator=(FilamentWarningDialog&&) = delete;
    FilamentWarningDialog &operator=(const FilamentWarningDialog&) = delete;
    virtual ~FilamentWarningDialog() = default;

private:
    void BuildContent();
    std::vector<FilamentWarningInfo> m_messages;
};

} // namespace GUI
} // namespace Slic3r

#endif // slic3r_MsgDialog_hpp_
