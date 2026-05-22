#pragma once

// Phase 3 TODO: Qt port of MonitorBasePanel (was wxFormBuilder-generated)

#include <QWidget>
#include <QSplitter>

// Forward declarations for panels referenced by subclasses
class Button;
class SwitchButton;

namespace Slic3r {
namespace GUI {

class MediaFilePanel;

class MonitorBasePanel : public QWidget
{
    Q_OBJECT
public:
    explicit MonitorBasePanel(QWidget* parent = nullptr,
                              const QSize& size = QSize(1440, 900));
    ~MonitorBasePanel() override = default;

protected:
    QSplitter*  m_splitter            = nullptr;
    QWidget*    m_panel_splitter_left  = nullptr;
    QWidget*    m_panel_splitter_right = nullptr;
    QWidget*    m_panel_printer        = nullptr;
    QWidget*    m_panel_status_tab     = nullptr;
    QWidget*    m_panel_time_lapse_tab = nullptr;
    QWidget*    m_panel_video_tab      = nullptr;
    QWidget*    m_panel_task_list_tab  = nullptr;

    virtual void on_printer_clicked(QMouseEvent* event) {}
    virtual void on_status(QMouseEvent* event) {}
    virtual void on_timelapse(QMouseEvent* event) {}
    virtual void on_video(QMouseEvent* event) {}
    virtual void on_tasklist(QMouseEvent* event) {}
};


class VideoMonitoringBasePanel : public QWidget
{
    Q_OBJECT
public:
    explicit VideoMonitoringBasePanel(QWidget* parent = nullptr,
                                      const QSize& size = QSize(1258, 834));
    ~VideoMonitoringBasePanel() override = default;
};


class TaskListBasePanel : public QWidget
{
    Q_OBJECT
public:
    explicit TaskListBasePanel(QWidget* parent = nullptr,
                               const QSize& size = QSize(1258, 834));
    ~TaskListBasePanel() override = default;

protected:
    QWidget* m_panel_model_name_caption = nullptr;
    QWidget* m_panel_model_name_content = nullptr;
    QWidget* m_panel_plater_caption     = nullptr;
    QWidget* m_panel_plater_content     = nullptr;
};

} // namespace GUI
} // namespace Slic3r
