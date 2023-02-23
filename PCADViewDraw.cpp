
#define _USE_MATH_DEFINES

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <optional>
#include <limits>
#include <cmath>
#include <wx/colour.h>
#include <wx/pen.h>
#include <wx/brush.h>
#include <wx/gdicmn.h>
#include <wx/palette.h>
#include "PCADFile.h"
#include "PCADViewerApp.h"
#include "PCADViewDraw.h"
#include "DrawGraphicalEntities.h"
#include "svg.h"

using namespace std;
using namespace DrawEntities;
using namespace aperture;

namespace
{
    const wxColor PCAD_palette_color[] =
        {wxColor(0, 0, 0), wxColor(128, 0, 0), wxColor(255, 0, 0),
        wxColor(0, 128, 0), wxColor(0, 255, 0), wxColor(0, 0, 128),
        wxColor(0, 0, 255), wxColor(128, 128, 0), wxColor(255, 255, 0),
        wxColor(0, 128, 128), wxColor(0, 255, 255), wxColor(128, 0, 128),
        wxColor(255, 0, 255), wxColor(128, 128, 128), wxColor(192, 192, 192),
        wxColor(255, 255, 255)};
    const uint32_t std_line_masks[] =
        {SOLID_LINE_MASK, DOTTED_LINE_MASK, DASHED_LINE_MASK};

    int AngleQuarterNumber(double angle)
    { // Возвращает номер четверти (от 1 до 4), к которой принадлежит угол angle.
      // Угол принадлежит диапазону [-M_PI, M_PI], как его возвращает функция atan2().
        if (angle >= 0 && angle <= M_PI / 2)
            return 1;
        else if (angle >= M_PI / 2)
            return 2;
        else if (angle <= -M_PI / 2)
            return 3;
        else
            return 4;
    }

    double QuarterLowEdgeAngle(int quarter_number)
    { // Возвращает начальный угол quarter_number (значения от 1 до 4) четверти.
        switch(quarter_number)
        {
        case 1:
            return 0.0; // Начальный угол 1 четверти
        case 2:
            return M_PI / 2; // Величина начального угла 2 четверти
        case 3:
            return -M_PI; // Начальный угол 3 четверти
        case 4:
            return -M_PI / 2; // Величина начального угла 4 четверти
        }
        return 0.0;
    }

    double QuarterHighEdgeAngle(int quarter_number)
    { // Возвращает конечный угол quarter_number (значения от 1 до 4) четверти.
        switch(quarter_number)
        {
        case 1:
            return M_PI / 2; // Конечный угол 1 четверти
        case 2:
            return M_PI; // Величина конечного угла 2 четверти
        case 3:
            return -M_PI / 2; // Конечный угол 3 четверти
        case 4:
            return 0.0; // Величина конечного угла 4 четверти
        }
        return 0.0;
    }

    wxRect CountArcBoundRect(wxPoint arc_center, wxPoint arc_begin, wxPoint arc_end, int line_width_hor, int line_width_vert)
    { //Функция вычисляет параметры прямоугольника, описанного вокруг дуги, заданной центром и двумя граничными точками
        wxRect frame_rect;
        double radius_arc = sqrt((arc_begin.x - arc_center.x) * (arc_begin.x - arc_center.x) +
                                 (arc_center.y - arc_begin.y) * (arc_center.y - arc_begin.y));
        double start_angle = atan2(arc_center.y - arc_begin.y, arc_begin.x - arc_center.x);
        double end_angle = atan2(arc_center.y - arc_end.y, arc_end.x - arc_center.x);

        arc_end = wxPoint(arc_center.x + radius_arc * cos(end_angle),
                          arc_center.y - radius_arc * sin(end_angle));
        line_width_hor = max(line_width_hor, 1);
        line_width_vert = max(line_width_hor, 1);

        int current_line_width = CountCurrentLineWidth(ConvertTO2PI(start_angle), line_width_hor, line_width_vert);
        frame_rect.Union(wxRect(arc_begin.x, arc_begin.y, current_line_width, current_line_width));

        current_line_width = CountCurrentLineWidth(ConvertTO2PI(end_angle), line_width_hor, line_width_vert);
        frame_rect.Union(wxRect(arc_end.x, arc_end.y, current_line_width, current_line_width));

        int start_angle_quarter = AngleQuarterNumber(start_angle);
        int end_angle_quarter = AngleQuarterNumber(end_angle);

        if (start_angle_quarter == end_angle_quarter)
        {
            if (start_angle > end_angle)
            { // Рисуется почти полная окружность, добавляем к вычисляемому описанному
              // прямоугольнику все 4 крайние точки окружности
                // Правая крайняя точка окружности
                current_line_width = CountCurrentLineWidth(0, line_width_hor, line_width_vert);
                frame_rect.Union(wxRect(arc_center.x + radius_arc, arc_center.y, current_line_width, current_line_width));
                // Верхняя крайняя точка окружности
                current_line_width = CountCurrentLineWidth(M_PI / 2, line_width_hor, line_width_vert);
                frame_rect.Union(wxRect(arc_center.x, arc_center.y - radius_arc, current_line_width, current_line_width));
                // Левая крайняя точка окружности
                current_line_width = CountCurrentLineWidth(M_PI, line_width_hor, line_width_vert);
                frame_rect.Union(wxRect(arc_center.x - radius_arc, arc_center.y, current_line_width, current_line_width));
                // Нижняя крайняя точка окружности
                current_line_width = CountCurrentLineWidth(3 *  (M_PI / 2), line_width_hor, line_width_vert);
                frame_rect.Union(wxRect(arc_center.x, arc_center.y + radius_arc, current_line_width, current_line_width));
            }
        }
        else
        {
            int i = start_angle_quarter;
            while (i != end_angle_quarter)
            {
                switch (i)
                {
                case 1: // Дуга переходит из первого квардранта во второй - добавляем верхнюю точку окружности
                    // Угол PI / 2 - верхняя крайняя точка окружности
                    current_line_width = CountCurrentLineWidth(M_PI / 2, line_width_hor, line_width_vert);
                    frame_rect.Union(wxRect(arc_center.x, arc_center.y - radius_arc, current_line_width, current_line_width));
                    break;
                case 2: // Дуга переходит из второго квардранта в третий - добавляем левую точку окружности
                    // Угол PI - левая крайняя точка окружности
                    current_line_width = CountCurrentLineWidth(M_PI, line_width_hor, line_width_vert);
                    frame_rect.Union(wxRect(arc_center.x - radius_arc, arc_center.y, current_line_width, current_line_width));
                    break;
                case 3: // Дуга переходит из третьего квардранта в четвёртый - добавляем нижнюю точку окружности
                    // Угол 3 * PI / 2 - нижняя крайняя точка окружности
                    current_line_width = CountCurrentLineWidth(3 *  (M_PI / 2), line_width_hor, line_width_vert);
                    frame_rect.Union(wxRect(arc_center.x, arc_center.y + radius_arc, current_line_width, current_line_width));
                    break;
                case 4: // Дуга переходит из четвёртого квардранта во первый - добавляем правую точку окружности
                    // Угол 0 - правая крайняя точка окружности
                    current_line_width = CountCurrentLineWidth(0, line_width_hor, line_width_vert);
                    frame_rect.Union(wxRect(arc_center.x + radius_arc, arc_center.y, current_line_width, current_line_width));
                    break;
                }
                ++i;
                if (i > 4)
                    i = 1;
            }
        }
        return frame_rect;
    }

    double CountArcDistance(double start_angle, double end_angle)
    { // Вычисляет угловое расстояние по дуге между углами start_angle и end_angle.
      // Углы распределяются по четвертям так, как полагает функция atan2().
        int start_angle_quarter = AngleQuarterNumber(start_angle);
        int end_angle_quarter = AngleQuarterNumber(end_angle);

        if (start_angle_quarter == end_angle_quarter)
        {
            if (end_angle >= start_angle)
                return end_angle - start_angle;
            else
                return 2 * M_PI - (start_angle - end_angle);
        }
        else if (start_angle_quarter > end_angle_quarter)
        {
            double angle_accum = QuarterHighEdgeAngle(end_angle_quarter) - end_angle;
            angle_accum += (M_PI / 2) * (start_angle_quarter - end_angle_quarter - 1);
            angle_accum += start_angle - QuarterLowEdgeAngle(start_angle_quarter);
            return 2 * M_PI - angle_accum;
        }
        else
        {
            double angle_accum = QuarterHighEdgeAngle(start_angle_quarter) - start_angle;
            angle_accum += (M_PI / 2) * (end_angle_quarter - start_angle_quarter - 1);
            angle_accum += end_angle - QuarterLowEdgeAngle(end_angle_quarter);
            return angle_accum;
        }
    }

    TextSignature GetTextSignature(TextOrientation text_orientation, TextAlign text_align)
    {
        TextSignature result;

        result.text_orientation = text_orientation;
        result.text_align = text_align;
        switch (text_orientation)
        {
            case TextOrientation::TEXT_LEFT_RIGHT:
                result.text_angle = 0;
                result.is_reverse = false;
                result.is_horizontal = true;
                result.is_mirror = false;
                break;
            case TextOrientation::TEXT_LEFT_RIGHT_MIRROR:
                result.text_angle = 180;
                result.is_reverse = true;
                result.is_horizontal = true;
                result.is_mirror = true;
                break;
            case TextOrientation::TEXT_UP_DOWN:
                result.text_angle = 270;
                result.is_reverse = false;
                result.is_horizontal = false;
                result.is_mirror = false;
                break;
            case TextOrientation::TEXT_UP_DOWN_MIRROR:
                result.text_angle = 90;
                result.is_reverse = true;
                result.is_horizontal = false;
                result.is_mirror = true;
                break;
            case TextOrientation::TEXT_RIGHT_LEFT:
                result.text_angle = 180;
                result.is_reverse = true;
                result.is_horizontal = true;
                result.is_mirror = false;
                break;
            case TextOrientation::TEXT_RIGHT_LEFT_MIRROR:
                result.text_angle = 0;
                result.is_reverse = false;
                result.is_horizontal = true;
                result.is_mirror = true;
                break;
            case TextOrientation::TEXT_DOWN_UP:
                result.text_angle = 90;
                result.is_reverse = true;
                result.is_horizontal = false;
                result.is_mirror = false;
                break;
            case TextOrientation::TEXT_DOWN_UP_MIRROR:
                result.text_angle = 270;
                result.is_reverse = false;
                result.is_horizontal = false;
                result.is_mirror = true;
                break;
        }

        switch (text_align)
        {
            case TextAlign::TEXT_CENTER_DOWN: // Точка привязки текста по центру снизу
                if (result.is_horizontal)
                {
                    if (!result.is_reverse)
                        result.coord_x = TextCoordAlign::TEXTCOORD_MINUS_HALFWIDTH;
                    else
                        result.coord_x = TextCoordAlign::TEXTCOORD_PLUS_HALFWIDTH;
                    result.coord_y = TextCoordAlign::TEXTCOORD_NOOP_HEIGHT;
                }
                else
                {
                    if (!result.is_reverse)
                    {
                        result.coord_x = TextCoordAlign::TEXTCOORD_NOOP_HEIGHT; //
                        result.coord_y = TextCoordAlign::TEXTCOORD_MINUS_HALFWIDTH; //
                    }
                    else
                    {
                        result.coord_x = TextCoordAlign::TEXTCOORD_MINUS_HALFHEIGHT;
                        result.coord_y = TextCoordAlign::TEXTCOORD_MINUS_WIDTH;
                    }
                }
                break;
            case TextAlign::TEXT_CENTER_UP:
                if (result.is_horizontal)
                {
                    if (!result.is_reverse)
                    {
                        result.coord_x = TextCoordAlign::TEXTCOORD_MINUS_HALFWIDTH;
                        result.coord_y = TextCoordAlign::TEXTCOORD_MINUS_HEIGHT;
                    }
                    else
                    {
                        result.coord_x = TextCoordAlign::TEXTCOORD_PLUS_HALFWIDTH;
                        result.coord_y = TextCoordAlign::TEXTCOORD_PLUS_HEIGHT;
                    }
                }
                else
                {
                    if (!result.is_reverse)
                    {
                        result.coord_x = TextCoordAlign::TEXTCOORD_PLUS_HEIGHT;
                        result.coord_y = TextCoordAlign::TEXTCOORD_MINUS_HALFWIDTH;
                    }
                    else
                    {
                        result.coord_x = TextCoordAlign::TEXTCOORD_MINUS_HEIGHT;
                        result.coord_y = TextCoordAlign::TEXTCOORD_PLUS_HALFWIDTH;
                    }
                }
                break;
            case TextAlign::TEXT_CENTER_CENTER:
                if (result.is_horizontal)
                {
                    result.coord_x = TextCoordAlign::TEXTCOORD_PLUS_HALFWIDTH;
                    result.coord_y = TextCoordAlign::TEXTCOORD_PLUS_HALFHEIGHT;
                }
                else
                {
                    if (!result.is_reverse)
                    {
                        result.coord_x = TextCoordAlign::TEXTCOORD_PLUS_HALFHEIGHT;
                        result.coord_y = TextCoordAlign::TEXTCOORD_MINUS_HALFWIDTH;
                    }
                    else
                    {
                        result.coord_x = TextCoordAlign::TEXTCOORD_MINUS_HALFHEIGHT;
                        result.coord_y = TextCoordAlign::TEXTCOORD_PLUS_HALFWIDTH;
                    }
                }
                break;
            case TextAlign::TEXT_LEFT_DOWN:
                if (result.is_horizontal)
                {
                    result.coord_x = TextCoordAlign::TEXTCOORD_NOOP_WIDTH;
                    result.coord_y = TextCoordAlign::TEXTCOORD_NOOP_HEIGHT;
                }
                else
                {
                    result.coord_x = TextCoordAlign::TEXTCOORD_NOOP_HEIGHT;
                    result.coord_y = TextCoordAlign::TEXTCOORD_NOOP_WIDTH;
                }
                break;
            case TextAlign::TEXT_LEFT_UP:
                if (result.is_horizontal)
                {
                    result.coord_x = TextCoordAlign::TEXTCOORD_NOOP_WIDTH;
                    if (!result.is_reverse)
                        result.coord_y = TextCoordAlign::TEXTCOORD_MINUS_HEIGHT;
                    else
                        result.coord_y = TextCoordAlign::TEXTCOORD_PLUS_HEIGHT;
                }
                else
                {
                    if (!result.is_reverse)
                        result.coord_x = TextCoordAlign::TEXTCOORD_PLUS_HEIGHT;
                    else
                        result.coord_x = TextCoordAlign::TEXTCOORD_MINUS_HEIGHT;
                    result.coord_y = TextCoordAlign::TEXTCOORD_NOOP_WIDTH;
                }
                break;
            case TextAlign::TEXT_LEFT_CENTER:
                if (result.is_horizontal)
                {
                    result.coord_x = TextCoordAlign::TEXTCOORD_NOOP_WIDTH;
                    if (!result.is_reverse)
                        result.coord_y = TextCoordAlign::TEXTCOORD_MINUS_HALFHEIGHT;
                    else
                        result.coord_y = TextCoordAlign::TEXTCOORD_PLUS_HALFHEIGHT;
                }
                else
                {
                    if (!result.is_reverse)
                        result.coord_x = TextCoordAlign::TEXTCOORD_PLUS_HALFHEIGHT;
                    else
                        result.coord_x = TextCoordAlign::TEXTCOORD_MINUS_HALFHEIGHT;
                    result.coord_y = TextCoordAlign::TEXTCOORD_NOOP_WIDTH;
                }
                break;
            case TextAlign::TEXT_RIGHT_DOWN:
                if (result.is_horizontal)
                {
                    if (!result.is_reverse)
                        result.coord_x = TextCoordAlign::TEXTCOORD_MINUS_WIDTH;
                    else
                        result.coord_x = TextCoordAlign::TEXTCOORD_PLUS_WIDTH;
                    result.coord_y = TextCoordAlign::TEXTCOORD_NOOP_HEIGHT;
                }
                else
                {
                    result.coord_x = TextCoordAlign::TEXTCOORD_NOOP_HEIGHT;
                    if (!result.is_reverse)
                        result.coord_y = TextCoordAlign::TEXTCOORD_MINUS_WIDTH;
                    else
                        result.coord_y = TextCoordAlign::TEXTCOORD_PLUS_WIDTH;
                }
                break;
            case TextAlign::TEXT_RIGHT_UP:
                if (result.is_horizontal)
                {
                    if (!result.is_reverse)
                    {
                        result.coord_x = TextCoordAlign::TEXTCOORD_MINUS_WIDTH;
                        result.coord_y = TextCoordAlign::TEXTCOORD_MINUS_HEIGHT;
                    }
                    else
                    {
                        result.coord_x = TextCoordAlign::TEXTCOORD_PLUS_WIDTH;
                        result.coord_y = TextCoordAlign::TEXTCOORD_PLUS_HEIGHT;
                    }
                }
                else
                {
                    if (!result.is_reverse)
                    {
                        result.coord_x = TextCoordAlign::TEXTCOORD_PLUS_HEIGHT;
                        result.coord_y = TextCoordAlign::TEXTCOORD_MINUS_WIDTH;
                    }
                    else
                    {
                        result.coord_x = TextCoordAlign::TEXTCOORD_MINUS_HEIGHT;
                        result.coord_y = TextCoordAlign::TEXTCOORD_PLUS_WIDTH;
                    }
                }
                break;
            case TextAlign::TEXT_RIGHT_CENTER: // Точка привязки текста справа по центру
                if (result.is_horizontal)
                {
                    if (!result.is_reverse)
                    {
                        result.coord_x = TextCoordAlign::TEXTCOORD_MINUS_WIDTH;
                        result.coord_y = TextCoordAlign::TEXTCOORD_MINUS_HALFHEIGHT;
                    }
                    else
                    {
                        result.coord_x = TextCoordAlign::TEXTCOORD_PLUS_WIDTH;
                        result.coord_y = TextCoordAlign::TEXTCOORD_PLUS_HALFHEIGHT;
                    }
                }
                else
                {
                    if (!result.is_reverse)
                    {
                        result.coord_x = TextCoordAlign::TEXTCOORD_PLUS_HALFHEIGHT;
                        result.coord_y = TextCoordAlign::TEXTCOORD_MINUS_WIDTH;
                    }
                    else
                    {
                        result.coord_x = TextCoordAlign::TEXTCOORD_MINUS_HALFHEIGHT;
                        result.coord_y = TextCoordAlign::TEXTCOORD_PLUS_WIDTH;
                    }
                }
                break;
        }

        if (result.is_mirror)
        {
            if (!result.is_horizontal)
            {
                switch (result.coord_x)
                { // Текст вертикальный, координата x отражается, исходя из высоты текста
                    case TextCoordAlign::TEXTCOORD_MINUS_HEIGHT:
                        result.coord_x = TextCoordAlign::TEXTCOORD_NOOP_HEIGHT;
                        break;
                    case TextCoordAlign::TEXTCOORD_MINUS_HALFHEIGHT:
                        if (!result.is_reverse)
                        {
                            result.coord_x = TextCoordAlign::TEXTCOORD_MINUS_HEIGHT;
                        }
                        else
                        {
                            if (text_align == TextAlign::TEXT_CENTER_DOWN)
                                result.coord_x = TextCoordAlign::TEXTCOORD_MINUS_HEIGHT;
                            else
                                result.coord_x = TextCoordAlign::TEXTCOORD_MINUS_HALFHEIGHT;
                        }
                        break;
                    case TextCoordAlign::TEXTCOORD_NOOP_HEIGHT:
                        if (!result.is_reverse)
                            result.coord_x = TextCoordAlign::TEXTCOORD_PLUS_HEIGHT;
                        else
                            result.coord_x = TextCoordAlign::TEXTCOORD_MINUS_HEIGHT;
                        break;
                    case TextCoordAlign::TEXTCOORD_PLUS_HALFHEIGHT:
                        result.coord_x = TextCoordAlign::TEXTCOORD_PLUS_HALFHEIGHT;
                        break;
                    case TextCoordAlign::TEXTCOORD_PLUS_HEIGHT:
                        result.coord_x = TextCoordAlign::TEXTCOORD_NOOP_HEIGHT;
                        break;
                    default:
                        break;
                }

                switch (result.coord_y)
                { // Текст вертикальный, координата y отражается, исходя из ширины текста
                    case TextCoordAlign::TEXTCOORD_MINUS_WIDTH:
                        if (!result.is_reverse)
                            result.coord_y = TextCoordAlign::TEXTCOORD_MINUS_WIDTH;
                        else
                            result.coord_y = TextCoordAlign::TEXTCOORD_PLUS_HALFWIDTH;
                        break;
                    case TextCoordAlign::TEXTCOORD_MINUS_HALFWIDTH:
                        result.coord_y = TextCoordAlign::TEXTCOORD_MINUS_HALFWIDTH;
                        break;
                    case TextCoordAlign::TEXTCOORD_PLUS_HALFWIDTH:
                        result.coord_y = TextCoordAlign::TEXTCOORD_PLUS_HALFWIDTH;
                        break;
                    case TextCoordAlign::TEXTCOORD_PLUS_WIDTH:
                        result.coord_y = TextCoordAlign::TEXTCOORD_PLUS_WIDTH;
                        break;
                    default:
                        break;
                }
            }
            else
            {
                switch (result.coord_x)
                {
                    case TextCoordAlign::TEXTCOORD_MINUS_WIDTH:
                        break;
                    case TextCoordAlign::TEXTCOORD_MINUS_HALFWIDTH:
                        if (!result.is_reverse)
                            result.coord_x = TextCoordAlign::TEXTCOORD_MINUS_HALFWIDTH;
                        else
                            result.coord_x = TextCoordAlign::TEXTCOORD_PLUS_HALFWIDTH;
                        break;
                    case TextCoordAlign::TEXTCOORD_PLUS_HALFWIDTH:
                        if (!result.is_reverse)
                            result.coord_x = TextCoordAlign::TEXTCOORD_MINUS_HALFWIDTH;
                        else
                            result.coord_x = TextCoordAlign::TEXTCOORD_PLUS_HALFWIDTH;
                        break;
                    case TextCoordAlign::TEXTCOORD_PLUS_WIDTH:
                        result.coord_x = TextCoordAlign::TEXTCOORD_PLUS_WIDTH;
                        break;
                    default:
                        break;
                }

                switch (result.coord_y)
                {
                    case TextCoordAlign::TEXTCOORD_MINUS_HEIGHT:
                        result.coord_y = TextCoordAlign::TEXTCOORD_NOOP_HEIGHT;
                        break;
                    case TextCoordAlign::TEXTCOORD_MINUS_HALFHEIGHT:
                        result.coord_y = TextCoordAlign::TEXTCOORD_MINUS_HALFHEIGHT;
                        break;
                    case TextCoordAlign::TEXTCOORD_NOOP_HEIGHT:
                        if (!result.is_reverse)
                            result.coord_y = TextCoordAlign::TEXTCOORD_MINUS_HEIGHT;
                        else
                            result.coord_y = TextCoordAlign::TEXTCOORD_PLUS_HEIGHT;
                        break;
                    case TextCoordAlign::TEXTCOORD_PLUS_HALFHEIGHT:
                        if (!result.is_reverse)
                            result.coord_y = TextCoordAlign::TEXTCOORD_MINUS_HALFHEIGHT;
                        else
                            result.coord_y = TextCoordAlign::TEXTCOORD_PLUS_HALFHEIGHT;
                        break;
                    case TextCoordAlign::TEXTCOORD_PLUS_HEIGHT:
                        result.coord_y = TextCoordAlign::TEXTCOORD_NOOP_HEIGHT; //
                        break;
                    default:
                        break;
                }
            }
        }

        return result;
    }

    wxPoint GetCorrectedTextPos(TextSignature text_signat, wxPoint text_pos, wxSize text_size)
    {
        wxPoint text_pos_corr;

        switch (text_signat.coord_x)
        {
            case TextCoordAlign::TEXTCOORD_NOOP_WIDTH:
            case TextCoordAlign::TEXTCOORD_NOOP_HEIGHT:
                text_pos_corr.x = text_pos.x;
                break;
            case TextCoordAlign::TEXTCOORD_PLUS_HALFWIDTH:
                text_pos_corr.x = text_pos.x + text_size.GetWidth() / 2;
                break;
            case TextCoordAlign::TEXTCOORD_PLUS_WIDTH:
                text_pos_corr.x = text_pos.x + text_size.GetWidth();
                break;
            case TextCoordAlign::TEXTCOORD_MINUS_HALFWIDTH:
                text_pos_corr.x = text_pos.x - text_size.GetWidth() / 2;
                break;
            case TextCoordAlign::TEXTCOORD_MINUS_WIDTH:
                text_pos_corr.x = text_pos.x - text_size.GetWidth();
                break;
            case TextCoordAlign::TEXTCOORD_PLUS_HALFHEIGHT:
                text_pos_corr.x = text_pos.x + text_size.GetHeight() / 2;
                break;
            case TextCoordAlign::TEXTCOORD_PLUS_HEIGHT:
                text_pos_corr.x = text_pos.x + text_size.GetHeight();
                break;
            case TextCoordAlign::TEXTCOORD_MINUS_HALFHEIGHT:
                text_pos_corr.x = text_pos.x - text_size.GetHeight() / 2;
                break;
            case TextCoordAlign::TEXTCOORD_MINUS_HEIGHT:
                text_pos_corr.x = text_pos.x - text_size.GetHeight();
                break;
        }

        switch (text_signat.coord_y)
        {
            case TextCoordAlign::TEXTCOORD_NOOP_WIDTH:
            case TextCoordAlign::TEXTCOORD_NOOP_HEIGHT:
                text_pos_corr.y = text_pos.y;
                break;
            case TextCoordAlign::TEXTCOORD_PLUS_HALFWIDTH:
                text_pos_corr.y = text_pos.y + text_size.GetWidth() / 2;
                break;
            case TextCoordAlign::TEXTCOORD_PLUS_WIDTH:
                text_pos_corr.y = text_pos.y + text_size.GetWidth();
                break;
            case TextCoordAlign::TEXTCOORD_MINUS_HALFWIDTH:
                text_pos_corr.y = text_pos.y - text_size.GetWidth() / 2;
                break;
            case TextCoordAlign::TEXTCOORD_MINUS_WIDTH:
                text_pos_corr.y = text_pos.y - text_size.GetWidth();
                break;
            case TextCoordAlign::TEXTCOORD_PLUS_HALFHEIGHT:
                text_pos_corr.y = text_pos.y + text_size.GetHeight() / 2;
                break;
            case TextCoordAlign::TEXTCOORD_PLUS_HEIGHT:
                text_pos_corr.y = text_pos.y + text_size.GetHeight();
                break;
            case TextCoordAlign::TEXTCOORD_MINUS_HALFHEIGHT:
                text_pos_corr.y = text_pos.y - text_size.GetHeight() / 2;
                break;
            case TextCoordAlign::TEXTCOORD_MINUS_HEIGHT:
                text_pos_corr.y = text_pos.y - text_size.GetHeight();
                break;
        }

        return text_pos_corr;
    }

    wxRect GetCorrectedTextRect(TextSignature text_signat, wxPoint text_pos_corr, wxSize text_size)
    { // Функция рассчитывает положение и размер прямоугольника, описанного вокруг текста с
      // сигнатурой text_signat, начальной точкой вывода text_pos и размером text_size.
        wxRect result;
        if (abs(text_signat.text_angle) < ZERO_TOLERANCE)
            // Вывод текста слева направо (угол 0 градусов)
            return wxRect(text_pos_corr, text_size);
        else if (abs(text_signat.text_angle - 90) < ZERO_TOLERANCE)
            // Вывод текста снизу вверх (угол 90 градусов)
            return wxRect(text_pos_corr.x, text_pos_corr.y - text_size.GetWidth(),
                          text_size.GetHeight(), text_size.GetWidth());
        else if (abs(text_signat.text_angle - 180) < ZERO_TOLERANCE)
            // Вывод текста справа налево (угол 180 градусов)
            return wxRect(text_pos_corr.x - text_size.GetWidth(), text_pos_corr.y - text_size.GetHeight(),
                          text_size.GetWidth(), text_size.GetHeight());
        else if (abs(text_signat.text_angle - 270) < ZERO_TOLERANCE)
            // Вывод текста сверху вниз (угол 270 градусов)
            return wxRect(text_pos_corr.x - text_size.GetHeight(), text_pos_corr.y,
                          text_size.GetHeight(), text_size.GetWidth());
        else
            // Прочие варианты, которые не должны встречаться
            return wxRect(text_pos_corr, text_size);

        return result;
    }

    std::pair<svg::TextAnchorType, svg::StringPoint> GetSVGCorrTextParams(TextSignature& text_signat)
    {
        svg::TextAnchorType text_anchor;
        svg::StringPoint offset_point;

        switch (text_signat.text_align)
        {
            case TextAlign::TEXT_CENTER_DOWN:
                offset_point = svg::StringPoint("0"s, "1em"s);
                text_anchor = svg::TextAnchorType::TEXT_ANCHOR_MIDDLE;
                break;
            case TextAlign::TEXT_CENTER_UP:
                offset_point = svg::StringPoint("0"s, "0"s);
                text_anchor = svg::TextAnchorType::TEXT_ANCHOR_MIDDLE;
                break;
            case TextAlign::TEXT_CENTER_CENTER:
                offset_point = svg::StringPoint("0"s, "0.5em"s);
                text_anchor = svg::TextAnchorType::TEXT_ANCHOR_MIDDLE;
                break;
            case TextAlign::TEXT_LEFT_DOWN:
                offset_point = svg::StringPoint("0"s, "1em"s);
                text_anchor = svg::TextAnchorType::TEXT_ANCHOR_START;
                break;
            case TextAlign::TEXT_LEFT_UP:
                offset_point = svg::StringPoint("0"s, "0"s);
                text_anchor = svg::TextAnchorType::TEXT_ANCHOR_START;
                break;
            case TextAlign::TEXT_LEFT_CENTER:
                offset_point = svg::StringPoint("0"s, "0.5em"s);
                text_anchor = svg::TextAnchorType::TEXT_ANCHOR_START;
                break;
            case TextAlign::TEXT_RIGHT_DOWN:
                offset_point = svg::StringPoint("0"s, "1em"s);
                text_anchor = svg::TextAnchorType::TEXT_ANCHOR_END;
                break;
            case TextAlign::TEXT_RIGHT_UP:
                offset_point = svg::StringPoint("0"s, "0"s);
                text_anchor = svg::TextAnchorType::TEXT_ANCHOR_END;
                break;
            case TextAlign::TEXT_RIGHT_CENTER:
                offset_point = svg::StringPoint("0"s, "0.5em"s);
                text_anchor = svg::TextAnchorType::TEXT_ANCHOR_END;
                break;
        }

        if (text_signat.is_mirror)
        {
            if (abs(text_signat.text_angle) < ZERO_TOLERANCE)
                text_signat.text_angle = 180;
            else if (abs(text_signat.text_angle - 90) < ZERO_TOLERANCE)
                text_signat.text_angle = 270;
            else if (abs(text_signat.text_angle - 180) < ZERO_TOLERANCE)
                text_signat.text_angle = 0;
            else if (abs(text_signat.text_angle - 270) < ZERO_TOLERANCE)
                text_signat.text_angle = 90;
        }

        return {text_anchor, offset_point};
    }
} // namespace

const wxSize StdWXObjects::std_canvas_size[] =
    {wxSize(0, 0), wxSize(841, 1189), wxSize(594, 841),
     wxSize(420, 594), wxSize(297, 420), wxSize(210, 297), wxSize(148, 210),
     wxSize(105, 148), wxSize(74, 105), wxSize(52, 74)};

const wxSize StdWXObjects::MinCanvasSizeInMM = wxSize(1, 1);
const wxSize StdWXObjects::MinCanvasSizeInPoints = wxSize(1, 1);
const wxSize StdWXObjects::MaxCanvasSizeInMM = wxSize(1500, 1500);
const wxSize StdWXObjects::MaxCanvasSizeInPoints =
    wxSize(ceil(1500 * POINTS_IN_MILLIMETER), ceil(1500 * POINTS_IN_MILLIMETER));

StdWXObjects std_wx_objects;

void DrawContext::InitDrawContextByColor(const SelColorType& set_color)
{
    if (holds_alternative<unsigned char>(set_color))
    {
        size_t col_num = get<unsigned char>(set_color);
        size_t col_num_pos = min(col_num, std_wx_objects.std_colors.size() - 1);
        if (color == std_wx_objects.std_colors[col_num_pos])
            return;
        // Запоминаем инструменты прямого цвета
        color = std_wx_objects.std_colors[col_num_pos];
        pen_1 = std_wx_objects.std_pens_1[col_num_pos];
        brush_solid = std_wx_objects.std_brushes_solid[col_num_pos];
        brush_hatch = std_wx_objects.std_brushes_hatch[col_num_pos];
        // Запоминаем инструменты обращённого цвета
        inv_color = std_wx_objects.inv_std_colors[col_num_pos];;
        inv_pen_1 = std_wx_objects.inv_std_pens_1[col_num_pos];
        inv_brush_solid = std_wx_objects.inv_std_brushes_solid[col_num_pos];
        inv_brush_hatch = std_wx_objects.inv_std_brushes_hatch[col_num_pos];
    }
    else
    {   // Формируем инструменты прямого цвета
        if (color == get<wxColor>(set_color))
            return;
        color = get<wxColor>(set_color);
        pen_1 = wxPen(color);
        brush_solid = wxBrush(color);
        brush_hatch = wxBrush(color, wxCROSS_HATCH);
        // Формируем инструменты обращённого цвета
        inv_color = StdWXObjects::InverseColor(color);
        inv_pen_1 = wxPen(inv_color);
        inv_brush_solid = wxBrush(inv_color);
        inv_brush_hatch = wxBrush(inv_color, wxCROSS_HATCH);
    }
}

void DrawContext::InitColorPenBrush()
{   // Инициализируем начальными значениями поля цветов и инструментов контекста
    InitDrawContextByColor(*wxWHITE);
}

DrawContext::DrawContext()
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);

    pcad_doc_ptr = &this_app->pcad_file;
    target_dc_ptr = nullptr;
    target_dc_size = wxSize(0, 0);
    InitColorPenBrush();
}

DrawContext::DrawContext(wxDC* arg_target_dc_ptr, bool is_create_recognize_dc)
{
    PCADViewerApp* this_app = static_cast<PCADViewerApp*>(wxTheApp);

    // Инициализируем значащие поля контекста соответствующими значениями
    pcad_doc_ptr = &this_app->pcad_file;
    target_dc_ptr = arg_target_dc_ptr;
    target_dc_size = arg_target_dc_ptr->GetSize();
    InitColorPenBrush();
    if (is_create_recognize_dc)
    { // Если требуется, создаём в контексте идентификационную поверхность
        // Создаём новую вспомогательную ("идентификационную") поверхность рисования нужного размера.
        recognize_bitmap = wxBitmap(target_dc_size.GetWidth(), target_dc_size.GetHeight(), 32);
        // Привязываем к контексту новую, только что созданную поверхность.
        recognize_dc.SelectObject(recognize_bitmap);
    }
}

void DrawContext::Copier(const DrawContext& rhs)
{ // Копируем в целевую переменную значащие поля переменной-источника.
    pcad_doc_ptr = rhs.pcad_doc_ptr;
    target_dc_ptr = rhs.target_dc_ptr;
    target_dc_size = rhs.target_dc_size;
    context_font = rhs.context_font;
    is_use_selected = rhs.is_use_selected;
    color = rhs.color;
    inv_color = rhs.inv_color;
    background_color = rhs.background_color;
    pen_1 = rhs.pen_1;
    inv_pen_1 = rhs.inv_pen_1;
    pen_width = rhs.pen_width;
    inv_pen_width = rhs.inv_pen_width;
    brush_solid = rhs.brush_solid;
    inv_brush_solid = rhs.inv_brush_solid;
    brush_hatch = rhs.brush_hatch;
    inv_brush_hatch = rhs.inv_brush_hatch;
    brush_temp = rhs.brush_temp;
    pen_recognize = rhs.pen_recognize;
    brush_recognize = rhs.brush_recognize;
    background_brush_recognize = rhs.background_brush_recognize;
    scale_x = rhs.scale_x;
    scale_y = rhs.scale_y;
    project_rect = rhs.project_rect;
    clipping_rect = rhs.clipping_rect;
}

DrawContext::DrawContext(const DrawContext& rhs)
{
    Copier(rhs);
    // Если в источнике есть непустая идентификационная поверхность,
    // воссоздаём аналогичный новый её экземпляр в целевой переменной.
    if (rhs.recognize_bitmap.IsOk() && rhs.recognize_bitmap.GetWidth() &&
        rhs.recognize_bitmap.GetHeight())
    {
        recognize_bitmap = wxBitmap(target_dc_size.GetWidth(), target_dc_size.GetHeight(), 32);
        recognize_dc.SelectObject(recognize_bitmap);
    }
}

DrawContext::DrawContext(DrawContext&& rhs)
{
    Copier(rhs);
    // Если в источнике есть непустая идентификационная поверхность,
    // переносим её к нам.
    if (rhs.recognize_bitmap.IsOk() && rhs.recognize_bitmap.GetWidth() &&
        rhs.recognize_bitmap.GetHeight())
    {
        recognize_bitmap = move(rhs.recognize_bitmap);
        recognize_dc.SelectObject(recognize_bitmap);
    }
}

DrawContext& DrawContext::operator=(const DrawContext& rhs)
{
    if (this != &rhs)
    {
        recognize_dc.SelectObject(wxNullBitmap);
        recognize_bitmap = wxNullBitmap;
        Copier(rhs);
        // Если в источнике есть непустая идентификационная поверхность,
        // воссоздаём аналогичный новый её экземпляр в целевой переменной.
        if (rhs.recognize_bitmap.IsOk() && rhs.recognize_bitmap.GetWidth() &&
            rhs.recognize_bitmap.GetHeight())
        {
            recognize_bitmap = wxBitmap(target_dc_size.GetWidth(), target_dc_size.GetHeight(), 32);
            recognize_dc.SelectObject(recognize_bitmap);
        }
    }
    return *this;
}

void DrawContext::ResetProjectRect(const wxRect& new_project_rect)
{
    project_rect = new_project_rect;
    if (!project_rect.IsEmpty())
    { // Расчёт текущих масштабов по осям X и Y, исходя из текущей области отсечения
      // и размера целевой поверхности рисования.
        scale_x = static_cast<double>(target_dc_size.GetWidth()) / project_rect.GetWidth();
        scale_y = static_cast<double>(target_dc_size.GetHeight()) / project_rect.GetHeight();
    }
}

void DrawContext::ResetTargetSize(wxSize new_target_size, bool is_from_target_dc)
{
    if (is_from_target_dc)
        target_dc_size = target_dc_ptr->GetSize();
    else
        target_dc_size = new_target_size;

    if (!project_rect.IsEmpty())
    { // Расчёт текущих масштабов по осям X и Y, исходя из текущей области отсечения
      // и размера целевой поверхности рисования.
        scale_x = static_cast<double>(target_dc_size.GetWidth()) / project_rect.GetWidth();
        scale_y = static_cast<double>(target_dc_size.GetHeight()) / project_rect.GetHeight();
    }
}

void DrawContext::ResetTargetDC(wxDC* arg_target_dc_ptr, bool is_create_recognize_dc)
{
    if (!is_create_recognize_dc)
    {
        recognize_dc.SelectObject(wxNullBitmap);
        recognize_bitmap = wxNullBitmap;
    }
    target_dc_ptr = arg_target_dc_ptr;
    target_dc_size = arg_target_dc_ptr->GetSize();
    if (!project_rect.IsEmpty())
    { // Расчёт текущих масштабов по осям X и Y, исходя из текущей области отсечения
      // и размера целевой поверхности рисования.
        scale_x = static_cast<double>(target_dc_size.GetWidth()) / project_rect.GetWidth();
        scale_y = static_cast<double>(target_dc_size.GetHeight()) / project_rect.GetHeight();
    }

    if (is_create_recognize_dc)
    { // Требуется пересоздать идентификационную поверхность
        if (recognize_bitmap.IsOk() && recognize_bitmap.GetWidth() == target_dc_size.GetWidth() &&
            recognize_bitmap.GetHeight() == target_dc_size.GetHeight())
            return;
        recognize_dc.SelectObject(wxNullBitmap);
        recognize_bitmap = wxNullBitmap;
        recognize_bitmap = wxBitmap(target_dc_size.GetWidth(), target_dc_size.GetHeight(), 32);
        recognize_dc.SelectObject(recognize_bitmap);
    }
}

void DrawContext::ResetRecognizeDC(wxSize new_dc_size)
{
    recognize_dc.SelectObject(wxNullBitmap);
    recognize_bitmap = wxBitmap(new_dc_size.GetWidth(), new_dc_size.GetHeight(), 32);
    recognize_dc.SelectObject(recognize_bitmap);
}

wxPoint DrawContext::ConvertPointToDevice(wxPoint pict_point)
{ // Преобразует точку в координатном пространстве PCAD-изображения в точку в системе координат целевой поверхности target_dc.
    wxPoint result;
    double ratio_x = 0, ratio_y = 0;
    if (project_rect.width > 1)
        ratio_x = static_cast<double>(pict_point.x - project_rect.x) / (project_rect.width - 1);
    if (project_rect.height > 1)
        ratio_y = static_cast<double>(pict_point.y - project_rect.y) / (project_rect.height - 1);
    result.x = (target_dc_size.GetWidth() - 1) * ratio_x;
    result.y = (target_dc_size.GetHeight() - 1) * ratio_y;
    return result;
}

wxRect DrawContext::ConvertRectToDevice(wxRect pict_rect)
{
    wxPoint point_lh = ConvertPointToDevice(wxPoint{pict_rect.x, pict_rect.y});
    wxPoint point_rd = ConvertPointToDevice(wxPoint{pict_rect.x + pict_rect.width - 1,
                                                    pict_rect.y + pict_rect.height - 1});
    return {point_lh, point_rd};
}

wxPoint DrawContext::ConvertPointFromDevice(wxPoint pict_point)
{ // Преобразует точку на целевой поверхности target_dc в точку в координатном пространстве PCAD-изображения.
    wxPoint result;
    double ratio_x = 0, ratio_y = 0;
    if (project_rect.width > 1)
        ratio_x = static_cast<double>(pict_point.x) / (target_dc_size.GetWidth() - 1);
    if (project_rect.height > 1)
        ratio_y = static_cast<double>(pict_point.y) / (target_dc_size.GetHeight() - 1);
    result.x = project_rect.x + (project_rect.width - 1) * ratio_x;
    result.y = project_rect.y + (project_rect.height - 1) * ratio_y;
    return result;
}

wxRect DrawContext::ConvertRectFromDevice(wxRect pict_rect)
{
    wxPoint point_lh = ConvertPointFromDevice(wxPoint{pict_rect.x, pict_rect.y});
    wxPoint point_rd = ConvertPointFromDevice(wxPoint{pict_rect.x + pict_rect.width - 1,
                                                      pict_rect.y + pict_rect.height - 1});
    return {point_lh, point_rd};
}

ObjSelColor::ObjSelColor(int layer_number, unsigned char set_color, const FileDefValues& file_values) :
                         GraphObj(layer_number), set_color_(set_color)
{
    frame_rect_ = file_values.frame_rect;
}

ObjSelColor::ObjSelColor(int layer_number, wxColor set_color, const FileDefValues& file_values) :
                         GraphObj(layer_number), set_color_(set_color)
{
    frame_rect_ = file_values.frame_rect;
}

ObjLine::ObjLine(int layer_number, LineType line_type, int line_width, wxPoint point1, wxPoint point2) :
        ContourGraphObj(layer_number, line_type, line_width), point1_(point1), point2_(point2)
{
    frame_rect_ = wxRect(min(point1_.x, point2_.x), min(point1_.y, point2_.y),
                    abs(point1_.x - point2_.x) + 1, abs(point1_.y - point2_.y) + 1);
    frame_rect_.Inflate(line_width_ / 2);
}

ObjRect::ObjRect(int layer_number, LineType line_type, int line_width, wxRect rect) :
    ContourGraphObj(layer_number, line_type, line_width), rect_(rect)
{
    frame_rect_ = rect_;
    frame_rect_.Inflate(line_width_ / 2);
}

ObjCirc::ObjCirc(int layer_number, LineType line_type, int line_width, wxPoint center, int radius) :
    ContourGraphObj(layer_number, line_type, line_width), center_(center), radius_(radius)
{
    frame_rect_ = wxRect(center_.x - radius_, center_.y - radius_, 2 * radius_, 2 * radius_);
    frame_rect_.Inflate(line_width_ / 2);
}

ObjArc::ObjArc(int layer_number, LineType line_type, int line_width, wxPoint arc_center,
               wxPoint arc_begin, wxPoint arc_end) :
    ContourGraphObj(layer_number, line_type, line_width),
    arc_center_(arc_center), arc_begin_(arc_begin), arc_end_(arc_end)
{
    double radius_arc = sqrt((arc_begin_.x - arc_center_.x) * (arc_begin_.x - arc_center_.x) +
                             (arc_center_.y - arc_begin_.y) * (arc_center_.y - arc_begin_.y));
    //double start_angle = atan2(arc_center_.y - arc_begin_.y, arc_begin_.x - arc_center_.x);
    double end_angle = atan2(arc_center_.y - arc_end_.y, arc_end_.x - arc_center_.x);

    arc_end_ = wxPoint(arc_center_.x + radius_arc * cos(end_angle), arc_center_.y - radius_arc * sin(end_angle));

    frame_rect_ = CountArcBoundRect(arc_center_, arc_begin_, arc_end_, line_width_, line_width_);
}

ObjText::ObjText(int layer_number, wxPoint text_point, TextOrientation text_orientation,
                 int text_height, TextAlign text_align, std::string text) :
    GraphObj(layer_number), text_point_(text_point), text_orientation_(text_orientation),
    text_height_(text_height), text_align_(text_align), text_(std::move(text))
{
    TextSignature text_signat(GetTextSignature(text_orientation_, text_align_));
    wxString wx_text(text_.c_str(), wxConvUTF8);
    wxFont text_font(wxSize(text_height_, 0), wxFontFamily::wxFONTFAMILY_ROMAN,
                     wxFontStyle::wxFONTSTYLE_NORMAL, wxFontWeight::wxFONTWEIGHT_NORMAL);
    wxBitmap wx_bitmap(10, 10);
    wxMemoryDC memoDC(wx_bitmap);
    memoDC.SetFont(text_font);
    wxSize text_size(memoDC.GetTextExtent(wx_text));
    wxPoint text_point_corr = GetCorrectedTextPos(text_signat, text_point_, text_size);
    frame_rect_ = GetCorrectedTextRect(text_signat, text_point_corr, text_size);
}

void ObjFlash::RecountFrameRect(const FileDefValues& file_values, const ApertureProvider& aperture_provider)
{
    FlashDesc fd = aperture_provider.GetFlashDesc(aperture_number_);
    double aperture_size;
    if (file_values.file_flags & FILE_FLAG_UNIT_INCHES)
        aperture_size = fd.flash_size_inch * file_values.DBU_in_measure_unit;
    else
        aperture_size = fd.flash_size_mm * file_values.DBU_in_measure_unit;

    wxPoint flash_left_up(flash_point_.x - aperture_size / 2,
                          flash_point_.y - aperture_size / 2);
    frame_rect_ = wxRect(flash_left_up, wxSize(aperture_size, aperture_size));
}

ObjFlash::ObjFlash(int layer_number, wxPoint flash_point, FlashOrientation flash_orientation, int aperture_number,
                   const FileDefValues& file_values, const ApertureProvider& aperture_provider) :
                   GraphObj(layer_number), aperture_number_(aperture_number), flash_point_(flash_point),
                   flash_orientation_(flash_orientation)
{
    RecountFrameRect(file_values, aperture_provider);
}


ObjPoly::ObjPoly(int layer_number, FillType hatch_type, int aperture_width, vector<wxPoint> points,
                 vector<ObjCVoid> cvoids, vector<ObjPVoid> pvoids) :
                 GraphObj(layer_number), hatch_type_(hatch_type), aperture_width_(aperture_width),
                 points_(move(points)), cvoids_(move(cvoids)), pvoids_(move(pvoids))
{
    frame_rect_ = wxRect();
    for (wxPoint wxp : points_)
        frame_rect_.Union(wxRect(wxp.x, wxp.y, 1, 1));
    frame_rect_.Inflate(aperture_width_ / 2);
}

ObjCVoid::ObjCVoid(wxPoint void_center, int void_radius) :
         GraphObj(-1), void_center_(void_center), void_radius_(void_radius)
{
    frame_rect_ = wxRect(void_center_.x - void_radius_, void_center_.y - void_radius_,
                         2 * void_radius_, 2 * void_radius_);
}

ObjPVoid::ObjPVoid(std::vector<wxPoint> points) : GraphObj(-1), points_(std::move(points))
{
    frame_rect_ = wxRect();
    for (wxPoint wxp : points_)
        frame_rect_.Union(wxRect(wxp.x, wxp.y, 1, 1));
}

StdWXObjects::StdWXObjects()
{
    for (wxColor wxc : PCAD_palette_color)
    {
        std_colors.push_back(wxc);
        std_pens_1.push_back(wxPen(wxc));
        std_brushes_solid.push_back(wxBrush(wxc));
        std_brushes_hatch.push_back(wxBrush(wxc, wxCROSS_HATCH));
        wxColor inv_wxc = InverseColor(wxc);
        inv_std_colors.push_back(inv_wxc);
        inv_std_pens_1.push_back(wxPen(inv_wxc));
        inv_std_brushes_solid.push_back(wxBrush(inv_wxc));
        inv_std_brushes_hatch.push_back(wxBrush(inv_wxc, wxCROSS_HATCH));
    }
}

void GraphObj::InitDrawContextByLayerNum(DrawContext& draw_context) const
{
    if (layer_number_ < 0 || layer_number_ >= static_cast<int>(draw_context.pcad_doc_ptr->layers_size()))
        return;
    auto layer_it = draw_context.pcad_doc_ptr->layers_begin() + layer_number_;
    draw_context.InitDrawContextByColor(layer_it->layer_wx_color);
}

void GraphObj::SetPenBrushExt(DrawContext& draw_context, int pen_width) const
{   // Функция устанавливает перо толщины pen_width (по умолчанию = 1) и сплошную кисть
    // для рисования площадных объектов.
    bool is_direct_color = !(graph_obj_attributes_.is_selected && draw_context.is_use_selected);

    if (pen_width == 1)
    {
        if (is_direct_color)
            draw_context.target_dc_ptr->SetPen(draw_context.pen_1);
        else
            draw_context.target_dc_ptr->SetPen(draw_context.inv_pen_1);
    }
    else
    {
        draw_context.target_dc_ptr->SetPen(wxNullPen);
        if (is_direct_color)
        {
            draw_context.pen_width = wxPen(draw_context.color, pen_width);
            draw_context.target_dc_ptr->SetPen(draw_context.pen_width);
        }
        else
        {
            draw_context.inv_pen_width = wxPen(draw_context.inv_color, pen_width);
            draw_context.target_dc_ptr->SetPen(draw_context.inv_pen_width);
        }
    }

    if (is_direct_color)
    {
        draw_context.target_dc_ptr->SetBrush(draw_context.brush_solid);
        draw_context.target_dc_ptr->SetTextForeground(draw_context.color);
    }
    else
    {
        draw_context.target_dc_ptr->SetBrush(draw_context.inv_brush_solid);
        draw_context.target_dc_ptr->SetTextForeground(draw_context.inv_color);
    }
    draw_context.target_dc_ptr->SetTextBackground(draw_context.background_color);

    // Установка индикаторного цвета пера и кисти, соответствующего порядковому номеру примитива
    if (draw_context.recognize_dc.IsOk())
    {
        wxColor indic_color = StdWXObjects::FromLong(graph_object_ordinal_);
        draw_context.pen_recognize = wxPen(indic_color, pen_width);
        draw_context.brush_recognize = wxBrush(indic_color);
        draw_context.recognize_dc.SetPen(draw_context.pen_recognize);
        draw_context.recognize_dc.SetBrush(draw_context.brush_recognize);
        draw_context.recognize_dc.SetTextForeground(indic_color);
    }
}

void GraphObj::ClearPenBrush(DrawContext& draw_context) const
{   // Функция сбрасывает все установленные перья и кисти, делая возможным их дальнейшее уничтожение
    draw_context.target_dc_ptr->SetPen(wxNullPen);
    draw_context.target_dc_ptr->SetBrush(wxNullBrush);
    if (draw_context.recognize_dc.IsOk())
    {
        draw_context.recognize_dc.SetPen(wxNullPen);
        draw_context.recognize_dc.SetBrush(wxNullBrush);
    }
}

double ContourGraphObj::CountLineWidthDbl(DrawContext& draw_context, wxPoint first_point, wxPoint last_point) const
{
    int line_width = line_width_ ? line_width_ : 1;
    // Толщина рисования горизонтальных линий с учётом масштаба по оси x.
    double line_width_hor = line_width * draw_context.scale_x;
    // Толщина рисования вертикальных линий с учётом масштаба по оси y.
    double line_width_vert = line_width * draw_context.scale_y;
    double line_width_ratio = atan2(abs(last_point.y - first_point.y), abs(last_point.x - first_point.x)) / (M_PI / 2);
    return line_width_hor + line_width_ratio * (line_width_vert - line_width_hor);
}

int ContourGraphObj::CountLineWidth(DrawContext& draw_context, wxPoint first_point, wxPoint last_point) const
{
    double line_width_count = round(CountLineWidthDbl(draw_context, first_point, last_point));
    return max(static_cast<int>(line_width_count), 1);
}

uint32_t ContourGraphObj::GetLinePattern() const
{
    switch (line_type_)
    {
        case LineType::LINE_DOTTED:
            return DOTTED_LINE_MASK;
        case LineType::LINE_DASHED:
            return DASHED_LINE_MASK;
        default:
            return SOLID_LINE_MASK;
    }
}

void  ContourGraphObj::CountHorVertWidthDbl(DrawContext& draw_context, double* line_width_hor_ptr, double* line_width_vert_ptr) const
{
    int line_width = line_width_ ? line_width_ : 1;
    if (line_width_hor_ptr) // Толщина рисования горизонтальных линий с учётом масштаба по оси x.
        *line_width_hor_ptr = line_width * draw_context.scale_x;
    if (line_width_vert_ptr) // Толщина рисования вертикальных линий с учётом масштаба по оси y.
        *line_width_vert_ptr = line_width * draw_context.scale_y;
}

void  ContourGraphObj::CountHorVertWidth(DrawContext& draw_context, int* line_width_hor_ptr, int* line_width_vert_ptr) const
{
    int line_width = line_width_ ? line_width_ : 1;
    if (line_width_hor_ptr) // Толщина рисования горизонтальных линий с учётом масштаба по оси x.
        *line_width_hor_ptr = max(static_cast<int>(round(line_width * draw_context.scale_x)), 1);
    if (line_width_vert_ptr) // Толщина рисования вертикальных линий с учётом масштаба по оси y.
        *line_width_vert_ptr = max(static_cast<int>(round(line_width * draw_context.scale_y)), 1);
}

void ContourGraphObj::SetPenBrushInt(DrawContext& draw_context, int pen_width) const
{   // Функция устанавливает перо толщиной pen_width (по умолчанию = 1) и прозрачную
    // кисть для рисования контурных объектов.
    bool is_direct_color = !(graph_obj_attributes_.is_selected && draw_context.is_use_selected);

    if (pen_width <= 1)
    {
        if (is_direct_color)
            draw_context.target_dc_ptr->SetPen(draw_context.pen_1);
        else
            draw_context.target_dc_ptr->SetPen(draw_context.inv_pen_1);
    }
    else
    {
        draw_context.target_dc_ptr->SetPen(wxNullPen);
        if (is_direct_color)
        {
            draw_context.pen_width = wxPen(draw_context.color, pen_width);
            draw_context.target_dc_ptr->SetPen(draw_context.pen_width);
        }
        else
        {
            draw_context.inv_pen_width = wxPen(draw_context.inv_color, pen_width);
            draw_context.target_dc_ptr->SetPen(draw_context.inv_pen_width);
        }
    }
    draw_context.target_dc_ptr->SetBrush(*wxTRANSPARENT_BRUSH);

    if (is_direct_color)
        draw_context.target_dc_ptr->SetTextForeground(draw_context.color);
    else
        draw_context.target_dc_ptr->SetTextForeground(draw_context.inv_color);

    draw_context.target_dc_ptr->SetTextBackground(draw_context.background_color);


    if (draw_context.recognize_dc.IsOk())
    {
        draw_context.recognize_dc.SetPen(wxNullPen);
        wxColor indic_color = StdWXObjects::FromLong(graph_object_ordinal_);
        draw_context.pen_recognize = wxPen{indic_color, pen_width};
        draw_context.recognize_dc.SetPen(draw_context.pen_recognize);
        draw_context.recognize_dc.SetBrush(*wxTRANSPARENT_BRUSH);
        draw_context.recognize_dc.SetTextForeground(indic_color);
        draw_context.recognize_dc.SetTextBackground(wxColor(0, 0, 0));
    }
}

wxRect ObjSelColor::DrawObject(DrawContext& draw_context) const
{
    draw_context.InitDrawContextByColor(set_color_);
    return {0, 0, numeric_limits<int>::max(), numeric_limits<int>::max()};
}

void ObjSelColor::DrawObjectSVG(svg::Document& svg_doc, DrawContext& draw_context) const
{
    DrawObject(draw_context);
}

wxRect ObjLine::DrawObject(DrawContext& draw_context) const
{
    InitDrawContextByLayerNum(draw_context);
    wxPoint point1_conv = draw_context.ConvertPointToDevice(point1_);
    wxPoint point2_conv = draw_context.ConvertPointToDevice(point2_);
    // Расчёт толщины линии, которой будет рисоваться наш отрезок.
    double line_width_using = CountLineWidth(draw_context, point1_conv, point2_conv);

    if (line_type_ == LineType::LINE_SOLID)
    {
        SetPenBrushInt(draw_context, line_width_using);
        draw_context.target_dc_ptr->DrawLine(point1_conv, point2_conv);
        // Рисование поискового образа отрезка индикаторным цветом, соответствующим
        // порядковому номеру примитива.
        if (draw_context.recognize_dc.IsOk())
            draw_context.recognize_dc.DrawLine(point1_conv, point2_conv);
    }
    else
    {
        SetPenBrushExt(draw_context);
        DrawDashDotLine(*draw_context.target_dc_ptr, point1_conv, point2_conv, line_width_using, GetLinePattern());

        if (draw_context.recognize_dc.IsOk())
            DrawDashDotLine(draw_context.recognize_dc, point1_conv, point2_conv, line_width_using, GetLinePattern());
    }

    ClearPenBrush(draw_context);
    wxRect result_rect = wxRect(min(point1_conv.x, point2_conv.x), min(point1_conv.y, point2_conv.y),
                    abs(point1_conv.x - point2_conv.x) + 1, abs(point1_conv.y - point2_conv.y) + 1);
    return result_rect.Inflate(line_width_using / 2);
}

void ObjLine::DrawObjectSVG(svg::Document& svg_doc, DrawContext& draw_context) const
{
    InitDrawContextByLayerNum(draw_context);
    wxPoint point1_conv = draw_context.ConvertPointToDevice(point1_);
    wxPoint point2_conv = draw_context.ConvertPointToDevice(point2_);
    // Расчёт толщины линии, которой будет рисоваться наш отрезок.
    double line_width_using = CountLineWidth(draw_context, point1_conv, point2_conv);

    svg::Line line;
    line.SetPoint(1, ToSVGPoint(point1_conv))
        .SetPoint(2, ToSVGPoint(point2_conv))
        .SetStrokeColor(ToSVGColor(draw_context.color))
        .SetStrokeWidth(line_width_using)
        .SetStrokeDashArray(ToSVGDashArray(line_type_));
    svg_doc.Add(line);
}

void ObjLine::ShiftObject(int shift_direction_x, int shift_direction_y)
{
    point1_.x += shift_direction_x;
    point1_.y += shift_direction_y;
    point2_.x += shift_direction_x;
    point2_.y += shift_direction_y;

    frame_rect_.SetX(frame_rect_.GetX() + shift_direction_x);
    frame_rect_.SetY(frame_rect_.GetY() + shift_direction_y);
}

wxRect ObjRect::DrawObject(DrawContext& draw_context) const
{
    InitDrawContextByLayerNum(draw_context);
    wxRect rect_conv = draw_context.ConvertRectToDevice(rect_);
    int line_width_hor, line_width_vert;
    CountHorVertWidth(draw_context, &line_width_hor, &line_width_vert);

    if (line_type_ == LineType::LINE_SOLID)
    {
        // Расчёт координат точек вершин прямоугольника
        wxPoint point1{rect_conv.x, rect_conv.y};
        wxPoint point2{rect_conv.x + rect_conv.width - 1, rect_conv.y};
        wxPoint point3{rect_conv.x + rect_conv.width - 1, rect_conv.y + rect_conv.height - 1};
        wxPoint point4{rect_conv.x, rect_conv.y + rect_conv.height - 1};
         // Горизонтальные линии
        SetPenBrushInt(draw_context, line_width_hor);
        draw_context.target_dc_ptr->DrawLine(point1, point2);
        draw_context.target_dc_ptr->DrawLine(point3, point4);
        draw_context.target_dc_ptr->SetPen(wxNullPen);
        // Вертикальные линии
        SetPenBrushInt(draw_context, line_width_vert);
        draw_context.target_dc_ptr->DrawLine(point2, point3);
        draw_context.target_dc_ptr->DrawLine(point4, point1);
        draw_context.target_dc_ptr->SetPen(wxNullPen);

        if (draw_context.recognize_dc.IsOk())
        {
            wxColor indic_color = StdWXObjects::FromLong(graph_object_ordinal_);
             // Горизонтальные линии
            draw_context.pen_recognize = wxPen(indic_color, line_width_hor);
            draw_context.recognize_dc.SetPen(draw_context.pen_recognize);
            draw_context.recognize_dc.DrawLine(point1, point2);
            draw_context.recognize_dc.DrawLine(point3, point4);
            draw_context.recognize_dc.SetPen(wxNullPen);
            // Вертикальные линии
            draw_context.pen_recognize = wxPen(indic_color, line_width_vert);
            draw_context.recognize_dc.SetPen(draw_context.pen_recognize);
            draw_context.recognize_dc.DrawLine(point2, point3);
            draw_context.recognize_dc.DrawLine(point4, point1);
            draw_context.recognize_dc.SetPen(wxNullPen);
        }
    }
    else
    {
        SetPenBrushExt(draw_context);
        DrawDashDotRect(*draw_context.target_dc_ptr, rect_conv, line_width_hor,
                        line_width_vert, GetLinePattern());
        if (draw_context.recognize_dc.IsOk())
            DrawDashDotRect(draw_context.recognize_dc, rect_conv, line_width_hor,
                            line_width_vert, GetLinePattern());
    }
    ClearPenBrush(draw_context);
    return rect_conv.Inflate(CountHalfWidth(line_width_hor), CountHalfWidth(line_width_vert));
}

void ObjRect::DrawObjectSVG(svg::Document& svg_doc, DrawContext& draw_context) const
{
    InitDrawContextByLayerNum(draw_context);
    double line_width_hor, line_width_vert;
    CountHorVertWidthDbl(draw_context, &line_width_hor, &line_width_vert);
    svg::Point top_left = ToSVGPoint(draw_context.ConvertPointToDevice(rect_.GetTopLeft()));
    svg::Point right_bottom = ToSVGPoint(draw_context.ConvertPointToDevice(rect_.GetRightBottom()));
    double use_line_width = (line_width_hor + line_width_vert) / 2;

    svg::Rectangle rect;
    rect.SetTopLeftCorner(top_left).SetWidth(right_bottom.x - top_left.x)
        .SetHeight(right_bottom.y - top_left.y)
        .SetStrokeColor(ToSVGColor(draw_context.color))
        .SetStrokeWidth(use_line_width)
        .SetStrokeDashArray(ToSVGDashArray(line_type_))
        .SetFillColor("none"s);
    svg_doc.Add(rect);
}

void ObjRect::ShiftObject(int shift_direction_x, int shift_direction_y)
{
    rect_.SetX(rect_.GetX() + shift_direction_x);
    rect_.SetY(rect_.GetY() + shift_direction_y);

    frame_rect_.SetX(frame_rect_.GetX() + shift_direction_x);
    frame_rect_.SetY(frame_rect_.GetY() + shift_direction_y);
}

wxRect ObjFillRect::DrawObject(DrawContext& draw_context) const
{
    InitDrawContextByLayerNum(draw_context);
    SetPenBrushExt(draw_context);
    wxRect conv_rect = draw_context.ConvertRectToDevice(rect_);
    draw_context.target_dc_ptr->DrawRectangle(conv_rect);

    if (draw_context.recognize_dc.IsOk())
        draw_context.recognize_dc.DrawRectangle(conv_rect);

    ClearPenBrush(draw_context);
    return conv_rect;
}

void ObjFillRect::DrawObjectSVG(svg::Document& svg_doc, DrawContext& draw_context) const
{
    InitDrawContextByLayerNum(draw_context);
    svg::Point top_left = ToSVGPoint(draw_context.ConvertPointToDevice(rect_.GetTopLeft()));
    svg::Point right_bottom = ToSVGPoint(draw_context.ConvertPointToDevice(rect_.GetRightBottom()));
    svg::Color use_color;
    svg::Rectangle rect;
    rect.SetTopLeftCorner(top_left).SetWidth(right_bottom.x - top_left.x)
        .SetHeight(right_bottom.y - top_left.y)
        .SetStrokeColor(ToSVGColor(draw_context.color))
        .SetFillColor(ToSVGColor(draw_context.color))
        .SetStrokeWidth(1);
    svg_doc.Add(rect);
}

void ObjFillRect::ShiftObject(int shift_direction_x, int shift_direction_y)
{
    rect_.SetX(rect_.GetX() + shift_direction_x);
    rect_.SetY(rect_.GetY() + shift_direction_y);

    frame_rect_.SetX(frame_rect_.GetX() + shift_direction_x);
    frame_rect_.SetY(frame_rect_.GetY() + shift_direction_y);
}

wxRect ObjCirc::DrawObject(DrawContext& draw_context) const
{
    InitDrawContextByLayerNum(draw_context);
    int radius_x = max(static_cast<int>(radius_ * draw_context.scale_x), 1);
    int radius_y = max(static_cast<int>(radius_ * draw_context.scale_y), 1);
    wxPoint conv_center = draw_context.ConvertPointToDevice(center_);
    int line_width_hor, line_width_vert;
    CountHorVertWidth(draw_context, &line_width_hor, &line_width_vert);
    wxPoint left_up_pt(conv_center.x - radius_x, conv_center.y - radius_y);
    wxSize circ_rect_size(2 * radius_x, 2 * radius_y);

    if (line_type_ == LineType::LINE_SOLID && line_width_hor == line_width_vert)
    {
        SetPenBrushInt(draw_context, line_width_hor);
        if (radius_x == radius_y)
        {
            draw_context.target_dc_ptr->DrawCircle(conv_center, radius_x);
            if (draw_context.recognize_dc.IsOk())
                draw_context.recognize_dc.DrawCircle(conv_center, radius_x);
        }
        else
        {
            draw_context.target_dc_ptr->DrawEllipse(left_up_pt, circ_rect_size);
            if (draw_context.recognize_dc.IsOk())
                draw_context.recognize_dc.DrawEllipse(left_up_pt, circ_rect_size);
        }
    }
    else
    {
        SetPenBrushExt(draw_context);
        DrawDashDotEllipticArc(*draw_context.target_dc_ptr, conv_center, radius_x, radius_y, 0, 0,
                               line_width_hor, line_width_vert, GetLinePattern());
        if (draw_context.recognize_dc.IsOk())
            DrawDashDotEllipticArc(draw_context.recognize_dc, conv_center, radius_x, radius_y, 0, 0,
                                   line_width_hor, line_width_vert, GetLinePattern());
    }
    ClearPenBrush(draw_context);
    wxRect rect_conv = wxRect(conv_center.x - radius_x, conv_center.y - radius_y, 2 * radius_x, 2 * radius_y);
    return rect_conv.Inflate(CountHalfWidth(line_width_hor), CountHalfWidth(line_width_vert));
}

void ObjCirc::DrawObjectSVG(svg::Document& svg_doc, DrawContext& draw_context) const
{
    InitDrawContextByLayerNum(draw_context);
    double radius_x = radius_ * draw_context.scale_x;
    double radius_y = radius_ * draw_context.scale_y;
    wxPoint conv_center = draw_context.ConvertPointToDevice(center_);
    double line_width_hor, line_width_vert;
    CountHorVertWidthDbl(draw_context, &line_width_hor, &line_width_vert);
    double use_line_width = (line_width_hor + line_width_vert) / 2;

    if (abs(radius_x - radius_y) < ZERO_TOLERANCE)
    {
        svg::Circle circle;
        circle.SetCenter(ToSVGPoint(conv_center)).SetRadius(radius_x)
              .SetStrokeColor(ToSVGColor(draw_context.color))
              .SetStrokeWidth(use_line_width)
              .SetStrokeDashArray(ToSVGDashArray(line_type_))
              .SetFillColor("none"s);
        svg_doc.Add(circle);
    }
    else
    {
        svg::Ellipse ellipse;
        ellipse.SetCenter(ToSVGPoint(conv_center)).SetRadiusX(radius_x).SetRadiusY(radius_y)
               .SetStrokeColor(ToSVGColor(draw_context.color))
               .SetStrokeWidth(use_line_width)
               .SetStrokeDashArray(ToSVGDashArray(line_type_))
               .SetFillColor("none"s);
        svg_doc.Add(ellipse);
    }
}

void ObjCirc::ShiftObject(int shift_direction_x, int shift_direction_y)
{
    center_.x += shift_direction_x;
    center_.y += shift_direction_y;
    frame_rect_.SetX(frame_rect_.GetX() + shift_direction_x);
    frame_rect_.SetY(frame_rect_.GetY() + shift_direction_y);
}

wxRect ObjArc::DrawObject(DrawContext& draw_context) const
{
    InitDrawContextByLayerNum(draw_context);
    double radius_arc = sqrt((arc_begin_.x - arc_center_.x) * (arc_begin_.x - arc_center_.x) +
                             (arc_center_.y - arc_begin_.y) * (arc_center_.y - arc_begin_.y));
    int radius_x = radius_arc * draw_context.scale_x;
    int radius_y = radius_arc * draw_context.scale_y;
    wxPoint conv_arc_center = draw_context.ConvertPointToDevice(arc_center_);
    wxPoint conv_arc_begin = draw_context.ConvertPointToDevice(arc_begin_);
    wxPoint conv_arc_end = draw_context.ConvertPointToDevice(arc_end_);
    int line_width_hor, line_width_vert;
    CountHorVertWidth(draw_context, &line_width_hor, &line_width_vert);

    if (line_type_ == LineType::LINE_SOLID && line_width_hor == line_width_vert)
    {
        wxPoint left_up_pt(conv_arc_center.x - radius_x, conv_arc_center.y - radius_y);
        wxSize circ_rect_size(2 * radius_x, 2 * radius_y);
        double start_angle = ConvertTO2PI(atan2(conv_arc_center.y - conv_arc_begin.y, conv_arc_begin.x - conv_arc_center.x));
        double end_angle = ConvertTO2PI(atan2(conv_arc_center.y - conv_arc_end.y, conv_arc_end.x - conv_arc_center.x));

        SetPenBrushInt(draw_context, line_width_hor);
        draw_context.target_dc_ptr->DrawEllipticArc(left_up_pt, circ_rect_size,
                                                    ToDegrees(start_angle), ToDegrees(end_angle));
        if (draw_context.recognize_dc.IsOk())
            draw_context.recognize_dc.DrawEllipticArc(left_up_pt, circ_rect_size,
                                                      ToDegrees(start_angle), ToDegrees(end_angle));
    }
    else
    {
        SetPenBrushExt(draw_context);
        DrawDashDotEllipticArc(*draw_context.target_dc_ptr, conv_arc_center, radius_x, radius_y, conv_arc_begin,
                               conv_arc_end, line_width_hor, line_width_vert, GetLinePattern());
        if (draw_context.recognize_dc.IsOk())
            DrawDashDotEllipticArc(draw_context.recognize_dc, conv_arc_center, radius_x, radius_y, conv_arc_begin,
                                   conv_arc_end, line_width_hor, line_width_vert, GetLinePattern());
    }
    ClearPenBrush(draw_context);
    return CountArcBoundRect(conv_arc_center, conv_arc_begin, conv_arc_end, line_width_hor, line_width_vert);
}

void ObjArc::DrawObjectSVG(svg::Document& svg_doc, DrawContext& draw_context) const
{
    InitDrawContextByLayerNum(draw_context);
    double radius_arc = sqrt((arc_begin_.x - arc_center_.x) * (arc_begin_.x - arc_center_.x) +
                             (arc_center_.y - arc_begin_.y) * (arc_center_.y - arc_begin_.y));
    double radius_x = radius_arc * draw_context.scale_x;
    double radius_y = radius_arc * draw_context.scale_y;
    wxPoint conv_arc_center = draw_context.ConvertPointToDevice(arc_center_);
    wxPoint conv_arc_begin =  draw_context.ConvertPointToDevice(arc_begin_);
    wxPoint conv_arc_end = draw_context.ConvertPointToDevice(arc_end_);
    double line_width_hor, line_width_vert;
    CountHorVertWidthDbl(draw_context, &line_width_hor, &line_width_vert);
    double use_line_width = (line_width_hor + line_width_vert) / 2;

    double start_angle = atan2(conv_arc_center.y - conv_arc_begin.y, conv_arc_begin.x - conv_arc_center.x);
    double end_angle = atan2(conv_arc_center.y - conv_arc_end.y, conv_arc_end.x - conv_arc_center.x);
    bool is_large_arc = CountArcDistance(start_angle, end_angle) > M_PI;

    svg::EllipticArc ellipse_arc;
    ellipse_arc.SetStartPoint(ToSVGPoint(conv_arc_begin)).SetEndPoint(ToSVGPoint(conv_arc_end))
               .SetRadiusX(radius_x).SetRadiusY(radius_y)
               .SetLargeArcFlag(is_large_arc).SetSweepFlag(false)
               .SetStrokeColor(ToSVGColor(draw_context.color))
               .SetStrokeWidth(use_line_width)
               .SetStrokeDashArray(ToSVGDashArray(line_type_))
               .SetFillColor("none"s);
    svg_doc.Add(ellipse_arc);
}

void ObjArc::ShiftObject(int shift_direction_x, int shift_direction_y)
{
    arc_center_.x += shift_direction_x;
    arc_center_.y += shift_direction_y;

    arc_begin_.x += shift_direction_x;
    arc_begin_.y += shift_direction_y;

    arc_end_.x += shift_direction_x;
    arc_end_.y += shift_direction_y;

    frame_rect_.SetX(frame_rect_.GetX() + shift_direction_x);
    frame_rect_.SetY(frame_rect_.GetY() + shift_direction_y);
}

wxRect ObjText::DrawObject(DrawContext& draw_context) const
{
    InitDrawContextByLayerNum(draw_context);
    int text_height_conv;
    TextSignature text_signat(GetTextSignature(text_orientation_, text_align_));
    if (text_signat.is_horizontal)
        text_height_conv = max(text_height_ * draw_context.scale_x, 1.0);
    else
        text_height_conv = max(text_height_ * draw_context.scale_y, 1.0);

    wxString wx_text(text_.c_str(), wxConvUTF8);
    wxFont text_font(wxSize(0, text_height_conv), draw_context.context_font.GetFamily(),
                     draw_context.context_font.GetStyle(), draw_context.context_font.GetWeight(),
                     draw_context.context_font.GetUnderlined(), draw_context.context_font.GetFaceName(),
                     draw_context.context_font.GetEncoding());
    draw_context.target_dc_ptr->SetFont(text_font);
    wxPoint text_point_conv = draw_context.ConvertPointToDevice(text_point_);
    wxSize text_size(draw_context.target_dc_ptr->GetTextExtent(wx_text));
    wxPoint text_point_corr = GetCorrectedTextPos(text_signat, text_point_conv, text_size);
    wxRect bound_rect = GetCorrectedTextRect(text_signat, text_point_corr, text_size);

    SetPenBrushExt(draw_context);
    draw_context.target_dc_ptr->DrawRotatedText(wx_text, text_point_corr, text_signat.text_angle);
    draw_context.target_dc_ptr->SetPen(*wxWHITE_PEN);
    //draw_context.target_dc_ptr->SetBrush(*wxTRANSPARENT_BRUSH);
    //draw_context.target_dc_ptr->DrawCircle(text_point_corr, 2);
    //draw_context.target_dc_ptr->DrawRectangle(bound_rect);
    if (draw_context.recognize_dc.IsOk())
    {
        //draw_context.recognize_dc.SetFont(text_font);
        //draw_context.recognize_dc.DrawRotatedText(wx_text, text_point_corr, text_signat.text_angle);
        draw_context.recognize_dc.DrawRectangle(bound_rect);
    }

    return bound_rect;
}

void ObjText::DrawObjectSVG(svg::Document& svg_doc, DrawContext& draw_context) const
{
    InitDrawContextByLayerNum(draw_context);
    int text_height_conv;
    TextSignature text_signat(GetTextSignature(text_orientation_, text_align_));
    if (text_signat.is_horizontal)
        text_height_conv = max(text_height_ * draw_context.scale_x, 1.0);
    else
        text_height_conv = max(text_height_ * draw_context.scale_y, 1.0);
    wxPoint text_point_conv = draw_context.ConvertPointToDevice(text_point_);

    auto [text_anchor, offset_point] = GetSVGCorrTextParams(text_signat);

    svg::Text svg_text;
    svg_text.SetPosition(ToSVGPoint(text_point_conv))
            .SetOffset(offset_point)
            .SetStrokeColor(ToSVGColor(draw_context.color))
            .SetFillColor(ToSVGColor(draw_context.color))
            .SetFontSize(text_height_conv)
            .SetTextAnchor(text_anchor)
            .SetMirrorAttr(text_signat.is_mirror)
            .SetFontFamily(ToSVGFontFamily(static_cast<wxFontFamily>(draw_context.context_font.GetFamily())))
            .SetFontWeight(ToSVGFontWeight(static_cast<wxFontWeight>(draw_context.context_font.GetWeight())))
            .SetData(text_);
    if (abs(text_signat.text_angle) >= ZERO_TOLERANCE)
        svg_text.SetRotateAngle(360 - text_signat.text_angle);

    svg_doc.Add(svg_text);
}

void ObjText::ShiftObject(int shift_direction_x, int shift_direction_y)
{
    text_point_.x += shift_direction_x;
    text_point_.y += shift_direction_y;

    frame_rect_.SetX(frame_rect_.GetX() + shift_direction_x);
    frame_rect_.SetY(frame_rect_.GetY() + shift_direction_y);
}

wxRect ObjFlash::DrawObject(DrawContext& draw_context) const
{
    InitDrawContextByLayerNum(draw_context);
    FlashDesc fd = draw_context.pcad_doc_ptr->GetFlashDesc(aperture_number_);
    FileDefValues file_values = draw_context.pcad_doc_ptr->GetFileDefValues();

    double aperture_scale_width, aperture_scale_height;
    if (file_values.file_flags & FILE_FLAG_UNIT_INCHES)
    {
        aperture_scale_width = fd.flash_size_inch * file_values.DBU_in_measure_unit * draw_context.scale_x;
        aperture_scale_height = fd.flash_size_inch * file_values.DBU_in_measure_unit * draw_context.scale_y;
    }
    else
    {
        aperture_scale_width = fd.flash_size_mm * file_values.DBU_in_measure_unit * draw_context.scale_x;
        aperture_scale_height = fd.flash_size_mm * file_values.DBU_in_measure_unit * draw_context.scale_y;
    }

    if (flash_orientation_ == FlashOrientation::FLASH_90_DEGREES || flash_orientation_ == FlashOrientation::FLASH_270_DEGREES)
        swap(aperture_scale_width, aperture_scale_height);

    wxPoint flash_point_conv = draw_context.ConvertPointToDevice(flash_point_);
    wxPoint flash_left_up(flash_point_conv.x - aperture_scale_width / 2,
                          flash_point_conv.y - aperture_scale_height / 2);
    wxRect aperture_rect(flash_left_up, wxSize(aperture_scale_width, aperture_scale_height));

    SetPenBrushExt(draw_context); // Установка требуемого пера и требуемой сплошной кисти.
    if (fd.flash_shape == FlashShape::FLASH_SQUARE)
    { // Обработка квадратных апертур
        draw_context.target_dc_ptr->DrawRectangle(aperture_rect);
        if (draw_context.recognize_dc.IsOk())
            draw_context.recognize_dc.DrawRectangle(aperture_rect);
    }
    else
    { // Все остальные засветки обрабатываем как круглые
        draw_context.target_dc_ptr->DrawEllipse(aperture_rect);
        if (draw_context.recognize_dc.IsOk())
            draw_context.recognize_dc.DrawEllipse(aperture_rect);
    }
    ClearPenBrush(draw_context);
    return aperture_rect;
}

void ObjFlash::DrawObjectSVG(svg::Document& svg_doc, DrawContext& draw_context) const
{
    InitDrawContextByLayerNum(draw_context);
    FlashDesc fd = draw_context.pcad_doc_ptr->GetFlashDesc(aperture_number_);
    FileDefValues file_values = draw_context.pcad_doc_ptr->GetFileDefValues();
    double aperture_scale_width, aperture_scale_height;
    if (file_values.file_flags & FILE_FLAG_UNIT_INCHES)
    {
        aperture_scale_width = fd.flash_size_inch * file_values.DBU_in_measure_unit * draw_context.scale_x;
        aperture_scale_height = fd.flash_size_inch * file_values.DBU_in_measure_unit * draw_context.scale_y;
    }
    else
    {
        aperture_scale_width = fd.flash_size_mm * file_values.DBU_in_measure_unit * draw_context.scale_x;
        aperture_scale_height = fd.flash_size_mm * file_values.DBU_in_measure_unit * draw_context.scale_y;
    }

    if (flash_orientation_ == FlashOrientation::FLASH_90_DEGREES || flash_orientation_ == FlashOrientation::FLASH_270_DEGREES)
        swap(aperture_scale_width, aperture_scale_height);

    wxPoint flash_point_conv = draw_context.ConvertPointToDevice(flash_point_);
    wxPoint flash_left_up(flash_point_conv.x - aperture_scale_width / 2,
                          flash_point_conv.y - aperture_scale_height / 2);

    if (fd.flash_shape == FlashShape::FLASH_SQUARE)
    { // Обработка квадратных апертур
        svg::Rectangle rect;
        rect.SetTopLeftCorner(ToSVGPoint(flash_left_up)).SetWidth(aperture_scale_width)
            .SetHeight(aperture_scale_height)
            .SetStrokeColor(ToSVGColor(draw_context.color))
            .SetStrokeWidth(1).SetFillColor(ToSVGColor(draw_context.color));
        svg_doc.Add(rect);
    }
    else
    { // Все остальные засветки обрабатываем как круглые
        if (abs(aperture_scale_width - aperture_scale_height) < ZERO_TOLERANCE)
        { // Эта вспышка получилась действительно круглая
            svg::Circle circle;
            circle.SetCenter(ToSVGPoint(flash_point_conv)).SetRadius(aperture_scale_width / 2)
                  .SetStrokeColor(ToSVGColor(draw_context.color))
                  .SetStrokeWidth(1).SetFillColor(ToSVGColor(draw_context.color));
            svg_doc.Add(circle);
        }
        else
        { // А здесь она оказалась эллиптической
            svg::Ellipse ellipse;
            ellipse.SetCenter(ToSVGPoint(flash_point_conv))
                   .SetRadiusX(aperture_scale_width / 2).SetRadiusY(aperture_scale_height / 2)
                   .SetStrokeColor(ToSVGColor(draw_context.color))
                   .SetStrokeWidth(1).SetFillColor(ToSVGColor(draw_context.color));
            svg_doc.Add(ellipse);
        }
    }
}

void ObjFlash::ShiftObject(int shift_direction_x, int shift_direction_y)
{
    flash_point_.x += shift_direction_x;
    flash_point_.y += shift_direction_y;

    frame_rect_.SetX(frame_rect_.GetX() + shift_direction_x);
    frame_rect_.SetY(frame_rect_.GetY() + shift_direction_y);
}

wxRect ObjCVoid::DrawObject(DrawContext& draw_context) const
{
    double void_scale_width = void_radius_ * draw_context.scale_x * 2;
    double void_scale_height = void_radius_ * draw_context.scale_y * 2;

    wxPoint void_center_conv = draw_context.ConvertPointToDevice(void_center_);

    wxPoint void_left_up(void_center_conv.x - void_scale_width / 2,
                         void_center_conv.y - void_scale_height / 2);
    wxRect void_rect(void_left_up, wxSize(void_scale_width, void_scale_height));

    // Установка пера и сплошной кисти фонового цвета. Таким образом будут создаваться выемки в полигоне.
    draw_context.pen_width = wxPen(draw_context.background_color);
    draw_context.brush_temp = wxBrush(draw_context.background_color);
    draw_context.target_dc_ptr->SetPen(draw_context.pen_width);
    draw_context.target_dc_ptr->SetBrush(draw_context.brush_temp);

    draw_context.target_dc_ptr->DrawEllipse(void_rect);

    ClearPenBrush(draw_context);
    return void_rect;
}

void ObjCVoid::DrawObjectSVG(svg::Document& svg_doc, DrawContext& draw_context) const
{
    double void_scale_width = void_radius_ * draw_context.scale_x * 2;
    double void_scale_height = void_radius_ * draw_context.scale_y * 2;

    wxPoint void_center_conv = draw_context.ConvertPointToDevice(void_center_);

    if (abs(void_scale_width - void_scale_height) < ZERO_TOLERANCE)
    { // Эта вспышка получилась действительно круглая
        svg::Circle circle;
        circle.SetCenter(ToSVGPoint(void_center_conv)).SetRadius(void_scale_width / 2)
              .SetStrokeColor(ToSVGColor(draw_context.background_color))
              .SetStrokeWidth(1).SetFillColor(ToSVGColor(draw_context.background_color));
        svg_doc.Add(circle);
    }
    else
    { // А здесь она оказалась эллиптической
        svg::Ellipse ellipse;
        ellipse.SetCenter(ToSVGPoint(void_center_conv))
               .SetRadiusX(void_scale_width / 2).SetRadiusY(void_scale_height / 2)
               .SetStrokeColor(ToSVGColor(draw_context.background_color))
               .SetStrokeWidth(1).SetFillColor(ToSVGColor(draw_context.background_color));
        svg_doc.Add(ellipse);
    }
}

void ObjCVoid::ShiftObject(int shift_direction_x, int shift_direction_y)
{
    void_center_.x += shift_direction_x;
    void_center_.y += shift_direction_y;

    frame_rect_.SetX(frame_rect_.GetX() + shift_direction_x);
    frame_rect_.SetY(frame_rect_.GetY() + shift_direction_y);
}

wxRect ObjPVoid::DrawObject(DrawContext& draw_context) const
{
    if (!points_.size())
        return {};
    std::vector<wxPoint> converted_points;
    for (wxPoint wxp : points_)
        converted_points.push_back(draw_context.ConvertPointToDevice(wxp));

    // Установка пера и сплошной кисти фонового цвета. Таким образом будут создаваться выемки в полигоне.
    draw_context.pen_width = wxPen(draw_context.background_color);
    draw_context.brush_temp = wxBrush(draw_context.background_color);
    draw_context.target_dc_ptr->SetPen(draw_context.pen_width);
    draw_context.target_dc_ptr->SetBrush(draw_context.brush_temp);

    draw_context.target_dc_ptr->DrawPolygon(converted_points.size(), &converted_points[0]);
    ClearPenBrush(draw_context);

    // Рассчитаем положение и размер покрывающего фигуру прямоугольника
    wxRect covered_rect;
    for (wxPoint wxp : converted_points)
    {
        wxRect point_rect(wxp.x, wxp.y, 1, 1);
        covered_rect.Union(point_rect);
    }
    return covered_rect;
}

void ObjPVoid::DrawObjectSVG(svg::Document& svg_doc, DrawContext& draw_context) const
{
   if (!points_.size())
        return;
    std::vector<wxPoint> converted_points;
    for (wxPoint wxp : points_)
        converted_points.push_back(draw_context.ConvertPointToDevice(wxp));
    svg::Polygon polygon;
    polygon.SetStrokeColor(ToSVGColor(draw_context.background_color))
           .SetStrokeWidth(1).SetFillColor(ToSVGColor(draw_context.background_color));
    for (wxPoint wxp : converted_points)
        polygon.AddPoint(ToSVGPoint(wxp));
    svg_doc.Add(polygon);
}

void ObjPVoid::ShiftObject(int shift_direction_x, int shift_direction_y)
{
    for (wxPoint& current_point : points_)
    {
        current_point.x += shift_direction_x;
        current_point.y += shift_direction_y;
    }

    frame_rect_.SetX(frame_rect_.GetX() + shift_direction_x);
    frame_rect_.SetY(frame_rect_.GetY() + shift_direction_y);
}

wxRect ObjPoly::DrawObject(DrawContext& draw_context) const
{
    InitDrawContextByLayerNum(draw_context);
    bool is_ordinary_draw = !(graph_obj_attributes_.is_selected && draw_context.is_use_selected);

    if (!points_.size())
        return {};
    std::vector<wxPoint> converted_points;
    for (wxPoint wxp : points_)
        converted_points.push_back(draw_context.ConvertPointToDevice(wxp));

    // Рассчитаем размер "эффективной" ширины линии, образованной требуемой апертурой.
    double aperture_scale_size = static_cast<double>(aperture_width_) *
                                 (draw_context.scale_x + draw_context.scale_y) / 2;
    if (aperture_scale_size < 1)
        aperture_scale_size = 1;

    // Перья, как рисующее, так и индикаторное, будут иметь рассчитанную выше толщину
    // апертуры, которой будет рисоваться полигон.

    if (draw_context.recognize_dc.IsOk())
    { // Установка индикаторного цвета пера и кисти, соответствующего порядковому номеру примитива
        wxColor indic_color = StdWXObjects::FromLong(graph_object_ordinal_);
        draw_context.recognize_dc.SetPen(wxPen{indic_color, aperture_scale_size});
        draw_context.recognize_dc.SetBrush(wxBrush{indic_color});
    }
    // Установка рисующего пера и настройка индикаторных рисующих инструментов
    SetPenBrushExt(draw_context, aperture_scale_size);
    if (is_ordinary_draw)
    {
        if (hatch_type_ == FillType::POLY_FILL_SOLID)
            draw_context.target_dc_ptr->SetBrush(draw_context.brush_solid); // Сплошное залитие
        else
            draw_context.target_dc_ptr->SetBrush(draw_context.brush_hatch); // Штриховка
    }
    else
    {
        if (hatch_type_ == FillType::POLY_FILL_SOLID)
            draw_context.target_dc_ptr->SetBrush(draw_context.inv_brush_solid); // Сплошное залитие
        else
            draw_context.target_dc_ptr->SetBrush(draw_context.inv_brush_hatch); // Штриховка
    }

    draw_context.target_dc_ptr->DrawPolygon(converted_points.size(), &converted_points[0]);
    if (draw_context.recognize_dc.IsOk())
        draw_context.recognize_dc.DrawPolygon(converted_points.size(), &converted_points[0]);
    ClearPenBrush(draw_context);
    // Отрисуем все выемки, которые включены в состав полигона
    for (const ObjCVoid& cvoid : cvoids_)
        cvoid.DrawObject(draw_context);

    for (const ObjPVoid& pvoid : pvoids_)
        pvoid.DrawObject(draw_context);

    // Рассчитаем положение и размер покрывающего фигуру прямоугольника
    wxRect covered_rect;
    double aperture_scale_halfsize = aperture_scale_size / 2;
    for (wxPoint wxp : converted_points)
    {
        wxRect point_rect(wxp.x, wxp.y, 1, 1);
        point_rect.Inflate(aperture_scale_halfsize, aperture_scale_halfsize);
        covered_rect.Union(point_rect);
    }
    return covered_rect;
}

void ObjPoly::DrawObjectSVG(svg::Document& svg_doc, DrawContext& draw_context) const
{
    if (!points_.size())
        return;
    InitDrawContextByLayerNum(draw_context);
    std::vector<wxPoint> converted_points;
    for (wxPoint wxp : points_)
        converted_points.push_back(draw_context.ConvertPointToDevice(wxp));

    // Рассчитаем размер "эффективной" ширины линии, образованной требуемой апертурой.
    double aperture_scale_size = static_cast<double>(aperture_width_) *
                                 (draw_context.scale_x + draw_context.scale_y) / 2;
    if (aperture_scale_size < 1)
        aperture_scale_size = 1;

    svg::Polygon polygon;
    polygon.SetStrokeColor(ToSVGColor(draw_context.color))
           .SetStrokeWidth(aperture_scale_size);
    string hatch_name;
    if (hatch_type_ == FillType::POLY_FILL_HATCH)
    { // Находим или создаём в SVG-документе соответствующий трафарет штриховки
        hatch_name = "hatch"s + to_string(draw_context.color.Red()) +
                     to_string(draw_context.color.Green()) + to_string(draw_context.color.Blue());
        auto rm_pair = svg_doc.ResourceManager(svg::ResourceType::SVG_RESOURCE_PATTERN, hatch_name, true);
        auto pattern_shared_ptr = rm_pair.first.lock();
        svg::Pattern* pattern_ptr = dynamic_cast<svg::Pattern*>(pattern_shared_ptr.get());
        if (!pattern_ptr)
            return;
        if (!rm_pair.second)
        { // Такой трафарет штриховки ранее ещё не встречался, он создан впервые, теперь нужно его заполнить
            svg::Line first_pattern_line;
            first_pattern_line.SetPoint(1, ToSVGPoint(wxPoint(1, 20)))
                              .SetPoint(2, ToSVGPoint(wxPoint(20, 1)))
                              .SetStrokeColor(ToSVGColor(draw_context.color))
                              .SetStrokeWidth(1);
            pattern_ptr->Add(first_pattern_line);
            svg::Line second_pattern_line;
            second_pattern_line.SetPoint(1, ToSVGPoint(wxPoint(0, 1)))
                               .SetPoint(2, ToSVGPoint(wxPoint(1, 0)))
                               .SetStrokeColor(ToSVGColor(draw_context.color))
                               .SetStrokeWidth(1);
            pattern_ptr->Add(second_pattern_line);
            pattern_ptr->SetWidth(20).SetHeight(20);
        }
        polygon.SetFillColor("url(#"s + hatch_name + ')');
    }
    else
    {
        polygon.SetFillColor(ToSVGColor(draw_context.color));
    }
    // Добавляем в список основного полигона все его вершины
    for (wxPoint wxp : converted_points)
        polygon.AddPoint(ToSVGPoint(wxp));
    svg_doc.Add(polygon);
    // Отрисуем все выемки, которые включены в состав полигона
    for (const ObjCVoid& cvoid : cvoids_)
        cvoid.DrawObjectSVG(svg_doc, draw_context);
    for (const ObjPVoid& pvoid : pvoids_)
        pvoid.DrawObjectSVG(svg_doc, draw_context);
}

void ObjPoly::ShiftObject(int shift_direction_x, int shift_direction_y)
{
    for (wxPoint& current_point : points_)
    {
        current_point.x += shift_direction_x;
        current_point.y += shift_direction_y;
    }

    for (ObjCVoid& current_cvoid : cvoids_)
        current_cvoid.ShiftObject(shift_direction_x, shift_direction_y);

    for (ObjPVoid& current_pvoid : pvoids_)
        current_pvoid.ShiftObject(shift_direction_x, shift_direction_y);

    frame_rect_.SetX(frame_rect_.GetX() + shift_direction_x);
    frame_rect_.SetY(frame_rect_.GetY() + shift_direction_y);
}
