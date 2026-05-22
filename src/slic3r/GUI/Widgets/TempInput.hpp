#ifndef slic3r_GUI_TempInput_hpp_
#define slic3r_GUI_TempInput_hpp_

#include "../QtExtensions.hpp"
#include "StaticBox.hpp"
#include "StateColor.hpp"
#include <QLineEdit>
#include <QLabel>
#include <unordered_set>

enum TempInputType {
    TEMP_OF_MAIN_NOZZLE_TYPE,
    TEMP_OF_DEPUTY_NOZZLE_TYPE,
    TEMP_OF_NORMAL_TYPE
};

class TempInput : public StaticBox
{
    Q_OBJECT
public:
    enum WarningType {
        WARNING_TOO_HIGH,
        WARNING_TOO_LOW,
        WARNING_UNKNOWN,
    };

    TempInput(QWidget *parent = nullptr);
    TempInput(QWidget *parent, int type, const QString &text,
              TempInputType input_type, const QString &label = {},
              const QString &normal_icon = {}, const QString &active_icon = {});

    void init(QWidget *parent, const QString &text,
              const QString &label = {},
              const QString &normal_icon = {},
              const QString &active_icon = {});

    int  temp_type = 0;
    bool active    = false;

    void SetTagTemp(int temp);
    void SetTagTemp(const QString &temp);
    void SetCurrTemp(int temp);
    void SetCurrTemp(const QString &temp);
    void SetCurrType(TempInputType type);
    TempInputType GetCurrType() const { return m_input_type; }

    bool AllisNum(const std::string &str) const;
    void SetFinish();
    void Warning(bool warn, WarningType type = WARNING_UNKNOWN);
    void SetIconActive();
    void SetIconNormal();
    void SetReadOnly(bool ro);

    void SetMaxTemp(int temp);
    void SetMinTemp(int temp);
    void AddTemp(int t) { additional_temps.insert(t); }

    int     GetType() const { return temp_type; }
    QString GetTagTemp() const;
    QString GetCurrTemp() const;
    int     get_max_temp() const { return max_temp; }
    void    SetLabel(const QString &label);

    void SetTextColor(const StateColor &color);
    void SetLabelColor(const StateColor &color);

    virtual void Rescale();
    bool Enable(bool enable = true);
    void SetMinSize(const QSize &size);

    QLineEdit *GetTextCtrl()       { return text_ctrl; }
    const QLineEdit *GetTextCtrl() const { return text_ctrl; }

    bool IsOnChanging()  const { return m_on_changing; }
    void SetOnChanging()       { m_on_changing = true; }
    void ReSetOnChanging()     { m_on_changing = false; }

signals:
    void tempFinished(const QString &value);
    void textChanged(const QString &text);

protected:
    void doRender(QPainter &painter) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    void measureSize();
    QString erasePending(QString &str);

    bool           m_hover         = false;
    bool           m_read_only     = false;
    bool           m_on_changing   = false;
    QSize          labelSize;
    ScalableBitmap normal_icon;
    ScalableBitmap active_icon;
    ScalableBitmap degree_icon;

    StateColor   label_color;
    StateColor   text_color;

    QLineEdit *text_ctrl    = nullptr;
    QLabel    *warning_text = nullptr;
    QLabel    *curr_label   = nullptr;

    int  max_temp     = 0;
    int  min_temp     = 0;
    bool warning_mode = false;
    std::unordered_set<int> additional_temps;
    TempInputType m_input_type = TEMP_OF_NORMAL_TYPE;

    int padding_left = 0;
    static constexpr int TempInputWidth  = 200;
    static constexpr int TempInputHeight = 50;
};

#endif // !slic3r_GUI_TempInput_hpp_
