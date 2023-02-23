
#include <iostream>
#include <string>
#include <vector>
#include <any>

#include "HandlerPLT.h"
#include "PCADFile.h"
#include "PCADViewDraw.h"
#include "ApertureFile.h"
#include <wx/colour.h>
#include <wx/gdicmn.h>

using namespace std;
using namespace aperture;

namespace HandlerPLT
{
    wxPoint PLT4FileWorkshop::ConvPntToLog(int pict_x, int pict_y) const
    {
        return {pict_x, fix_plt_head_.ruy - pict_y + fix_plt_head_.ldy};
    }

    wxPoint PLT4FileWorkshop::ConvPntToLog(wxPoint pict) const
    {
        return {pict.x, fix_plt_head_.ruy - pict.y + fix_plt_head_.ldy};
    }

    wxRect PLT4FileWorkshop::ConvRectToLog(int pict_ldx, int pict_ldy, int pict_rux, int pict_ruy) const
    {
        if (pict_ldx > pict_rux)
            swap (pict_ldx, pict_rux);
        if (pict_ldy > pict_ruy)
            swap(pict_ldy, pict_ruy);
        return {pict_ldx, fix_plt_head_.ruy - pict_ruy + fix_plt_head_.ldy,
                pict_rux - pict_ldx + 1, pict_ruy - pict_ldy + 1};
    }

    wxRect PLT4FileWorkshop::ConvRectToLog(wxRect pict_rect) const
    { // Полагаем, что в точке (pict_rect.x, pict_rect.y) хранятся координаты левого нижнего угла прямоугольника.
        int pict_ruy = pict_rect.y + pict_rect.height - 1;
        return {pict_rect.x, fix_plt_head_.ruy - pict_ruy + fix_plt_head_.ldy,
                pict_rect.width, pict_rect.height};
    }

    void PLT4FileWorkshop::FillFileValues(FileDefValues& fdv,
                                          FileWorkshop::AdditionalLoadInfo& additional_load_info) const
    {
        fdv.file_signature = wxT("PLT4");
        fdv.file_workshop = this;
        fdv.file_flags = 0;
        fdv.picture_filepath = additional_load_info.picture_file_path;
        // fix_plt_head_.measure_unit кодирует единицы измерения длины в чертеже (2 - дюймы, 7 - миллиметры)
        if (fix_plt_head_.measure_unit == PLT4_MEASURE_UNIT_MM)
        {
            if (fix_plt_head_.editor_type == PLT4_EDITOR_PCCAPS)
                fdv.DBU_in_measure_unit = 10; // 10 DBU на миллиметр
            else
                fdv.DBU_in_measure_unit = 100; // 100 DBU на миллиметр
        }
        else
        {
            if (fix_plt_head_.editor_type == PLT4_EDITOR_PCCAPS)
                fdv.DBU_in_measure_unit = 100; // 1 DBU на сотую дюйма -> 100 DBU на дюйм
            else
                fdv.DBU_in_measure_unit = 1000; // 1 DBU на мил -> 1000 DBU на дюйм
            fdv.file_flags |= FILE_FLAG_UNIT_INCHES;
        }

        if (fix_plt_head_.editor_type == PLT4_EDITOR_PCCAPS)
            fdv.file_flags |= FILE_FLAG_EDITOR_PCCAPS;

        fdv.frame_rect = wxRect(fix_plt_head_.ldx, fix_plt_head_.ldy,
                                fix_plt_head_.rux - fix_plt_head_.ldx + 1,
                                fix_plt_head_.ruy - fix_plt_head_.ldy + 1);
    }

    LineType PLT4FileWorkshop::GetLineType(unsigned char line_type) const
    {
        struct TransLineType
        {
            unsigned char line_type;
            LineType plt_line_type;
        };

        static const TransLineType ArrTransLineType[] =
        {
            {PLT4_GRAPH_LINE_SOLID, LineType::LINE_SOLID},
            {PLT4_GRAPH_LINE_DOTTED, LineType::LINE_DOTTED},
            {PLT4_GRAPH_LINE_DASHED, LineType::LINE_DASHED}
        };

        for (const TransLineType& tlt : ArrTransLineType)
            if (line_type == tlt.line_type)
                return tlt.plt_line_type;
        return LineType::LINE_SOLID;
    }

    TextOrientation PLT4FileWorkshop::GetTextOrient(unsigned char text_orientation) const
    {

       struct TransTextOrientation
        {
            unsigned char text_orientation;
            TextOrientation plt_text_orient;
        };

        static const TransTextOrientation ArrTransTextOrientation[] =
        {
            {PLT4_GRAPH_TEXT_LEFT_RIGHT, TextOrientation::TEXT_LEFT_RIGHT},
            {PLT4_GRAPH_TEXT_DOWN_UP, TextOrientation::TEXT_DOWN_UP},
            {PLT4_GRAPH_TEXT_RIGHT_LEFT, TextOrientation::TEXT_RIGHT_LEFT},
            {PLT4_GRAPH_TEXT_UP_DOWN, TextOrientation::TEXT_UP_DOWN},
            {PLT4_GRAPH_TEXT_LEFT_RIGHT_MIRROR, TextOrientation::TEXT_LEFT_RIGHT_MIRROR},
            {PLT4_GRAPH_TEXT_DOWN_UP_MIRROR, TextOrientation::TEXT_DOWN_UP_MIRROR},
            {PLT4_GRAPH_TEXT_RIGHT_LEFT_MIRROR, TextOrientation::TEXT_RIGHT_LEFT_MIRROR},
            {PLT4_GRAPH_TEXT_UP_DOWN_MIRROR, TextOrientation::TEXT_UP_DOWN_MIRROR}
        };

        for (const TransTextOrientation& tto : ArrTransTextOrientation)
            if (text_orientation == tto.text_orientation)
                return tto.plt_text_orient;
        return TextOrientation::TEXT_LEFT_RIGHT;
    }

    TextAlign PLT4FileWorkshop::GetTextAlign(unsigned char text_align) const
    {

        struct TransTextAlign
        {
            unsigned char text_align;
            TextAlign plt_text_align;
        };

        static const TransTextAlign ArrTransTextAlign[] =
        {
            {PLT4_GRAPH_TEXT_CENTER_DOWN, TextAlign::TEXT_CENTER_DOWN}, // Точка привязки текста по центру снизу
            {PLT4_GRAPH_TEXT_CENTER_UP, TextAlign::TEXT_CENTER_UP},
            {PLT4_GRAPH_TEXT_CENTER_CENTER, TextAlign::TEXT_CENTER_CENTER},
            {PLT4_GRAPH_TEXT_LEFT_DOWN, TextAlign::TEXT_LEFT_DOWN},
            {PLT4_GRAPH_TEXT_LEFT_UP, TextAlign::TEXT_LEFT_UP},
            {PLT4_GRAPH_TEXT_LEFT_CENTER, TextAlign::TEXT_LEFT_CENTER},
            {PLT4_GRAPH_TEXT_RIGHT_DOWN, TextAlign::TEXT_RIGHT_DOWN},
            {PLT4_GRAPH_TEXT_RIGHT_UP, TextAlign::TEXT_RIGHT_UP},
            {PLT4_GRAPH_TEXT_RIGHT_CENTER, TextAlign::TEXT_RIGHT_CENTER} // Точка привязки текста справа по центру
        };

        for (const TransTextAlign& ttal : ArrTransTextAlign)
            if (text_align == ttal.text_align)
                return ttal.plt_text_align;
        return TextAlign::TEXT_CENTER_CENTER;
    }

    string PLT4FileWorkshop::ReadTextData(istream& istr) const
    { // Считываем собственно текст, изображаемый текстовым примитивом и находящийся
      // после фиксированного заголовка примитива.
        string result;
        while (true)
        {
            int c = istr.get();
            if (!istr || c == char_traits<char>::eof())
                return result;
            if (c)
                result += c;
            else
                break;
        }
        // Рассчитаем длину отрезка-заполнителя, выравнивающего текст по длине вверх
        // до границы двойного слова. Именно так он хранится в файле.
        int align_len = (result.size() + 1);
        align_len = ((align_len + 3) & ~3) - align_len;
        while (align_len--) // пропускаем байты-заполнители
            istr.get();
        return result;
    }

    FlashOrientation PLT4FileWorkshop::GetFlashOrient(unsigned char flash_orientation) const
    {

       struct TransFlashOrient
        {
            unsigned char flash_orientation;
            FlashOrientation plt_flash_orient;
        };

        static const TransFlashOrient ArrTransFlashOrient[] =
        {
            {PLT4_GRAPH_FLASH_NORMAL, FlashOrientation::FLASH_NORMAL},
            {PLT4_GRAPH_FLASH_90_DEGREES, FlashOrientation::FLASH_90_DEGREES},
            {PLT4_GRAPH_FLASH_180_DEGREES, FlashOrientation::FLASH_180_DEGREES},
            {PLT4_GRAPH_FLASH_270_DEGREES, FlashOrientation::FLASH_270_DEGREES}
        };
        for (const TransFlashOrient& tfo : ArrTransFlashOrient)
            if (flash_orientation == tfo.flash_orientation)
                return tfo.plt_flash_orient;
        return FlashOrientation::FLASH_NORMAL;
    }

    FillType PLT4FileWorkshop::GetHatchType(unsigned char hatch_type) const
    {
        struct TransHatchType
        {
            unsigned char hatch_type;
            FillType plt_hatch_type;
        };

        static const TransHatchType ArrTransHatchType[] =
        {
            {PLT4_GRAPH_SOLID_FILL, FillType::POLY_FILL_SOLID},
            {PLT4_GRAPH_HATCH_FILL, FillType::POLY_FILL_HATCH}
        };
        for (const TransHatchType& tht : ArrTransHatchType)
            if (hatch_type == tht.hatch_type)
                return tht.plt_hatch_type;
        return FillType::POLY_FILL_SOLID;
    }

    vector<wxPoint> PLT4FileWorkshop::GetPolyVertexes(istream& istr, uint32_t& spec_sign) const
    {
        vector<wxPoint> result;
        while (true)
        {
            PPoint poly_point;
            istr.read(reinterpret_cast<char*>(&poly_point), sizeof(PPoint));
            if (!istr)
            {
                spec_sign = 0;
                return result;
            }
            spec_sign = *(reinterpret_cast<uint32_t*>(&poly_point));
            if (spec_sign == PLT4_GRAPH_CVOID_OBJ ||
                spec_sign == PLT4_GRAPH_PVOID_OBJ ||
                spec_sign == PLT4_GRAPH_END_POLY_POINTS)
                return result;
            result.push_back(ConvPntToLog(poly_point.xp, poly_point.yp));
        }

        return result;
    }

    vector<LayerDesc> PLT4FileWorkshop::ReadLayerInfo(istream& istr, PCADLoadError& process_error_code) const
    {
        process_error_code = PCADLoadError::LOAD_FILE_NO_ERROR;
        vector<LayerDesc> result_layers;
        for (int i = 0; i < fix_plt_head_.layer_count; i++)
        {
            LayerInfoS layer_orig;
            LayerDesc layer_desc;
            istr.read(reinterpret_cast<char*>(&layer_orig), sizeof(layer_orig));
            if (!istr)
            {
                process_error_code = PCADLoadError::LOAD_FILE_READ_ERROR;
                break;
            }

            layer_desc.layer_number = layer_orig.layer_num;
            layer_desc.layer_color = layer_orig.layer_color;
            layer_desc.layer_wx_color = *wxWHITE;
            layer_desc.layer_attributes = LayerAttributes{0};

            string name_lay(layer_orig.layer_name, 6);
            size_t j = name_lay.find('\0');
            if (j != string::npos)
                name_lay = name_lay.substr(0, j);

            layer_desc.layer_name = name_lay;
            result_layers.push_back(layer_desc);
        }
        return result_layers;
    }

    int PLT4FileWorkshop::DetectLayerNumber(int current_col_index, const vector<LayerDesc>& layers) const
    {
        for (size_t i = 0; i < layers.size(); ++i)
            if (layers[i].layer_color == current_col_index)
                return i;
        return -1;
    }

    FileWorkshop::LoadFileResult PLT4FileWorkshop::LoadPCADFile(istream& istr,
                        FileWorkshop::AdditionalLoadInfo& additional_load_info)
    {
        vector<GraphObj*> graph_objects; // Массив графических элементов файла
        vector<LayerDesc> layers; // Массив описателей слоёв изображения
        FileDefValues file_values;
        PCADLoadError process_error_code = PCADLoadError::LOAD_FILE_NO_ERROR;

        istr.read(reinterpret_cast<char*>(&fix_plt_head_), sizeof(fix_plt_head_));
        if (!istr)
            return {PCADFile{additional_load_info.aperture_provider}, PCADLoadError::LOAD_FILE_READ_ERROR};

        if (string(fix_plt_head_.file_desc, PLT4_FILE_DESC_LEN) != string(PLT4_FILE_DESC, PLT4_FILE_DESC_LEN))
            return {PCADFile{additional_load_info.aperture_provider}, PCADLoadError::LOAD_FILE_BAD_FILE_FORMAT};

        FillFileValues(file_values, additional_load_info);
        layers = ReadLayerInfo(istr, process_error_code); // считываем информацию о слоях
        if (process_error_code != PCADLoadError::LOAD_FILE_NO_ERROR)
            return {PCADFile{additional_load_info.aperture_provider}, process_error_code};

        // Ну а теперь считываем содержащиеся в файле примитивы
        int current_color_index = 0;
        unsigned short obj_code_desc;
        while (istr.good())
        {
            GraphObj* graph_obj_ptr = nullptr;
            istr.read(reinterpret_cast<char*>(&obj_code_desc), sizeof(obj_code_desc));
            if (!istr.good())
            {
                if (istr.gcount())
                    return {PCADFile(move(graph_objects), move(layers), additional_load_info.aperture_provider,
                                     move(file_values)), PCADLoadError::LOAD_FILE_READ_ERROR};
                break;
            }
            string text_text;
            int text_height = 0;
            vector<wxPoint> points;
            vector<wxPoint> pvoid_points;
            vector<ObjCVoid> cvoids;
            vector<ObjPVoid> pvoids;

            switch (obj_code_desc)
            {
                case PLT4_GRAPH_COLOR_PEN_OBJ:
                    ObjSelColorS pict_sel_color;
                    istr.read(reinterpret_cast<char*>(&pict_sel_color), sizeof(ObjSelColorS));
                    if (!istr)
                        break;
                    graph_obj_ptr = new ObjSelColor(-1, pict_sel_color.pen_color, file_values);
                    current_color_index = pict_sel_color.pen_color;
                    break;
                case PLT4_GRAPH_LINE_OBJ:
                    ObjLineS pict_line;
                    istr.read(reinterpret_cast<char*>(&pict_line), sizeof(ObjLineS));
                    if (!istr)
                        break;
                    graph_obj_ptr = new ObjLine(DetectLayerNumber(current_color_index, layers),
                                                GetLineType(pict_line.line_type), pict_line.line_width,
                                                ConvPntToLog(pict_line.begin_x, pict_line.begin_y),
                                                ConvPntToLog(pict_line.end_x, pict_line.end_y));
                    break;
                case PLT4_GRAPH_RECT_OBJ:
                    ObjRectS pict_rect;
                    istr.read(reinterpret_cast<char*>(&pict_rect), sizeof(ObjRectS));
                    if (!istr)
                        break;
                    graph_obj_ptr = new ObjRect(DetectLayerNumber(current_color_index, layers),
                                                GetLineType(pict_rect.line_type), pict_rect.line_width,
                                                ConvRectToLog(pict_rect.rect_left_dn_x, pict_rect.rect_left_dn_y,
                                                              pict_rect.rect_right_up_x, pict_rect.rect_right_up_y));
                    break;
                case PLT4_GRAPH_FRECT_OBJ:
                    ObjFRecS pict_fill_rect;
                    istr.read(reinterpret_cast<char*>(&pict_fill_rect), sizeof(ObjFRecS));
                    if (!istr)
                        break;

                    graph_obj_ptr = new ObjFillRect(DetectLayerNumber(pict_fill_rect.fill_color, layers),
                                            pict_fill_rect.fill_color,
                                            ConvRectToLog(pict_fill_rect.rect_left_dn_x, pict_fill_rect.rect_left_dn_y,
                                                          pict_fill_rect.rect_right_up_x, pict_fill_rect.rect_right_up_y));
                    break;
                case PLT4_GRAPH_CIRC_OBJ:
                    ObjCircS pict_circ;
                    istr.read(reinterpret_cast<char*>(&pict_circ), sizeof(ObjCircS));
                    if (!istr)
                        break;

                    graph_obj_ptr = new ObjCirc(DetectLayerNumber(current_color_index, layers),
                                                GetLineType(pict_circ.line_type), pict_circ.line_width,
                                                ConvPntToLog(pict_circ.circle_center_x, pict_circ.circle_center_y),
                                                pict_circ.circle_center_radius);
                    break;
                case PLT4_GRAPH_ARC_OBJ:
                    ObjArcS pict_arc;
                    istr.read(reinterpret_cast<char*>(&pict_arc), sizeof(ObjArcS));
                    if (!istr)
                        break;

                    graph_obj_ptr = new ObjArc(DetectLayerNumber(current_color_index, layers),
                                               GetLineType(pict_arc.line_type), pict_arc.line_width,
                                               ConvPntToLog(pict_arc.arc_center_x, pict_arc.arc_center_y),
                                               ConvPntToLog(pict_arc.arc_begin_x, pict_arc.arc_begin_y),
                                               ConvPntToLog(pict_arc.arc_end_x, pict_arc.arc_end_y));
                    break;
                case PLT4_GRAPH_TEXT_OBJ:
                    ObjTextS pict_text;
                    istr.read(reinterpret_cast<char*>(&pict_text), sizeof(ObjTextS));
                    if (!istr)
                        break;
                    text_text = ReadTextData(istr);
                    if (!istr)
                        break;
                    text_height = static_cast<int>(pict_text.text_height_hi_byte) * 128 +
                                  static_cast<int>(pict_text.text_height_lo_byte);
                    graph_obj_ptr = new ObjText(DetectLayerNumber(current_color_index, layers),
                                                ConvPntToLog(pict_text.text_x, pict_text.text_y),
                                                GetTextOrient(pict_text.text_orientation),
                                                text_height, GetTextAlign(pict_text.text_align), text_text);
                    break;
                case PLT4_GRAPH_FLASH_OBJ:
                    ObjFlashS pict_flash;
                    istr.read(reinterpret_cast<char*>(&pict_flash), sizeof(ObjFlashS));
                    if (!istr)
                        break;

                    graph_obj_ptr = new ObjFlash(DetectLayerNumber(current_color_index, layers),
                                                 ConvPntToLog(pict_flash.flash_x, pict_flash.flash_y),
                                                 GetFlashOrient(pict_flash.flash_orientation),
                                                 pict_flash.aperture_number, file_values,
                                                 additional_load_info.aperture_provider);
                    break;
                case PLT4_GRAPH_POLY_OBJ:
                    ObjPolyS pict_poly;
                    ObjCVoidS pict_cvoid;
                    istr.read(reinterpret_cast<char*>(&pict_poly), sizeof(ObjPolyS));
                    if (!istr)
                        break;
                    uint32_t spec_sign;
                    points = GetPolyVertexes(istr, spec_sign);
                    // Считываем все выемки, расположенные на площади полигона.
                    while (istr && spec_sign != PLT4_GRAPH_END_POLY_POINTS)
                    {
                        switch (spec_sign)
                        {
                        case PLT4_GRAPH_CVOID_OBJ:
                            istr.read(reinterpret_cast<char*>(&pict_cvoid), sizeof(ObjCVoidS));
                            if (!istr)
                                break;
                            cvoids.push_back(ObjCVoid(ConvPntToLog(pict_cvoid.void_center_x, pict_cvoid.void_center_y),
                                             pict_cvoid.void_radius));
                            istr.read(reinterpret_cast<char*>(&spec_sign), sizeof(spec_sign));
                            break;
                        case PLT4_GRAPH_PVOID_OBJ:
                            pvoid_points = GetPolyVertexes(istr, spec_sign);
                            if (!istr)
                                break;
                            pvoids.push_back(ObjPVoid(move(pvoid_points)));
                            break;
                        case PLT4_GRAPH_END_POLY_POINTS:
                            break;
                        default:
                            return {PCADFile(move(graph_objects), move(layers), additional_load_info.aperture_provider,
                                             move(file_values)), PCADLoadError::LOAD_FILE_BAD_RECORD_FORMAT};
                        }
                    }

                    if (!istr)
                        break;

                    graph_obj_ptr = new ObjPoly(DetectLayerNumber(current_color_index, layers),
                                                GetHatchType(pict_poly.hatch_type),
                                                pict_poly.aperture_width, points, cvoids, pvoids);
                    break;
                default:
                    return {PCADFile(move(graph_objects), move(layers), additional_load_info.aperture_provider,
                                     move(file_values)), PCADLoadError::LOAD_FILE_BAD_RECORD_FORMAT};
            }

            if (!istr.good())
                return {PCADFile(move(graph_objects), move(layers), additional_load_info.aperture_provider,
                                 move(file_values)), PCADLoadError::LOAD_FILE_READ_ERROR};
            if (!graph_obj_ptr)
                return {PCADFile(move(graph_objects), move(layers), additional_load_info.aperture_provider,
                                 move(file_values)), PCADLoadError::LOAD_FILE_MEMORY_ERROR};
            // Успешно сформирован очередной графический примитив - добавим его в список примитивов чертежа.
            graph_objects.push_back(graph_obj_ptr);
        }

        return {PCADFile(move(graph_objects), move(layers), additional_load_info.aperture_provider,
                         move(file_values)), PCADLoadError::LOAD_FILE_NO_ERROR};
    }
} // namespace HandlerPLT
