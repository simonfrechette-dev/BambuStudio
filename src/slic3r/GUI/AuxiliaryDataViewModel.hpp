#ifndef slic3r_GUI_AuxiliaryDataViewModel_hpp_
#define slic3r_GUI_AuxiliaryDataViewModel_hpp_


#include "I18N.hpp"

#include <boost/filesystem.hpp>

class AuxiliaryModelNode;
using AuxiliaryModelNodePtrArray = std::vector<AuxiliaryModelNode*>;

namespace fs = boost::filesystem;

class AuxiliaryModelNode
{
public:
    AuxiliaryModelNode()
    {
        m_parent = NULL;
        name = "";
        m_container = true;
        m_root = true;
    }

    AuxiliaryModelNode(AuxiliaryModelNode* parent, const QString& abs_path, bool is_container)
    {
        m_parent = parent;
        m_container = is_container;
        m_root = false;
        path = abs_path;
        fs::path path_obj(path.toStdString().c_str());
        name = QString::fromStdWString(path_obj.filename().generic_wstring());

        parent->m_children.push_back(this);
    }

    ~AuxiliaryModelNode()
    {
        // free all our children nodes
        size_t count = m_children.size();
        for (size_t i = 0; i < count; i++)
        {
            AuxiliaryModelNode* child = m_children[i];
            delete child;
        }
    }

    bool IsContainer() const
    {
        return m_container;
    }

    AuxiliaryModelNode* GetParent()
    {
        return m_parent;
    }

    void SetParent(AuxiliaryModelNode* parent)
    {
        m_parent = parent;
    }

    AuxiliaryModelNodePtrArray& GetChildren()
    {
        return m_children;
    }
    AuxiliaryModelNode* GetNthChild(unsigned int n)
    {
        return m_children[n];
    }
    void Insert(AuxiliaryModelNode* child, unsigned int n)
    {
        m_children.insert(m_children.begin() + n, child);
    }
    void Append(AuxiliaryModelNode* child)
    {
        m_children.push_back(child);
    }
    unsigned int GetChildCount() const
    {
        return m_children.size();
    }

public:
    QString    name;
    QString    path;

private:
    AuxiliaryModelNode* m_parent;
    AuxiliaryModelNodePtrArray   m_children;
    bool m_container;
    bool m_root;

};

class AuxiliaryModel : public QAbstractItemModel
{
public:
    AuxiliaryModel();
    ~AuxiliaryModel();

    // helper methods to change the model
    QModelIndex CreateFolder(QString name = QString());
    QModelIndexList ImportFile(AuxiliaryModelNode* sel, QStringList file_paths);
    void Delete(const QModelIndex& item);
    void MoveItem(const QModelIndex& dropped_item, const QModelIndex& dragged_item);
    bool IsOrphan(const QModelIndex& item);
    bool Rename(const QModelIndex& item, const QString& name);
    AuxiliaryModelNode* GetParent(AuxiliaryModelNode* node) const;
    void Reparent(AuxiliaryModelNode* node, AuxiliaryModelNode* new_parent);

    void Init(QString aux_path);
    void Reload(QString aux_path);

    // override sorting to always sort branches ascendingly

    int Compare(const QModelIndex& item1, const QModelIndex& item2,
        unsigned int column, bool ascending) const ;

    // implementation of base class virtuals to define model

    virtual unsigned int GetColumnCount() const {
        return 1;
    }

    virtual QString GetColumnType(unsigned int col) const {
        return "string";
    }

    virtual void GetValue(QVariant& variant,
        const QModelIndex& item, unsigned int col) const ;
    virtual bool SetValue(const QVariant& variant,
        const QModelIndex& item, unsigned int col);

    virtual bool IsEnabled(const QModelIndex& item,
        unsigned int col) const ;

    virtual QModelIndex GetParent(const QModelIndex& item) const ;
    virtual bool IsContainer(const QModelIndex& item) const ;
    virtual unsigned int GetChildren(const QModelIndex& parent,
        QModelIndexList& array) const ;

private:
    AuxiliaryModelNode* m_root;
    QString m_root_dir;
};

#endif // slic3r_GUI_AuxiliaryDataViewModel_hpp_

