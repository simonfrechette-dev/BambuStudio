// Qt6 stub for Plater.cpp - placeholder implementation
#include "Plater.hpp"
#include "MainFrame.hpp"
#include "BBLTopbar.hpp"
#include "GUI_App.hpp"
#include "libslic3r/PrintConfig.hpp"

#include <algorithm>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QLabel>
#include <QFrame>
#include <QSizePolicy>
#include <QTreeWidget>
#include <QToolBar>
#include <QAction>
#include <QPushButton>
#include <QFileDialog>
#include <QFileInfo>
#include <QMainWindow>
#include <QMessageBox>
#include <QMenu>
#include <QInputDialog>
#include <QShortcut>
#include <QUndoStack>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QUndoCommand>
#include <QLineEdit>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QGridLayout>
#include <QComboBox>
#include "SimpleGLView.hpp"
#include "libslic3r/Model.hpp"
#include "libslic3r/Format/STL.hpp"
#include "libslic3r/Format/3mf.hpp"
#include "libslic3r/Format/OBJ.hpp"
#include "libslic3r/Print.hpp"
#include "libslic3r/Layer.hpp"
#include "libslic3r/PresetBundle.hpp"
#include <boost/log/trivial.hpp>
#include <QThread>

namespace Slic3r {
namespace GUI {

// ── Mesh helper ──────────────────────────────────────────────────────────────

// Extract flat triangle vertex list (3 vertices per face, world-space)
// from a ModelObject applying its first instance's transform.
static std::vector<QVector3D> extract_mesh_verts(const Slic3r::ModelObject& obj)
{
    std::vector<QVector3D> result;
    const Slic3r::ModelInstance* inst =
        obj.instances.empty() ? nullptr : obj.instances.front();
    for (const Slic3r::ModelVolume* vol : obj.volumes) {
        if (!vol->is_model_part()) continue;
        const auto& its = vol->mesh().its;
        if (its.vertices.empty() || its.indices.empty()) continue;
        result.reserve(result.size() + its.indices.size() * 3);
        for (const auto& face : its.indices) {
            for (int k = 0; k < 3; ++k) {
                const Slic3r::Vec3f& v = its.vertices[face(k)];
                if (inst) {
                    const Slic3r::Vec3d vt = inst->get_matrix() * v.cast<double>();
                    result.emplace_back(float(vt.x()), float(vt.y()), float(vt.z()));
                } else {
                    result.emplace_back(v.x(), v.y(), v.z());
                }
            }
        }
    }
    return result;
}

// ── Undo commands ────────────────────────────────────────────────────────────

// Helper: rebuild the GL canvas from the current tree contents.
// Tree items may carry a QVariantList of 6 floats as Qt::UserRole
// (raw mesh bbox: min_x, min_y, min_z, max_x, max_y, max_z).
static void sync_canvas_from_tree(SimpleGLView* canvas, QTreeWidget* tree)
{
    if (!canvas) return;
    canvas->clear_objects();
    if (!tree) return;

    float x_offset = 20.f;
    const float spacing = 10.f;

    for (int i = 0; i < tree->topLevelItemCount(); ++i) {
        QTreeWidgetItem* it = tree->topLevelItem(i);
        if (!(it->flags() & Qt::ItemIsSelectable)) continue;  // skip placeholder

        // Read stored mesh dimensions (width x depth x height)
        float w = 40.f, d = 40.f, h = 50.f;
        const QVariant stored = it->data(0, Qt::UserRole);
        if (!stored.isNull()) {
            const QVariantList bl = stored.toList();
            if (bl.size() == 6) {
                w = qMax(1.f, bl[3].toFloat() - bl[0].toFloat());
                d = qMax(1.f, bl[4].toFloat() - bl[1].toFloat());
                h = qMax(1.f, bl[5].toFloat() - bl[2].toFloat());
            }
        }

        canvas->add_object(it->text(0),
            QVector3D(x_offset,       20.f,     0.f),
            QVector3D(x_offset + w,   20.f + d, h));
        x_offset += w + spacing;
    }
}

// Command for adding an object (item) to the tree.
// bbox_data: optional QVariantList of 6 floats [min_x,min_y,min_z,max_x,max_y,max_z]
class AddObjectCommand : public QUndoCommand
{
public:
    AddObjectCommand(QTreeWidget* tree, SimpleGLView* canvas, const QString& name,
                     const QVariant& bbox_data = QVariant(),
                     QUndoCommand* parent = nullptr)
        : QUndoCommand(QObject::tr("Add %1").arg(name), parent)
        , m_tree(tree), m_canvas(canvas), m_name(name), m_bbox_data(bbox_data) {}

    void redo() override {
        // Remove placeholder "(no objects loaded)" if present
        for (int i = m_tree->topLevelItemCount() - 1; i >= 0; --i) {
            QTreeWidgetItem* it = m_tree->topLevelItem(i);
            if (it->flags() == Qt::ItemIsEnabled)
                delete m_tree->takeTopLevelItem(i);
        }
        m_item = new QTreeWidgetItem(m_tree);
        m_item->setText(0, m_name);
        m_item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        if (!m_bbox_data.isNull())
            m_item->setData(0, Qt::UserRole, m_bbox_data);
        m_tree->clearSelection();
        m_tree->setCurrentItem(m_item);
        sync_canvas_from_tree(m_canvas, m_tree);
    }
    void undo() override {
        if (m_item) {
            delete m_item;
            m_item = nullptr;
        }
        // Re-add placeholder if list is now empty
        if (m_tree->topLevelItemCount() == 0) {
            auto* ph = new QTreeWidgetItem(m_tree);
            ph->setText(0, QObject::tr("(no objects loaded)"));
            ph->setFlags(Qt::ItemIsEnabled);
            m_tree->addTopLevelItem(ph);
        }
        sync_canvas_from_tree(m_canvas, m_tree);
    }

private:
    QTreeWidget*     m_tree;
    SimpleGLView*    m_canvas;
    QString          m_name;
    QVariant         m_bbox_data;
    QTreeWidgetItem* m_item{ nullptr };
};

// Command for removing selected objects from the tree
class RemoveObjectsCommand : public QUndoCommand
{
public:
    struct Entry { QString name; };

    RemoveObjectsCommand(QTreeWidget* tree, SimpleGLView* canvas, QList<QTreeWidgetItem*> items, QUndoCommand* parent = nullptr)
        : QUndoCommand(QObject::tr("Delete object(s)"), parent)
        , m_tree(tree), m_canvas(canvas)
    {
        for (auto* item : items)
            m_entries.append({ item->text(0) });
    }

    void redo() override {
        // Delete all selectable items matching our saved names
        for (auto& e : m_entries) {
            const auto matches = m_tree->findItems(e.name, Qt::MatchExactly, 0);
            for (auto* item : matches)
                if (item->flags() & Qt::ItemIsSelectable)
                    delete item;
        }
        // Re-add placeholder if list is now empty
        if (m_tree->topLevelItemCount() == 0) {
            auto* ph = new QTreeWidgetItem(m_tree);
            ph->setText(0, QObject::tr("(no objects loaded)"));
            ph->setFlags(Qt::ItemIsEnabled);
            m_tree->addTopLevelItem(ph);
        }
        sync_canvas_from_tree(m_canvas, m_tree);
    }
    void undo() override {
        // Remove placeholder if present
        for (int i = m_tree->topLevelItemCount() - 1; i >= 0; --i) {
            QTreeWidgetItem* it = m_tree->topLevelItem(i);
            if (it->flags() == Qt::ItemIsEnabled)
                delete m_tree->takeTopLevelItem(i);
        }
        for (auto& e : m_entries) {
            auto* item = new QTreeWidgetItem(m_tree);
            item->setText(0, e.name);
            item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        }
        sync_canvas_from_tree(m_canvas, m_tree);
    }

private:
    QTreeWidget*   m_tree;
    SimpleGLView*  m_canvas;
    QList<Entry>   m_entries;
};

// Command for moving (repositioning) an object on the canvas.
class MoveObjectCommand : public QUndoCommand
{
public:
    MoveObjectCommand(SimpleGLView* canvas, QTreeWidget* tree,
                      int canvas_idx,
                      const QVector3D& old_min, const QVector3D& old_max,
                      const QVector3D& new_min, const QVector3D& new_max,
                      QUndoCommand* parent = nullptr)
        : QUndoCommand(QObject::tr("Move object"), parent)
        , m_canvas(canvas), m_tree(tree), m_idx(canvas_idx)
        , m_old_min(old_min), m_old_max(old_max)
        , m_new_min(new_min), m_new_max(new_max)
    {}

    void redo() override {
        m_canvas->move_object(m_idx, m_new_min, m_new_max);
        update_tree_bbox(m_new_min, m_new_max);
    }
    void undo() override {
        m_canvas->move_object(m_idx, m_old_min, m_old_max);
        update_tree_bbox(m_old_min, m_old_max);
    }

    int id() const override { return 1001; }
    bool mergeWith(const QUndoCommand* other) override {
        // Merge consecutive moves on the same object into one command
        const auto* o = static_cast<const MoveObjectCommand*>(other);
        if (o->m_idx != m_idx) return false;
        m_new_min = o->m_new_min;
        m_new_max = o->m_new_max;
        return true;
    }

private:
    void update_tree_bbox(const QVector3D& bmin, const QVector3D& bmax) {
        // Update the bbox stored in the tree item at index m_idx
        int sel_idx = 0;
        for (int i = 0; i < m_tree->topLevelItemCount(); ++i) {
            QTreeWidgetItem* it = m_tree->topLevelItem(i);
            if (!(it->flags() & Qt::ItemIsSelectable)) continue;
            if (sel_idx == m_idx) {
                QVariantList bl;
                bl << bmin.x() << bmin.y() << bmin.z()
                   << bmax.x() << bmax.y() << bmax.z();
                it->setData(0, Qt::UserRole, QVariant(bl));
                break;
            }
            ++sel_idx;
        }
    }

    SimpleGLView* m_canvas;
    QTreeWidget*  m_tree;
    int           m_idx;
    QVector3D     m_old_min, m_old_max;
    QVector3D     m_new_min, m_new_max;
};

// All Plater method stubs are intentionally empty.
// This is a port stub - full implementation pending.

// Minimal priv definition to allow unique_ptr destruction
struct Plater::priv {};

// Destructor (out-of-line so priv is complete)
Plater::~Plater() = default;

// Model accessors
Model&       Plater::model()       { return m_model; }
const Model& Plater::model() const { return m_model; }

// Constructor
Plater::Plater(QWidget* parent, MainFrame* /*main_frame*/)
    : QWidget(parent)
{
    // ── Outer vertical layout: [splitter] then [status bar] ─────────────────
    auto* outer_lay = new QVBoxLayout(this);
    outer_lay->setContentsMargins(0, 0, 0, 0);
    outer_lay->setSpacing(0);

    // ── Horizontal split: object list | 3D canvas ─────────────────────────
    auto* root = new QHBoxLayout();
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // ── Left panel: object list with toolbar ────────────────────────────────
    auto* left_panel = new QFrame(this);
    left_panel->setFrameShape(QFrame::StyledPanel);
    left_panel->setFixedWidth(230);
    auto* left_lay = new QVBoxLayout(left_panel);
    left_lay->setContentsMargins(2, 2, 2, 2);
    left_lay->setSpacing(2);

    // ── Preset quick-select panel ──────────────────────────────────────────
    // Matches the top of the original wx Sidebar: printer / process / filament.
    {
        auto* preset_box = new QGroupBox(tr("Presets"), left_panel);
        preset_box->setFlat(true);
        auto* pg = new QGridLayout(preset_box);
        pg->setContentsMargins(2, 2, 2, 4);
        pg->setSpacing(3);
        pg->setColumnStretch(1, 1);

        // Printer row
        pg->addWidget(new QLabel(tr("Printer:"), preset_box), 0, 0, Qt::AlignRight);
        m_combo_printer = new QComboBox(preset_box);
        m_combo_printer->setToolTip(tr("Select printer preset"));
        m_combo_printer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        pg->addWidget(m_combo_printer, 0, 1);

        // Process row
        pg->addWidget(new QLabel(tr("Process:"), preset_box), 1, 0, Qt::AlignRight);
        m_combo_process = new QComboBox(preset_box);
        m_combo_process->setToolTip(tr("Select print settings preset"));
        m_combo_process->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        pg->addWidget(m_combo_process, 1, 1);

        // Filament row (color swatch + combo)
        pg->addWidget(new QLabel(tr("Filament:"), preset_box), 2, 0, Qt::AlignRight);
        auto* fila_row = new QWidget(preset_box);
        auto* fila_hlay = new QHBoxLayout(fila_row);
        fila_hlay->setContentsMargins(0, 0, 0, 0);
        fila_hlay->setSpacing(3);
        // Color swatch (14×14 px colored square)
        m_lbl_filament_color = new QLabel(fila_row);
        m_lbl_filament_color->setFixedSize(14, 14);
        m_lbl_filament_color->setStyleSheet(
            QStringLiteral("background:#888888; border:1px solid #555;"));
        m_combo_filament = new QComboBox(fila_row);
        m_combo_filament->setToolTip(tr("Select filament preset"));
        m_combo_filament->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        fila_hlay->addWidget(m_lbl_filament_color);
        fila_hlay->addWidget(m_combo_filament, 1);
        pg->addWidget(fila_row, 2, 1);

        left_lay->addWidget(preset_box);

        // Populate now (preset bundle should already be loaded by MainFrame)
        populate_preset_combos();

        // Wire combo changes → select the corresponding preset
        connect(m_combo_printer, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, [this](int /*idx*/) {
            auto* pb = wxGetApp().preset_bundle;
            if (!pb || !m_combo_printer) return;
            const QString name = m_combo_printer->currentText();
            pb->printers.select_preset_by_name(name.toStdString(), false);
        });
        connect(m_combo_process, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, [this](int /*idx*/) {
            auto* pb = wxGetApp().preset_bundle;
            if (!pb || !m_combo_process) return;
            const QString name = m_combo_process->currentText();
            pb->prints.select_preset_by_name(name.toStdString(), false);
        });
        connect(m_combo_filament, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, [this](int /*idx*/) {
            auto* pb = wxGetApp().preset_bundle;
            if (!pb || !m_combo_filament) return;
            const QString name = m_combo_filament->currentText();
            pb->filaments.select_preset_by_name(name.toStdString(), false);
            // Update swatch color from filament_colour config key
            const Preset& sel = pb->filaments.get_selected_preset();
            if (const ConfigOption* opt = sel.config.option("filament_colour")) {
                std::string hex = opt->serialize();
                // filament_colour is stored as vector; take first element
                if (!hex.empty() && hex[0] == '#') {
                    // Direct hex color
                } else if (hex.size() > 1 && hex.front() == '"') {
                    hex = hex.substr(1, hex.size() - 2);
                }
                // Strip leading hash if present in serialized form
                const QColor col(QString::fromStdString(hex));
                if (m_lbl_filament_color && col.isValid())
                    m_lbl_filament_color->setStyleSheet(
                        QStringLiteral("background:%1; border:1px solid #555;")
                            .arg(col.name()));
            }
        });
    }

    // Objects header
    auto* obj_title = new QLabel(tr("Objects"), left_panel);
    QFont bold_f = obj_title->font();
    bold_f.setBold(true);
    obj_title->setFont(bold_f);
    left_lay->addWidget(obj_title);

    // List toolbar: Add / Delete / Arrange
    auto* list_toolbar = new QToolBar(left_panel);
    list_toolbar->setIconSize(QSize(14, 14));
    list_toolbar->setToolButtonStyle(Qt::ToolButtonTextOnly);
    auto* act_add = list_toolbar->addAction(tr("+ Add"));
    act_add->setToolTip(tr("Add file to plate"));
    connect(act_add, &QAction::triggered, this, [this]() { load_project(); });
    auto* act_del = list_toolbar->addAction(tr("Delete"));
    act_del->setToolTip(tr("Remove selected object"));
    connect(act_del, &QAction::triggered, this, [this]() {
        const auto sel = m_object_list->selectedItems();
        if (sel.isEmpty()) return;
        auto* undo = static_cast<QUndoStack*>(m_undo_stack);
        undo->push(new RemoveObjectsCommand(m_object_list, m_gl_canvas, sel));
        update_status_label();
    });
    list_toolbar->addSeparator();
    auto* act_arrange = list_toolbar->addAction(tr("Arrange"));
    act_arrange->setToolTip(tr("Auto-arrange objects on the plate"));
    connect(act_arrange, &QAction::triggered, this, [this]() {
        arrange_objects();
    });
    auto* act_center = list_toolbar->addAction(tr("Center"));
    act_center->setToolTip(tr("Center selected objects on the plate (or zoom to fit all)"));
    connect(act_center, &QAction::triggered, this, [this]() {
        if (!m_gl_canvas) return;
        // If there's a selected object, center it on the plate
        const int sel = m_gl_canvas->selected();
        if (sel >= 0 && sel < m_gl_canvas->object_count()) {
            const ObjectBBox& bbox = m_gl_canvas->get_object_bbox(sel);
            const QVector3D ext  = bbox.max_pt - bbox.min_pt;
            const float plate_cx = m_gl_canvas->m_plate_w * 0.5f;
            const float plate_cy = m_gl_canvas->m_plate_h * 0.5f;
            const QVector3D new_min(plate_cx - ext.x() * 0.5f,
                                    plate_cy - ext.y() * 0.5f,
                                    bbox.min_pt.z());
            const QVector3D new_max = new_min + ext;
            m_gl_canvas->move_object(sel, new_min, new_max);
        } else {
            m_gl_canvas->zoom_to_fit();
        }
    });
    list_toolbar->addSeparator();
    // Undo/Redo buttons
    auto* act_undo = list_toolbar->addAction(tr("↩"));
    act_undo->setToolTip(tr("Undo (Ctrl+Z)"));
    act_undo->setShortcut(QKeySequence::Undo);
    connect(act_undo, &QAction::triggered, this, [this]() {
        if (m_undo_stack) static_cast<QUndoStack*>(m_undo_stack)->undo();
    });
    auto* act_redo = list_toolbar->addAction(tr("↪"));
    act_redo->setToolTip(tr("Redo (Ctrl+Y)"));
    act_redo->setShortcut(QKeySequence::Redo);
    connect(act_redo, &QAction::triggered, this, [this]() {
        if (m_undo_stack) static_cast<QUndoStack*>(m_undo_stack)->redo();
    });
    left_lay->addWidget(list_toolbar);

    // Filter / search box for object list
    auto* filter_edit = new QLineEdit(left_panel);
    filter_edit->setPlaceholderText(tr("Filter objects…"));
    filter_edit->setClearButtonEnabled(true);
    filter_edit->setFixedHeight(22);
    connect(filter_edit, &QLineEdit::textChanged, this, [this](const QString& text) {
        const QString lower = text.toLower();
        for (int i = 0; i < m_object_list->topLevelItemCount(); ++i) {
            QTreeWidgetItem* it = m_object_list->topLevelItem(i);
            if (!(it->flags() & Qt::ItemIsSelectable)) {
                // Always show placeholder
                it->setHidden(false);
                continue;
            }
            it->setHidden(!text.isEmpty() && !it->text(0).toLower().contains(lower));
        }
    });
    left_lay->addWidget(filter_edit);

    // View presets toolbar (iso / top / front)
    auto* view_toolbar = new QToolBar(left_panel);
    view_toolbar->setIconSize(QSize(14, 14));
    view_toolbar->setToolButtonStyle(Qt::ToolButtonTextOnly);
    auto* act_iso   = view_toolbar->addAction(tr("Iso"));
    auto* act_top   = view_toolbar->addAction(tr("Top"));
    auto* act_front = view_toolbar->addAction(tr("Front"));
    auto* act_left  = view_toolbar->addAction(tr("Left"));
    act_iso->setToolTip(tr("Isometric view"));
    act_top->setToolTip(tr("Top-down view"));
    act_front->setToolTip(tr("Front view"));
    act_left->setToolTip(tr("Left side view"));
    connect(act_iso,   &QAction::triggered, this, [this]() { if (m_gl_canvas) m_gl_canvas->set_view("iso"); });
    connect(act_top,   &QAction::triggered, this, [this]() { if (m_gl_canvas) m_gl_canvas->set_view("top"); });
    connect(act_front, &QAction::triggered, this, [this]() { if (m_gl_canvas) m_gl_canvas->set_view("front"); });
    connect(act_left,  &QAction::triggered, this, [this]() { if (m_gl_canvas) m_gl_canvas->set_view("left"); });
    left_lay->addWidget(view_toolbar);

    // Object tree widget (placeholder — real ObjectDataViewModel pending)
    m_object_list = new QTreeWidget(left_panel);
    m_object_list->setHeaderHidden(true);
    m_object_list->setColumnCount(1);
    m_object_list->setRootIsDecorated(true);
    m_object_list->setContextMenuPolicy(Qt::CustomContextMenu);
    auto* ph_item = new QTreeWidgetItem(m_object_list);
    ph_item->setText(0, tr("(no objects loaded)"));
    ph_item->setFlags(Qt::ItemIsEnabled);
    m_object_list->addTopLevelItem(ph_item);
    left_lay->addWidget(m_object_list, 1);

    // Right-click context menu for object list
    connect(m_object_list, &QTreeWidget::customContextMenuRequested, this, [this](const QPoint& pos) {
        QMenu ctx_menu(m_object_list);
        ctx_menu.addAction(tr("Add File…"), this, [this]() { load_project(); });
        ctx_menu.addSeparator();

        const auto sel = m_object_list->selectedItems();
        const bool has_selection = !sel.isEmpty();

        // Delete
        auto* act_del = ctx_menu.addAction(tr("Delete"), this, [this, sel]() {
            if (sel.isEmpty()) return;
            auto* undo = static_cast<QUndoStack*>(m_undo_stack);
            undo->push(new RemoveObjectsCommand(m_object_list, m_gl_canvas, sel));
            update_status_label();
        });
        act_del->setEnabled(has_selection);

        // Duplicate
        auto* act_dup = ctx_menu.addAction(tr("Duplicate"), this, [this, sel]() {
            if (sel.isEmpty()) return;
            QTreeWidgetItem* src = sel.first();
            if (!(src->flags() & Qt::ItemIsSelectable)) return;
            // Find source index
            int src_idx = 0;
            for (int i = 0; i < m_object_list->topLevelItemCount(); ++i) {
                QTreeWidgetItem* t = m_object_list->topLevelItem(i);
                if (!(t->flags() & Qt::ItemIsSelectable)) continue;
                if (t == src) break;
                ++src_idx;
            }
            // Copy mesh in m_model if index is valid
            QVariant bbox_data = src->data(0, Qt::UserRole);
            std::vector<QVector3D> dup_mesh_verts;
            if (src_idx < int(m_model.objects.size())) {
                Slic3r::ModelObject* orig = m_model.objects[src_idx];
                Slic3r::ModelObject* dup  = m_model.add_object(*orig);
                // Offset the duplicate slightly so it doesn't overlap
                dup->translate(20.0, 0.0, 0.0);
                const auto& bb = dup->raw_mesh_bounding_box();
                QVariantList bl;
                bl << float(bb.min(0)) << float(bb.min(1)) << float(bb.min(2))
                   << float(bb.max(0)) << float(bb.max(1)) << float(bb.max(2));
                bbox_data = QVariant(bl);
                // Extract mesh verts for the duplicate (with offset applied)
                dup_mesh_verts = extract_mesh_verts(*dup);
            }
            const QString new_name = src->text(0) + tr(" (copy)");
            auto* undo = static_cast<QUndoStack*>(m_undo_stack);
            undo->push(new AddObjectCommand(m_object_list, m_gl_canvas, new_name, bbox_data));
            // Set mesh data on the newly added canvas object
            if (m_gl_canvas && !dup_mesh_verts.empty()) {
                const int new_idx = m_gl_canvas->object_count() - 1;
                if (new_idx >= 0)
                    m_gl_canvas->set_object_mesh(new_idx, std::move(dup_mesh_verts));
            }
            update_status_label();
        });
        act_dup->setEnabled(has_selection);

        // Scale
        auto* act_scale = ctx_menu.addAction(tr("Scale…"), this, [this, sel]() {
            if (sel.isEmpty() || !m_gl_canvas) return;
            QTreeWidgetItem* src = sel.first();
            if (!(src->flags() & Qt::ItemIsSelectable)) return;
            // Find canvas index
            int cidx = 0;
            for (int i = 0; i < m_object_list->topLevelItemCount(); ++i) {
                QTreeWidgetItem* t = m_object_list->topLevelItem(i);
                if (!(t->flags() & Qt::ItemIsSelectable)) continue;
                if (t == src) break;
                ++cidx;
            }
            if (cidx >= m_gl_canvas->object_count()) return;
            bool ok = false;
            const double factor = QInputDialog::getDouble(
                this, tr("Scale Object"),
                tr("Scale factor (%):"),
                100.0, 1.0, 10000.0, 1, &ok);
            if (!ok) return;
            const double f = factor / 100.0;
            const ObjectBBox& bbox = m_gl_canvas->get_object_bbox(cidx);
            const QVector3D ctr = (bbox.min_pt + bbox.max_pt) * 0.5f;
            const QVector3D ext = bbox.max_pt - bbox.min_pt;
            const QVector3D new_ext = ext * float(f);
            const QVector3D new_min(ctr.x() - new_ext.x() * 0.5f,
                                    ctr.y() - new_ext.y() * 0.5f,
                                    bbox.min_pt.z());
            const QVector3D new_max = new_min + new_ext;
            m_gl_canvas->scale_object(cidx, new_min, new_max);
            // Update bbox_data stored in tree item
            QVariantList bl;
            bl << new_min.x() << new_min.y() << new_min.z()
               << new_max.x() << new_max.y() << new_max.z();
            src->setData(0, Qt::UserRole, QVariant(bl));
            update_transform_panel(cidx);
        });
        act_scale->setEnabled(has_selection);

        // Lay Flat (set Z to 0 — move object so min_z = 0)
        auto* act_lay = ctx_menu.addAction(tr("Lay Flat"), this, [this, sel]() {
            if (sel.isEmpty() || !m_gl_canvas) return;
            QTreeWidgetItem* src = sel.first();
            if (!(src->flags() & Qt::ItemIsSelectable)) return;
            int cidx = 0;
            for (int i = 0; i < m_object_list->topLevelItemCount(); ++i) {
                QTreeWidgetItem* t = m_object_list->topLevelItem(i);
                if (!(t->flags() & Qt::ItemIsSelectable)) continue;
                if (t == src) break;
                ++cidx;
            }
            if (cidx >= m_gl_canvas->object_count()) return;
            const ObjectBBox& bbox = m_gl_canvas->get_object_bbox(cidx);
            const float dz = -bbox.min_pt.z();
            m_gl_canvas->move_object(cidx,
                bbox.min_pt + QVector3D(0, 0, dz),
                bbox.max_pt + QVector3D(0, 0, dz));
            update_transform_panel(cidx);
        });
        act_lay->setEnabled(has_selection);

        // Rename
        auto* act_rename = ctx_menu.addAction(tr("Rename…"), this, [this, sel]() {
            if (sel.isEmpty()) return;
            QTreeWidgetItem* it = sel.first();
            if (!(it->flags() & Qt::ItemIsSelectable)) return;
            bool ok = false;
            const QString new_name = QInputDialog::getText(
                this, tr("Rename Object"),
                tr("New name:"), QLineEdit::Normal,
                it->text(0), &ok);
            if (ok && !new_name.trimmed().isEmpty()) {
                it->setText(0, new_name.trimmed());
                // Update the corresponding GL canvas object name
                int idx = 0;
                for (int i = 0; i < m_object_list->topLevelItemCount(); ++i) {
                    QTreeWidgetItem* t = m_object_list->topLevelItem(i);
                    if (!(t->flags() & Qt::ItemIsSelectable)) continue;
                    if (t == it) {
                        if (m_gl_canvas && idx < m_gl_canvas->object_count())
                            m_gl_canvas->rename_object(idx, new_name.trimmed());
                        break;
                    }
                    ++idx;
                }
            }
        });
        act_rename->setEnabled(has_selection);

        ctx_menu.addSeparator();

        // Zoom to selected in GL canvas
        auto* act_zoom = ctx_menu.addAction(tr("Zoom to Object"), this, [this, sel]() {
            if (sel.isEmpty() || !m_gl_canvas) return;
            QTreeWidgetItem* it = sel.first();
            int idx = 0;
            for (int i = 0; i < m_object_list->topLevelItemCount(); ++i) {
                QTreeWidgetItem* t = m_object_list->topLevelItem(i);
                if (!(t->flags() & Qt::ItemIsSelectable)) continue;
                if (t == it) { m_gl_canvas->zoom_to_object(idx); break; }
                ++idx;
            }
        });
        act_zoom->setEnabled(has_selection);

        ctx_menu.exec(m_object_list->mapToGlobal(pos));
    });

    // Delete key removes selected objects
    auto* del_shortcut = new QShortcut(QKeySequence::Delete, m_object_list);
    connect(del_shortcut, &QShortcut::activated, this, [this]() {
        const auto sel = m_object_list->selectedItems();
        if (sel.isEmpty()) return;
        auto* undo = static_cast<QUndoStack*>(m_undo_stack);
        undo->push(new RemoveObjectsCommand(m_object_list, m_gl_canvas, sel));
        update_status_label();
    });

    // ── Centre 3D canvas ──────────────────────────────────────────────────────
    m_gl_canvas = new SimpleGLView(this);
    m_gl_canvas->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_gl_canvas->setAcceptDrops(false);  // Plater itself handles drops

    // Sync selection from object list to GL canvas highlight
    connect(m_object_list, &QTreeWidget::itemSelectionChanged, this, [this]() {
        if (!m_gl_canvas) return;
        const auto sel = m_object_list->selectedItems();
        if (sel.isEmpty()) {
            m_gl_canvas->set_selected(-1);
            update_transform_panel(-1);
            return;
        }
        // Find the index of the selected item among selectable items
        QTreeWidgetItem* target = sel.first();
        int idx = 0;
        for (int i = 0; i < m_object_list->topLevelItemCount(); ++i) {
            QTreeWidgetItem* it = m_object_list->topLevelItem(i);
            if (!(it->flags() & Qt::ItemIsSelectable)) continue;
            if (it == target) {
                m_gl_canvas->set_selected(idx);
                update_transform_panel(idx);
                return;
            }
            ++idx;
        }
        m_gl_canvas->set_selected(-1);
        update_transform_panel(-1);
    });

    // Double-click object → zoom GL canvas to that object
    connect(m_object_list, &QTreeWidget::itemDoubleClicked, this, [this](QTreeWidgetItem* item, int) {
        if (!m_gl_canvas || !item) return;
        if (!(item->flags() & Qt::ItemIsSelectable)) return;
        int idx = 0;
        for (int i = 0; i < m_object_list->topLevelItemCount(); ++i) {
            QTreeWidgetItem* it = m_object_list->topLevelItem(i);
            if (!(it->flags() & Qt::ItemIsSelectable)) continue;
            if (it == item) { m_gl_canvas->zoom_to_object(idx); return; }
            ++idx;
        }
    });

    // GL canvas right-click context menu: Delete selected object
    connect(m_gl_canvas, &SimpleGLView::deleteObjectRequested, this, [this](int canvas_idx) {
        // Find the nth selectable item matching canvas_idx
        int idx = 0;
        for (int i = 0; i < m_object_list->topLevelItemCount(); ++i) {
            QTreeWidgetItem* it = m_object_list->topLevelItem(i);
            if (!(it->flags() & Qt::ItemIsSelectable)) continue;
            if (idx == canvas_idx) {
                auto* undo = static_cast<QUndoStack*>(m_undo_stack);
                undo->push(new RemoveObjectsCommand(m_object_list, m_gl_canvas, {it}));
                update_status_label();
                return;
            }
            ++idx;
        }
    });

    // When user clicks an object in the GL canvas, sync the tree selection
    connect(m_gl_canvas, &SimpleGLView::canvasSelectionChanged, this, [this](int canvas_idx) {
        if (!m_object_list) return;
        // Block signals to avoid feedback loop with itemSelectionChanged
        QSignalBlocker blocker(m_object_list);
        m_object_list->clearSelection();
        update_transform_panel(canvas_idx);
        if (canvas_idx < 0) return;
        int idx = 0;
        for (int i = 0; i < m_object_list->topLevelItemCount(); ++i) {
            QTreeWidgetItem* it = m_object_list->topLevelItem(i);
            if (!(it->flags() & Qt::ItemIsSelectable)) continue;
            if (idx == canvas_idx) {
                it->setSelected(true);
                m_object_list->scrollToItem(it);
                return;
            }
            ++idx;
        }
    });

    // ── Object position / size panel ─────────────────────────────────────────
    {
        auto* xform_box = new QGroupBox(tr("Position"), left_panel);
        xform_box->setFlat(true);
        auto* xgrid = new QGridLayout(xform_box);
        xgrid->setContentsMargins(4, 2, 4, 4);
        xgrid->setSpacing(3);

        xgrid->addWidget(new QLabel("X:", xform_box), 0, 0, Qt::AlignRight);
        m_spin_pos_x = new QDoubleSpinBox(xform_box);
        m_spin_pos_x->setRange(-1000.0, 1000.0);
        m_spin_pos_x->setDecimals(1);
        m_spin_pos_x->setSuffix(" mm");
        m_spin_pos_x->setFixedWidth(86);
        xgrid->addWidget(m_spin_pos_x, 0, 1);

        xgrid->addWidget(new QLabel("Y:", xform_box), 0, 2, Qt::AlignRight);
        m_spin_pos_y = new QDoubleSpinBox(xform_box);
        m_spin_pos_y->setRange(-1000.0, 1000.0);
        m_spin_pos_y->setDecimals(1);
        m_spin_pos_y->setSuffix(" mm");
        m_spin_pos_y->setFixedWidth(86);
        xgrid->addWidget(m_spin_pos_y, 0, 3);

        m_lbl_size_whd = new QLabel(xform_box);
        m_lbl_size_whd->setStyleSheet("color:#888;font-size:10px;");
        xgrid->addWidget(m_lbl_size_whd, 1, 0, 1, 4, Qt::AlignLeft);

        xform_box->setEnabled(false);
        m_transform_panel = xform_box;
        left_lay->addWidget(xform_box);

        // When position spinboxes change, move the object in the canvas (live preview)
        auto move_fn = [this]() {
            if (!m_gl_canvas) return;
            const int idx = m_gl_canvas->selected();
            if (idx < 0 || idx >= m_gl_canvas->object_count()) return;
            const ObjectBBox& bbox = m_gl_canvas->get_object_bbox(idx);
            const QVector3D ext = bbox.max_pt - bbox.min_pt;
            const float nx = float(m_spin_pos_x->value());
            const float ny = float(m_spin_pos_y->value());
            m_gl_canvas->move_object(idx,
                QVector3D(nx, ny, bbox.min_pt.z()),
                QVector3D(nx + ext.x(), ny + ext.y(), bbox.max_pt.z()));
        };
        connect(m_spin_pos_x, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, move_fn);
        connect(m_spin_pos_y, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, move_fn);

        // On editing finished, push a MoveObjectCommand to the undo stack
        auto commit_move = [this]() {
            if (!m_gl_canvas || !m_undo_stack) return;
            const int idx = m_gl_canvas->selected();
            if (idx < 0 || idx >= m_gl_canvas->object_count()) return;
            const ObjectBBox& cur = m_gl_canvas->get_object_bbox(idx);
            // Recover old position from tree item's stored bbox
            QVector3D old_min = cur.min_pt, old_max = cur.max_pt;
            int sel_idx = 0;
            for (int i = 0; i < m_object_list->topLevelItemCount(); ++i) {
                QTreeWidgetItem* it = m_object_list->topLevelItem(i);
                if (!(it->flags() & Qt::ItemIsSelectable)) continue;
                if (sel_idx == idx) {
                    const QVariant stored = it->data(0, Qt::UserRole);
                    if (!stored.isNull()) {
                        const QVariantList bl = stored.toList();
                        if (bl.size() == 6) {
                            old_min = QVector3D(bl[0].toFloat(), bl[1].toFloat(), bl[2].toFloat());
                            old_max = QVector3D(bl[3].toFloat(), bl[4].toFloat(), bl[5].toFloat());
                        }
                    }
                    break;
                }
                ++sel_idx;
            }
            // Only push a command if the position actually changed
            if (qFuzzyCompare(old_min.x(), cur.min_pt.x()) &&
                qFuzzyCompare(old_min.y(), cur.min_pt.y())) return;
            auto* undo = static_cast<QUndoStack*>(m_undo_stack);
            undo->push(new MoveObjectCommand(m_gl_canvas, m_object_list, idx,
                old_min, old_max, cur.min_pt, cur.max_pt));
        };
        connect(m_spin_pos_x, &QDoubleSpinBox::editingFinished, this, commit_move);
        connect(m_spin_pos_y, &QDoubleSpinBox::editingFinished, this, commit_move);
    }

    auto* splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(left_panel);
    splitter->addWidget(m_gl_canvas);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);

    root->addWidget(splitter);

    // Status bar at the bottom
    m_status_label = new QLabel(tr("No objects loaded"), this);
    m_status_label->setFixedHeight(22);
    m_status_label->setStyleSheet(QStringLiteral(
        "QLabel { padding: 2px 6px; font-size: 11px; color: #888; "
        "background: #2a2a2a; border-top: 1px solid #444; }"));

    outer_lay->addLayout(root, 1);
    outer_lay->addWidget(m_status_label);

    // Undo stack — no commands yet, but enables Ctrl+Z / Ctrl+Y through the menus
    m_undo_stack = new QUndoStack(this);

    // Accept drag-and-drop of 3D files
    setAcceptDrops(true);

}


// ─── Object transform panel helper ──────────────────────────────────────────

void Plater::update_transform_panel(int canvas_idx)
{
    if (!m_transform_panel || !m_gl_canvas) return;
    if (canvas_idx < 0 || canvas_idx >= m_gl_canvas->object_count()) {
        m_transform_panel->setEnabled(false);
        if (m_lbl_size_whd) m_lbl_size_whd->clear();
        return;
    }
    m_transform_panel->setEnabled(true);
    const ObjectBBox& bbox = m_gl_canvas->get_object_bbox(canvas_idx);
    // Block spinbox signals while updating from canvas
    QSignalBlocker bx(m_spin_pos_x), by(m_spin_pos_y);
    m_spin_pos_x->setValue(double(bbox.min_pt.x()));
    m_spin_pos_y->setValue(double(bbox.min_pt.y()));
    const QVector3D ext = bbox.max_pt - bbox.min_pt;
    if (m_lbl_size_whd)
        m_lbl_size_whd->setText(tr("W:%1 D:%2 H:%3 mm")
            .arg(ext.x(), 0, 'f', 1)
            .arg(ext.y(), 0, 'f', 1)
            .arg(ext.z(), 0, 'f', 1));
}

// ─── Preset combo populate ────────────────────────────────────────────────────

void Plater::populate_preset_combos()
{
    auto* pb = wxGetApp().preset_bundle;
    if (!pb) return;

    auto fill_combo = [](QComboBox* combo, const PresetCollection& coll) {
        if (!combo) return;
        QSignalBlocker blocker(combo);
        combo->clear();
        const std::string sel_name = coll.get_selected_preset().name;
        int sel_idx = 0;
        int idx = 0;
        for (const Preset& p : coll) {
            combo->addItem(QString::fromStdString(p.name));
            if (p.name == sel_name) sel_idx = idx;
            ++idx;
        }
        if (combo->count() > 0) combo->setCurrentIndex(sel_idx);
    };

    fill_combo(m_combo_printer,  pb->printers);
    fill_combo(m_combo_process,  pb->prints);
    fill_combo(m_combo_filament, pb->filaments);

    // Update filament color swatch from the selected filament preset
    const Preset& fila_sel = pb->filaments.get_selected_preset();
    if (const ConfigOption* opt = fila_sel.config.option("filament_colour")) {
        // filament_colour is a ConfigOptionStrings (per-extruder list) or a string
        std::string hex;
        if (opt->type() == coStrings) {
            const auto* strs = static_cast<const ConfigOptionStrings*>(opt);
            if (!strs->values.empty()) hex = strs->values[0];
        } else {
            hex = opt->serialize();
        }
        // Strip quotes if present
        if (hex.size() >= 2 && hex.front() == '"')
            hex = hex.substr(1, hex.size() - 2);
        const QColor col(QString::fromStdString(hex));
        if (m_lbl_filament_color && col.isValid())
            m_lbl_filament_color->setStyleSheet(
                QStringLiteral("background:%1; border:1px solid #555;")
                    .arg(col.name()));
    }
}


// PartPlate list (singleton placeholder)
PartPlateList& Plater::get_partplate_list() {
    static PartPlateList s_list(nullptr, nullptr);
    return s_list;
}

bool Plater::set_printer_technology(PrinterTechnology /*pt*/) { return false; }

Preset* get_printer_preset(const MachineObject*) { return nullptr; }

std::vector<int> get_min_flush_volumes(const DynamicPrintConfig& /*full_config*/, size_t /*nozzle_id*/)
{
    return {};
}

// Project operations
int  Plater::load_project(QString const& filename,
                          QString const& /*originfile*/)
{
    // If no filename given, open a file dialog
    QString path = filename;
    if (path.isEmpty()) {
        path = QFileDialog::getOpenFileName(
            this,
            tr("Open 3D File"),
            QString(),
            tr("3D Files (*.3mf *.stl *.obj *.amf);;3MF Files (*.3mf);;STL Files (*.stl);;All Files (*)")
        );
    }
    if (path.isEmpty()) return 0;

    const bool is_project_file = path.endsWith(QStringLiteral(".3mf"), Qt::CaseInsensitive)
                               || path.endsWith(QStringLiteral(".amf"), Qt::CaseInsensitive);

    if (m_object_list) {
        if (is_project_file) {
            // Project file: replace all objects
            m_object_list->clear();
            if (m_gl_canvas) m_gl_canvas->clear_objects();
            m_model.clear_objects();

            // Load the .3mf using the libslic3r API
            const std::string path_str = path.toStdString();
            Slic3r::DynamicPrintConfig loaded_cfg;
            Slic3r::ConfigSubstitutionContext cfg_subs(Slic3r::ForwardCompatibilitySubstitutionRule::Enable);
            const bool ok = Slic3r::load_3mf(path_str.c_str(), loaded_cfg, cfg_subs, &m_model, /*check_version=*/false);

            if (ok && !m_model.objects.empty()) {
                // Apply loaded config to preset bundle if available
                auto* pb = wxGetApp().preset_bundle;
                if (pb) pb->load_config_model("3mf", std::move(loaded_cfg));

                // Add each object to the tree
                for (auto* obj : m_model.objects) {
                    const auto& bbox = obj->raw_mesh_bounding_box();
                    QVariantList blist;
                    blist << float(bbox.min(0)) << float(bbox.min(1)) << float(bbox.min(2))
                          << float(bbox.max(0)) << float(bbox.max(1)) << float(bbox.max(2));
                    const QString obj_name = obj->name.empty()
                        ? QFileInfo(path).baseName()
                        : QString::fromStdString(obj->name);
                    auto* item = new QTreeWidgetItem(m_object_list);
                    item->setText(0, obj_name);
                    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
                    item->setData(0, Qt::UserRole, QVariant(blist));
                    m_object_list->addTopLevelItem(item);
                }
                sync_canvas_from_tree(m_gl_canvas, m_object_list);
                // Set real mesh data for solid rendering
                if (m_gl_canvas) {
                    for (int ci = 0; ci < int(m_model.objects.size()); ++ci)
                        m_gl_canvas->set_object_mesh(ci, extract_mesh_verts(*m_model.objects[ci]));
                }
            } else {
                // Fallback: show filename as placeholder
                QFileInfo fi(path);
                auto* item = new QTreeWidgetItem(m_object_list);
                item->setText(0, fi.fileName());
                item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
                m_object_list->addTopLevelItem(item);
                if (m_gl_canvas) sync_canvas_from_tree(m_gl_canvas, m_object_list);
                if (!ok)
                    BOOST_LOG_TRIVIAL(warning) << "load_3mf failed for: " << path_str;
            }
        } else {
            // Mesh file (STL/OBJ): load with libslic3r to get real bounding box
            QFileInfo fi(path);
            const std::string path_str = path.toStdString();
            const bool is_stl = path.endsWith(QStringLiteral(".stl"), Qt::CaseInsensitive);
            const bool is_obj = path.endsWith(QStringLiteral(".obj"), Qt::CaseInsensitive);

            // Remove placeholder if present
            for (int i = m_object_list->topLevelItemCount() - 1; i >= 0; --i) {
                QTreeWidgetItem* it = m_object_list->topLevelItem(i);
                if (it->flags() == Qt::ItemIsEnabled)
                    delete m_object_list->takeTopLevelItem(i);
            }

            bool mesh_loaded = false;
            if (is_stl || is_obj) {
                Slic3r::Model tmp_model;
                bool ok = false;
                if (is_stl) {
                    ok = Slic3r::load_stl(path_str.c_str(), &tmp_model,
                                          fi.fileName().toStdString().c_str());
                } else {
                    Slic3r::ObjInfo obj_info;
                    std::string msg;
                    ok = Slic3r::load_obj(path_str.c_str(), &tmp_model, obj_info, msg,
                                          fi.fileName().toStdString().c_str());
                    if (!msg.empty())
                        BOOST_LOG_TRIVIAL(warning) << "load_obj: " << msg;
                }
                if (ok && !tmp_model.objects.empty()) {
                    // Deep-copy objects into the persistent m_model (properly wires parent ptr)
                    for (auto* obj : tmp_model.objects)
                        m_model.add_object(*obj);

                    auto* undo = static_cast<QUndoStack*>(m_undo_stack);
                    // Walk m_model from the end (newly added objects)
                    const int new_start = int(m_model.objects.size()) - int(tmp_model.objects.size());
                    for (int i = new_start; i < int(m_model.objects.size()); ++i) {
                        auto* obj = m_model.objects[i];
                        const auto& bbox = obj->raw_mesh_bounding_box();
                        QVariantList blist;
                        blist << float(bbox.min(0)) << float(bbox.min(1)) << float(bbox.min(2))
                              << float(bbox.max(0)) << float(bbox.max(1)) << float(bbox.max(2));
                        const QString obj_name = obj->name.empty()
                            ? fi.baseName()
                            : QString::fromStdString(obj->name);
                        if (undo) {
                            undo->push(new AddObjectCommand(m_object_list, m_gl_canvas,
                                                            obj_name, QVariant(blist)));
                        } else {
                            auto* item = new QTreeWidgetItem(m_object_list);
                            item->setText(0, obj_name);
                            item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
                            item->setData(0, Qt::UserRole, QVariant(blist));
                            m_object_list->addTopLevelItem(item);
                        }
                    }
                    if (!undo)
                        sync_canvas_from_tree(m_gl_canvas, m_object_list);
                    mesh_loaded = true;
                }
            }
            if (!mesh_loaded) {
                // Fallback: show filename with default placeholder dimensions
                auto* undo = static_cast<QUndoStack*>(m_undo_stack);
                if (undo) {
                    undo->push(new AddObjectCommand(m_object_list, m_gl_canvas, fi.fileName()));
                } else {
                    auto* item = new QTreeWidgetItem(m_object_list);
                    item->setText(0, fi.fileName());
                    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
                    m_object_list->addTopLevelItem(item);
                    sync_canvas_from_tree(m_gl_canvas, m_object_list);
                }
            }
        }

        // Pass real mesh data to the GL canvas for solid rendering
        if (m_gl_canvas && !m_model.objects.empty()) {
            int c_idx = 0;
            for (int i = 0; i < m_object_list->topLevelItemCount(); ++i) {
                QTreeWidgetItem* it = m_object_list->topLevelItem(i);
                if (!(it->flags() & Qt::ItemIsSelectable)) continue;
                if (c_idx < int(m_model.objects.size())) {
                    m_gl_canvas->set_object_mesh(c_idx, extract_mesh_verts(*m_model.objects[c_idx]));
                }
                ++c_idx;
            }
        }

        m_object_list->expandAll();
    }

    // Only update the project path for actual project files
    if (is_project_file)
        m_loaded_path = path;

    // Update the window title via MainFrame and add to recent list
    if (auto* mf = wxGetApp().mainframe) {
        mf->update_title();
        mf->add_to_recent_projects(path);
        if (mf->topbar()) {
            mf->topbar()->EnableSaveItem(true);
            mf->topbar()->EnableUndoRedoItems();
        }
        // Reload auxiliary panel to show any files in <project>_files/ folder
        if (mf->m_auxiliary && !m_loaded_path.isEmpty()) {
            const QString aux_dir = QFileInfo(m_loaded_path).dir().absolutePath()
                + "/" + QFileInfo(m_loaded_path).completeBaseName() + "_files";
            mf->m_auxiliary->Reload(aux_dir);
        }
    } else if (auto* qmf = qobject_cast<QMainWindow*>(window())) {
        qmf->setWindowTitle(QFileInfo(path).fileName() + QStringLiteral(" - BambuStudio"));
    }

    update_status_label();
    return 1;
}

int Plater::save_project(bool saveAs)
{
    // Determine save path
    QString save_path = m_loaded_path;
    if (saveAs || save_path.isEmpty() || !save_path.endsWith(QStringLiteral(".3mf"), Qt::CaseInsensitive)) {
        save_path = QFileDialog::getSaveFileName(
            this,
            tr("Save Project"),
            save_path.isEmpty() ? QStringLiteral("project.3mf") : QFileInfo(save_path).dir().filePath(QFileInfo(save_path).completeBaseName() + QStringLiteral(".3mf")),
            tr("3MF Files (*.3mf);;All Files (*)")
        );
    }
    if (save_path.isEmpty()) return 0;

    m_loaded_path = save_path;

    // Export the model as a real 3MF file
    bool ok = false;
    if (!m_model.objects.empty()) {
        auto* pb = wxGetApp().preset_bundle;
        Slic3r::DynamicPrintConfig cfg;
        if (pb) cfg = pb->full_config();
        ok = Slic3r::store_3mf(save_path.toUtf8().constData(), &m_model, &cfg,
                                /*fullpath_sources=*/false);
    } else {
        // No geometry loaded yet — write empty placeholder
        QJsonObject root_obj;
        root_obj["version"] = QLatin1String("1.0");
        root_obj["objects"] = QJsonArray{};
        QFile f(save_path);
        if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
            f.write(QJsonDocument(root_obj).toJson());
            ok = true;
        }
    }

    if (auto* mf = wxGetApp().mainframe) {
        if (ok) {
            mf->add_to_recent_projects(save_path);
            mf->update_title();
            if (mf->topbar()) mf->topbar()->EnableSaveItem(true);
            mf->set_status_message(tr("Project saved to %1").arg(QFileInfo(save_path).fileName()));
        } else {
            mf->set_status_message(tr("Failed to save project to %1").arg(QFileInfo(save_path).fileName()));
        }
    }
    return ok ? 1 : 0;
}

void Plater::export_stl(bool /*extended*/, bool /*selection_only*/, bool /*multi_stls*/)
{
    if (m_model.objects.empty()) {
        QMessageBox::information(this, tr("Export STL"), tr("No objects to export."));
        return;
    }

    const QString default_name = m_loaded_path.isEmpty()
        ? QStringLiteral("model.stl")
        : QFileInfo(m_loaded_path).dir().filePath(
              QFileInfo(m_loaded_path).completeBaseName() + QStringLiteral(".stl"));

    const QString path = QFileDialog::getSaveFileName(
        this, tr("Export STL"), default_name,
        tr("STL Files (*.stl);;All Files (*)"));
    if (path.isEmpty()) return;

    const bool ok = Slic3r::store_stl(path.toUtf8().constData(), &m_model, /*binary=*/true);
    if (auto* mf = wxGetApp().mainframe) {
        mf->set_status_message(ok
            ? tr("Exported STL to %1").arg(QFileInfo(path).fileName())
            : tr("Failed to export STL to %1").arg(QFileInfo(path).fileName()));
    }
}

void Plater::export_core_3mf()
{
    save_project(/*saveAs=*/true);
}

void Plater::export_gcode(bool /*prefer_removable*/)
{
    if (m_model.objects.empty()) {
        QMessageBox::information(this, tr("Export GCode"),
            tr("No objects to slice and export."));
        return;
    }

    // Determine default output path next to the loaded file
    const QString default_name = m_loaded_path.isEmpty()
        ? QStringLiteral("output.gcode")
        : QFileInfo(m_loaded_path).dir().filePath(
              QFileInfo(m_loaded_path).completeBaseName() + QStringLiteral(".gcode"));

    const QString save_path = QFileDialog::getSaveFileName(
        this, tr("Export GCode"), default_name,
        tr("GCode Files (*.gcode *.g);;All Files (*)"));
    if (save_path.isEmpty()) return;

    auto* mf = wxGetApp().mainframe;
    if (mf) mf->set_status_message(tr("Slicing and exporting GCode…"));

    // Get print config
    auto* pb = wxGetApp().preset_bundle;
    if (!pb) {
        if (mf) mf->set_status_message(tr("Export GCode failed: no preset bundle."));
        return;
    }
    DynamicPrintConfig cfg = pb->full_config();
    Slic3r::Model model_copy = m_model;
    const std::string gcode_path = save_path.toStdString();

    auto* thread = QThread::create([this, model_copy = std::move(model_copy),
                                    cfg = std::move(cfg), gcode_path, save_path]() mutable {
        try {
            Slic3r::Print print;
            print.apply(model_copy, std::move(cfg));
            std::unordered_map<std::string, long long> slice_time;
            print.process(&slice_time);
            // Export GCode to the requested path
            print.export_gcode(gcode_path, nullptr, nullptr);
            QMetaObject::invokeMethod(this, [this, save_path]() {
                if (auto* mf2 = wxGetApp().mainframe)
                    mf2->set_status_message(tr("GCode exported to %1")
                        .arg(QFileInfo(save_path).fileName()));
            }, Qt::QueuedConnection);
        } catch (const std::exception& e) {
            const QString err = QString::fromStdString(e.what());
            QMetaObject::invokeMethod(this, [this, err]() {
                if (auto* mf2 = wxGetApp().mainframe)
                    mf2->set_status_message(tr("Export GCode error: %1").arg(err));
            }, Qt::QueuedConnection);
        }
    });
    QObject::connect(thread, &QThread::finished, thread, &QObject::deleteLater);
    thread->start();
}

void Plater::export_gcode_3mf(bool /*export_all*/)
{
    save_project(/*saveAs=*/true);
}

QString Plater::get_export_gcode_filename(const QString& extension,
                                          bool /*only_filename*/,
                                          bool /*export_all*/) const
{
    if (m_loaded_path.isEmpty()) return QString{};
    const QString base = QFileInfo(m_loaded_path).completeBaseName();
    const QString ext = extension.isEmpty() ? QStringLiteral(".gcode") : extension;
    return base + ext;
}

bool Plater::is_export_gcode_scheduled() const { return false; }

int Plater::export_3mf(const boost::filesystem::path& output_path,
                       SaveStrategy /*strategy*/, int /*export_plate_idx*/,
                       Export3mfProgressFn /*proFn*/)
{
    if (!output_path.empty()) {
        // Headless export
        auto* pb = wxGetApp().preset_bundle;
        Slic3r::DynamicPrintConfig cfg;
        if (pb) cfg = pb->full_config();
        const bool ok = Slic3r::store_3mf(output_path.string().c_str(), &m_model, &cfg, false);
        return ok ? 1 : 0;
    }
    return save_project(/*saveAs=*/true);
}

int Plater::new_project(bool /*skip_confirm*/, bool /*silent*/, const QString& /*project_name*/)
{
    // Clear object list
    if (m_object_list) {
        m_object_list->clear();
        auto* ph_item = new QTreeWidgetItem(m_object_list);
        ph_item->setText(0, tr("(no objects loaded)"));
        ph_item->setFlags(Qt::ItemIsEnabled);
        m_object_list->addTopLevelItem(ph_item);
    }
    // Sync canvas
    if (m_gl_canvas) {
        m_gl_canvas->clear_objects();
        m_gl_canvas->zoom_to_fit();
    }
    m_loaded_path.clear();
    if (auto* mf = wxGetApp().mainframe) {
        mf->update_title();
        if (mf->topbar()) {
            mf->topbar()->EnableSaveItem(false);
            mf->topbar()->DisableUndoRedoItems();
        }
    }
    update_status_label();
    return 1;
}

void Plater::reslice()
{
    // Require at least one real object
    int obj_count = 0;
    if (m_object_list) {
        for (int i = 0; i < m_object_list->topLevelItemCount(); ++i) {
            if (m_object_list->topLevelItem(i)->flags() & Qt::ItemIsSelectable)
                ++obj_count;
        }
    }
    if (obj_count == 0) {
        QMessageBox::information(
            this, tr("No Objects"),
            tr("No objects are loaded on the plate.\n\n"
               "Add a model file first (3MF, STL, OBJ) to slice it."));
        return;
    }

    // Need actual mesh data in m_model to slice
    if (m_model.objects.empty()) {
        if (auto* mf = wxGetApp().mainframe)
            mf->set_status_message(tr("Slicing: no mesh data available — load a STL/OBJ file first."));
        return;
    }

    auto* mf = wxGetApp().mainframe;
    if (mf) mf->set_status_message(tr("Slicing %1 object(s)…").arg(obj_count));

    // Get full print config from preset bundle
    auto* pb = wxGetApp().preset_bundle;
    if (!pb) {
        if (mf) mf->set_status_message(tr("Slicing failed: no preset bundle available."));
        return;
    }
    DynamicPrintConfig cfg = pb->full_config();

    // Run Print::apply() + Print::process() in a background thread.
    // Capture the model by copy and config by value so the thread is self-contained.
    Slic3r::Model model_copy = m_model;

    auto* thread = QThread::create([this, model_copy = std::move(model_copy),
                                    cfg = std::move(cfg), obj_count]() mutable {
        try {
            Slic3r::Print print;
            print.apply(model_copy, std::move(cfg));
            std::unordered_map<std::string, long long> slice_time;
            print.process(&slice_time);

            const auto& stats = print.print_statistics();
            const QString est_normal  = QString::fromStdString(stats.estimated_normal_print_time);
            const QString est_silent  = QString::fromStdString(stats.estimated_silent_print_time);
            const int layers = print.objects().empty() ? 0
                : int(print.objects().front()->layers().size());
            const double filament_mm = stats.total_used_filament;   // mm
            const double weight_g    = stats.total_weight;           // grams

            // Collect layer Z heights from first object
            QVector<float> layer_zs;
            if (!print.objects().empty()) {
                const auto& obj_layers = print.objects().front()->layers();
                layer_zs.reserve(int(obj_layers.size()));
                for (const auto* l : obj_layers)
                    layer_zs.push_back(float(l->print_z));
            }

            // Post result back to main thread
            QMetaObject::invokeMethod(this, [this, est_normal, est_silent, layers, obj_count,
                                              filament_mm, weight_g,
                                              layer_zs = std::move(layer_zs)]() mutable {
                auto* mf2 = wxGetApp().mainframe;
                const QString time_str = est_normal.isEmpty() ? est_silent : est_normal;
                const QString fil_str  = QString("%1 mm").arg(filament_mm, 0, 'f', 1);
                const QString wgt_str  = QString("%1 g").arg(weight_g, 0, 'f', 1);
                const QString msg = tr("Slice done — %1 object(s), %2 layer(s), est. %3")
                    .arg(obj_count).arg(layers).arg(time_str.isEmpty() ? tr("N/A") : time_str);
                if (mf2) {
                    mf2->set_status_message(msg);
                    mf2->update_estimates(
                        time_str.isEmpty() ? tr("N/A") : time_str,
                        fil_str,
                        wgt_str,
                        QString::number(layers));
                    mf2->set_layer_data(std::move(layer_zs));
                }
            }, Qt::QueuedConnection);
        } catch (const std::exception& e) {
            const QString err = QString::fromStdString(e.what());
            QMetaObject::invokeMethod(this, [this, err]() {
                auto* mf2 = wxGetApp().mainframe;
                if (mf2) mf2->set_status_message(tr("Slicing error: %1").arg(err));
            }, Qt::QueuedConnection);
        }
    });

    // Auto-clean the thread object when it finishes
    QObject::connect(thread, &QThread::finished, thread, &QObject::deleteLater);
    thread->start();
}

void Plater::undo()
{
    if (auto* stack = qobject_cast<QUndoStack*>(m_undo_stack))
        if (stack->canUndo()) stack->undo();
}

void Plater::redo()
{
    if (auto* stack = qobject_cast<QUndoStack*>(m_undo_stack))
        if (stack->canRedo()) stack->redo();
}

bool Plater::can_undo() const
{
    if (auto* stack = qobject_cast<QUndoStack*>(m_undo_stack))
        return stack->canUndo();
    return false;
}

bool Plater::can_redo() const
{
    if (auto* stack = qobject_cast<QUndoStack*>(m_undo_stack))
        return stack->canRedo();
    return false;
}

bool Plater::is_project_dirty() const
{
    if (auto* stack = qobject_cast<QUndoStack*>(m_undo_stack))
        return !stack->isClean();
    return false;
}

QString Plater::get_project_name()
{
    if (!m_loaded_path.isEmpty())
        return QFileInfo(m_loaded_path).completeBaseName();
    return {};
}

QString Plater::get_project_filename(const QString& extension) const
{
    if (m_loaded_path.isEmpty()) return {};
    if (extension.isEmpty()) return m_loaded_path;
    return QFileInfo(m_loaded_path).path() + "/" +
           QFileInfo(m_loaded_path).completeBaseName() + extension;
}

void Plater::update_status_label()
{
    if (!m_status_label) return;
    // Count only "real" objects (not the placeholder which only has Qt::ItemIsEnabled)
    int obj_count = 0;
    if (m_object_list) {
        for (int i = 0; i < m_object_list->topLevelItemCount(); ++i) {
            if (m_object_list->topLevelItem(i)->flags() & Qt::ItemIsSelectable)
                ++obj_count;
        }
    }
    if (obj_count == 0) {
        m_status_label->setText(tr("No objects loaded"));
    } else {
        m_status_label->setText(tr("%1 object(s) loaded").arg(obj_count));
    }
    // Mirror count to the main window status bar
    if (auto* mf = wxGetApp().mainframe) {
        mf->set_status_objects(obj_count);
        if (obj_count == 0)
            mf->set_status_message(_L("Ready"));
        else
            mf->set_status_message(tr("Plate has %1 object(s)").arg(obj_count));
    }
    emit objectsChanged();
}

void Plater::select_view(const std::string& direction)
{
    if (m_gl_canvas) m_gl_canvas->set_view(direction);
}

void Plater::select_view_3D(const std::string& name, bool /*no_slice*/)
{
    select_view(name);
}

void Plater::zoom_to_bed()
{
    if (m_gl_canvas) m_gl_canvas->zoom_to_fit();
}

void Plater::arrange_objects()
{
    // Grid arrangement: lay objects out in rows on the build plate.
    // Uses move_object() to preserve mesh data.
    if (!m_gl_canvas) return;

    const int n = m_gl_canvas->object_count();
    if (n == 0) return;

    const float gap    = 12.f;
    const float margin = 15.f;
    const float plate_w = m_gl_canvas->m_plate_w;
    const float plate_d = m_gl_canvas->m_plate_h;

    // Snapshot current bounding-box sizes
    struct ItemSize { float w, d, h; };
    std::vector<ItemSize> sizes;
    sizes.reserve(n);
    for (int i = 0; i < n; ++i) {
        const auto& bbox = m_gl_canvas->get_object_bbox(i);
        const QVector3D ext = bbox.max_pt - bbox.min_pt;
        sizes.push_back({
            ext.x() > 1.f ? ext.x() : 40.f,
            ext.y() > 1.f ? ext.y() : 40.f,
            ext.z() > 1.f ? ext.z() : 50.f
        });
    }

    // Determine column count
    const float avg_w = [&] {
        float sum = 0.f;
        for (const auto& s : sizes) sum += s.w;
        return sum / float(sizes.size());
    }();
    const int cols = std::max(1, (int)((plate_w - margin * 2 + gap) / (avg_w + gap)));

    // Move each object to its new grid position (preserves mesh_verts)
    int idx = 0;
    for (int i = 0; i < n; ++i) {
        const auto& s = sizes[i];
        const int row = i / cols;
        const int col = i % cols;

        float x0 = margin;
        for (int c = 0; c < col; ++c)
            x0 += sizes[i - (col - c)].w + gap;

        const float y0 = margin + row * (s.d + gap);
        const float cx = std::min(x0, plate_w - s.w - margin);
        const float cy = std::min(y0, plate_d - s.d - margin);

        m_gl_canvas->move_object(i,
            QVector3D(cx, cy, 0.f),
            QVector3D(cx + s.w, cy + s.d, s.h));
        ++idx;
    }
    m_gl_canvas->zoom_to_fit();

    if (auto* mf = wxGetApp().mainframe)
        mf->set_status_message(tr("Arranged %1 object(s)").arg(idx));
}

void Plater::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasUrls()) {
        for (const QUrl& url : event->mimeData()->urls()) {
            const QString s = url.toLocalFile().toLower();
            if (s.endsWith(".3mf") || s.endsWith(".stl") || s.endsWith(".obj")
                || s.endsWith(".amf") || s.endsWith(".step") || s.endsWith(".stp")) {
                event->acceptProposedAction();
                // Give visual hint in the GL view
                if (m_gl_canvas)
                    m_gl_canvas->setStyleSheet(
                        QStringLiteral("background-color: rgba(0,120,255,30);"));
                return;
            }
        }
    }
}

void Plater::dragLeaveEvent(QDragLeaveEvent* event)
{
    QWidget::dragLeaveEvent(event);
    if (m_gl_canvas)
        m_gl_canvas->setStyleSheet(QString());
}

void Plater::dropEvent(QDropEvent* event)
{
    if (m_gl_canvas)
        m_gl_canvas->setStyleSheet(QString());  // remove highlight
    if (!event->mimeData()->hasUrls()) return;
    for (const QUrl& url : event->mimeData()->urls()) {
        const QString path = url.toLocalFile();
        if (!path.isEmpty()) load_project(path);
    }
    event->acceptProposedAction();
    update_status_label();
}

const NotificationManager* Plater::get_notification_manager() const { return nullptr; }
NotificationManager* Plater::get_notification_manager() { return nullptr; }

} // namespace GUI
} // namespace Slic3r
