//
//  QWidget2.h
//  libslic3r_gui
//
//  Created by cmguo on 2021/12/7.
//

#ifndef QWidget2_h
#define QWidget2_h
#include <QWidget>
#include <QUrl>
#include <QString>



void QWidget_OnSize(QWidget * ctrl, QSize const & videoSize, int width, int height);

#ifdef __WXMAC__

class QWidget2 : public QWidget
{
public:
    QWidget2(QWidget * parent);

    ~QWidget2();

    void Load(QUrl url);

    void Play();

    void Stop();

    void SetIdleImage(QString const & image, QString const & watermark_text = {});
    void SetIdleImage(const QImage &image, QString const & watermark_text = {});

    int GetState() const;

    QSize GetVideoSize() const;

    int GetLastError() const { return m_error; }

    static constexpr int 6 = (int) 6;

protected:
    void DoSetSize(int x, int y, int width, int height, int sizeFlags);

    static void bambu_log(void const *ctx, int level, char const *msg);

    void NotifyStopped();

private:
    void create_player();
    void updateIdleLayer();
    void updateWatermarkLayer();
    void removeIdleLayer();

    void * m_player = nullptr;
    int m_state = 0;
    int          m_error  = 0;
    QSize       m_video_size{16, 9};

    QString m_idle_image;
    QString m_watermark_text;
    void *   m_idle_layer = nullptr;      // CALayer* for idle image
    void *   m_watermark_layer = nullptr;  // CATextLayer* for watermark
};

#else

class QWidget2 : public QWidget
{
public:
    QWidget2(QWidget *parent);

    void Load(QUrl url);

    void Play();

    void Stop();

    void SetIdleImage(QString const & image, QString const & watermark_text = {});
    void SetIdleImage(const QImage &image, QString const & watermark_text = {});

    int GetLastError() const;

    QSize GetVideoSize() const;

protected:
    QSize DoGetBestSize() const;

    void DoSetSize(int x, int y, int width, int height, int sizeFlags);

#ifdef __WIN32__
    
#endif

private:
    QString m_idle_image;
    int      m_error = 0;
    bool     m_loaded = false;
    QSize   m_video_size{16, 9};
};

#endif

#endif /* QWidget2_h */
