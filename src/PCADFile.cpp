
#include "PCADFile.h"
#include "PCADViewDraw.h"
#include "DrawGraphicalEntities.h"
#include "ApertureProvider.h"

using namespace aperture;
using namespace DrawEntities;
using namespace std;

const LayerDesc LayerDesc::LAYER_DESC_INVALID{.layer_name = {}, .layer_number = -1, .layer_color = -1, .layer_wx_color = {}};

static const GraphObj* ScanForGraphObject(uint32_t graph_object_ordinal, const std::vector<GraphObj*>& graph_objects)
{
    for (GraphObj* graph_obj_ptr : graph_objects)
        if (graph_obj_ptr->GetGraphObjectOrdinal() == graph_object_ordinal)
            return graph_obj_ptr;
    return nullptr;
}

static const GraphObj* ScanForGraphObject(wxColor graph_obj_ord_as_color, const std::vector<GraphObj*>& graph_objects)
{
    for (GraphObj* graph_obj_ptr : graph_objects)
        if (StdWXObjects::CompareColorLong(graph_obj_ord_as_color, graph_obj_ptr->GetGraphObjectOrdinal()))
            return graph_obj_ptr;
    return nullptr;
}

void ComponentSectDef::Clear() noexcept
{
    ComponentSectDefVar* sect_def_var = static_cast<ComponentSectDefVar*>(this);
    if (holds_alternative<ComponentPKGSectDef>(*sect_def_var))
        get<ComponentPKGSectDef>(*sect_def_var).Clear();
    else if (holds_alternative<ComponentSPKGSectDef>(*sect_def_var))
        get<ComponentSPKGSectDef>(*sect_def_var).Clear();

    *sect_def_var = monostate;
}

// Подсчёт полного количества секций в компоненте.
int ComponentSectDef::GetCount() const noexcept
{
    ComponentSectDefVar* sect_def_var = static_cast<ComponentSectDefVar*>(this);
    if (holds_alternative<ComponentPKGSectDef>(*sect_def_var))
    {
        const ComponentPKGSectDef& pkg_sect_def = get<ComponentPKGSectDef>(*sect_def_var);
        return static_cast<int>(pkg_sect_def.pin_pkg_data.size());
    }
    else if (holds_alternative<ComponentSPKGSectDef>(*sect_def_var))
    {
        const ComponentSPKGSectDef& spkg_sect_def = get<ComponentSPKGSectDef>(*sect_def_var);
        size_t sect_count = 0;
        for (const SectNameToPackInfo& group_pin_pkg_data : spkg_sect_def.sect_spkg_data)
            sect_count += group_pin_pkg_data.size();

        return static_cast<int>(sect_count);
    }
    else
    {
        return -1;
    }
}

// Возвращает "ИСТИНУ", если объект содержит состоятельную информацию о секциях компонента.
bool ComponentSectDef::IsValid() const noexcept
{
    ComponentSectDefVar* sect_def_var = static_cast<ComponentSectDefVar*>(this);
    return !holds_alternative<monostate_t>(*sect_def_var);
}

// Тип хранящейся в данной структуре информации.
bool ComponentSectDef::IsSPKG() const noexcept
{
    ComponentSectDefVar* sect_def_var = static_cast<ComponentSectDefVar*>(this);
    return holds_alternative<ComponentSPKGSectDef>(*sect_def_var);
}

// Возврат ссылки на поле типа ComponentPKGSectDef, если оно содержится в структуре.
const ComponentPKGSectDef& ComponentSectDef::GetPKGSectDef() const
{
    ComponentSectDefVar* sect_def_var = static_cast<ComponentSectDefVar*>(this);
    if (holds_alternative<ComponentPKGSectDef>(*sect_def_var))
        return get<ComponentPKGSectDef>(*sect_def_var);
    else
        throw runtime_error("Секционирование не соответствует модели ComponentPKGSectDef");
}

// Возврат ссылки на поле типа ComponentSPKGSectDef, если оно содержится в структуре.
const ComponentSPKGSectDef& ComponentSectDef::GetSPKGSectDef() const
{
    ComponentSectDefVar* sect_def_var = static_cast<ComponentSectDefVar*>(this);
    if (holds_alternative<ComponentSPKGSectDef>(*sect_def_var))
        return get<ComponentSPKGSectDef>(*sect_def_var);
    else
        throw runtime_error("Секционирование не соответствует модели ComponentSPKGSectDef");
}

void RadioComponentDesc::Clear() noexcept
{
    for (GraphObj* graph_obj_ptr : graph_objects_)
        delete graph_obj_ptr;
    (*this) = {};
}

RadioComponentDesc::RadioComponentDesc(SourceData&& radio_component_data) :
    ExAttrsCollection(move(radio_component_data.ex_attr_collection)),
    comp_name_(move(radio_component_data.comp_name)),
    graph_objects_(move(radio_component_data.graph_objects)),
    pins_(move(radio_component_data.pins)),
    package_id_(move(radio_component_data.package_id)),
    refdes_(move(radio_component_data.refdes)),
    sections_def_(move(radio_component_data.sections_def)),
    // Переносим внутренние атрибуты.
    org_pos_(radio_component_data.org_pos),
    ty_id_(radio_component_data.ty_id),
    is_smd_(radio_component_data.is_smd),
    is_jumper_(radio_component_data.is_jumper)
{}

RadioComponentDesc::RadioComponentDesc(RadioComponentDesc&& other) noexcept :
    ExAttrsCollection(move(other)),
    comp_name_(move(other.comp_name_)),
    graph_objects_(move(other.graph_objects_)),
    pins_(move(other.pins_)),
    package_id_(move(other.package_id_)),
    refdes_(move(other.refdes_)),
    sections_def_(move(other.sections_def_)),
    // Переносим внутренние атрибуты.
    org_pos_(other.org_pos_),
    ty_id_(other.ty_id_),
    is_smd_(other.is_smd_),
    is_jumper_(other.is_jumper_)
{
    other.org_pos_ = wxPoint();
    other.ty_id_ = 0;
    other.is_smd_ = false;
    other.is_jumper_ = false;
}

RadioComponentDesc::~RadioComponentDesc() noexcept
{
    Clear();
}

RadioComponentDesc& RadioComponentDesc::operator=(RadioComponentDesc&& other) noexcept
{
    if (this != &other)
    {
        Clear();
        *static_cast<ExAttrsCollection*>(this) = move(other);
        comp_name_ = move(other.comp_name_);
        graph_objects_ = move(other.graph_objects_);
        pins_ = move(other.pins_);
        package_id_ = move(other.package_id_);
        refdes_ = move(other.refdes_);
        sections_def_ = move(other.sections_def_);
        // Переносим внутренние атрибуты.
        org_pos_ = other.org_pos_;
        other.org_pos_ = wxPoint();
        ty_id_ = other.ty_id_;
        other.ty_id_ = 0;
        is_smd_ = other.is_smd_;
        other.is_smd_ = false;
        is_jumper_ = other.is_jumper_;
        other.is_jumper_ = false;
    }
    return *this;
}

// Постобработка компонента - вызвается после полного первоначального заполнения всех исходных данных PDIF-базы.
void RadioComponentDesc::ComponentPostProcess(const PCADFile* pcad_document)
{
    pcad_document_ = pcad_document;

}

// Функция возвращает индекс вывода с именем pin_name.
size_t RadioComponentDesc::FindPinByName(const string& pin_name) const
{
    for (auto pin_it = pins_.begin(); pin_it != pins_.end(); ++pin_it)
        // pin_it - итератор, указывающий на переменную типа ComponentPinDef.
        if (pin_it->pin_name == pin_name)
            return pin_it - pins_.begin();
    // Вывод с именем pin_name не обнаружен.
    return string::npos;
}

// Функция возвращает индекс ножки с "алфавитно-цифровым номером" pin_al_number.
size_t RadioComponentDesc::FindPinByAlNumber(const string& pin_al_number) const
{
    for (auto pin_it = pins_.begin(); pin_it != pins_.end(); ++pin_it)
        // pin_it - итератор, указывающий на переменную типа ComponentPinDef.
        if (pin_it->pin_al_number == pin_al_number)
            return pin_it - pins_.begin();
    // Вывод с именем pin_name не обнаружен.
    return string::npos;
}

const GraphObj* RadioComponentDesc::ScanForGraphObject(uint32_t graph_object_ordinal) const
{
    return ::ScanForGraphObject(graph_object_ordinal, graph_objects_);
}

const GraphObj* RadioComponentDesc::ScanForGraphObject(wxColor graph_obj_ord_as_color) const
{
    return ::ScanForGraphObject(graph_obj_ord_as_color, graph_objects_);
}

// Функция-член общей очистки данных структуры RadioComponentInsertion.
void RadioComponentInsertion::Clear() noexcept
{
    delete refdes_obj_;
    refdes_obj_ = nullptr;
    for (GraphObj* graph_obj_ptr : pin_labels_)
        delete graph_obj_ptr;
    (*this) = {};
}

// Функция-член постобработки записи о вставке экземпляра радиокомпонента, производимой после полного первоначального заполнения
// всех массивов общего PDIF-документа.
void RadioComponentInsertion::InsertPostProcess(const PCADFile* pcad_document)
{
    pcad_document_ = pcad_document;

}

RadioComponentInsertion::RadioComponentInsertion(SourceData&& component_insert_data) :
    ExAttrsCollection(move(component_insert_data.ex_attr_collection)),
    comp_name_(move(component_insert_data.comp_name)),
    insertion_name_(move(component_insert_data.insertion_name)),
    refdes_obj_(move(component_insert_data.refdes_obj)),
    pin_labels_(move(component_insert_data.pin_labels)),
    pin_type_info_(move(component_insert_data.pin_type_info)),
    // Перенос внутренних атрибутов вставки.
    is_mirror_(component_insert_data.is_mirror),
    on_top_side_(component_insert_data.on_top_side),
    ins_name_pos_(component_insert_data.ins_name_pos),
    is_user_ins_name_(component_insert_data.is_user_ins_name),
    place_pos_(component_insert_data.place_pos),
    scale_x_(component_insert_data.scale_x),
    scale_y_(component_insert_data.scale_y),
    rotate_factor_(component_insert_data.rotate_factor),
    set_angle_(component_insert_data.set_angle)
{
    for (const std::pair<std::string, std::string>& conn_info_pair : component_insert_data.connect_info)
    {
        PinNetConnectInfo new_pin_net_conn{.pin_name = conn_info_pair.first, .net_name = conn_info_pair.second};
        connect_info_.push_back(move(new_pin_net_conn));
    }
    component_insert_data.connect_info.clear();
}

RadioComponentInsertion::RadioComponentInsertion(RadioComponentInsertion&& other) noexcept :
    ExAttrsCollection(move(other)),
    comp_name_(move(other.comp_name_)),
    insertion_name_(move(other.insertion_name_)),
    comp_number_(other.comp_number_),
    refdes_obj_(other.refdes_obj_),
    pin_labels_(move(other.pin_labels_)),
    connect_info_(move(other.connect_info_)),
    pin_type_info_(move(other.pin_type_info_)),
    // Перенос внутренних атрибутов вставки.
    is_mirror_(other.is_mirror_),
    on_top_side_(other.on_top_side_),
    ins_name_pos_(other.ins_name_pos_),
    is_user_ins_name_(other.is_user_ins_name_),
    place_pos_(other.place_pos_),
    scale_x_(other.scale_x_),
    scale_y_(other.scale_y_),
    rotate_factor_(other.rotate_factor_),
    set_angle_(other.set_angle_)
{
    other.comp_number_ = -1;
    other.refdes_obj_ = nullptr;
    other.is_mirror_ = false;
    other.on_top_side_ = true;
    other.is_user_ins_name_ = false;
    other.place_pos_ = wxPoint();
    other.scale_x_ = 1.0;
    other.scale_y_ = 1.0;
    other.rotate_factor_ = 0;
    other.ins_name_pos_ = {};
    other.set_angle_ = 0.0;
}

RadioComponentInsertion::~RadioComponentInsertion() noexcept
{
    Clear();
}

RadioComponentInsertion& RadioComponentInsertion::operator=(RadioComponentInsertion&& other) noexcept
{
    if (this != &other)
    {
        Clear();
        *static_cast<ExAttrsCollection*>(this) = move(other);
        comp_name_ = move(other.comp_name_);
        comp_number_ = other.comp_number_;
        other.comp_number_ = -1;
        refdes_obj_ = other.refdes_obj_;
        other.refdes_obj_ = nullptr;
        pin_labels_ = move(other.pin_labels_);
        connect_info_ = move(other.connect_info_);
        pin_type_info_ = move(other.pin_type_info_);
        // Перенос внутренних атрибутов вставки.
        is_mirror_ = other.is_mirror_;
        other.is_mirror_ = false;
        on_top_side_ = other.on_top_side_;
        other.on_top_side_ = true;
        is_user_ins_name_ = other.is_user_ins_name_;
        other.is_user_ins_name_ = false;
        place_pos_ = other.place_pos_;
        other.place_pos_ = wxPoint();
        scale_x_ = other.scale_x_;
        other.scale_x_ = 1.0;
        scale_y_ = other.scale_y_;
        other.scale_y_ = 1.0;
        rotate_factor_ = other.rotate_factor_;
        other.rotate_factor_ = 0;
        ins_name_pos_ = other.ins_name_pos_;
        other.ins_name_pos_ = {};
        set_angle_ = other.set_angle_;
        other.set_angle_ = 0.0;
    }
    return *this;
}

// Функция-член полной очистки объекта класса NetDefDesc.
void NetDefDesc::Clear() noexcept
{
    for (GraphObj* graph_obj_ptr : net_parts_)
        delete graph_obj_ptr;
    (*this) = {};
}

// Постобработка цепи - вызвается после полного первоначального заполнения всех исходных данных PDIF-базы.
void NetDefDesc::NetPostProcess(const PCADFile* pcad_document)
{
    pcad_document_ = pcad_document;

}

NetDefDesc::NetDefDesc(SourceData&& net_def_data) :
    ExAttrsCollection(move(net_def_data.ex_attr_collection)),
    net_name_(move(net_def_data.net_name)),
    net_parts_(move(net_def_data.net_parts)),
    // Внутренние атрибуты токопроводящей цепи.
    is_user_net_name_(net_def_data.is_user_net_name)
{}

NetDefDesc::NetDefDesc(NetDefDesc&& other) noexcept :
    ExAttrsCollection(move(other)),
    net_name_(move(other.net_name_)),
    net_parts_(move(other.net_parts_)),
    // Внутренние атрибуты токопроводящей цепи.
    is_user_net_name_(other.is_user_net_name_)
{
    other.is_user_net_name_ = false;
}

NetDefDesc::~NetDefDesc() noexcept
{
    Clear();
}

NetDefDesc& NetDefDesc::operator=(NetDefDesc&& other) noexcept
{
    if (this != &other)
    {
        Clear();
        *static_cast<ExAttrsCollection*>(this) = move(other);
        net_name_ = move(other.net_name_);
        net_parts_ = move(other.net_parts_);
        is_user_net_name_ = other.is_user_net_name_;
        other.is_user_net_name_ = false;
    }
    return *this;
}

const GraphObj* NetDefDesc::ScanForGraphObject(uint32_t graph_object_ordinal) const
{
    return ::ScanForGraphObject(graph_object_ordinal, net_parts_);
}

const GraphObj* NetDefDesc::ScanForGraphObject(wxColor graph_obj_ord_as_color) const
{
    return ::ScanForGraphObject(graph_obj_ord_as_color, net_parts_);
}

void PCADFile::DrawFile(DrawContext& draw_context, const CanvasContext& canvas_context, SelectContourData& select_contour)
{
    draw_context.pcad_doc_ptr = this;
    // Расчёт действительных масштабов по осям X и Y, исходя из размера текущей исходной
    // области (проецируемого прямоугольника в пространстве исходного изображения) и размера
    // целевой поверхности рисования.
    wxSize target_dc_size = draw_context.target_dc_ptr->GetSize();
    double scale_x = static_cast<double>(target_dc_size.GetWidth()) /
                     draw_context.project_rect.GetWidth();
    double scale_y = static_cast<double>(target_dc_size.GetHeight()) /
                     draw_context.project_rect.GetHeight();
    draw_context.scale_x = scale_x;
    draw_context.scale_y = scale_y;
    draw_context.is_use_selected = true;

    if (draw_context.recognize_dc.IsOk())
    { // Очищаем индикаторную поверхность путём заполнения её нулевым цветом
        wxColor zero_color = StdWXObjects::FromLong(0);
        draw_context.background_brush_recognize = wxBrush(zero_color);
        draw_context.recognize_dc.SetBackground(draw_context.background_brush_recognize);
        draw_context.recognize_dc.SetTextBackground(zero_color);
        draw_context.recognize_dc.Clear();
    }
    // Очищаем целевую поверхность, заполняя её фоновым цветом
    wxPen temp_pen(draw_context.background_color);
    wxBrush temp_brush(draw_context.background_color);
    draw_context.target_dc_ptr->SetPen(temp_pen);
    draw_context.target_dc_ptr->SetBrush(temp_brush);
    draw_context.target_dc_ptr->DrawRectangle(0, 0, target_dc_size.GetWidth(), target_dc_size.GetHeight());
    draw_context.target_dc_ptr->SetPen(wxNullPen);
    draw_context.target_dc_ptr->SetBrush(wxNullBrush);

    const wxRect clipping_copy(draw_context.clipping_rect);
    if (canvas_context.is_canvas)
    { // Если холст включён, выводим его на целевую поверхность
        // src_canvas_rect - очертание холста на исходной поверхности PCAD-рисунка
        wxSize src_canvas_size(canvas_context.canvas_size.GetWidth() / draw_context.scale_x,
                               canvas_context.canvas_size.GetHeight() / draw_context.scale_y);
        wxRect src_canvas_rect(canvas_context.canvas_position, src_canvas_size);
        // dev_canvas_rect - очертание холста на поверхности его текущего масштабного изображения
        wxPoint dev_canvas_position = draw_context.ConvertPointToDevice(canvas_context.canvas_position);
        wxRect dev_canvas_rect(dev_canvas_position, canvas_context.canvas_size);

        if (clipping_copy.IsEmpty() || !clipping_copy.Intersect(src_canvas_rect).IsEmpty())
        {
            temp_pen = wxPen(canvas_context.canvas_color);
            temp_brush = wxBrush(canvas_context.canvas_color);
            draw_context.target_dc_ptr->SetPen(temp_pen);
            draw_context.target_dc_ptr->SetBrush(temp_brush);
            draw_context.target_dc_ptr->DrawRectangle(dev_canvas_rect);
            draw_context.target_dc_ptr->SetPen(wxNullPen);
            draw_context.target_dc_ptr->SetBrush(wxNullBrush);
        }
    }

    const wxRect project_copy(draw_context.project_rect);
    for (GraphObj* obj_ptr : graph_objects_)
    {
        // Проверим состояние слоя, на котором расположен примитив.
        const wxRect obj_frame = obj_ptr->GetFrameRect();
        const int lay_num = obj_ptr->GetLayerNumber();
        if (lay_num >= 0)
            if (lay_num >= static_cast<int>(layers_.size()) ||
                GetLayerAttributes(lay_num).layer_on_avl_act == LAYER_ON_AVL_ACT_OFF)
                continue;
        // Проверим нахождение примитива (хотя бы частично) в пределах прямоугольника проекции.
        if (project_copy.Intersect(obj_frame).IsEmpty())
            continue;
        // Проверим нахождение примитива (хотя бы частично) в пределах прямоугольника отсечения, если он установлен.
        if (!clipping_copy.IsEmpty() && clipping_copy.Intersect(obj_frame).IsEmpty())
            continue;
        obj_ptr->DrawObject(draw_context);
    }

    // Выводим на целевую поверхность также выделяющий контур. Если, конечно же, он существует
    if (select_contour.is_contour_builded)
        if (clipping_copy.IsEmpty() || !clipping_copy.Intersect(select_contour.contour_rect).IsEmpty())
            DrawSelectingContour(draw_context, select_contour);
}

void PCADFile::ShiftDocument(int shift_direction_x, int shift_direction_y)
{
    for (GraphObj* obj_ptr : graph_objects_)
        obj_ptr->ShiftObject(shift_direction_x, shift_direction_y);
    file_values_.frame_rect.SetX(file_values_.frame_rect.GetX() + shift_direction_x);
    file_values_.frame_rect.SetY(file_values_.frame_rect.GetY() + shift_direction_y);
}

PCADFile::PCADFile(ApertureProvider& aperture_provider) :
    aperture_provider_(aperture_provider)
{
    file_values_.file_signature = wxT("Не загружен");
    file_values_.file_workshop = nullptr;
    file_values_.file_flags = 0;
    file_values_.DBU_in_measure_unit = 100;
    file_values_.frame_rect = wxRect(0, 0, 10000, 10000);
}

// Упрощённый конструктор PCAD-документа, принимающий только прямую графическую информацию.
PCADFile::PCADFile(vector<GraphObj*> graph_objects, vector<LayerDesc> layers,
                   ApertureProvider& aperture_provider, FileDefValues file_values) :
    graph_objects_(move(graph_objects)), layers_(move(layers)),
    file_values_(file_values), aperture_provider_(aperture_provider)
{
    InitLayersColor();
}

void PCADFile::PCADFileClear()
{
    for (GraphObj* graph_obj_ptr : graph_objects_)
        delete graph_obj_ptr;
    graph_objects_.clear();
    layers_.clear();
    cross_layers_pinholes_.clear();
    radio_components_.clear();
    radio_comp_inserts_.clear();
    nets_.clear();
}

PCADFile::PCADFile(PCADFile&& other) :
    graph_objects_(move(other.graph_objects_)),
    layers_(move(other.layers_)),
    cross_layers_pinholes_(move(other.cross_layers_pinholes_)),
    radio_components_(move(other.radio_components_)),
    radio_comp_inserts_(move(other.radio_comp_inserts_)),
    nets_(move(other.nets_)),
    file_values_(move(other.file_values_)),
    aperture_provider_(other.aperture_provider_)
{
    other.graph_objects_.clear();
}

void PCADFile::InitLayersColor()
{
    StdWXObjects std_wx_objects;

    for (LayerDesc& current_layer : layers_)
    {
        if (current_layer.layer_color >= 0 &&
            current_layer.layer_color < static_cast<int>(std_wx_objects.std_colors.size()))
            current_layer.layer_wx_color = std_wx_objects.std_colors[current_layer.layer_color];
    }
}

// Полноценный конструктор, принимающий всю совокупность существующих данных из структуры PCADFileSource.
PCADFile::PCADFile(PCADFileSource&& data_source, aperture::ApertureProvider& aperture_provider) :
    graph_objects_(move(data_source.graph_objects)), layers_(move(data_source.layers)),
    cross_layers_pinholes_(move(data_source.cross_layers_pinholes)),
    radio_components_(move(data_source.radio_components)),
    radio_comp_inserts_(move(data_source.radio_comp_inserts)),
    nets_(move(data_source.nets)),
    file_values_(data_source.file_values), aperture_provider_(aperture_provider)
{
    InitLayersColor();
    PCADPostProcess();
}

PCADFile::~PCADFile()
{
    PCADFileClear();
}

PCADFile& PCADFile::operator=(PCADFile&& other)
{
    if (this != &other)
    {
        PCADFileClear();
        graph_objects_ = move(other.graph_objects_);
        other.graph_objects_.clear();
        layers_ = move(other.layers_);
        cross_layers_pinholes_ = move(other.cross_layers_pinholes_);
        radio_components_ = move(other.radio_components_);
        other.radio_components_.clear();
        radio_comp_inserts_ = move(other.radio_comp_inserts_);
        nets_ = move(other.nets_);
        file_values_ = move(other.file_values_);
    }
    return *this;
}

size_t PCADFile::flashes_size() const
{
    return aperture_provider_.size();
}

vector<FlashDesc>::const_iterator PCADFile::flashes_begin() const
{
    return aperture_provider_.begin();
}

vector<FlashDesc>::const_iterator PCADFile::flashes_end() const
{
    return aperture_provider_.end();
}

vector<FlashDesc>::const_reverse_iterator PCADFile::flashes_rbegin() const
{
    return aperture_provider_.rbegin();
}

vector<FlashDesc>::const_reverse_iterator PCADFile::flashes_rend() const
{
    return aperture_provider_.rend();
}

void PCADFile::SetApertureType(UsingApertureType using_aperture_type) const
{
    aperture_provider_.SetApertureType(using_aperture_type);
    for (GraphObj* graph_obj_ptr : graph_objects_)
    {
        ObjFlash* obj_flash_ptr = dynamic_cast<ObjFlash*>(graph_obj_ptr);
        if (obj_flash_ptr)
            obj_flash_ptr->RecountFrameRect(file_values_, aperture_provider_);
    }
}

// Постобработка документа после загрузки его внешних данных. Рассчитывает значения некоторых полей, вычисляемых на основе
// загруженных данных, а также вырабатывает визуальное (видимое) представление всех его элементов, помимо прямо иллюстративных
// (которые загружаются непосредственно из файла базы данных).
void PCADFile::PCADPostProcess()
{
    for (RadioComponentDesc& radio_component : radio_components_)
        radio_component.ComponentPostProcess(this);
    for (RadioComponentInsertion& insertion : radio_comp_inserts_)
        insertion.InsertPostProcess(this);
    for (NetDefDesc& net : nets_)
        net.NetPostProcess(this);
}

FlashDesc PCADFile::GetFlashDesc(int flash_num) const
{ // Номер апертуры flash_num должен лежать в диапазоне от 1 до максимального номера среди загруженных апертур.
  // Для Gerber 32, как правило, доступны 24 диафрагмы с номерами от 1 до 24, а для Gerber Laser - 255 апертур
  // с номерами от 1 до 255.
    return aperture_provider_.GetFlashDesc(flash_num);
}

// Возврат описателя слоя по его порядковому индексу.
LayerDesc PCADFile::GetLayerDesc(int layer_num) const
{
    if (layer_num >= 0 && layer_num < static_cast<int>(layers_.size()))
        return layers_[layer_num];
    else //  Слой по умолчанию с невозможными параметрами
        return LAYER_DESC_INVALID;
}

// Поиск слоя по его имени.
int PCADFile::FindLayerDesc(const std::string& find_layer_name) const
{
    for (int scan_layer_num = 0; scan_layer_num < static_cast<int>(layers_.size()); ++scan_layer_num)
    {
        if (layers_[scan_layer_num].layer_name == find_layer_name)
            return scan_layer_num;
    }
    return -1;
}

// Установка вспомогательных атрибутов слоя.
void PCADFile::SetLayerAttributes(int layer_num, LayerAttributes layer_attributes) const
{
    if (layer_num >= 0 && layer_num < static_cast<int>(layers_.size()))
        layers_[layer_num].layer_attributes = layer_attributes;
}

// Получение текущего значения вспомогательных атрибутов слоя.
LayerAttributes PCADFile::GetLayerAttributes(int layer_num) const
{
    if (layer_num >= 0 && layer_num < static_cast<int>(layers_.size()))
        return layers_[layer_num].layer_attributes;
    else
        return LayerAttributes{0};
}

const GraphObj* PCADFile::ScanForGraphObject(uint32_t graph_object_ordinal) const
{
    for (GraphObj* graph_obj_ptr : graph_objects_)
        if (graph_obj_ptr->GetGraphObjectOrdinal() == graph_object_ordinal)
            return graph_obj_ptr;
    return nullptr;
}

const GraphObj* PCADFile::ScanForGraphObject(wxColor graph_obj_ord_as_color) const
{
    for (GraphObj* graph_obj_ptr : graph_objects_)
        if (StdWXObjects::CompareColorLong(graph_obj_ord_as_color, graph_obj_ptr->GetGraphObjectOrdinal()))
            return graph_obj_ptr;
    return nullptr;
}

double PCADFile::ConvertDBUToInch(double DBU_size) const
{
    if (file_values_.file_flags & FILE_FLAG_UNIT_INCHES)
        return DBU_size / file_values_.DBU_in_measure_unit;
    else
        return DBU_size / file_values_.DBU_in_measure_unit / MILLIMETERS_IN_INCH;
}

double PCADFile::ConvertDBUToMM(double DBU_size) const
{
    if (!(file_values_.file_flags & FILE_FLAG_UNIT_INCHES))
        return DBU_size / file_values_.DBU_in_measure_unit;
    else
        return DBU_size / file_values_.DBU_in_measure_unit * MILLIMETERS_IN_INCH;
}

double PCADFile::ConvertInchToDBU(double inch_size) const
{
    if (file_values_.file_flags & FILE_FLAG_UNIT_INCHES)
        return inch_size * file_values_.DBU_in_measure_unit;
    else
        return inch_size * MILLIMETERS_IN_INCH * file_values_.DBU_in_measure_unit;
}

double PCADFile::ConvertMMToDBU(double mm_size) const
{
    if (!(file_values_.file_flags & FILE_FLAG_UNIT_INCHES))
        return mm_size * file_values_.DBU_in_measure_unit;
    else
        return (mm_size / MILLIMETERS_IN_INCH) * file_values_.DBU_in_measure_unit;
}
