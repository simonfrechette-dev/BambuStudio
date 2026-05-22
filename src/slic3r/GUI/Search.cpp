// Qt6 stub for Search.cpp
#include "Search.hpp"

namespace Slic3r { namespace Search {

// ---------------------------------------------------------------------------
// SearchItem stubs
// ---------------------------------------------------------------------------

void SearchItem::paintEvent(QPaintEvent* /*event*/) {}
void SearchItem::enterEvent(QEnterEvent* /*evt*/) {}
void SearchItem::leaveEvent(QEvent* /*evt*/) {}
void SearchItem::mousePressEvent(QMouseEvent* /*evt*/) {}
void SearchItem::mouseReleaseEvent(QMouseEvent* /*evt*/) {}

// ---------------------------------------------------------------------------
// SearchListModel stubs
// ---------------------------------------------------------------------------

int SearchListModel::rowCount(const QModelIndex& /*parent*/) const { return 0; }
QVariant SearchListModel::data(const QModelIndex& /*index*/, int /*role*/) const { return {}; }

// ---------------------------------------------------------------------------
// SearchDialog stubs
// ---------------------------------------------------------------------------

void SearchDialog::Popup(QWidget* /*anchor*/) {}
void SearchDialog::Dismiss() {}

SearchDialog::~SearchDialog() {}

// ---------------------------------------------------------------------------
// SearchObjectDialog stubs
// ---------------------------------------------------------------------------

void SearchObjectDialog::Popup(QWidget* /*anchor*/) {}
void SearchObjectDialog::Dismiss() {}

SearchObjectDialog::~SearchObjectDialog() {}

}} // namespace Slic3r::Search

