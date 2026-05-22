#ifndef slic3r_GUI_DragCanvas_hpp_
#define slic3r_GUI_DragCanvas_hpp_
#include <QWidget>
#include <QString>


namespace Slic3r { namespace GUI {

class DragShape : public QObject
{
public:
    DragShape(const QPixmap& bitmap, int index);
    ~DragShape() {}

    QPoint GetPosition() const { return m_pos; }
    void SetPosition(const QPoint& pos) { m_pos = pos; }

    const QPixmap& GetBitmap() const { return m_bitmap; }
    void SetBitmap(const QPixmap& bitmap) { m_bitmap = bitmap; }

    int get_index() { return m_index; }

    bool hit_test(const QPoint& pt) const;
    void paint(QPainter& dc, bool highlight = false);

protected:
    QPoint     m_pos;
    QPixmap    m_bitmap;
    int         m_index;
};


enum class DragMode {
    NONE,
    DRAGGING,
};
class DragCanvas : public QWidget
{
public:
    DragCanvas(QWidget* parent, const std::vector<std::string>& colors, const std::vector<int>& order);
    ~DragCanvas();
    void set_shape_list(const std::vector<std::string>& colors, const std::vector<int>& order);
    std::vector<int> get_shape_list_order();
    std::vector<DragShape*> get_ordered_shape_list();

protected:
    void on_paint(QPaintEvent& event);
    void on_erase(QEvent& event);
    void on_mouse(QMouseEvent& event);
    DragShape* find_shape(const QPoint& pt) const;

private:
    std::vector<DragShape*>    m_dragshape_list;
    DragMode                   m_drag_mode;
    DragShape*                 m_dragging_shape{ nullptr };
    DragShape*                 m_slot_shape{ nullptr }; // The shape that's being highlighted
    QDrag*               m_drag_image{ nullptr };
    QPoint                    m_drag_start_pos;
    QPixmap                   m_arrow_bmp;
    QPoint                    m_max_shape_pos;
};


}}
#endif
