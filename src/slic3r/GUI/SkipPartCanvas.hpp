#ifndef SKIPPARTCANVAS_H
#define SKIPPARTCANVAS_H
#include <QWidget>
#include <QString>
#include <QOpenGLWidget>
#include <QOpenGLContext>
#include <QSurfaceFormat>
#include <GL/gl.h>
#include <opencv2/opencv.hpp>
#include <vector>
#include <expat.h>
#include <libslic3r/Color.hpp>
#include <boost/thread/mutex.hpp>
#include "PartSkipCommon.hpp"


namespace Slic3r {
namespace GUI {


using Coord = float;
using FloatPoint = std::array<Coord, 2>;


struct ObjectInfo {
    std::string name{""};
    int identify_id{-1};
    PartState state{psUnCheck};
};

class SkipPartCanvas : public QOpenGLWidget
{
    union SkipIdHelper
    {
        uint32_t value = 0;

        struct
        {
            uint8_t b;
            uint8_t g;
            uint8_t r;
            uint8_t _padding;
        };

        uint8_t bytes[4];

        SkipIdHelper() = default;

        SkipIdHelper(uint8_t red, uint8_t green, uint8_t blue)
            : r(red), g(green), b(blue), _padding(0) {}

        SkipIdHelper(uint32_t val): value(val){}
        void reverse() {
            uint8_t tmp{r};
            r = b;
            b = tmp;
        }
    };
public:
    SkipPartCanvas(QWidget *parent, const QSurfaceFormat& dispAttrs);
    ~SkipPartCanvas() = default;

    void SetParentBackground(const ColorRGB& color) {
        parent_color_ = color;
    }

    void LoadPickImage(const std::string& path);
    void LoadBackgroundImage(const std::string& path);
    void ZoomIn(const int zoom_percent);
    void ZoomOut(const int zoom_percent);
    void SwitchDrag(const bool drag_on);
    void UpdatePartsInfo(const PartsInfo& parts);
    void SetZoomPercent(const int value);
    void SetOffset(const QPoint &value);

    QLineEdit* log_ctrl;
protected:
    void OnPaint(QPaintEvent& event);
    void OnSize(QResizeEvent& event);
    void OnMouseLeftDown(QMouseEvent& event);
    void OnMouseLeftUp(QMouseEvent& event);
    void OnMouseRightDown(QMouseEvent& event);
    void OnMouseRightUp(QMouseEvent& event);
    void OnMouseMotion(QMouseEvent& event);
    void OnMouseWheel(QMouseEvent& event);
private:
    QOpenGLContext* context_;
    cv::Mat pick_image_;
    std::unordered_map < uint32_t, std::vector<std::vector<FloatPoint>>> parts_triangles_;
    std::unordered_map < uint32_t, std::vector<std::vector<cv::Point>>> pick_parts_;
    std::unordered_map<uint32_t, PartState> parts_state_;
    bool gl_inited_{false};
    int zoom_percent_{100};
    QPoint offset_{0,0};
    QPoint drag_start_offset_{0,0};
    QPoint drag_start_pt_{0,0};
    bool is_draging_{false};
    bool fixed_draging_{false};
    bool left_down_{false};
    ColorRGB parent_color_ = ColorRGB();
    int hover_id_{-1};
    double image_view_scale_{1};
    GLuint bg_texture_id_{0};

    void SendSelectEvent(int id, PartState state);
    void SendZoomEvent(int zoom_percent);

    inline double Zoom() const;
    inline QPoint ViewPtToImagePt(const QPoint& view_pt) const;
    uint32_t GetIdAtImagePt(const QPoint& image_pt) const;
    inline uint32_t GetIdAtViewPt(const QPoint& view_pt) const;

    void ProcessHover(const QPoint& mouse_pt);
    void AutoSetCursor();
    void StartDrag(const QPoint& mouse_pt);
    void ProcessDrag(const QPoint& mouse_pt);
    void EndDrag();

    void Render();

    void DebugLogLine(std::string str);
};

class _BBS_3MF_Base
{
    mutable boost::mutex mutex;
    mutable std::vector<std::string> m_errors;

protected:
    void add_error(const std::string& error) const;
    void clear_errors();

public:
    void log_errors();
};

struct PlateInfo
{
    int                     index{-1};
    std::vector<ObjectInfo> objects;
    bool                    label_object_enabled = false;
};

class ModelSettingHelper : public _BBS_3MF_Base
{
    struct ParseContext
    {
        std::vector<PlateInfo> plates;
        PlateInfo              current_plate;
        ObjectInfo             temp_object;
        bool                   in_plate = false;
    };

public:
    ModelSettingHelper(const std::string &path);

    bool                    Parse();
    std::vector<ObjectInfo> GetPlateObjects(int plate_idx);
    bool                    GetLabelObjectEnabled(int plate_idx);

private:
    std::string  path_;
    ParseContext context_;

    static void XMLCALL StartElementHandler(void *userData, const XML_Char *name, const XML_Char **atts);
    static void XMLCALL EndElementHandler(void *userData, const XML_Char *name);
    void                DataHandler(const XML_Char *s, int len);
};

}
}
#endif //SKIPPARTCANVAS_H