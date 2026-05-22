#ifndef slic3r_Tabbook_hpp_
#define slic3r_Tabbook_hpp_

// Phase 4 TODO: Qt port of Tabbook (was wxBookCtrlBase-based tab widget)

#include <QTabWidget>
#include <QWidget>
#include <QString>
#include <vector>
#include <string>

class ScalableButton;
class TabButton;

// replaces wxDECLARE_EVENT(wxCUSTOMEVT_TABBOOK_SEL_CHANGED, QEvent)
static constexpr int wxCUSTOMEVT_TABBOOK_SEL_CHANGED = 10200;


class TabButtonsListCtrl : public QWidget
{
    Q_OBJECT
public:
    explicit TabButtonsListCtrl(QWidget* parent = nullptr, QBoxLayout* side_tools = nullptr);
    ~TabButtonsListCtrl() override = default;

    void SetSelection(int sel);
    void showNewTag(int sel, bool show = false);
    void Rescale();
    bool InsertPage(size_t n, const QString& text, bool bSelect = false, const std::string& bmp_name = "");
    void RemovePage(size_t n);
    bool SetPageImage(size_t n, const std::string& bmp_name);
    void SetPageText(size_t n, const QString& strText);
    QString GetPageText(size_t n) const;
    TabButton* pageButton = nullptr;

private:
    std::vector<TabButton*> m_pageButtons;
    int m_selection = -1;
};


class Tabbook : public QTabWidget
{
    Q_OBJECT
public:
    explicit Tabbook(QWidget* parent = nullptr,
                     QBoxLayout* side_tools = nullptr,
                     long style = 0);
    ~Tabbook() override = default;

    bool AddPage(QWidget* page, const QString& text, const std::string& bmp_name = "", bool bSelect = false);
    bool InsertPage(size_t n, QWidget* page, const QString& text, bool bSelect = false, int imageId = -1);
    bool InsertNewPage(size_t n, QWidget* page, const QString& text, const std::string& bmp_name = "", bool bSelect = false);
    bool RemovePage(size_t n);
    bool ShowNewPage(QWidget* page);
    void Rescale();
    QString GetPageText(size_t n) const;
    void SetPageText(size_t n, const QString& strText);

    void SetEffects(int showEffect, int hideEffect) {}
    void SetEffect(int effect) {}
    void SetEffectsTimeouts(unsigned showTimeout, unsigned hideTimeout) {}
    void SetEffectTimeout(unsigned timeout) {}

    TabButtonsListCtrl* GetBtnsListCtrl() { return m_bookctrl; }
    void showNewTag(int sel, bool show = false);

private:
    TabButtonsListCtrl* m_bookctrl = nullptr;
};

#endif // slic3r_Tabbook_hpp_
