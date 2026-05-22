#include "I18N.hpp"

namespace Slic3r { namespace GUI { 

QString L_str(const std::string &str)
{
    return Slic3r::GUI::I18N::translate(str);
}

} }
