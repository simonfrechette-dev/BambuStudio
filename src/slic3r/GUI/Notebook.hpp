#ifndef slic3r_Notebook_hpp_
#define slic3r_Notebook_hpp_
#include <QWidget>
#include <QTabWidget>
#include <QBoxLayout>
#include <QGridLayout>
#include <QString>
#include <vector>
#include <string>

class ModeSizer;
class ScalableButton;
class Button;

// Custom tab button list control (tab bar replacement)
class ButtonsListCtrl : public QWidget
{
public:
    ButtonsListCtrl(QWidget* parent, QBoxLayout* side_tools = nullptr);
    ~ButtonsListCtrl() {}

    void OnPaint(QPaintEvent&);
    void SetSelection(int sel);
    void UpdateMode();
    void Rescale();
    bool InsertPage(size_t n, const QString &text, bool bSelect = false,
                   const std::string &bmp_name = "",
                   const std::string &inactive_bmp_name = "");
    void RemovePage(size_t n);
    bool SetPageImage(size_t n, const std::string& bmp_name) const;
    void SetPageText(size_t n, const QString& strText);
    QString GetPageText(size_t n) const;

private:
    QGridLayout*         m_buttons_sizer {nullptr};
    QBoxLayout*          m_sizer         {nullptr};
    std::vector<Button*> m_pageButtons;
    int                  m_selection     {-1};
    int                  m_btn_margin    {0};
    int                  m_line_margin   {0};
};

// Custom notebook (tab widget with custom tab bar)
class Notebook : public QTabWidget
{
public:
    Notebook(QWidget* parent,
             int winid = -1,
             const QPoint& pos = QPoint(),
             const QSize& size = QSize(),
             QBoxLayout* side_tools = nullptr,
             long style = 0);
    ~Notebook() {}

    // Add/insert pages
    bool ShowNewPage(QWidget* page);

    bool AddPage(QWidget* page,
                 const QString& text,
                 const std::string& bmp_name,
                 const std::string& inactive_bmp_name,
                 bool bSelect = false);

    bool InsertPage(size_t n,
                    QWidget* page,
                    const QString& text,
                    const std::string& bmp_name = "",
                    const std::string& inactive_bmp_name = "",
                    bool bSelect = false);

    bool InsertPage(size_t n,
                    QWidget* page,
                    const QString& text,
                    bool bSelect,
                    int imageId = -1);

    // Selection
    int  SetSelection(size_t n);
    int  ChangeSelection(size_t n);

    // Page text/image
    bool    SetPageText(size_t n, const QString& strText);
    QString GetPageText(size_t n) const;
    bool    SetPageImage(size_t n, const std::string& bmp_name);
    bool    SetPageImage(size_t n, int imageId);
    int     GetPageImage(size_t n) const;

    // Effect stubs (no-ops in Qt port)
    void SetEffects(int showEffect, int hideEffect) {}
    void SetEffect(int effect)                      {}
    void SetEffectsTimeouts(unsigned showTimeout, unsigned hideTimeout) {}
    void SetEffectTimeout(unsigned timeout)         {}

    // Access to the custom button list control
    ButtonsListCtrl* GetBtnsListCtrl() const { return m_bookctrl; }
    void UpdateMode();
    void Rescale();

private:
    ButtonsListCtrl* m_bookctrl    {nullptr};
    int              m_showEffect  {0};
    int              m_hideEffect  {0};
    unsigned         m_showTimeout {0};
    unsigned         m_hideTimeout {0};

    void Init();
};

#endif // slic3r_Notebook_hpp_
