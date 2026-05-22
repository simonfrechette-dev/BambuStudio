// Qt6 port of AboutDialog.cpp — matches the original wxWidgets look and feel
#include "AboutDialog.hpp"

#include <QPainter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTextBrowser>
#include <QDesktopServices>
#include <QUrl>
#include <QFrame>

#include "I18N.hpp"
#include "libslic3r/libslic3r.h"
#include "Widgets/Button.hpp"
#include "Widgets/Label.hpp"
#include "QtExtensions.hpp"

namespace Slic3r { namespace GUI {

// ---------------------------------------------------------------------------
// BannerWidget — 560×237 banner with BambuStudio_about.svg + version overlay
// ---------------------------------------------------------------------------

class BannerWidget : public QWidget
{
    QPixmap m_bmp;
public:
    explicit BannerWidget(QWidget *parent) : QWidget(parent)
    {
        setFixedSize(560, 237);
        ScalableBitmap sb(this, "BambuStudio_about", 250);
        if (!sb.bmp().isNull())
            m_bmp = sb.bmp();

        // Version label overlaid — positioned ~165px from top, centered
        auto *layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 165, 0, 0);
        layout->setSpacing(0);

        std::string ver = SLIC3R_VERSION;
        QString ver_str = _L("Version") + " " + QString::fromStdString(ver);

        auto *ver_lbl = new QLabel(ver_str, this);
        ver_lbl->setFont(Label::Head_16);
        ver_lbl->setAlignment(Qt::AlignCenter);
        ver_lbl->setStyleSheet(
            "QLabel { background-color: #00AF42; color: #FFFFFD; padding: 4px 10px; }");
        layout->addWidget(ver_lbl, 0, Qt::AlignHCenter);
        layout->addStretch(1);
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter painter(this);
        if (!m_bmp.isNull())
            painter.drawPixmap(rect(), m_bmp, m_bmp.rect());
    }
};

// ---------------------------------------------------------------------------
// AboutDialogLogo — draws BambuStudio_192px.png centered on white background
// ---------------------------------------------------------------------------

AboutDialogLogo::AboutDialogLogo(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet("background-color: white;");
    ScalableBitmap bmp(this, "BambuStudio_192px", 192);
    if (!bmp.bmp().isNull()) {
        m_logo = bmp.bmp();
        setFixedSize(bmp.GetBmpSize());
    } else {
        setFixedSize(192, 192);
    }
}

void AboutDialogLogo::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.fillRect(rect(), Qt::white);
    if (!m_logo.isNull()) {
        int x = (width()  - m_logo.width())  / 2;
        int y = (height() - m_logo.height()) / 2;
        painter.drawPixmap(x, y, m_logo);
    }
}

// ---------------------------------------------------------------------------
// CopyrightsDialog
// ---------------------------------------------------------------------------

CopyrightsDialog::CopyrightsDialog()
    : DPIDialog(nullptr)
{
    setWindowTitle(QString::fromStdString(SLIC3R_APP_FULL_NAME)
                   + " - " + _L("Portions copyright"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setMinimumSize(870, 520);
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet("background-color: white;");

    auto *vbox = new QVBoxLayout(this);
    vbox->setContentsMargins(15, 5, 15, 15);
    vbox->setSpacing(0);

    auto *line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    vbox->addWidget(line);

    fill_entries();

    auto *browser = new QTextBrowser(this);
    browser->setOpenExternalLinks(true);
    browser->setHtml(get_html_text());
    browser->setMinimumSize(870, 520);
    vbox->addWidget(browser, 1);
}

void CopyrightsDialog::fill_entries()
{
    m_entries = {
        { "Admesh",                                          "", "https://admesh.readthedocs.io/" },
        { "Anti-Grain Geometry",                             "", "http://antigrain.com" },
        { "Assimp",                                          "", "https://www.assimp.org" },
        { "ArcWelderLib",                                    "", "https://plugins.octoprint.org/plugins/arc_welder" },
        { "Boost",                                           "", "http://www.boost.org" },
        { "Cereal",                                          "", "http://uscilab.github.io/cereal" },
        { "CGAL",                                            "", "https://www.cgal.org" },
        { "Clipper",                                         "", "http://www.angusj.com" },
        { "libcurl",                                         "", "https://curl.se/libcurl" },
        { "Eigen3",                                          "", "http://eigen.tuxfamily.org" },
        { "Expat",                                           "", "http://www.libexpat.org" },
        { "fast_float",                                      "", "https://github.com/fastfloat/fast_float" },
        { "GLEW (The OpenGL Extension Wrangler Library)",    "", "http://glew.sourceforge.net" },
        { "GLFW",                                            "", "https://www.glfw.org" },
        { "GNU gettext",                                     "", "https://www.gnu.org/software/gettext" },
        { "ImGUI",                                           "", "https://github.com/ocornut/imgui" },
        { "Libigl",                                         "", "https://libigl.github.io" },
        { "libnest2d",                                       "", "https://github.com/tamasmeszaros/libnest2d" },
        { "lib_fts",                                         "", "https://www.forrestthewoods.com" },
        { "Mesa 3D",                                         "", "https://mesa3d.org" },
        { "Miniz",                                           "", "https://github.com/richgel999/miniz" },
        { "Nanosvg",                                         "", "https://github.com/memononen/nanosvg" },
        { "nlohmann/json",                                   "", "https://json.nlohmann.me" },
        { "Qhull",                                           "", "http://qhull.org" },
        { "Open Cascade",                                    "", "https://www.opencascade.com" },
        { "OpenGL",                                          "", "https://www.opengl.org" },
        { "PoEdit",                                          "", "https://poedit.net" },
        { "PrusaSlicer",                                     "", "https://www.prusa3d.com" },
        { "Real-Time DXT1/DXT5 C compression library",       "", "https://github.com/Cyan4973/RygsDXTc" },
        { "SemVer",                                          "", "https://semver.org" },
        { "Shinyprofiler",                                   "", "https://code.google.com/p/shinyprofiler" },
        { "SuperSlicer",                                     "", "https://github.com/supermerill/SuperSlicer" },
        { "TBB",                                             "", "https://www.intel.cn/content/www/cn/zh/developer/tools/oneapi/onetbb.html" },
        { "wxWidgets",                                       "", "https://www.wxwidgets.org" },
        { "zlib",                                            "", "http://zlib.net" },
    };
}

QString CopyrightsDialog::get_html_text()
{
    QString text = "<html><body>";
    text += "<p><b>" + _L("License") + "</b></p>";
    text += "<p>" + _L("Bambu Studio is licensed under ") +
            "<a href=\"https://www.gnu.org/licenses/agpl-3.0.html\">" +
            _L("GNU Affero General Public License, version 3") + ".</a></p>";
    text += "<p>" + _L("Bambu Studio is based on PrusaSlicer by Prusa Research, which is from "
                       "Slic3r by Alessandro Ranellucci and the RepRap community") + ".</p>";
    text += "<br/><p><b>" + _L("Libraries") + "</b></p>";
    text += "<p>" + _L("This software uses open source components whose copyright and other "
                       "proprietary rights belong to their respective owners") + ":</p>";

    for (const auto &entry : m_entries) {
        text += "<p>" + QString::fromStdString(entry.lib_name) + "<br/>";
        text += "<a href=\"" + QString::fromStdString(entry.link) + "\">"
                + QString::fromStdString(entry.link) + "</a></p>";
    }

    text += "</body></html>";
    return text;
}

// ---------------------------------------------------------------------------
// AboutDialog
// ---------------------------------------------------------------------------

AboutDialog::AboutDialog()
    : DPIDialog(nullptr)
{
    setWindowTitle(QString("%1 - %2")
        .arg(QString::fromStdString(SLIC3R_APP_FULL_NAME), _L("About")));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet("background-color: white;");

    auto *main_sizer = new QVBoxLayout(this);
    main_sizer->setContentsMargins(0, 0, 0, 0);
    main_sizer->setSpacing(0);

    // ── Top banner (BambuStudio_about.svg + version overlay) ──────────────
    auto *banner = new BannerWidget(this);
    main_sizer->addWidget(banner);

    // ── Text body section ──────────────────────────────────────────────────
    auto *ver_sizer = new QVBoxLayout;
    ver_sizer->setContentsMargins(0, 0, 0, 0);
    ver_sizer->setSpacing(0);
    main_sizer->addLayout(ver_sizer);

    // Attribution texts (5 paragraphs, gray, max 520px wide)
    auto *text_hor = new QHBoxLayout;
    text_hor->setContentsMargins(20, 0, 0, 0);
    auto *text_sizer = new QVBoxLayout;
    text_hor->addLayout(text_sizer);
    text_hor->addStretch(1);

    QStringList attribution = {
        _L("Bambu Studio is based on PrusaSlicer by PrusaResearch and SuperSlicer by Merill(supermerill)."),
        _L("PrusaSlicer is originally based on Slic3r by Alessandro Ranellucci."),
        _L("Slic3r was created by Alessandro Ranellucci with the help of many other contributors."),
        _L("Bambu Studio also referenced some ideas from Cura by Ultimaker."),
        _L("There many parts of the software that come from community contributions, so we're unable to "
           "list them one-by-one, and instead, they'll be attributed in the corresponding code comments."),
    };

    text_sizer->addSpacing(33);
    for (const auto &txt : attribution) {
        auto *lbl = new QLabel(this);
        lbl->setText(txt);
        lbl->setWordWrap(true);
        lbl->setMaximumWidth(520);
        lbl->setFont(Label::Body_12);
        lbl->setStyleSheet("QLabel { color: rgb(107,107,107); background-color: white; }");
        text_sizer->addWidget(lbl);
        text_sizer->addSpacing(3);
    }

    ver_sizer->addLayout(text_hor);
    ver_sizer->addSpacing(43);

    // ── Copyright + link + Portions copyright button ──────────────────────
    auto *copyright_hor = new QHBoxLayout;
    copyright_hor->setContentsMargins(20, 0, 20, 0);
    copyright_hor->setSpacing(0);

    auto *copyright_col = new QVBoxLayout;
    copyright_hor->addLayout(copyright_col);
    copyright_hor->addStretch(1);

    auto *copyright_lbl = new QLabel(
        "Copyright(C) 2021-2025 Lunkuo All Rights Reserved", this);
    copyright_lbl->setStyleSheet(
        "QLabel { color: rgb(107,107,107); background-color: white; }");
    copyright_col->addWidget(copyright_lbl);

    auto *link_lbl = new QLabel(
        "<a href='https://www.bambulab.com'>www.bambulab.com</a>", this);
    link_lbl->setOpenExternalLinks(true);
    link_lbl->setStyleSheet("background-color: white;");
    copyright_col->addWidget(link_lbl);

    // "Portions copyright" button — white bg, dark border/text, 12dp corner radius
    auto *btn_col = new QVBoxLayout;
    btn_col->addSpacing(10);

    auto *portions_btn = new Button(this, _L("Portions copyright"));
    portions_btn->setFont(Label::Body_12);
    portions_btn->SetCornerRadius(12);
    portions_btn->setMinimumSize(QSize(120, 24));
    portions_btn->SetBackgroundColor(StateColor(
        std::make_pair(QColor(255, 255, 255), (int)StateColor::Disabled),
        std::make_pair(QColor(206, 206, 206), (int)StateColor::Pressed),
        std::make_pair(QColor(238, 238, 238), (int)StateColor::Hovered),
        std::make_pair(QColor(255, 255, 255), (int)StateColor::Normal)));
    portions_btn->SetBorderColor(StateColor(
        std::make_pair(QColor(144, 144, 144), (int)StateColor::Disabled),
        std::make_pair(QColor(38, 46, 48),    (int)StateColor::Normal)));
    portions_btn->SetTextColor(StateColor(
        std::make_pair(QColor(144, 144, 144), (int)StateColor::Disabled),
        std::make_pair(QColor(38, 46, 48),    (int)StateColor::Normal)));

    connect(portions_btn, &Button::clicked, this, []() {
        CopyrightsDialog dlg;
        dlg.exec();
    });
    btn_col->addWidget(portions_btn);
    copyright_hor->addLayout(btn_col);

    ver_sizer->addLayout(copyright_hor);
    ver_sizer->addSpacing(30);

    adjustSize();
    setFixedSize(sizeHint());
}

}} // namespace Slic3r::GUI
