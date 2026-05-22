//**********************************************************/
/* File: uiAMSBestPositionPopup.hpp
*  Description: The popup with suggest best ams position
*
//**********************************************************/

#pragma once
#include "slic3r/GUI/Widgets/AMSItem.hpp"
#include "slic3r/GUI/Widgets/Label.hpp"
#include "slic3r/GUI/Widgets/PopupWindow.hpp"
#include "slic3r/GUI/wxExtensions.hpp"
#include "slic3r/GUI/Widgets/Button.hpp"
#include "slic3r/GUI/DeviceCore/DevFilaSystem.h"
#include "slic3r/GUI/DeviceCore/DevFilaSwitch.h"
#include <algorithm>
#include "libslic3r/ProjectTask.hpp"

#include <tuple>

namespace Slic3r { namespace GUI {


class UiStyledAMSPanel : public QWidget
{
public:
    UiStyledAMSPanel(QWidget* parent,
                int id = -1,
                const QPoint& pos = QPoint(),
                const QSize& size = QSize(),
                const QColor& borderColor = QColor(200, 200, 200),
                const QColor& bgColor = QColor(255, 255, 255),
                bool borderDashed = true,
                QString name = "",
                bool isTop = false);

protected:
    void OnPaint(QPaintEvent& event);

private:

    bool m_borderDashed;
    int m_borderWidth;
    int m_radius;
    QColor m_borderColor;
    QColor m_bgColor;
    QString m_name;
    bool m_isTop;
};

class UiStyledSwitchPanel : public QWidget
{
public:
    UiStyledSwitchPanel(QWidget* parent,
                        int id,
                        const QPoint& pos,
                        const QSize& size,
                        const QColor& borderColor,
                        const QColor& bgColor,
                        bool borderDashed,
                        int borderWidth,
                        int radius,
                        bool isTop);


    void AddToLeft(QWidget* window, int proportion = 0, int flag = 0, int border = 0);
    void AddToRight(QWidget* window, int proportion = 0, int flag = 0, int border = 0);
    void Clear(bool deleteWindows);

    QLayout* GetLeftSizer() { return m_leftSizer; }
    QLayout* GetRightSizer() { return m_rightSizer; }
    void LayoutAndFit()
    {
        m_leftSizer->activate();
        // m_leftSizer->Fit();
        m_rightSizer->activate();
        // m_rightSizer->Fit();
        m_splitSizer->activate();
        // m_splitSizer->Fit();
        m_contentSizer->activate();
        // m_contentSizer->Fit();
        m_mainSizer->activate();
        // m_mainSizer->Fit();
        adjustSize();
    }
protected:
    void OnPaint(QPaintEvent& event);
private:

    bool m_borderDashed;
    int m_borderWidth;
    int m_radius;
    QColor m_borderColor;
    QColor m_bgColor;
    bool m_isTop;
 
    QBoxLayout* m_mainSizer{nullptr};
    QBoxLayout* m_contentSizer{nullptr};
    QBoxLayout* m_splitSizer{nullptr};
    QLayout* m_leftSizer{nullptr};
    QLayout* m_rightSizer{nullptr};
    
    static constexpr int labelHeight = 30;

};


enum DataStatusType {
    ADJUST,
    OK,
    UNMATCHED
};

class UiAMSSlot : public QWidget
{
public:
    UiAMSSlot(QWidget* parent,
            const std::vector<QColor>& bgColours,
            const QString&  text,
            DataStatusType status,
            int id = -1,
            const QPoint& pos = QPoint(),
            const QSize& size = QSize(),
            double colourFactor = 1.0,
            double scaleFactor = 1.0);


private:
    void OnPaint(QPaintEvent&);
    void DrawRectangle(QPainter& dc, const QSize& cli);
    void DrawLine(QPainter& dc, const QSize& cli);
    QColor LightenColour(const QColor& original);
    bool IsDark(const QColor& c)
    {
        int brightness = (c.red() * 299 + c.green() * 587 + c.blue() * 114) / 1000;
        return brightness < 128;  // 0-255 range，128 mid
    }
private:
    const std::vector<QColor>& m_bgColours;
    QString m_text;
    DataStatusType m_status;
    QSize m_size;
    ScalableBitmap *m_ams_slot_readonly{nullptr};
    int m_rectangleW = 44;
    int m_rectangleH = 62;
    int rectangleW = 44;
    int rectangleH = 62;
    double m_colourFactor = 1.0f;
    double m_scaleFactor = 1.0f;
};

struct DataAmsSlotInfo
{
    QString amsName;
    QString name;
    std::vector<QColor> colours;
    double colourFactor;
    double scaleFactor;
    DataStatusType status;
};

class UiAMS : public UiStyledAMSPanel
{
public:
    UiAMS(QWidget* parent,
        const std::vector<DataAmsSlotInfo>& amsInfo,
        int id,
        const QPoint& pos,
        const QSize& minSize);            
private:
    void init();
    std::vector<DataAmsSlotInfo> m_amsInfo;
    QString m_amsTitle;
    QSize m_minSize;
};


struct DataStatusParam {
    // int count = 0;
    int width = 0;
    int height = 0;
    std::vector<DataAmsSlotInfo> ams_slots;
};

class ReselectMachineDialog : public QDialog
{
public:
    ReselectMachineDialog(QWidget* parent);
    ~ReselectMachineDialog();
    void Update(MachineObject* obj,
                const std::map<int, int>&  best_pos_map,
                const std::vector<FilamentInfo>& ams_mapping,
                QString save_time);

private:
    int CaculateSwitcherDistribution(MachineObject* obj, const std::map<int, int>&  best_pos_map, const std::vector<FilamentInfo>& ams_mapping);
    QString getTrayID(MachineObject* obj, const std::string& amsID, const std::string& slotID);
    void OnRefreshButton(QEvent& event);

private:
    int saveTimes{0};
    QBoxLayout* mainSizer{nullptr};
    QWidget* textPanel{nullptr};
    QBoxLayout* textSizer{nullptr};
    Label* suggestText{nullptr};
    // QLabel* linkwiki{nullptr};
    Label* linkwiki{nullptr};
    Label* summaryText{nullptr};
    UiStyledSwitchPanel* filamentSwitch{nullptr};
    QLabel* filamentTips{nullptr};
    std::vector<std::vector<DataAmsSlotInfo>> inAAMS{};
    std::vector<std::vector<DataAmsSlotInfo>> inBAMS{};
    QWidget* statusBar{nullptr};
    QBoxLayout* btnSizer{nullptr};
    Button* m_buttonClose{nullptr };
    Button* m_buttonRefresh{ nullptr };
    std::vector<QColor>colourAdjust{QColor("#675AFF")};
    std::vector<QColor>colourOK{QColor("#FF8181")};
    std::vector<QColor>colourUnused{QColor("#FF818140")};
    // QLabel* m_bitmapSelectMachine{nullptr};
};


}} // namespace Slic3r::GUI