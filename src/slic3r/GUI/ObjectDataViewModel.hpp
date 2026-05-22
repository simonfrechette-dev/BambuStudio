#ifndef slic3r_GUI_ObjectDataViewModel_hpp_
#define slic3r_GUI_ObjectDataViewModel_hpp_
// Qt6 port of ObjectDataViewModel.hpp
// Original wx version backed up to ObjectDataViewModel.hpp.wx-backup

#include <boost/log/trivial.hpp>

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QPixmap>
#include <QString>
#include <QTreeView>
#include <QEvent>
#include <vector>
#include <map>

#include "ExtraRenderers.hpp"

namespace Slic3r {
class ModelObject;
class ModelVolume;
enum class ModelVolumeType : int;

namespace GUI {
class PartPlate;

typedef double                          coordf_t;
typedef std::pair<coordf_t, coordf_t>   t_layer_height_range;

// Qt event type equivalent for wxCUSTOMEVT_LAST_VOLUME_IS_DELETED
inline const QEvent::Type EVT_LAST_VOLUME_IS_DELETED = static_cast<QEvent::Type>(QEvent::registerEventType());

// ----------------------------------------------------------------------------
// Enums
// ----------------------------------------------------------------------------
enum ItemType {
    itUndef         = 0,
    itPlate         = 1,
    itObject        = 2,
    itVolume        = 4,
    itInstanceRoot  = 8,
    itInstance      = 16,
    itSettings      = 32,
    itLayerRoot     = 64,
    itLayer         = 128,
    itInfo          = 256,
};

enum ColumnNumber
{
    colName         = 0,
    colHeight          ,
    colPrint           ,
    colFilament        ,
    colSupportPaint    ,
    colFuzzySkin       ,
    colColorPaint      ,
    colSinking         ,
    colEditing         ,
    colCount           ,
};

enum PrintIndicator
{
    piUndef         = 0,
    piPrintable        ,
    piUnprintable      ,
};

enum VaryHeightIndicator
{
    hiUnVariable,
    hiVariable,
};

enum class InfoItemType
{
    Undef,
    CustomSupports,
    FuzzySkin,
    MmuSegmentation,
    CutConnectors,
};

// ----------------------------------------------------------------------------
// ObjectDataViewModelNode
// ----------------------------------------------------------------------------
class ObjectDataViewModelNode
{
    ObjectDataViewModelNode*                    m_parent;
    std::vector<ObjectDataViewModelNode*>       m_children;
    QPixmap                                     m_empty_bmp;
    size_t                                      m_volumes_cnt = 0;
    std::vector<std::string>                    m_opt_categories;
    t_layer_height_range                        m_layer_range = { 0.0f, 0.0f };

    QString                         m_name;
    QPixmap*                        m_bmp  = &m_empty_bmp;
    ItemType                        m_type;
    int                             m_idx = -1;
    int                             m_plate_idx = -1;
    bool                            m_container = false;
    QString                         m_extruder;
    QPixmap                         m_extruder_bmp;
    QPixmap                         m_action_icon;
    QPixmap                         m_support_icon;
    QPixmap                         m_fuzzyskin_icon;
    QPixmap                         m_color_icon;
    QPixmap                         m_sinking_icon;
    PrintIndicator                  m_printable{piUndef};
    QPixmap                         m_printable_icon;
    VaryHeightIndicator             m_variable_height{hiUnVariable};
    QPixmap                         m_variable_height_icon;
    std::string                     m_warning_icon_name;
    bool                            m_has_lock{false};
    std::string                     m_action_icon_name;
    ModelVolumeType                 m_volume_type = ModelVolumeType(-1);
    bool                            m_is_text_volume{false};
    bool                            m_is_svg_volume{false};
    InfoItemType                    m_info_item_type{InfoItemType::Undef};
    bool                            m_action_enable = false;
    bool                            m_support_enable = false;
    bool                            m_fuzzyskin_enable = false;
    bool                            m_color_enable = false;
    bool                            m_sink_enable = false;

public:
    PartPlate*                      m_part_plate{nullptr};
    ModelObject*                    m_model_object{nullptr};

public:
    ObjectDataViewModelNode(const QString& name,
                            const QString& extruder,
                            const int plate_idx,
                            ModelObject *model_object)
        : m_parent(nullptr)
        , m_name(name)
        , m_type(itObject)
        , m_extruder(extruder)
        , m_plate_idx(plate_idx)
        , m_model_object(model_object)
    {
        set_icons();
        init_container();
    }

    ObjectDataViewModelNode(ObjectDataViewModelNode* parent,
                            const QString& sub_obj_name,
                            Slic3r::ModelVolumeType type,
                            const bool is_svg_volume,
                            const QPixmap& bmp,
                            const QString& extruder,
                            const int idx = -1,
                            const std::string& warning_icon_name = {});

    ObjectDataViewModelNode(ObjectDataViewModelNode* parent,
                            const t_layer_height_range& layer_range,
                            const int idx = -1,
                            const QString extruder = {});

    ObjectDataViewModelNode(PartPlate* part_plate, QString name);

    ObjectDataViewModelNode(ObjectDataViewModelNode* parent, const ItemType type, const int plate_idx = -1);
    ObjectDataViewModelNode(ObjectDataViewModelNode* parent, const InfoItemType type);

    ~ObjectDataViewModelNode()
    {
        for (auto* child : m_children)
            delete child;
#ifndef NDEBUG
        m_idx = -2;
#endif
    }

    void     init_container();
    bool     IsContainer() const { return m_container; }

    ObjectDataViewModelNode* GetParent()
    {
        assert(m_parent == nullptr || m_parent->valid());
        return m_parent;
    }
    std::vector<ObjectDataViewModelNode*>& GetChildren() { return m_children; }
    ObjectDataViewModelNode*  GetNthChild(unsigned int n) { return m_children[n]; }
    int  GetChildIndex(ObjectDataViewModelNode* child) const {
        for (int i = 0; i < (int)m_children.size(); ++i)
            if (m_children[i] == child) return i;
        return -1;
    }

    void Insert(ObjectDataViewModelNode* child, unsigned int n) {
        if (!m_container) m_container = true;
        m_children.insert(m_children.begin() + n, child);
    }
    void Append(ObjectDataViewModelNode* child) {
        if (!m_container) m_container = true;
        m_children.push_back(child);
    }
    void RemoveAllChildren() {
        for (int id = (int)m_children.size() - 1; id >= 0; --id) {
            if (!m_children[id]->m_children.empty())
                m_children[id]->RemoveAllChildren();
            delete m_children[id];
        }
        m_children.clear();
    }
    size_t GetChildCount() const { return m_children.size(); }

    void            SetName(const QString &name) { m_name = name; }
    bool            SetValue(const QVariant &variant, unsigned int col);
    void            SetVolumeType(ModelVolumeType type) { m_volume_type = type; }
    void            SetBitmap(const QPixmap &icon) { m_bmp = const_cast<QPixmap*>(&icon); }
    void            SetExtruder(const QString &extruder) { m_extruder = extruder; }
    void            SetWarningIconName(const std::string &w) { m_warning_icon_name = w; }
    void            SetWarningBitmap(const QPixmap& icon, const std::string& w);
    void            SetLock(bool has_lock) { m_has_lock = has_lock; }
    const QPixmap&  GetBitmap() const               { return *m_bmp; }
    const QString&  GetName() const                 { return m_name; }
    ItemType        GetType() const                 { return m_type; }
    InfoItemType    GetInfoItemType() const         { return m_info_item_type; }
    void            SetIdx(const int& idx);
    int             GetIdx() const                  { return m_idx; }
    void            SetPlateIdx(const int& idx);
    int             GetPlateIdx() const             { return m_plate_idx; }
    ModelVolumeType GetVolumeType()                 { return m_volume_type; }
    t_layer_height_range GetLayerRange() const      { return m_layer_range; }
    QString         GetExtruder()                   { return m_extruder; }
    PrintIndicator  IsPrintable() const             { return m_printable; }
    VaryHeightIndicator IsVaribaleHeight() const    { return m_variable_height; }
    bool            HasColorPainting() const        { return m_color_enable; }
    bool            HasSupportPainting() const      { return m_support_enable; }
    bool            HasFuzzySkinPainting() const    { return m_fuzzyskin_enable; }
    bool            HasSinking() const              { return m_sink_enable; }
    bool            IsActionEnabled() const         { return m_action_enable; }
    void            UpdateExtruderAndColorIcon(const QString &extruder = {});

    void AssignAllVal(ObjectDataViewModelNode& from_node) {
        m_name     = from_node.m_name;
        m_bmp      = from_node.m_bmp;
        m_idx      = from_node.m_idx;
        m_extruder = from_node.m_extruder;
        m_type     = from_node.m_type;
    }

    bool SwapChildrens(int frst_id, int scnd_id) {
        if ((int)m_children.size() < 2 ||
            frst_id < 0 || frst_id >= (int)m_children.size() ||
            scnd_id < 0 || scnd_id >= (int)m_children.size())
            return false;
        ObjectDataViewModelNode new_scnd = *m_children[frst_id];
        ObjectDataViewModelNode new_frst = *m_children[scnd_id];
        new_scnd.m_idx = m_children[scnd_id]->m_idx;
        new_frst.m_idx = m_children[frst_id]->m_idx;
        m_children[frst_id]->AssignAllVal(new_frst);
        m_children[scnd_id]->AssignAllVal(new_scnd);
        return true;
    }

    void        set_icons();
    void        set_extruder_icon();
    void        set_printable_icon(PrintIndicator printable);
    void        set_variable_height_icon(VaryHeightIndicator vari_height);
    void        set_action_icon(bool enable);
    void        set_color_icon(bool enable, bool force = false);
    void        set_support_icon(bool enable, bool force = false);
    void        set_fuzzyskin_icon(bool enable, bool force = false);
    void        set_sinking_icon(bool enable, bool force = false);
    void        set_warning_icon(const std::string& warning_icon);
    void        update_settings_digest_bitmaps();
    bool        update_settings_digest(const std::vector<std::string>& categories);
    int         volume_type() const { return int(m_volume_type); }
    bool        is_text_volume() const { return m_is_text_volume; }
    bool        is_svg_volume() const { return m_is_svg_volume; }
    void        msw_rescale();

#ifndef NDEBUG
    bool        valid();
#endif
    bool        invalid() const { return m_idx < -1; }
    bool        has_warning_icon() const { return !m_warning_icon_name.empty(); }
    std::string warning_icon_name() const { return m_warning_icon_name; }
    bool        has_lock() const { return m_has_lock; }
    void        sys_color_changed();

private:
    friend class ObjectDataViewModel;
};

// ----------------------------------------------------------------------------
// ObjectDataViewModel
// ----------------------------------------------------------------------------

class ObjectDataViewModel : public QAbstractItemModel
{
    Q_OBJECT

    std::vector<ObjectDataViewModelNode*>               m_plates;
    std::vector<ObjectDataViewModelNode*>               m_objects;
    std::vector<QPixmap>                                m_volume_bmps;
    std::vector<QPixmap>                                m_text_volume_bmps;
    std::vector<QPixmap>                                m_svg_volume_bmps;
    std::map<InfoItemType, QPixmap>                     m_info_bmps;
    QPixmap                                             m_empty_bmp;
    QPixmap                                             m_warning_bmp;
    QPixmap                                             m_warning_manifold_bmp;
    QPixmap                                             m_lock_bmp;

    ObjectDataViewModelNode*                            m_plate_outside{nullptr};

    QTreeView*                                          m_ctrl{nullptr};
    std::vector<std::tuple<ObjectDataViewModelNode*, QString, QString>> assembly_name_list;
    std::vector<std::tuple<ObjectDataViewModelNode*, QString, QString>> search_found_list;
    std::map<int, std::map<int, int>>                   m_ui_and_3d_volume_maps;

public:
    ObjectDataViewModel();
    ~ObjectDataViewModel();

    void Init();
    std::map<int, std::map<int, int>>& get_ui_and_3d_volume_map() { return m_ui_and_3d_volume_maps; }

    int get_real_volume_index_in_3d(int ui_object_value, int ui_volume_value)
    {
        auto it = m_ui_and_3d_volume_maps.find(ui_object_value);
        if (it != m_ui_and_3d_volume_maps.end()) {
            auto& cur = it->second;
            auto jt = cur.find(ui_volume_value);
            if (jt != cur.end()) return jt->second;
        }
        return ui_volume_value;
    }
    int get_real_volume_index_in_ui(int ui_object_value, int _3d_value)
    {
        auto it = m_ui_and_3d_volume_maps.find(ui_object_value);
        if (it != m_ui_and_3d_volume_maps.end()) {
            for (auto& p : it->second)
                if (p.second == _3d_value) return p.first;
        }
        return _3d_value;
    }

    QModelIndex AddPlate(PartPlate* part_plate, const QString& name = {}, bool refresh = true);
    QModelIndex AddObject(ModelObject* model_object, const std::string& warning_bitmap, bool has_lock = false, bool refresh = true);
    QModelIndex AddVolumeChild(const QModelIndex &parent_item,
                               const QString &name,
                               const Slic3r::ModelVolumeType volume_type,
                               const bool is_svg_volume,
                               const std::string& warning_icon_name = {},
                               const int extruder = 0,
                               const bool create_frst_child = true);
    QModelIndex AddSettingsChild(const QModelIndex &parent_item);
    QModelIndex AddInfoChild(const QModelIndex &parent_item, InfoItemType info_type);
    QModelIndex AddInstanceChild(const QModelIndex &parent_item, size_t num);
    QModelIndex AddInstanceChild(const QModelIndex &parent_item, const std::vector<bool>& print_indicator, const std::vector<int>& plate_indicator);
    QModelIndex AddLayersRoot(const QModelIndex &parent_item);
    QModelIndex AddLayersChild(const QModelIndex &parent_item,
                               const t_layer_height_range& layer_range,
                               const int extruder = 0,
                               const int index = -1);
    size_t         GetItemIndexForFirstVolume(ObjectDataViewModelNode* node_parent);
    QModelIndex    DeletePlate(const int plate_idx);
    QModelIndex    Delete(const QModelIndex &item);
    QModelIndex    DeleteLastInstance(const QModelIndex &parent_item, size_t num);
    void ResetAll();
    void DeleteChildren(QModelIndex& parent);
    void DeleteVolumeChildren(QModelIndex& parent);
    void DeleteSettings(const QModelIndex& parent);
    QModelIndex GetItemByPlateId(int plate_idx);
    void        SetCurSelectedPlateFullName(int plate_idx, const std::string&);
    QModelIndex GetItemById(int obj_idx);
    QModelIndex GetItemById(const int obj_idx, const int sub_obj_idx, const ItemType parent_type);
    QModelIndex GetItemByVolumeId(int obj_idx, int volume_idx);
    QModelIndex GetItemByInstanceId(int obj_idx, int inst_idx);
    QModelIndex GetItemByLayerId(int obj_idx, int layer_idx);
    QModelIndex GetItemByLayerRange(const int obj_idx, const t_layer_height_range& layer_range);
    int  GetItemIdByLayerRange(const int obj_idx, const t_layer_height_range& layer_range);
    int  GetIdByItem(const QModelIndex& item) const;
    int  GetPlateIdByItem(const QModelIndex& item) const;
    int  GetIdByItemAndType(const QModelIndex& item, const ItemType type) const;
    int  GetObjectIdByItem(const QModelIndex& item) const;
    int  GetVolumeIdByItem(const QModelIndex& item) const;
    int  GetInstanceIdByItem(const QModelIndex& item) const;
    int  GetLayerIdByItem(const QModelIndex& item) const;
    void GetItemInfo(const QModelIndex& item, ItemType& type, int& obj_idx, int& idx);
    int  GetRowByItem(const QModelIndex& item) const;
    bool IsEmpty() { return m_objects.empty(); }
    bool InvalidItem(const QModelIndex& item);

    QString     GetName(const QModelIndex &item) const;
    QPixmap&    GetBitmap(const QModelIndex &item) const;
    QString     GetExtruder(const QModelIndex &item) const;
    int         GetExtruderNumber(const QModelIndex &item) const;

    // QAbstractItemModel overrides
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& child) const override;
    int         rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int         columnCount(const QModelIndex& parent = QModelIndex()) const override { return colCount; }
    QVariant    data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool        setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    bool        hasChildren(const QModelIndex& parent = QModelIndex()) const override;

    void SetExtruder(const QString& extruder, QModelIndex item);
    void OnPlateChange(const int plate_idx, QModelIndex item);
    void SetPlateIdx(const int plate_idx, QModelIndex item);
    bool SetName(const QString& new_name, QModelIndex item);

    QModelIndex ReorganizeChildren(const int cur_volume_id,
                                   const int new_volume_id,
                                   const QModelIndex &parent);
    QModelIndex ReorganizeObjects(int current_id, int new_id);

    QModelIndex GetParentItem(const QModelIndex &item) const;
    QModelIndex GetObject(const QModelIndex& item) const;
    QModelIndex GetTopParent(const QModelIndex &item) const;
    bool        IsContainer(const QModelIndex &item) const;
    QModelIndexList GetAllChildren(const QModelIndex &parent) const;
    bool        HasInfoItem(InfoItemType type) const;

    ItemType        GetItemType(const QModelIndex &item) const;
    ItemType        GetItemType(const QModelIndex &item, int& plate_idx) const;
    InfoItemType    GetInfoItemType(const QModelIndex &item) const;
    QModelIndex     GetItemByType(const QModelIndex &parent_item, ItemType type) const;
    QModelIndex     GetSettingsItem(const QModelIndex &item) const;
    QModelIndex     GetInstanceRootItem(const QModelIndex &item) const;
    QModelIndex     GetLayerRootItem(const QModelIndex &item) const;
    QModelIndex     GetInfoItemByType(const QModelIndex &parent_item, InfoItemType type) const;
    QModelIndex     GetObjectItem(const ModelObject* mo) const;
    QModelIndex     GetVolumeItem(const QModelIndex& parent, int vol_idx) const;
    bool    IsSettingsItem(const QModelIndex &item) const;
    void    UpdateSettingsDigest(const QModelIndex &item,
                                 const std::vector<std::string>& categories);

    bool    IsPrintable(const QModelIndex &item) const;
    void    UpdateObjectPrintable(QModelIndex parent_item);
    void    UpdateInstancesPrintable(QModelIndex parent_item);
    bool    IsVariableHeight(const QModelIndex& item) const;

    void        SetVolumeType(const QModelIndex &item, const Slic3r::ModelVolumeType type);
    ModelVolumeType GetVolumeType(const QModelIndex &item);
    QModelIndex SetPrintableState(PrintIndicator printable, int obj_idx,
                                  int subobj_idx = -1,
                                  ItemType subobj_type = itInstance);
    QModelIndex SetObjectPrintableState(PrintIndicator printable, QModelIndex obj_item);
    QModelIndex SetObjectVariableHeightState(VaryHeightIndicator vari_height, QModelIndex obj_item);
    bool    IsColorPainted(QModelIndex& item) const;
    bool    IsSupportPainted(QModelIndex &item) const;
    bool    IsFuzzySkinPainted(QModelIndex &item) const;
    bool    IsSinked(QModelIndex &item) const;
    void    SetColorPaintState(const bool painted, QModelIndex obj_item, bool force = false);
    void    SetSupportPaintState(const bool painted, QModelIndex obj_item, bool force = false);
    void    SetFuzzySkinPaintState(const bool painted, QModelIndex obj_item, bool force = false);
    void    SetSinkState(const bool painted, QModelIndex obj_item, bool force = false);

    void SetAssociatedControl(QTreeView* ctrl) { m_ctrl = ctrl; }
    void Rescale();

    QPixmap     GetVolumeIcon(const Slic3r::ModelVolumeType vol_type,
                              const std::string& warning_icon_name = {});
    void        AddWarningIcon(const QModelIndex& item, const std::string& warning_name);
    void        DeleteWarningIcon(const QModelIndex& item, const bool unmark_object = false);
    void        UpdateWarningIcon(const QModelIndex& item, const std::string& warning_name);
    void        UpdateCutObjectIcon(const QModelIndex &item, bool has_cut_icon);
    bool        HasWarningIcon(const QModelIndex& item) const;
    t_layer_height_range GetLayerRangeByItem(const QModelIndex& item) const;

    bool    UpdateColumValues(unsigned col);
    void    UpdateExtruderBitmap(QModelIndex item);
    void    UpdateVolumesExtruderBitmap(QModelIndex object_item, bool use_obj_extruder = false);
    int     GetDefaultExtruderIdx(QModelIndex item);

    void    UpdateItemNames();

    void    assembly_name(ObjectDataViewModelNode* item, const QString& name);
    void    assembly_name();
    std::vector<std::tuple<ObjectDataViewModelNode*, QString, QString>> get_assembly_name_list() const { return assembly_name_list; }
    void    search_object(const QString& search_text);
    std::vector<std::tuple<ObjectDataViewModelNode*, QString, QString>> get_found_list() const { return search_found_list; }

    void    sys_color_changed();

private:
    QModelIndex AddRoot(const QModelIndex& parent_item, const ItemType root_type);
    QModelIndex AddInstanceRoot(const QModelIndex& parent_item);
    void        AddAllChildren(const QModelIndex& parent);

    QPixmap&    GetWarningBitmap(const std::string& warning_icon_name);
    void        ReparentObject(ObjectDataViewModelNode* plate, ObjectDataViewModelNode* object);
    QModelIndex AddOutsidePlate(bool refresh = true);

    void UpdateBitmapForNode(ObjectDataViewModelNode *node);
    void UpdateBitmapForNode(ObjectDataViewModelNode *node, const std::string &warning_icon_name, bool has_lock);

    // Helper: get node pointer from QModelIndex
    static ObjectDataViewModelNode* nodeFromIndex(const QModelIndex& idx);
};

} // namespace GUI
} // namespace Slic3r

#endif // slic3r_GUI_ObjectDataViewModel_hpp_
