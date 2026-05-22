#ifndef slic3r_GUI_KBShortcutsDialog_hpp_
#define slic3r_GUI_KBShortcutsDialog_hpp_
#include <unordered_map>
#include <QWidget>
#include <QString>

#include <map>
#include <vector>

#include "GUI_Utils.hpp"
#include "wxExtensions.hpp"

namespace Slic3r {
namespace GUI {

class Select
{
public:
    int       m_index;
    QWidget *m_tab_button;
    QWidget *m_tab_text;
};
using SelectHash = std::unordered_map<int, Select*>;

class KBShortcutsDialog : public DPIDialog
{
    typedef std::pair<std::string, std::string> Shortcut;
    typedef std::vector<Shortcut> Shortcuts;
    typedef std::pair<std::pair<QString, QString>, Shortcuts> ShortcutsItem;
    typedef std::vector<ShortcutsItem> ShortcutsVec;

    ShortcutsVec    m_full_shortcuts;
    ScalableBitmap  m_logo_bmp;
    QLabel* m_header_bitmap;
    std::vector<QWidget*> m_pages;

public:
    KBShortcutsDialog();
    QWidget* create_button(int id, QString text);
    void          OnSelectTabel(QEvent &event);
    QWidget *m_panel_selects;
    QBoxLayout *m_sizer_right;
    QStackedWidget *m_simplebook;
    QBoxLayout *  m_sizer_body;
    SelectHash  m_hash_selector;

protected:
    void on_dpi_changed(const QRect &suggested_rect) override;

private:
    void fill_shortcuts();
    QWidget* create_header(QWidget* parent, const QFont& bold_font);
    QWidget* create_page(QWidget* parent, const ShortcutsItem& shortcuts, const QFont& font, const QFont& bold_font);
};

} // namespace GUI
} // namespace Slic3r

#endif
