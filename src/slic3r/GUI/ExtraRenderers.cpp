// Qt6 stub for ExtraRenderers.cpp
#include "ExtraRenderers.hpp"
#include <QPainter>

// ---------------------------------------------------------------------------
// BitmapTextRenderer
// ---------------------------------------------------------------------------

void BitmapTextRenderer::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QStyledItemDelegate::paint(painter, option, index);
}

QSize BitmapTextRenderer::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    return QStyledItemDelegate::sizeHint(option, index);
}

QWidget* BitmapTextRenderer::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    return QStyledItemDelegate::createEditor(parent, option, index);
}

void BitmapTextRenderer::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    QStyledItemDelegate::setEditorData(editor, index);
}

void BitmapTextRenderer::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    QStyledItemDelegate::setModelData(editor, model, index);
}

// ---------------------------------------------------------------------------
// BitmapChoiceRenderer
// ---------------------------------------------------------------------------

void BitmapChoiceRenderer::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QStyledItemDelegate::paint(painter, option, index);
}

QSize BitmapChoiceRenderer::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    return QStyledItemDelegate::sizeHint(option, index);
}

QWidget* BitmapChoiceRenderer::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    return QStyledItemDelegate::createEditor(parent, option, index);
}

void BitmapChoiceRenderer::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    QStyledItemDelegate::setEditorData(editor, index);
}

void BitmapChoiceRenderer::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    QStyledItemDelegate::setModelData(editor, model, index);
}

// ---------------------------------------------------------------------------
// TextRenderer
// ---------------------------------------------------------------------------

void TextRenderer::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QStyledItemDelegate::paint(painter, option, index);
}

QSize TextRenderer::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    return QStyledItemDelegate::sizeHint(option, index);
}

