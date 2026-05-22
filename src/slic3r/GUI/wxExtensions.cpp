// Phase 4: Qt stub implementation of wxExtensions.cpp
// Original wx implementation backed up to wxExtensions.cpp.wx-backup

#include "wxExtensions.hpp"
#include "BitmapCache.hpp"

// -----------------------------------------------------------------------
// Free functions
// -----------------------------------------------------------------------

QAction* append_menu_item(QMenu* menu, int /*id*/, const QString& label, const QString& description,
    std::function<void()> cb, const QPixmap& icon,
    QObject* /*event_handler*/,
    std::function<bool()> cb_condition,
    QWidget* /*parent*/, int /*insert_pos*/)
{
    if (!menu) return nullptr;
    auto* action = menu->addAction(icon, label);
    action->setToolTip(description);
    action->setEnabled(cb_condition());
    if (cb) QObject::connect(action, &QAction::triggered, cb);
    return action;
}

QAction* append_menu_item(QMenu* menu, int id, const QString& label, const QString& description,
    std::function<void()> cb, const std::string& /*icon*/,
    QObject* event_handler,
    std::function<bool()> cb_condition,
    QWidget* parent, int insert_pos)
{
    return append_menu_item(menu, id, label, description, cb, QPixmap(),
        event_handler, cb_condition, parent, insert_pos);
}

QAction* append_submenu(QMenu* menu, QMenu* sub_menu, int /*id*/, const QString& label, const QString& /*desc*/,
    const std::string& /*icon*/, std::function<bool()> cb_condition,
    QWidget* /*parent*/, int /*insert_pos*/)
{
    if (!menu || !sub_menu) return nullptr;
    sub_menu->setTitle(label);
    auto* action = menu->addMenu(sub_menu);
    if (action) action->setEnabled(cb_condition());
    return action;
}

QAction* append_menu_radio_item(QMenu* menu, int /*id*/, const QString& label,
    const QString& description, std::function<void()> cb, QObject* /*event_handler*/)
{
    if (!menu) return nullptr;
    auto* action = menu->addAction(label);
    action->setCheckable(true);
    action->setToolTip(description);
    if (cb) QObject::connect(action, &QAction::triggered, cb);
    return action;
}

QAction* append_menu_check_item(QMenu* menu, int /*id*/, const QString& label,
    const QString& description, std::function<void()> cb, QObject* /*event_handler*/,
    std::function<bool()> enable_condition, std::function<bool()> check_condition,
    QWidget* /*parent*/)
{
    if (!menu) return nullptr;
    auto* action = menu->addAction(label);
    action->setCheckable(true);
    action->setToolTip(description);
    action->setEnabled(enable_condition());
    action->setChecked(check_condition());
    if (cb) QObject::connect(action, &QAction::triggered, cb);
    return action;
}

void edit_tooltip(QString& /*tooltip*/) {}
void msw_buttons_rescale(QDialog* /*dlg*/, const int /*em_unit*/, const std::vector<int>& /*btn_ids*/) {}
int  em_unit(QWidget* /*win*/) { return 10; }
int  mode_icon_px_size() { return 16; }

QPixmap create_menu_bitmap(const std::string& /*bmp_name*/) { return {}; }

QPixmap create_scaled_bitmap(const std::string& /*bmp_name*/, QWidget* /*win*/,
    const int /*px_cnt*/, const bool /*grayscale*/,
    const std::string& /*new_color*/, const bool /*menu_bitmap*/,
    const bool /*resize*/, const bool /*bitmap2*/,
    const std::vector<std::string>& /*array_new_color*/)
{ return {}; }

QPixmap create_scaled_bitmap2(const std::string& /*bmp_name_in*/, Slic3r::GUI::BitmapCache& /*cache*/,
    QWidget* /*win*/, const int /*px_cnt*/, const bool /*grayscale*/,
    const bool /*resize*/, const std::vector<std::string>& /*array_new_color*/)
{ return {}; }

QPixmap* get_default_extruder_color_icon(bool /*thin_icon*/) { return nullptr; }
std::vector<QPixmap*> get_extruder_color_icons(bool /*thin_icon*/) { return {}; }
QPixmap* get_extruder_color_icon(std::string /*color*/, std::string /*label*/, int /*w*/, int /*h*/) { return nullptr; }
QPixmap* get_extruder_color_icon(std::vector<std::string> /*colors*/, bool /*is_gradient*/, std::string /*label*/, int /*w*/, int /*h*/) { return nullptr; }
std::vector<std::vector<std::string>> read_color_pack(std::vector<std::string> /*color_pack*/) { return {}; }
QColor show_sys_picker_dialog(QWidget* /*parent*/, const QColor& color) { return color; }

void apply_extruder_selector(Slic3r::GUI::BitmapComboBox** /*ctrl*/, QWidget* /*parent*/,
    const std::string& /*first_item*/, QPoint /*pos*/, QSize /*size*/, bool /*use_thin_icon*/) {}

// -----------------------------------------------------------------------
// ScalableBitmap — defined in QtExtensions.cpp; stub removed to avoid ODR violation
// -----------------------------------------------------------------------

// -----------------------------------------------------------------------
// LockButton
// -----------------------------------------------------------------------

LockButton::LockButton(QWidget* parent, int /*id*/, QPoint /*pos*/, QSize /*size*/)
    : QPushButton(parent)
{}

void LockButton::SetLock(bool lock) { m_is_pushed = lock; update_button_bitmaps(); }
void LockButton::update_button_bitmaps() {}
void LockButton::msw_rescale() {}

// -----------------------------------------------------------------------
// ScalableButton
// -----------------------------------------------------------------------

ScalableButton::ScalableButton(QWidget* parent, int /*id*/,
    const std::string& icon_name, const QString& label,
    QSize /*size*/, QPoint /*pos*/,
    long /*style*/, bool use_default_disabled_bitmap, int bmp_px_cnt)
    : QPushButton(label, parent)
    , m_parent(parent), m_current_icon_name(icon_name)
    , m_use_default_disabled_bitmap(use_default_disabled_bitmap)
    , m_px_cnt(bmp_px_cnt)
{}

ScalableButton::ScalableButton(QWidget* parent, int /*id*/,
    const ScalableBitmap& bitmap, const QString& label, long /*style*/)
    : QPushButton(label, parent)
    , m_parent(parent), m_current_icon_name(bitmap.name())
{}

void ScalableButton::SetBitmap_(const ScalableBitmap& bmp) { setIcon(QIcon(bmp.bmp())); }
bool ScalableButton::SetBitmap_(const std::string& bmp_name) { m_current_icon_name = bmp_name; return true; }
void ScalableButton::SetBitmapDisabled_(const ScalableBitmap& bmp) { (void)bmp; }
int  ScalableButton::GetBitmapHeight() { return m_px_cnt; }
void ScalableButton::UseDefaultBitmapDisabled() { m_use_default_disabled_bitmap = true; }
void ScalableButton::msw_rescale() {}

// -----------------------------------------------------------------------
// ModeButton
// -----------------------------------------------------------------------

ModeButton::ModeButton(QWidget* parent, int id,
    const std::string& icon_name, const QString& mode,
    QSize size, QPoint pos)
    : ScalableButton(parent, id, icon_name, mode, size, pos)
{}

ModeButton::ModeButton(QWidget* parent, const QString& mode,
    const std::string& icon_name, int px_cnt)
    : ScalableButton(parent, 0, icon_name, mode, {}, {}, 0, false, px_cnt)
{}

void ModeButton::Init(const QString& mode) { setText(mode); }
void ModeButton::SetState(const bool state) { m_is_selected = state; focus_button(state); }
void ModeButton::focus_button(bool /*focus*/) {}

// -----------------------------------------------------------------------
// ModeSizer
// -----------------------------------------------------------------------

ModeSizer::ModeSizer(QWidget* parent, int /*hgap*/)
    : QGridLayout(parent), m_parent(parent)
{}

void ModeSizer::SetMode(const int /*mode*/) {}
void ModeSizer::msw_rescale() {}

// -----------------------------------------------------------------------
// MenuWithSeparators
// -----------------------------------------------------------------------

void MenuWithSeparators::DestroySeparators()
{
    if (m_separator_frst) { removeAction(m_separator_frst); m_separator_frst = nullptr; }
    if (m_separator_scnd) { removeAction(m_separator_scnd); m_separator_scnd = nullptr; }
}

void MenuWithSeparators::SetFirstSeparator()
{
    if (!m_separator_frst) m_separator_frst = addSeparator();
}

void MenuWithSeparators::SetSecondSeparator()
{
    if (!m_separator_scnd) m_separator_scnd = addSeparator();
}

// -----------------------------------------------------------------------
// BlinkingBitmap
// -----------------------------------------------------------------------

BlinkingBitmap::BlinkingBitmap(QWidget* parent, const std::string& icon_name)
    : QLabel(parent), bmp(parent, icon_name)
{}

void BlinkingBitmap::msw_rescale() { bmp.msw_rescale(); }
void BlinkingBitmap::invalidate()  { show_bmp = false; setPixmap(QPixmap()); }
void BlinkingBitmap::activate()    { show_bmp = true;  setPixmap(bmp.bmp()); }
void BlinkingBitmap::blink()
{
    show_bmp = !show_bmp;
    setPixmap(show_bmp ? bmp.bmp() : QPixmap());
}

// -----------------------------------------------------------------------
// ImageTransientPopup
// -----------------------------------------------------------------------

ImageTransientPopup::ImageTransientPopup(QWidget* parent, bool /*scrolled*/, QPixmap pixmap)
    : QWidget(parent, Qt::Popup)
    , m_image(new QLabel(this))
{
    m_image->setPixmap(pixmap);
}

void ImageTransientPopup::SetImage(QPixmap pixmap) { if (m_image) m_image->setPixmap(pixmap); }
void ImageTransientPopup::Popup(QWidget* /*focus*/) { show(); }
void ImageTransientPopup::OnDismiss() { hide(); }
