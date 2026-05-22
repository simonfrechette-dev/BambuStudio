#ifndef slic3r_GUI_AboutDialog_hpp_
#define slic3r_GUI_AboutDialog_hpp_

#include <string>
#include <vector>
#include <QPixmap>
#include "GUI_Utils.hpp"

namespace Slic3r {
namespace GUI {

class AboutDialogLogo : public QWidget
{
    Q_OBJECT
public:
    explicit AboutDialogLogo(QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QPixmap m_logo;
};

class CopyrightsDialog : public DPIDialog
{
    Q_OBJECT
public:
    CopyrightsDialog();
    ~CopyrightsDialog() override = default;

    struct Entry {
        Entry(const std::string &lib_name, const std::string &copyright, const std::string &link)
            : lib_name(lib_name), copyright(copyright), link(link) {}
        std::string lib_name;
        std::string copyright;
        std::string link;
    };

protected:
    void on_dpi_changed(const QRect&) override {}

private:
    std::vector<Entry> m_entries;
    void fill_entries();
    QString get_html_text();
};

class AboutDialog : public DPIDialog
{
    Q_OBJECT
public:
    AboutDialog();
    ~AboutDialog() override = default;

protected:
    void on_dpi_changed(const QRect&) override {}
};

} // namespace GUI
} // namespace Slic3r

#endif // slic3r_GUI_AboutDialog_hpp_
