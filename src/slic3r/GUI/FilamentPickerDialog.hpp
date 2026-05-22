#ifndef slic3r_GUI_FilamentPickerDialog_hpp_
#define slic3r_GUI_FilamentPickerDialog_hpp_
#include <QWidget>
#include <QString>

#include "GUI_App.hpp"
#include "GUI.hpp"
#include "GUI_Utils.hpp"
#include "FilamentBitmapUtils.hpp"
#include "Widgets/Button.hpp"
#include "EncodedFilament.hpp"
#include <vector>
#include <string>

namespace Slic3r { namespace GUI {

class FilamentPickerDialog : public DPIDialog
{
public:
    FilamentPickerDialog(QWidget *parent, const QString &fila_id, const FilamentColor &fila_color, const std::string &fila_type);
    virtual ~FilamentPickerDialog();

    // Public interface methods
    bool IsDataLoaded() const { return m_is_data_loaded; }
    QColor GetSelectedColour() const;
    const FilamentColor& GetSelectedFilamentColor() const { return m_cur_filament_color; }

protected:
    void on_dpi_changed(const QRect &suggested_rect) override;

    // Event handlers
#ifdef __WXGTK__
    void OnWindowCreate(wxWindowCreateEvent& event);
#endif
    void OnMouseLeftDown(QMouseEvent& event);
    void OnMouseMove(QMouseEvent& event);
    void OnMouseLeftUp(QMouseEvent& event);
    void OnButtonPaint(QPaintEvent& event);
    void OnTimerCheck(QTimerEvent& event);
    void OnFlashTimer(QTimerEvent& event);

    // Platform-independent window detection
    bool IsClickOnTopMostWindow(const QPoint& mouse_pos);
    void StartClickDetection();
    void StopClickDetection();
    void CleanupTimers();

private:
    // UI creation methods
    QBoxLayout* CreatePreviewPanel(const FilamentColor& fila_color, const std::string& fila_type);
    QScrollArea* CreateColorGrid();
    QBoxLayout* CreateSeparatorLine();
    void CreateMoreInfoButton();
    QBoxLayout* CreateButtonPanel();
    void BindEvents();

    // Preview panel helper methods
    void CreateColorBitmap(const FilamentColor& fila_color);
    QBoxLayout* CreateInfoSection();
    void SetupLabelsContent(const FilamentColor& fila_color, const std::string& fila_type);

    // UI update methods
    void UpdatePreview(const FilamentColorCode& filament);
    void UpdateCustomColorPreview(const QColor& custom_color);
    void UpdateButtonStates(QPushButton* selected_btn);

    // Shaped window methods
    void SetWindowShape();
    void CreateShapedBitmap();

    // Data loading
    bool LoadFilamentData(const QString& fila_id);
    QColor GetSingleColorData();

    // Flash effect
    void StartFlashing();

    // UI elements
    QLabel* m_color_demo{nullptr};
    QLabel* m_label_preview_color{nullptr};
    QLabel* m_label_preview_idx{nullptr};
    QLabel* m_label_preview_type{nullptr};
    Button* m_more_btn{nullptr};
    Button* m_ok_btn{nullptr};
    Button* m_cancel_btn{nullptr};

    // Data members
    bool m_is_data_loaded{false};
    QString *m_cur_color_name{nullptr};
    FilamentColorCodeQuery* m_color_query{nullptr};
    FilamentColorCodes* m_cur_color_codes{nullptr};
    QPushButton* m_cur_selected_btn{nullptr};
    FilamentColor m_cur_filament_color;

    // Shaped window members
    QPixmap m_shape_bmp;
    int m_corner_radius{8};

    // Mouse drag members
    QPoint m_drag_delta;

    // Click detection timers
    QTimer* m_click_timer{nullptr};
    bool m_last_mouse_down{false};

    // Flash effect timer
    QTimer* m_flash_timer{nullptr};
    int m_flash_step{0};
};

}} // namespace Slic3r::GUI

#endif
