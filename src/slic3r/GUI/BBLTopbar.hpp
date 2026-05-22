#pragma once

#include <QToolBar>
#include <QAction>
#include <QMenu>
#include <QPixmap>
#include <QRect>
#include <QPoint>
#include <QString>
#include <QMainWindow>
#include <QToolButton>
#include <QLabel>
#include <QMouseEvent>
#include <QIcon>

#include "DeviceManager.hpp"

#include <string>

class BBLTopbar : public QToolBar
{
    Q_OBJECT
public:
    BBLTopbar(QWidget* pwin, QMainWindow* parent);
    BBLTopbar(QMainWindow* parent);
    void Init(QMainWindow *parent);
    ~BBLTopbar();
    void UpdateToolbarWidth(int width);
    void Rescale();
    void OnIconize();
    void OnFullScreen();
    void OnCloseFrame();
    void OnFileToolItem();
    void OnDropdownToolItem();
    void OnCalibToolItem();
    void OnMenuClose();
    void OnOpenProject();
    void show_publish_button(bool show);
    void OnSaveProject();
    void OnUndo();
    void OnRedo();
    void OnModelStoreClicked();
    void OnPublishClicked();

    void SetFileMenu(QMenu* file_menu);
    void AddDropDownSubMenu(QMenu* sub_menu, const QString& title);
    void AddDropDownMenuItem(QAction* menu_item);
    QMenu *GetTopMenu();
    QMenu *GetCalibMenu();
    void SetTitle(QString title);
    void SetMaximizedSize();
    void SetWindowSize();

    void EnableSaveItem(bool enable);
    void EnableUndoItem(bool enable);
    void EnableRedoItem(bool enable);
    void EnableUndoRedoItems();
    void DisableUndoRedoItems();

    void SaveNormalRect();

    void ShowCalibrationButton(bool show = true);
    void ShowSettingsButton(bool show = true);

    // Called by MainFrame to let topbar know current param-panel visibility
    // so the toggle button can reflect correct checked state.
    void SetSettingsPanelVisible(bool visible);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

private:
    // Toggle settings (parameters) sidebar
    void OnToggleSettings();

    // Load an SVG from resources/images/<name>.svg and return a QIcon.
    static QIcon loadSvgIcon(const std::string& name, int size = 18);

private:
    QMainWindow* m_frame{nullptr};
    QAction* m_file_menu_item{nullptr};
    QAction* m_dropdown_menu_item{nullptr};
    QRect m_normalRect;
    QMenu m_top_menu;
    QMenu* m_file_menu{nullptr};
    QMenu m_calib_menu;
    QLabel* m_title_label{nullptr};    // centred QLabel widget in toolbar
    QAction* m_title_item{nullptr};    // kept for API compatibility (SetTitle)
    QAction* m_account_item{nullptr};
    QAction* m_model_store_item{nullptr};

    QAction* m_publish_item{nullptr};
    QAction* m_save_item{nullptr};
    QAction* m_undo_item{nullptr};
    QAction* m_redo_item{nullptr};
    QAction* m_calib_item{nullptr};
    QAction* m_settings_item{nullptr};  // Toggle settings sidebar

    // Window control buttons (right end of topbar)
    QToolButton* m_btn_minimize{nullptr};
    QToolButton* m_btn_maximize{nullptr};
    QToolButton* m_btn_close{nullptr};

    QPixmap m_publish_bitmap;
    QPixmap m_publish_disable_bitmap;

    QPixmap maximize_bitmap;
    QPixmap window_bitmap;
    // kept for SetMaximizedSize / SetWindowSize
    QAction* maximize_btn{nullptr};

    int m_toolbar_h{0};
    bool m_skip_popup_file_menu{false};
    bool m_skip_popup_dropdown_menu{false};
    bool m_skip_popup_calib_menu{false};
};
