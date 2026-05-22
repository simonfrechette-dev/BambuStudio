#ifndef slic3r_MarkdownTip_hpp_
#define slic3r_MarkdownTip_hpp_
#include <QWidget>
#include <QString>



namespace Slic3r { namespace GUI {

class MarkdownTip : public QWidget
{
public:
    static bool ShowTip(std::string const &tip, std::string const &tooltip, QPoint pos);

    static void ExitTip();

    static void Reload();

    static void Recreate(QWidget *parent);

    static QWidget* AttachTo(QWidget * parent);

    static QWidget* DetachFrom(QWidget * parent);

private:
    static MarkdownTip* markdownTip(bool create = true);

    MarkdownTip();

    ~MarkdownTip();

    void LoadStyle();

    bool ShowTip(QPoint pos, std::string const &tip, std::string const & tooltip);

    std::string LoadTip(std::string const &tip, std::string const &tooltip);

    void RunScript(std::string const& script);

private:
    QWidget* CreateTipView(QWidget* parent);

    void OnLoaded(QEvent& event);

    void OnTitleChanged(QEvent& event);

    void OnError(QEvent& event);

    void OnTimer(QTimerEvent& event);
    
private:
    QWidget * _tipView = nullptr;
    std::string _lastTip;
    std::string _pendingScript = " ";
    std::string _language;
    QPoint _requestPos;
    double _lastHeight = 0;
    QTimer* _timer = nullptr;
    bool _hide = false;
    bool _data_dir = false;
};

}
}

#endif
