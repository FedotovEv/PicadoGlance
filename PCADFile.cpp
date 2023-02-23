
#include "PCADFile.h"
#include "PCADViewDraw.h"
#include "DrawGraphicalEntities.h"
#include "ApertureProvider.h"

using namespace aperture;
using namespace DrawEntities;
using namespace std;

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

PCADFile::PCADFile(vector<GraphObj*> graph_objects, vector<LayerDesc> layers,
                   ApertureProvider& aperture_provider, FileDefValues file_values) :
    graph_objects_(move(graph_objects)), layers_(move(layers)),
    aperture_provider_(aperture_provider), file_values_(file_values)
{
    StdWXObjects std_wx_objects;

    for (LayerDesc& current_layer : layers_)
    {
        if (current_layer.layer_color >= 0 &&
            current_layer.layer_color < static_cast<int>(std_wx_objects.std_colors.size()))
            current_layer.layer_wx_color = std_wx_objects.std_colors[current_layer.layer_color];
    }
}

void PCADFile::PCADFileClear()
{
    for (GraphObj* graph_obj_ptr : graph_objects_)
        delete graph_obj_ptr;
    graph_objects_.clear();
    layers_.clear();
}

PCADFile::PCADFile(PCADFile&& other) : aperture_provider_(other.aperture_provider_)
{
    graph_objects_ = move(other.graph_objects_);
    layers_ = move(other.layers_);
    file_values_ = move(other.file_values_);
    other.PCADFileClear();
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
        layers_ = move(other.layers_);
        file_values_ = move(other.file_values_);
        other.PCADFileClear();
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

FlashDesc PCADFile::GetFlashDesc(int flash_num) const
{ // Номер апертуры flash_num должен лежать в диапазоне от 1 до максимального номера среди загруженных апертур.
  // Для Gerber 32, как правило, доступны 24 диафрагмы с номерами от 1 до 24, а для Gerber Laser - 255 апертур
  // с номерами от 1 до 255.
    return aperture_provider_.GetFlashDesc(flash_num);
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
