#include "Tabbook.hpp"
#include "TabButton.hpp"

#include <QBoxLayout>
#include <QTabBar>

// ---------------------------------------------------------------------------
// TabButtonsListCtrl
// ---------------------------------------------------------------------------

TabButtonsListCtrl::TabButtonsListCtrl(QWidget* parent, QBoxLayout* side_tools)
    : QWidget(parent)
{
    // Custom tab buttons live inside the side_tools layout if provided.
    // TabButton is still a stub so keep this widget hidden until it's ready.
    hide();
    if (side_tools)
        side_tools->addWidget(this);
}

void TabButtonsListCtrl::SetSelection(int sel)
{
    m_selection = sel;
    // Visual update deferred until TabButton is fully implemented.
}

void TabButtonsListCtrl::showNewTag(int sel, bool show)
{
    if (sel >= 0 && sel < static_cast<int>(m_pageButtons.size()) && m_pageButtons[sel])
        m_pageButtons[sel]->ShowNewTag(show);
}

void TabButtonsListCtrl::Rescale()
{
    for (auto* btn : m_pageButtons)
        if (btn) btn->Rescale();
}

bool TabButtonsListCtrl::InsertPage(size_t n, const QString& /*text*/,
                                    bool bSelect, const std::string& /*bmp_name*/)
{
    if (n > m_pageButtons.size())
        n = m_pageButtons.size();
    m_pageButtons.insert(m_pageButtons.begin() + static_cast<ptrdiff_t>(n), nullptr);
    if (bSelect)
        m_selection = static_cast<int>(n);
    return true;
}

void TabButtonsListCtrl::RemovePage(size_t n)
{
    if (n < m_pageButtons.size())
        m_pageButtons.erase(m_pageButtons.begin() + static_cast<ptrdiff_t>(n));
}

bool TabButtonsListCtrl::SetPageImage(size_t /*n*/, const std::string& /*bmp_name*/)
{
    return true;
}

void TabButtonsListCtrl::SetPageText(size_t /*n*/, const QString& /*strText*/)
{
    // deferred until TabButton is implemented
}

QString TabButtonsListCtrl::GetPageText(size_t /*n*/) const
{
    return {};
}

// ---------------------------------------------------------------------------
// Tabbook
// ---------------------------------------------------------------------------

Tabbook::Tabbook(QWidget* parent, QBoxLayout* side_tools, long /*style*/)
    : QTabWidget(parent)
{
    m_bookctrl = new TabButtonsListCtrl(this, side_tools);
    // Use QTabWidget's built-in tab bar until custom TabButton rendering is ready.
}

bool Tabbook::AddPage(QWidget* page, const QString& text,
                      const std::string& bmp_name, bool bSelect)
{
    int idx = QTabWidget::addTab(page, text);
    m_bookctrl->InsertPage(static_cast<size_t>(idx), text, bSelect, bmp_name);
    if (bSelect)
        setCurrentIndex(idx);
    return true;
}

bool Tabbook::InsertPage(size_t n, QWidget* page, const QString& text,
                         bool bSelect, int /*imageId*/)
{
    int idx = QTabWidget::insertTab(static_cast<int>(n), page, text);
    m_bookctrl->InsertPage(n, text, bSelect);
    if (bSelect)
        setCurrentIndex(idx);
    return true;
}

bool Tabbook::InsertNewPage(size_t n, QWidget* page, const QString& text,
                            const std::string& bmp_name, bool bSelect)
{
    int idx = QTabWidget::insertTab(static_cast<int>(n), page, text);
    m_bookctrl->InsertPage(n, text, bSelect, bmp_name);
    if (bSelect)
        setCurrentIndex(idx);
    return true;
}

bool Tabbook::RemovePage(size_t n)
{
    QTabWidget::removeTab(static_cast<int>(n));
    m_bookctrl->RemovePage(n);
    return true;
}

bool Tabbook::ShowNewPage(QWidget* page)
{
    int idx = QTabWidget::indexOf(page);
    if (idx >= 0) {
        setCurrentIndex(idx);
        return true;
    }
    return false;
}

void Tabbook::Rescale()
{
    m_bookctrl->Rescale();
}

QString Tabbook::GetPageText(size_t n) const
{
    return QTabWidget::tabText(static_cast<int>(n));
}

void Tabbook::SetPageText(size_t n, const QString& strText)
{
    QTabWidget::setTabText(static_cast<int>(n), strText);
    m_bookctrl->SetPageText(n, strText);
}

void Tabbook::showNewTag(int sel, bool show)
{
    m_bookctrl->showNewTag(sel, show);
}

