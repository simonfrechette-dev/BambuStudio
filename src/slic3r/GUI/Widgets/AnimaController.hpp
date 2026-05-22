#ifndef slic3r_GUI_AnimaController_hpp_
#define slic3r_GUI_AnimaController_hpp_

#include "../QtExtensions.hpp"
#include "Label.hpp"

#include <QWidget>
#include <QLabel>
#include <QTimer>
#include <QPixmap>
#include <vector>
#include <string>

class AnimaIcon : public QWidget
{
    Q_OBJECT
public:
    AnimaIcon(QWidget *parent, int id, std::vector<std::string> img_list,
              std::string img_enable, int ivt = 1000, int size = 25);
    ~AnimaIcon() override;

    void Play();
    void Stop();
    void Enable();
    bool IsPlaying() const;
    bool IsRunning() const;
    void Rescale();

private slots:
    void onTimer();

private:
    std::string              m_img_enable;
    std::vector<std::string> m_img_list;
    bool                     m_enable        = false;
    bool                     m_playing       = false;
    QLabel                  *m_bitmap_label  = nullptr;
    QPixmap                  m_image_enable;
    std::vector<QPixmap>     m_images;
    QTimer                  *m_timer;
    int                      m_current_frame = 0;
    int                      m_ivt;
    int                      m_size;
};

#endif // !slic3r_GUI_AnimaController_hpp_
