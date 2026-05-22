#ifndef slic3r_GUI_ComboBox_hpp_
#define slic3r_GUI_ComboBox_hpp_

#include "TextInput.hpp"
#include "DropDown.hpp"

#define CB_NO_DROP_ICON DD_NO_CHECK_ICON
#define CB_NO_TEXT DD_NO_TEXT

class ComboBox : public TextInput
{
    Q_OBJECT
    typedef DropDown::Item Item;

    std::vector<Item> items;
    DropDown          drop;
    bool              drop_down  = false;
    bool              text_off   = false;
    bool              m_keep_drop_arrow = false;
    QString           replace_text;
    QString           image_for_text;

public:
    ComboBox(QWidget *parent, const QString &value = {},
             int n = 0, const QString choices[] = nullptr, long style = 0);

    DropDown &GetDropDown() { return drop; }

    void SetKeepDropArrow(bool keep) { m_keep_drop_arrow = keep; }

    bool setFont(const QFont &font);

    int  Append(const QString &item, const QPixmap &bitmap = {}, int item_style = 0);
    int  Append(const QString &item, const QPixmap &bitmap, void *clientData, int item_style = 0);
    int  Append(const QString &item, const QPixmap &bitmap,
                const QString &group, void *clientData = nullptr, int item_style = 0);

    int  SetItems(const std::vector<DropDown::Item> &the_items);

    void set_replace_text(const QString &text, const QString &image_name);
    unsigned int GetCount() const;

    int     GetSelection() const;
    void    SetSelection(int n);
    void    SelectAndNotify(int n);

    void    Rescale() override;

    QString GetValue() const;
    void    SetValue(const QString &value);

    void    SetLabel(const QString &label) override;
    QString GetLabel() const;

    int     GetFlag(unsigned int n) const;
    void    SetFlag(unsigned int n, int value);

    void    SetTextLabel(const QString &label);
    QString GetTextLabel() const;

    QString GetString(unsigned int n) const;
    void    SetString(unsigned int n, const QString &value);

    QString GetItemTooltip(unsigned int n) const;
    void    SetItemTooltip(unsigned int n, const QString &value);

    QString GetItemAlias(unsigned int n) const;
    void    SetItemAlias(unsigned int n, const QString &value);

    QPixmap GetItemBitmap(unsigned int n) const;
    void    SetItemBitmap(unsigned int n, const QPixmap &bitmap);

    bool    is_drop_down() const { return drop_down; }
    void    DeleteOneItem(unsigned int pos);
    void    DoClear();

signals:
    void selectionChanged(int index);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    void sendComboBoxEvent();
    void onDropDismissed();
    void onDropSelectionChanged(int idx);
};

#endif // !slic3r_GUI_ComboBox_hpp_
