#ifndef slic3r_GUI_ProgressBar_hpp_
#define slic3r_GUI_ProgressBar_hpp_

#include <QWidget>
#include <QColor>
#include <QString>

class ProgressBar : public QWidget
{
    Q_OBJECT
public:
    explicit ProgressBar(QWidget *parent = nullptr, int maxVal = 100,
                         bool showNumber = false);

    bool     m_shownumber  = false;
    bool     m_disable     = false;
    int      m_max         = 100;
    int      m_step        = 0;
    double   m_radius      = 7.0;
    double   m_proportion  = 0.0;
    QColor   m_progress_background_colour{233, 233, 233};
    QColor   m_progress_colour{0, 174, 66};
    QColor   m_progress_colour_disable{255, 111, 0};
    QString  m_disable_text;

    void ShowNumber(bool shown);
    void Disable(const QString &text);
    void SetValue(int step);
    void Reset();
    void SetProgress(int step);
    void SetRadius(double radius);
    void SetProgressForedColour(QColor colour);
    void SetProgressBackgroundColour(QColor colour);
    void Rescale() {}
    void SetHeight(int h) { setFixedHeight(h); m_radius = h / 2.0; }

    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void doRender(QPainter &painter);
};

#endif // !slic3r_GUI_ProgressBar_hpp_
