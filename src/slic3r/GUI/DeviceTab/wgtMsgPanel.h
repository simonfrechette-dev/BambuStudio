#pragma once
#include <QWidget>
#include <QString>


#include <string>

class Label;

namespace Slic3r
{
namespace GUI
{

// Single message item, including color, text and optional wiki link
class wgtMsgPanelItem : public QWidget
{
public:
    wgtMsgPanelItem(QWidget* parent,
                    const QColor& colour,
                    const QString& text,
                    int max_width,
                    const QString& wiki_url = QString());

    void SetColour(const QColor& colour);
    void SetText(const QString& text);
    void SetWiki(const QString& wiki_url);

private:
    void CreateGui();
    void OnClickWiki(QEvent& evt);

private:
    int m_max_width;
    QColor m_colour;
    QString m_text;
    QString m_wiki_url;

    Label* m_text_label{ nullptr };
    QLabel* m_wiki_link{ nullptr };
};

// Message panel based on wxWidget, shows wgtMsgPanelItem in a list
class wgtMsgPanel : public QWidget
{
public:
    explicit wgtMsgPanel(QWidget* parent);
    ~wgtMsgPanel() override = default;

    // Add a message (preferred API)
    void AddMessage(const QString& text,
                    const QColor& colour,
                    const QString& wiki_url = QString());
    void Clear();

    // Const API
    int GetMessageCount() const { return m_list_sizer ? m_list_sizer->count() : 0; }

private:
    void CreateGui();

private:
    Label*            m_label_title{ nullptr };
    QBoxLayout*       m_list_sizer{ nullptr };
};

}
} // namespace Slic3r::GUI
