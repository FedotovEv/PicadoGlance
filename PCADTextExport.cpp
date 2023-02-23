#define _USE_MATH_DEFINES
#include <iostream>
#include <string>
#include <vector>
#include <variant>
#include <limits>
#include <cmath>
#include <wx/colour.h>
#include <wx/pen.h>
#include <wx/brush.h>
#include <wx/gdicmn.h>
#include <wx/palette.h>
#include "PCADTextExport.h"
#include "PCADFile.h"

using namespace std;

extern const StdWXObjects std_wx_objects;
static const string zpt = " , ";

string ConvertSizeToString(double size_value, DrawContext& draw_context)
{
    ostringstream ostr;
    bool is_size_value_int = abs(size_value - round(size_value)) < ZERO_TOLERANCE;
    double size_value_mm, size_value_inch;
    bool is_size_value_mm_int = false, is_size_value_inch_int = false;

    if (draw_context.pcad_doc_ptr)
    {
        size_value_mm = draw_context.pcad_doc_ptr->ConvertDBUToMM(size_value);
        size_value_inch = draw_context.pcad_doc_ptr->ConvertDBUToInch(size_value);
        is_size_value_mm_int = abs(size_value_mm - round(size_value_mm)) < ZERO_TOLERANCE;
        is_size_value_inch_int = abs(size_value_inch - round(size_value_inch)) < ZERO_TOLERANCE;
    }

    int precision_value = 6;
    if (draw_context.text_export_style == TextExportStyleType::TEXT_EXPORT_CONDENSED_STYLE)
        precision_value = 2;

    switch (draw_context.measure_unit_type)
    {
    case MeasureUnitTypeData::MEASURE_UNIT_MM:
        is_size_value_int = is_size_value_mm_int;
        size_value = size_value_mm;
        break;
    case MeasureUnitTypeData::MEASURE_UNIT_INCH:
        is_size_value_int = is_size_value_inch_int;
        size_value = size_value_inch;
        break;
    default:
        break;
    }

    if (is_size_value_int)
        ostr << static_cast<int>(size_value);
    else
        ostr << fixed << setprecision(precision_value) << size_value;
    return ostr.str();
}

string ConvertPointToString(wxPoint pnt, DrawContext& draw_context)
{
    return '(' + ConvertSizeToString(pnt.x, draw_context) + zpt +
           ConvertSizeToString(pnt.y, draw_context) + ')';
}

string ConvertRectToString(const wxRect& rect, DrawContext& draw_context)
{
    return "rect(("s + ConvertSizeToString(rect.x, draw_context) + zpt +
        ConvertSizeToString(rect.y, draw_context) + ") : ("s +
        ConvertSizeToString(rect.width, draw_context) + zpt +
        ConvertSizeToString(rect.height, draw_context) + "))"s;
}

string ConvertRectToStringShort(const wxRect& rect, DrawContext& draw_context)
{
    return '(' + ConvertSizeToString(rect.x, draw_context) + ',' +
           ConvertSizeToString(rect.y, draw_context) + "):("s +
           ConvertSizeToString(rect.width, draw_context) + ',' +
           ConvertSizeToString(rect.height, draw_context) + ')';
}

string ConvertRectToStringShortEx(const wxRect& rect, DrawContext& draw_context)
{
    return ConvertPointToString(rect.GetTopLeft(), draw_context) + '-' +
           ConvertPointToString(rect.GetRightBottom(), draw_context);
}

string ConvertColorToString(const SelColorType& col)
{
    if (holds_alternative<unsigned char>(col))
    {
        size_t col_num = get<unsigned char>(col);
        wxColor wx_col = std_wx_objects.std_colors[min(col_num, std_wx_objects.std_colors.size() - 1)];
        return "color("s + to_string(col_num) + " > "s + to_string(wx_col.Red()) + zpt +
               to_string(wx_col.Green()) + zpt + to_string(wx_col.Blue()) + zpt +
               to_string(wx_col.Alpha()) + ')';
    }
    else
    {
        wxColor wx_col = get<wxColor>(col);
        return "color( > "s + to_string(wx_col.Red()) + zpt + to_string(wx_col.Green()) +
               zpt + to_string(wx_col.Blue())  + zpt + to_string(wx_col.Alpha()) + ')';
    }
}

string ConvertColorToStringShort(const SelColorType& col)
{
    if (holds_alternative<unsigned char>(col))
    {
        size_t col_num = get<unsigned char>(col);
        wxColor wx_col = std_wx_objects.std_colors[min(col_num, std_wx_objects.std_colors.size() - 1)];
        return '(' + to_string(col_num) + '>' + to_string(wx_col.Red()) + ':' +
               to_string(wx_col.Green()) + ':' + to_string(wx_col.Blue()) + ':' +
               to_string(wx_col.Alpha()) + ')';
    }
    else
    {
        wxColor wx_col = get<wxColor>(col);
        return '(' + to_string(wx_col.Red()) + ':' + to_string(wx_col.Green()) +
               ':' + to_string(wx_col.Blue())  + ':' + to_string(wx_col.Alpha()) + ')';
    }
}

string ObjSelColor::GetObjectLongText(DrawContext& draw_context) const
{
    return "SelectColor : "s + ConvertColorToString(set_color_);
}

string ObjSelColor::GetObjectShortText(DrawContext& draw_context) const
{
    return "Цвет:"s + ConvertColorToStringShort(set_color_);
}

string ObjLine::GetObjectLongText(DrawContext& draw_context) const
{
    return "Line : "s + ConvertPointToString(point1_, draw_context) + zpt +
           ConvertPointToString(point2_, draw_context);
}

string ObjLine::GetObjectShortText(DrawContext& draw_context) const
{
    return "Отрезок:"s + ConvertPointToString(point1_, draw_context) +
           '-' + ConvertPointToString(point2_, draw_context);
}

string ObjRect::GetObjectLongText(DrawContext& draw_context) const
{
    return "Rectangle : "s + ConvertRectToString(rect_, draw_context);
}

string ObjRect::GetObjectShortText(DrawContext& draw_context) const
{
    return "Прямоугольник:"s + ConvertRectToStringShort(rect_, draw_context);
}

string ObjFillRect::GetObjectLongText(DrawContext& draw_context) const
{
    return "FillRectangle : "s + ConvertRectToString(rect_, draw_context) +
           zpt + ConvertColorToString(rect_color_);
}

string ObjFillRect::GetObjectShortText(DrawContext& draw_context) const
{
    return "Сплошной:"s + ConvertRectToStringShort(rect_, draw_context) +
           ':' + ConvertColorToStringShort(rect_color_);
}

string ObjCirc::GetObjectLongText(DrawContext& draw_context) const
{
    return "Circle : "s + ConvertPointToString(center_, draw_context) +
            zpt + "radius("s + ConvertSizeToString(radius_, draw_context) + ')';
}

string ObjCirc::GetObjectShortText(DrawContext& draw_context) const
{
    return "Круг:"s + ConvertPointToString(center_, draw_context) + ":R"s +
           ConvertSizeToString(radius_, draw_context);
}

string ObjArc::GetObjectLongText(DrawContext& draw_context) const
{
    return "Arc3P : "s + ConvertPointToString(arc_center_, draw_context) + zpt +
           ConvertPointToString(arc_begin_, draw_context) + zpt +
           ConvertPointToString(arc_end_, draw_context);
}

string ObjArc::GetObjectShortText(DrawContext& draw_context) const
{
    return "Дуга:"s + ConvertPointToString(arc_center_, draw_context);
}

string ObjText::GetObjectLongText(DrawContext& draw_context) const
{
    return "Text : "s + ConvertPointToString(text_point_, draw_context) + zpt +
           "orientation("s + to_string(static_cast<int>(text_orientation_)) + ')' + zpt +
           "height("s + ConvertSizeToString(text_height_, draw_context) + ')' + zpt +
           "align("s + to_string(static_cast<int>(text_align_)) + ')' + zpt +
           "text("s + text_ + ')';
}

string ObjText::GetObjectShortText(DrawContext& draw_context) const
{
    return "Текст:"s + ConvertPointToString(text_point_, draw_context);
}

string ObjFlash::GetObjectLongText(DrawContext& draw_context) const
{
    return "Flash : "s + ConvertPointToString(flash_point_, draw_context) + zpt +
    "number("s + to_string(aperture_number_) + ')' + zpt +
    "orientation("s + to_string(static_cast<int>(flash_orientation_)) + ')';
}

string ObjFlash::GetObjectShortText(DrawContext& draw_context) const
{
    return "Вспышка:"s + ConvertPointToString(flash_point_, draw_context) +
           ':' + to_string(aperture_number_);
}

string ObjPoly::GetObjectLongText(DrawContext& draw_context) const
{
    string result = "Polygon : "s + "hatch("s + to_string(static_cast<int>(hatch_type_)) +
                    ')' + zpt + "width("s + ConvertSizeToString(aperture_width_, draw_context) + ')';
    for (wxPoint cur_point : points_)
        result += zpt + ConvertPointToString(cur_point, draw_context);
    return result;
}

string ObjPoly::GetObjectShortText(DrawContext& draw_context) const
{
    return "Полигон:"s + "W:"s + ConvertSizeToString(aperture_width_, draw_context);
}

string ObjCVoid::GetObjectLongText(DrawContext& draw_context) const
{
    return "CircleVoid : "s + ConvertPointToString(void_center_, draw_context) + zpt +
                         "radius("s + ConvertSizeToString(void_radius_, draw_context) + ')';
}

string ObjCVoid::GetObjectShortText(DrawContext& draw_context) const
{
    return "КруглыйВыем:"s + ConvertPointToString(void_center_, draw_context) + ":R"s +
           ConvertSizeToString(void_radius_, draw_context);
}

string ObjPVoid::GetObjectLongText(DrawContext& draw_context) const
{
    string result = "PolygonVoid : "s;
    size_t i = 0;
    for (wxPoint cur_point : points_)
    {
        result += ConvertPointToString(cur_point, draw_context);
        if (++i < points_.size())
            result += zpt;
    }
    return result;
}

string ObjPVoid::GetObjectShortText(DrawContext& draw_context) const
{
    return "МногоугВыем:"s;
}
