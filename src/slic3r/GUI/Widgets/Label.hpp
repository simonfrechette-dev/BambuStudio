#ifndef slic3r_GUI_Label_hpp_
#define slic3r_GUI_Label_hpp_

#include <QLabel>
#include <QFont>
#include <QColor>
#include <QString>
#include <QSize>
#include <string>

class QPainter;
class QFontMetrics;

class Label : public QLabel
{
    Q_OBJECT
public:
    static constexpr long LB_HYPERLINK             = 0x0020;
    static constexpr long LB_PROPAGATE_MOUSE_EVENT = 0x0040;
    static constexpr long LB_AUTO_WRAP             = 0x0080;
public:
    explicit Label(QWidget *parent, const QString &text = {}, long style = 0,
                   const QSize &size = QSize());
    Label(QWidget *parent, const QFont &font, const QString &text = {},
          long style = 0, const QSize &size = QSize());

    void setText(const QString &label);

    void setStyleFlags(long style);

    void Wrap(int width);

    QSize sizeHint() const override;

protected:
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    void applyStyle();
    void rewrap();

    QFont  m_font;
    QColor m_color;
    QString m_rawText;    // unwrapped source text
    long   m_style       = 0;
    bool   m_inRewrap    = false;

public:
    // ---- Static font catalogue (same names as wxWidgets version) ----
    static QFont Head_48;
    static QFont Head_32;
    static QFont Head_24;
    static QFont Head_20;
    static QFont Head_18;
    static QFont Head_16;
    static QFont Head_15;
    static QFont Head_14;
    static QFont Head_13;
    static QFont Head_12;
    static QFont Head_11;
    static QFont Head_10;

    static QFont Body_16;
    static QFont Body_15;
    static QFont Body_14;
    static QFont Body_13;
    static QFont Body_12;
    static QFont Body_11;
    static QFont Body_10;
    static QFont Body_9;
    static QFont Body_8;

    static void  initSysFont(std::string lang_code = "", bool load_font_resource = true);
    static QFont sysFont(int size, bool bold = false, std::string lang_code = "");

    /// Word-wrap \p text to \p width pixels using \p fm, at most \p maxCount
    /// lines.  Returns wrapped string and bounding size.
    static QSize split_lines(const QFontMetrics &fm, int width,
                             const QString &text, QString &out,
                             int maxCount = 0);
};

#endif // !slic3r_GUI_Label_hpp_
