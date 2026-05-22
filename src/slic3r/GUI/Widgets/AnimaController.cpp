#include "AnimaController.hpp"
#include "../QtExtensions.hpp"
#include <QVBoxLayout>

AnimaIcon::AnimaIcon(QWidget *parent, int /*id*/,
                     std::vector<std::string> img_list,
                     std::string img_enable, int ivt, int size)
    : QWidget(parent)
    , m_img_list(std::move(img_list))
    , m_img_enable(std::move(img_enable))
    , m_ivt(ivt)
    , m_size(size)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    m_bitmap_label = new QLabel(this);
    m_bitmap_label->setFixedSize(size, size);
    layout->addWidget(m_bitmap_label);
    setLayout(layout);

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &AnimaIcon::onTimer);

    // Pre-load frames
    for (const auto &name : m_img_list) {
        QPixmap px;
        ScalableBitmap bmp(this, name, m_size);
        px = bmp.bmp();
        m_images.push_back(px);
    }
    if (!m_img_enable.empty()) {
        ScalableBitmap bmp(this, m_img_enable, m_size);
        m_image_enable = bmp.bmp();
    }
    if (!m_images.empty()) m_bitmap_label->setPixmap(m_images[0]);
}

AnimaIcon::~AnimaIcon() = default;

void AnimaIcon::Play()
{
    if (!m_playing && !m_images.empty()) {
        m_playing = true;
        m_current_frame = 0;
        m_timer->start(m_ivt);
    }
}

void AnimaIcon::Stop()
{
    m_playing = false;
    m_timer->stop();
    if (!m_images.empty()) m_bitmap_label->setPixmap(m_images[0]);
}

void AnimaIcon::Enable()
{
    m_enable = true;
    if (!m_image_enable.isNull()) m_bitmap_label->setPixmap(m_image_enable);
}

bool AnimaIcon::IsPlaying() const { return m_playing; }
bool AnimaIcon::IsRunning() const { return m_playing; }

void AnimaIcon::Rescale()
{
    m_images.clear();
    for (const auto &name : m_img_list) {
        ScalableBitmap bmp(this, name, m_size);
        m_images.push_back(bmp.bmp());
    }
    if (!m_images.empty()) m_bitmap_label->setPixmap(m_images[0]);
}

void AnimaIcon::onTimer()
{
    if (!m_playing || m_images.empty()) return;
    m_current_frame = (m_current_frame + 1) % (int)m_images.size();
    m_bitmap_label->setPixmap(m_images[m_current_frame]);
}
