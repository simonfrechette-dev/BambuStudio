// Qt6 port of Notebook.cpp
#include "Notebook.hpp"

#include <QBoxLayout>
#include <QTabBar>
#include <QIcon>
#include <QPixmap>
#include <QSvgRenderer>
#include <QPainter>
#include "libslic3r/Utils.hpp"

namespace {
// Load an SVG from resources/images/<name>.svg and return it as a QIcon.
// Returns a null QIcon if the file is not found.
static QIcon load_tab_icon(const std::string& bmp_name, int size = 20)
{
    if (bmp_name.empty()) return {};
    const std::string path = Slic3r::var(bmp_name + ".svg");
    QSvgRenderer renderer(QString::fromStdString(path));
    if (!renderer.isValid()) return {};
    QPixmap pm(size, size);
    pm.fill(Qt::transparent);
    QPainter p(&pm);
    renderer.render(&p);
    p.end();
    return QIcon(pm);
}
} // namespace

// ---------------------------------------------------------------------------
// ButtonsListCtrl
// ---------------------------------------------------------------------------

ButtonsListCtrl::ButtonsListCtrl(QWidget* parent, QBoxLayout* side_tools)
    : QWidget(parent)
{
    hide();
    if (side_tools)
        side_tools->addWidget(this);
}

void ButtonsListCtrl::OnPaint(QPaintEvent&) {}

void ButtonsListCtrl::SetSelection(int sel)
{
    m_selection = sel;
}

void ButtonsListCtrl::UpdateMode() {}

void ButtonsListCtrl::Rescale() {}

bool ButtonsListCtrl::InsertPage(size_t n, const QString& /*text*/,
                                 bool bSelect,
                                 const std::string& /*bmp_name*/,
                                 const std::string& /*inactive_bmp_name*/)
{
    if (n > m_pageButtons.size())
        n = m_pageButtons.size();
    m_pageButtons.insert(m_pageButtons.begin() + static_cast<ptrdiff_t>(n), nullptr);
    if (bSelect)
        m_selection = static_cast<int>(n);
    return true;
}

void ButtonsListCtrl::RemovePage(size_t n)
{
    if (n < m_pageButtons.size())
        m_pageButtons.erase(m_pageButtons.begin() + static_cast<ptrdiff_t>(n));
}

bool ButtonsListCtrl::SetPageImage(size_t /*n*/, const std::string& /*bmp_name*/) const
{
    return true;
}

void ButtonsListCtrl::SetPageText(size_t /*n*/, const QString& /*strText*/) {}

QString ButtonsListCtrl::GetPageText(size_t /*n*/) const
{
    return {};
}

// ---------------------------------------------------------------------------
// Notebook
// ---------------------------------------------------------------------------

Notebook::Notebook(QWidget* parent, int /*winid*/,
                   const QPoint& /*pos*/, const QSize& /*size*/,
                   QBoxLayout* side_tools, long /*style*/)
    : QTabWidget(parent)
{
    m_bookctrl = new ButtonsListCtrl(this, side_tools);
    Init();
}

void Notebook::Init()
{
    // Dark tab bar matching the original Bambu wx design:
    //   background: #2D2D30  (dark charcoal)
    //   normal tab: #3B4446  text: #FEFEFE
    //   selected:   #00AE42  (Bambu green)
    //   hover:      #6B6B6B
    setStyleSheet(QStringLiteral(
        "QTabWidget::pane {"
        "  border: none;"
        "  background: #FFFFFF;"
        "}"
        "QTabBar {"
        "  background: #2D2D30;"
        "}"
        "QTabBar::tab {"
        "  background: #3B4446;"
        "  color: #FEFEFE;"
        "  padding: 6px 14px;"
        "  border: none;"
        "  min-width: 72px;"
        "  min-height: 36px;"
        "  margin-right: 1px;"
        "}"
        "QTabBar::tab:selected {"
        "  background: #00AE42;"
        "  color: #FEFEFE;"
        "}"
        "QTabBar::tab:hover:!selected {"
        "  background: #6B6B6B;"
        "}"
        // Corner widget area (holds Slice/Print buttons) — match tab bar bg
        "QTabWidget > QWidget {"
        "  background: #2D2D30;"
        "}"
    ));
}

bool Notebook::ShowNewPage(QWidget* page)
{
    int idx = QTabWidget::indexOf(page);
    if (idx >= 0) {
        setCurrentIndex(idx);
        return true;
    }
    return false;
}

bool Notebook::AddPage(QWidget* page,
                       const QString& text,
                       const std::string& bmp_name,
                       const std::string& inactive_bmp_name,
                       bool bSelect)
{
    int idx = QTabWidget::addTab(page, text);
    QIcon icon = load_tab_icon(bmp_name);
    if (!icon.isNull())
        QTabWidget::setTabIcon(idx, icon);
    m_bookctrl->InsertPage(static_cast<size_t>(idx), text, bSelect, bmp_name, inactive_bmp_name);
    if (bSelect)
        setCurrentIndex(idx);
    return true;
}

bool Notebook::InsertPage(size_t n, QWidget* page,
                           const QString& text,
                           const std::string& bmp_name,
                           const std::string& inactive_bmp_name,
                           bool bSelect)
{
    int idx = QTabWidget::insertTab(static_cast<int>(n), page, text);
    m_bookctrl->InsertPage(n, text, bSelect, bmp_name, inactive_bmp_name);
    if (bSelect)
        setCurrentIndex(idx);
    return true;
}

bool Notebook::InsertPage(size_t n, QWidget* page,
                           const QString& text,
                           bool bSelect, int /*imageId*/)
{
    int idx = QTabWidget::insertTab(static_cast<int>(n), page, text);
    m_bookctrl->InsertPage(n, text, bSelect);
    if (bSelect)
        setCurrentIndex(idx);
    return true;
}

int Notebook::SetSelection(size_t n)
{
    int prev = currentIndex();
    setCurrentIndex(static_cast<int>(n));
    m_bookctrl->SetSelection(static_cast<int>(n));
    return prev;
}

int Notebook::ChangeSelection(size_t n)
{
    return SetSelection(n);
}

bool Notebook::SetPageText(size_t n, const QString& strText)
{
    QTabWidget::setTabText(static_cast<int>(n), strText);
    m_bookctrl->SetPageText(n, strText);
    return true;
}

QString Notebook::GetPageText(size_t n) const
{
    return QTabWidget::tabText(static_cast<int>(n));
}

bool Notebook::SetPageImage(size_t n, const std::string& bmp_name)
{
    return m_bookctrl->SetPageImage(n, bmp_name);
}

bool Notebook::SetPageImage(size_t /*n*/, int /*imageId*/)
{
    return true;
}

int Notebook::GetPageImage(size_t /*n*/) const
{
    return -1;
}

void Notebook::UpdateMode()
{
    m_bookctrl->UpdateMode();
}

void Notebook::Rescale()
{
    m_bookctrl->Rescale();
}
