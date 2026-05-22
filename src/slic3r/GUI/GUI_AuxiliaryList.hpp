#ifndef slic3r_GUI_AuxiliaryList_hpp_
#define slic3r_GUI_AuxiliaryList_hpp_

// Phase 4 TODO: Qt port of AuxiliaryList (was QTreeView-based)

#include <map>
#include <vector>
#include <set>
#include <QWidget>
#include <QString>

class AuxiliaryModelNode;
class AuxiliaryModel;

class AuxiliaryList : public QWidget
{
    Q_OBJECT
public:
    explicit AuxiliaryList(QWidget* parent = nullptr);
    ~AuxiliaryList() override = default;

    QLayout* get_top_sizer() { return nullptr; }
    void init_auxiliary();
    void reload(const QString& aux_path);

private:
    AuxiliaryModel* m_auxiliary_model = nullptr;
};

#endif // slic3r_GUI_AuxiliaryList_hpp_
