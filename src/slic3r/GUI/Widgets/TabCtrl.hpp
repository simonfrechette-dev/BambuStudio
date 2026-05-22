#ifndef slic3r_GUI_TabCtrl_hpp_
#define slic3r_GUI_TabCtrl_hpp_

#include "Button.hpp"
#include "StateColor.hpp"

#include <QHBoxLayout>
#include <QFont>
#include <vector>

class TabCtrl : public StaticBox
{
    Q_OBJECT

    std::vector<Button *> btns;
    std::vector<void *>   item_data;
    QHBoxLayout          *sizer = nullptr;
    int                   sel   = -1;
    QFont                 bold_font;

public:
    explicit TabCtrl(QWidget *parent,
                     const QPoint &pos  = {},
                     const QSize  &size = {});
    ~TabCtrl() override;

    bool setFont(const QFont &font);

    int          AppendItem(const QString &item, int image = -1, int selImage = -1, void *clientData = nullptr);
    bool         DeleteItem(int item);
    void         DeleteAllItems();
    unsigned int GetCount() const;
    int          GetSelection() const { return sel; }
    void         SelectItem(int item);
    void         Unselect();
    void         Rescale();

    QString  GetItemText(unsigned int item) const;
    void     SetItemText(unsigned int item, const QString &value);
    bool     GetItemBold(unsigned int item) const;
    void     SetItemBold(unsigned int item, bool bold);
    void    *GetItemData(unsigned int item) const;
    void     SetItemData(unsigned int item, void *clientData);
    void     AssignImageList(void * /*unused*/) {}
    void     SetItemPaddingSize(unsigned int item, const QSize &size);
    void     SetItemTextColour(unsigned int item, const StateColor &col);

    // Fakes for API compatibility
    int  GetFirstVisibleItem() const;
    int  GetNextVisible(int item) const;
    bool IsVisible(unsigned int item) const { return true; }

signals:
    void selChanging(int newSel);
    void selChanged(int newSel);

protected:
    void doRender(QPainter &painter) override;
    void keyPressEvent(QKeyEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void relayout();
    void buttonClicked();
};

#endif // !slic3r_GUI_TabCtrl_hpp_
