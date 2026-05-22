#ifndef slic3r_GUI_wxExtensions_hpp_
#define slic3r_GUI_wxExtensions_hpp_

// Phase 4: Qt port of wxExtensions.hpp
// Original wx implementation backed up to wxExtensions.hpp.wx-backup

#include <vector>
#include <functional>
#include <string>

#include <QWidget>
#include <QPushButton>
#include <QMenu>
#include <QAction>
#include <QPixmap>
#include <QSize>
#include <QColor>
#include <QLabel>
#include <QGridLayout>
#include <QListWidget>
#include <QTreeWidget>
#include <QDialog>

#include "BitmapCache.hpp"
#include "QtExtensions.hpp"

namespace Slic3r { namespace GUI { class BitmapComboBox; } }

inline void msw_rescale_menu(QMenu* /*menu*/) {}

QAction* append_menu_item(QMenu* menu, int id, const QString& label, const QString& description,
    std::function<void()> cb, const QPixmap& icon = QPixmap(),
    QObject* event_handler = nullptr,
    std::function<bool()> cb_condition = []() { return true; },
    QWidget* parent = nullptr, int insert_pos = -1);

QAction* append_menu_item(QMenu* menu, int id, const QString& label, const QString& description,
    std::function<void()> cb, const std::string& icon = "",
    QObject* event_handler = nullptr,
    std::function<bool()> cb_condition = []() { return true; },
    QWidget* parent = nullptr, int insert_pos = -1);

QAction* append_submenu(QMenu* menu, QMenu* sub_menu, int id, const QString& label, const QString& description,
    const std::string& icon = "",
    std::function<bool()> cb_condition = []() { return true; },
    QWidget* parent = nullptr, int insert_pos = -1);

QAction* append_menu_radio_item(QMenu* menu, int id, const QString& label, const QString& description,
    std::function<void()> cb, QObject* event_handler);

QAction* append_menu_check_item(QMenu* menu, int id, const QString& label, const QString& description,
    std::function<void()> cb, QObject* event_handler,
    std::function<bool()> enable_condition = []() { return true; },
    std::function<bool()> check_condition  = []() { return true; },
    QWidget* parent = nullptr);

inline void enable_menu_item(QAction* action, std::function<bool()> cb_condition)
{ if (action) action->setEnabled(cb_condition()); }

void     edit_tooltip(QString& tooltip);
void     msw_buttons_rescale(QDialog* dlg, const int em_unit, const std::vector<int>& btn_ids);
int      em_unit(QWidget* win);
int      mode_icon_px_size();

QPixmap  create_menu_bitmap(const std::string& bmp_name);

QPixmap  create_scaled_bitmap(const std::string& bmp_name, QWidget* win = nullptr,
    const int px_cnt = 16, const bool grayscale = false,
    const std::string& new_color = std::string(),
    const bool menu_bitmap = false, const bool resize = false,
    const bool bitmap2 = false,
    const std::vector<std::string>& array_new_color = std::vector<std::string>());

QPixmap  create_scaled_bitmap2(const std::string& bmp_name_in, Slic3r::GUI::BitmapCache& cache,
    QWidget* win = nullptr,
    const int px_cnt = 16, const bool grayscale = false, const bool resize = false,
    const std::vector<std::string>& array_new_color = std::vector<std::string>());

QPixmap* get_default_extruder_color_icon(bool thin_icon = false);
std::vector<QPixmap*> get_extruder_color_icons(bool thin_icon = false);
QPixmap* get_extruder_color_icon(std::string color, std::string label, int icon_width, int icon_height);
QPixmap* get_extruder_color_icon(std::vector<std::string> colors, bool is_gradient, std::string label, int icon_width, int icon_height);
std::vector<std::vector<std::string>> read_color_pack(std::vector<std::string> color_pack);
QColor   show_sys_picker_dialog(QWidget* parent, const QColor& color);

void apply_extruder_selector(Slic3r::GUI::BitmapComboBox** ctrl,
    QWidget* parent,
    const std::string& first_item = "",
    QPoint pos = QPoint(),
    QSize size = QSize(),
    bool use_thin_icon = false);

// -----------------------------------------------------------------------
// Combo popup stubs
// -----------------------------------------------------------------------

class wxCheckListBoxComboPopup : public QListWidget
{
public:
    explicit wxCheckListBoxComboPopup(QWidget* parent = nullptr) : QListWidget(parent) {}
    QString GetStringValue() const { return QString(); }
    void    SetStringValue(const QString&) {}
};

class wxDataViewTreeCtrlComboPopup : public QTreeWidget
{
public:
    explicit wxDataViewTreeCtrlComboPopup(QWidget* parent = nullptr) : QTreeWidget(parent) {}
    QString GetStringValue() const { return m_text; }
    void    SetStringValue(const QString& v) { m_text = v; }
    void    SetItemsCnt(int cnt) { m_cnt_open_items = cnt; }
private:
    QString m_text;
    int     m_cnt_open_items{0};
};

// -----------------------------------------------------------------------
// LockButton
// -----------------------------------------------------------------------

class LockButton : public QPushButton
{
    Q_OBJECT
public:
    LockButton(QWidget* parent = nullptr, int id = 0,
        QPoint pos = QPoint(), QSize size = QSize());

    bool IsLocked() const { return m_is_pushed; }
    void SetLock(bool lock);
    void enable()  { m_disabled = false; }
    void disable() { m_disabled = true; }
    void msw_rescale();

protected:
    void update_button_bitmaps();

private:
    bool           m_is_pushed {false};
    bool           m_disabled  {false};
    ScalableBitmap m_bmp_lock_closed;
    ScalableBitmap m_bmp_lock_closed_f;
    ScalableBitmap m_bmp_lock_open;
    ScalableBitmap m_bmp_lock_open_f;
};

// -----------------------------------------------------------------------
// ScalableButton
// -----------------------------------------------------------------------

class ScalableButton : public QPushButton
{
    Q_OBJECT
public:
    ScalableButton() = default;
    ScalableButton(QWidget* parent, int id = 0,
        const std::string& icon_name = "",
        const QString& label = QString(),
        QSize size = QSize(), QPoint pos = QPoint(),
        long style = 0,
        bool use_default_disabled_bitmap = false,
        int bmp_px_cnt = 16);
    ScalableButton(QWidget* parent, int id,
        const ScalableBitmap& bitmap,
        const QString& label = QString(),
        long style = 0);

    void SetBitmap_(const ScalableBitmap& bmp);
    bool SetBitmap_(const std::string& bmp_name);
    void SetBitmapDisabled_(const ScalableBitmap& bmp);
    int  GetBitmapHeight();
    void UseDefaultBitmapDisabled();
    void msw_rescale();
    void UpdateDarkUI() { msw_rescale(); }

private:
    QWidget*    m_parent              {nullptr};
    std::string m_current_icon_name;
    std::string m_disabled_icon_name;
    int         m_width               {-1};
    int         m_height              {-1};
    bool        m_use_default_disabled_bitmap {false};
    int         m_px_cnt              {16};
    bool        m_has_border          {false};
};

// -----------------------------------------------------------------------
// ModeButton
// -----------------------------------------------------------------------

class ModeButton : public ScalableButton
{
    Q_OBJECT
public:
    ModeButton(QWidget* parent = nullptr, int id = 0,
        const std::string& icon_name = "",
        const QString& mode = QString(),
        QSize size = QSize(), QPoint pos = QPoint());
    ModeButton(QWidget* parent, const QString& mode,
        const std::string& icon_name = "", int px_cnt = 16);

    void Init(const QString& mode);
    void SetState(const bool state);
    bool is_selected() { return m_is_selected; }

protected:
    void focus_button(bool focus);

private:
    bool    m_is_selected {false};
    QString m_tt_selected;
    QString m_tt_focused;
};

// -----------------------------------------------------------------------
// ModeSizer
// -----------------------------------------------------------------------

class ModeSizer : public QGridLayout
{
public:
    explicit ModeSizer(QWidget* parent, int hgap = 0);

    void SetMode(const int mode);
    void set_items_flag(int flag)     { (void)flag; }
    void set_items_border(int border) { (void)border; }
    void msw_rescale();
    const std::vector<ModeButton*>& get_btns() { return m_mode_btns; }

private:
    std::vector<ModeButton*> m_mode_btns;
    QWidget*                 m_parent        {nullptr};
    double                   m_hgap_unscaled {0.0};
};

// -----------------------------------------------------------------------
// MenuWithSeparators
// -----------------------------------------------------------------------

class MenuWithSeparators : public QMenu
{
    Q_OBJECT
public:
    explicit MenuWithSeparators(const QString& title, QWidget* parent = nullptr)
        : QMenu(title, parent) {}
    explicit MenuWithSeparators(QWidget* parent = nullptr)
        : QMenu(parent) {}

    void DestroySeparators();
    void SetFirstSeparator();
    void SetSecondSeparator();

private:
    QAction* m_separator_frst {nullptr};
    QAction* m_separator_scnd {nullptr};
};

// -----------------------------------------------------------------------
// BlinkingBitmap
// -----------------------------------------------------------------------

class BlinkingBitmap : public QLabel
{
    Q_OBJECT
public:
    BlinkingBitmap() = default;
    BlinkingBitmap(QWidget* parent, const std::string& icon_name = "blank_16");

    void msw_rescale();
    void invalidate();
    void activate();
    void blink();

    const QPixmap& get_bmp() const { return bmp.bmp(); }

private:
    ScalableBitmap bmp;
    bool           show_bmp {false};
};

// -----------------------------------------------------------------------
// ImageTransientPopup
// -----------------------------------------------------------------------

class ImageTransientPopup : public QWidget
{
    Q_OBJECT
public:
    ImageTransientPopup(QWidget* parent, bool scrolled, QPixmap pixmap);
    virtual ~ImageTransientPopup() = default;

    void SetImage(QPixmap pixmap);
    void Popup(QWidget* focus = nullptr);
    void OnDismiss();

private:
    QLabel* m_image {nullptr};
};

#endif // slic3r_GUI_wxExtensions_hpp_
