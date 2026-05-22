#ifndef slic3r_MixedFilamentDialog_hpp_
#define slic3r_MixedFilamentDialog_hpp_

#include <string>
#include <vector>
#include <QDialog>
#include <QColor>

// Phase 4 TODO: full Qt port of mixed-filament picker dialog

namespace Slic3r {
namespace GUI {

struct MixedFilamentResult {
    std::vector<unsigned int> components;   // 1-based physical filament indices
    std::vector<int>          ratios;       // percentages, sum = 100
    bool gradient_enabled   = false;
    int  gradient_direction = 0;            // 0 = A->B, 1 = B->A
    bool per_part_gradient  = false;
};

class MixedFilamentDialog : public QDialog
{
    Q_OBJECT
public:
    MixedFilamentDialog(QWidget *parent,
                        const std::vector<std::string>& physical_colors,
                        const std::vector<std::string>& physical_names,
                        const std::vector<std::string>& physical_types = {});

    MixedFilamentDialog(QWidget *parent,
                        const MixedFilamentResult& existing,
                        const std::vector<std::string>& physical_colors,
                        const std::vector<std::string>& physical_names,
                        const std::vector<std::string>& physical_types = {});

    MixedFilamentResult get_result() const { return m_result; }

private:
    MixedFilamentResult         m_result;
    std::vector<std::string>    m_physical_colors;
    std::vector<std::string>    m_physical_names;
    std::vector<std::string>    m_physical_types;
};

} // namespace GUI
} // namespace Slic3r

#endif // slic3r_MixedFilamentDialog_hpp_
