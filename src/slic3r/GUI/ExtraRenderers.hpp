#ifndef slic3r_GUI_ExtraRenderers_hpp_
#define slic3r_GUI_ExtraRenderers_hpp_

// Qt port of ExtraRenderers.hpp
// Original used wxDataViewCustomRenderer; ported to QStyledItemDelegate stubs.

#include <functional>
#include <QString>
#include <QPixmap>
#include <QStyledItemDelegate>

// ----------------------------------------------------------------------------
// DataViewBitmapText: helper data container (text + bitmap for tree items)
// ----------------------------------------------------------------------------

class DataViewBitmapText
{
public:
    DataViewBitmapText(const QString &text = QString(),
                       const QPixmap& bmp = QPixmap()) :
        m_text(text),
        m_bmp(bmp)
    {}

    DataViewBitmapText(const DataViewBitmapText &other)
        : m_text(other.m_text),
          m_bmp(other.m_bmp)
    {}

    void SetText(const QString &text)       { m_text = text; }
    QString GetText() const                 { return m_text; }
    void SetBitmap(const QPixmap &bmp)      { m_bmp = bmp; }
    const QPixmap &GetBitmap() const        { return m_bmp; }

    bool IsSameAs(const DataViewBitmapText& other) const {
        return m_text == other.m_text;
    }

    bool operator==(const DataViewBitmapText& other) const { return IsSameAs(other); }
    bool operator!=(const DataViewBitmapText& other) const { return !IsSameAs(other); }

private:
    QString  m_text;
    QPixmap  m_bmp;
};

// ----------------------------------------------------------------------------
// BitmapTextRenderer — Qt stub (QStyledItemDelegate)
// ----------------------------------------------------------------------------

class BitmapTextRenderer : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit BitmapTextRenderer(bool use_markup = false, QObject* parent = nullptr)
        : QStyledItemDelegate(parent) {}

    ~BitmapTextRenderer() override = default;

    void EnableMarkup(bool enable = true) {}

    bool WasCanceled() const { return m_was_unusable_symbol; }
    void set_can_create_editor_ctrl_function(std::function<bool()> can_create_fn) { can_create_editor_ctrl = can_create_fn; }

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

private:
    DataViewBitmapText          m_value;
    bool                        m_was_unusable_symbol{ false };
    std::function<bool()>       can_create_editor_ctrl{ nullptr };
};

// ----------------------------------------------------------------------------
// BitmapChoiceRenderer — Qt stub (QStyledItemDelegate)
// ----------------------------------------------------------------------------

class BitmapChoiceRenderer : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit BitmapChoiceRenderer(QObject* parent = nullptr)
        : QStyledItemDelegate(parent) {}

    void set_can_create_editor_ctrl_function(std::function<bool()> can_create_fn) { can_create_editor_ctrl = can_create_fn; }
    void set_default_extruder_idx(std::function<int()> default_extruder_idx_fn)   { get_default_extruder_idx = default_extruder_idx_fn; }
    void set_has_default_extruder(std::function<bool()> has_default_extruder_fn)  { has_default_extruder = has_default_extruder_fn; }

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

private:
    DataViewBitmapText          m_value;
    std::function<bool()>       can_create_editor_ctrl{ nullptr };
    std::function<int()>        get_default_extruder_idx{ nullptr };
    std::function<bool()>       has_default_extruder{ nullptr };
};

// ----------------------------------------------------------------------------
// TextRenderer — Qt stub (QStyledItemDelegate)
// ----------------------------------------------------------------------------

class TextRenderer : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit TextRenderer(QObject* parent = nullptr)
        : QStyledItemDelegate(parent) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    QString m_value;
};

#endif // slic3r_GUI_ExtraRenderers_hpp_
