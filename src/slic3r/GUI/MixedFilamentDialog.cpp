// Phase 4 TODO: full Qt port of MixedFilamentDialog
#include "MixedFilamentDialog.hpp"

namespace Slic3r {
namespace GUI {

MixedFilamentDialog::MixedFilamentDialog(QWidget *parent,
                                         const std::vector<std::string>& physical_colors,
                                         const std::vector<std::string>& physical_names,
                                         const std::vector<std::string>& physical_types)
    : QDialog(parent)
    , m_physical_colors(physical_colors)
    , m_physical_names(physical_names)
    , m_physical_types(physical_types)
{}

MixedFilamentDialog::MixedFilamentDialog(QWidget *parent,
                                         const MixedFilamentResult& existing,
                                         const std::vector<std::string>& physical_colors,
                                         const std::vector<std::string>& physical_names,
                                         const std::vector<std::string>& physical_types)
    : QDialog(parent)
    , m_result(existing)
    , m_physical_colors(physical_colors)
    , m_physical_names(physical_names)
    , m_physical_types(physical_types)
{}

} // namespace GUI
} // namespace Slic3r
