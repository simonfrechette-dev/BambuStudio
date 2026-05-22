#pragma once

#include "slic3r/GUI/GUI_Utils.hpp"

#include <string>

namespace Slic3r { namespace GUI {

class UxProgramTermsDialog : public DPIDialog
{
public:
    explicit UxProgramTermsDialog(QWidget* parent = nullptr);

private:
    void*       m_webview{nullptr}; // stub
    std::string m_host_url;

protected:
    void on_dpi_changed(const QRect& suggested_rect) override;
};

}} // namespace Slic3r::GUI
