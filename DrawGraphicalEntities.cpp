
#include <cmath>
#include <tuple>
#include "DrawGraphicalEntities.h"

using namespace std;

namespace
{
    tuple<wxRect, wxRect, wxRect, wxRect> GetSelectingContourSides(DrawContext& draw_context,
                                                                   const SelectContourData& select_contour)
    {   //select_contour.contour_rect - выделяющий прямоугольник в пространстве исходного изображения.

        //dev_contour_rect - выделяющий прямоугольник в пространстве координат целевого
        //изображения (в целевом пространстве, в пространстве "отображающего устройства").
        wxRect dev_contour_rect(draw_context.ConvertRectToDevice(select_contour.contour_rect));

        wxRect left_side(dev_contour_rect.GetLeft() - SELECT_CONTOUR_WIDTH / 2,
                         dev_contour_rect.GetTop() - SELECT_CONTOUR_WIDTH / 2,
                         SELECT_CONTOUR_WIDTH,
                         dev_contour_rect.GetHeight() + SELECT_CONTOUR_WIDTH);
        wxRect right_side(dev_contour_rect.GetRight() - SELECT_CONTOUR_WIDTH / 2,
                          dev_contour_rect.GetTop() - SELECT_CONTOUR_WIDTH / 2,
                          SELECT_CONTOUR_WIDTH,
                          dev_contour_rect.GetHeight() + SELECT_CONTOUR_WIDTH);
        wxRect up_side(dev_contour_rect.GetLeft() - SELECT_CONTOUR_WIDTH / 2,
                       dev_contour_rect.GetTop() - SELECT_CONTOUR_WIDTH / 2,
                       dev_contour_rect.GetWidth() + SELECT_CONTOUR_WIDTH,
                       SELECT_CONTOUR_WIDTH);
        wxRect down_side(dev_contour_rect.GetLeft() - SELECT_CONTOUR_WIDTH / 2,
                         dev_contour_rect.GetBottom() - SELECT_CONTOUR_WIDTH / 2,
                         dev_contour_rect.GetWidth() + SELECT_CONTOUR_WIDTH,
                         SELECT_CONTOUR_WIDTH);
        return {left_side, right_side, up_side, down_side};
    }

    int isign(int arg)
    {
        if (arg < 0)
            return -1;
        else if (arg == 0)
            return 0;
        else
            return 1;
    }
}

namespace DrawEntities
{
    int CountHalfWidth(int full_width)
    {
        return max(static_cast<int>(round(static_cast<double>(full_width) / 2)), 1);
    }

    double CountCurrentLineWidthDbl(double current_angle, double line_width_hor, double line_width_vert)
    {  // current_angle должен находиться в диапазоне от 0 до 2 * PI.
        if (current_angle > (M_PI / 2) && current_angle <= M_PI)
            current_angle = M_PI - current_angle;
        else if (current_angle > M_PI && current_angle <= 3 * (M_PI / 2))
            current_angle -= M_PI;
        else if (current_angle > 3 * (M_PI / 2))
            current_angle = (2 * M_PI) - current_angle;
        return line_width_hor + (current_angle / (M_PI / 2)) * (line_width_vert - line_width_hor);
    }

    int CountCurrentLineWidthInt(double current_angle, int line_width_hor, int line_width_vert)
    {
        return max(static_cast<int>(round
            (CountCurrentLineWidthDbl(current_angle, line_width_hor, line_width_vert))), 1);
    }

    void DrawDashDotLine(wxDC &target_dc, const wxPoint start_point, const wxPoint end_point,
                         int line_width_hor, int line_width_vert, unsigned int line_pattern)
    {
        wxPointDouble start_dot_point(start_point.x , start_point.y);
        double line_width_ratio = atan2(abs(end_point.y - start_point.y), abs(end_point.x - start_point.x)) / (M_PI / 2);
        double line_width = line_width_hor + line_width_ratio * (line_width_vert - line_width_hor);

        //Все штрихи должен пролегать паралелльно вектору, идущему от точки current_point до точки end_point.
        //Рассчитываем вектор, нормальный к направлению штриха. Длина его должна быть в половину от ширины линии.
        wxPointDouble normal_vector(start_dot_point.y - end_point.y, end_point.x - start_dot_point.x);
        double normal_vector_len = sqrt(normal_vector.x * normal_vector.x + normal_vector.y * normal_vector.y);
        normal_vector.x = normal_vector.x * line_width / normal_vector_len / 2.0;
        normal_vector.y = normal_vector.y * line_width / normal_vector_len / 2.0;

        wxPointDouble end_dot_point(start_dot_point);
        unsigned int current_pattern_mask = 1;
        bool dot_status = line_pattern & current_pattern_mask;
        bool is_done = false;

        while (!is_done)
        {
            wxPointDouble l_size_point = wxPointDouble(end_point.x - end_dot_point.x, end_point.y - end_dot_point.y);
            double l_size = sqrt(l_size_point.x * l_size_point.x + l_size_point.y * l_size_point.y);
            double l_dot_size;
            if (l_size <= DOT_SIZE)
            {
                l_dot_size = l_size;
                is_done = true;
            }
            else
            {
                l_dot_size = DOT_SIZE;
            }

            current_pattern_mask <<= 1;
            if (!current_pattern_mask)
                current_pattern_mask = 1;

            end_dot_point.x = end_dot_point.x + (l_dot_size * (end_point.x - end_dot_point.x)) / l_size;
            end_dot_point.y = end_dot_point.y + (l_dot_size * (end_point.y - end_dot_point.y)) / l_size;

            if (dot_status)
            {
                if (!(line_pattern & current_pattern_mask) || is_done)
                {
                    wxPointDouble temp_point;
                    wxPoint point_arr[4];

                    temp_point = start_dot_point - normal_vector;
                    point_arr[0] = wxPoint(temp_point.x, temp_point.y);
                    temp_point = start_dot_point + normal_vector;
                    point_arr[1] = wxPoint(temp_point.x, temp_point.y);
                    temp_point = end_dot_point + normal_vector;
                    point_arr[2] = wxPoint(temp_point.x, temp_point.y);
                    temp_point = end_dot_point - normal_vector;
                    point_arr[3] = wxPoint(temp_point.x, temp_point.y);
                    target_dc.DrawPolygon(4, point_arr);
                    start_dot_point = end_dot_point;
                    dot_status = false;
                }
            }
            else
            {
                if (line_pattern & current_pattern_mask)
                {
                    start_dot_point = end_dot_point;
                    dot_status = true;
                }
            }
        }
    }

    void DrawDashDotLine(wxDC &target_dc, const wxPoint start_point, const wxPoint end_point,
                         int line_width, unsigned int line_pattern)
    {
         DrawDashDotLine(target_dc, start_point, end_point, line_width, line_width, line_pattern);
    }

    void DrawDashDotHLine(wxDC &target_dc, const wxPoint start_point, int line_length,
                          int line_width, unsigned int line_pattern)
    {
        if (!line_length)
            return;
        wxPoint start_dot_point(start_point.x , start_point.y);
        wxPoint end_point(start_point.x + line_length, start_point.y);
        wxPoint end_dot_point(start_dot_point);
        wxPoint normal_vector(0, line_width / 2);
        unsigned int current_pattern_mask = 1;
        bool dot_status = line_pattern & current_pattern_mask;
        bool is_done = false;

        while (!is_done)
        {
            if (abs(end_point.x - end_dot_point.x) <= DOT_SIZE_INT)
            {
                end_dot_point.x = end_point.x;
                is_done = true;
            }
            else
            {
                end_dot_point.x += isign(line_length) * DOT_SIZE_INT;
            }

            current_pattern_mask <<= 1;
            if (!current_pattern_mask)
                current_pattern_mask = 1;

            if (dot_status)
            {
                if (!(line_pattern & current_pattern_mask) || is_done)
                {
                    wxPoint point_arr[4];

                    point_arr[0] = start_dot_point - normal_vector;
                    point_arr[1] = start_dot_point + normal_vector;
                    point_arr[2] = end_dot_point + normal_vector;
                    point_arr[3] = end_dot_point - normal_vector;
                    target_dc.DrawPolygon(4, point_arr);
                    start_dot_point = end_dot_point;
                    dot_status = false;
                }
            }
            else
            {
                if (line_pattern & current_pattern_mask)
                {
                    start_dot_point = end_dot_point;
                    dot_status = true;
                }
            }
        }
    }

    void DrawDashDotVLine(wxDC &target_dc, const wxPoint start_point, int line_length,
                          int line_width, unsigned int line_pattern)
    {
        if (!line_length)
            return;
        wxPoint start_dot_point(start_point.x , start_point.y);
        wxPoint end_point(start_point.x, start_point.y + line_length);
        wxPoint end_dot_point(start_dot_point);
        wxPoint normal_vector(line_width / 2, 0);
        unsigned int current_pattern_mask = 1;
        bool dot_status = line_pattern & current_pattern_mask;
        bool is_done = false;

        while (!is_done)
        {
            if (abs(end_point.y - end_dot_point.y) <= DOT_SIZE_INT)
            {
                end_dot_point.y = end_point.y;
                is_done = true;
            }
            else
            {
                end_dot_point.y += isign(line_length) * DOT_SIZE_INT;
            }

            current_pattern_mask <<= 1;
            if (!current_pattern_mask)
                current_pattern_mask = 1;

            if (dot_status)
            {
                if (!(line_pattern & current_pattern_mask) || is_done)
                {
                    wxPoint point_arr[4];

                    point_arr[0] = start_dot_point - normal_vector;
                    point_arr[1] = start_dot_point + normal_vector;
                    point_arr[2] = end_dot_point + normal_vector;
                    point_arr[3] = end_dot_point - normal_vector;
                    target_dc.DrawPolygon(4, point_arr);
                    start_dot_point = end_dot_point;
                    dot_status = false;
                }
            }
            else
            {
                if (line_pattern & current_pattern_mask)
                {
                    start_dot_point = end_dot_point;
                    dot_status = true;
                }
            }
        }
    }

    void DrawDashDotEllipticArc(wxDC &target_dc, wxPoint center, int radius_x, int radius_y,
                                double arc_start, double arc_end, int line_width_hor,
                                int line_width_vert, unsigned int line_pattern)
    {
        if (!(radius_x && radius_y))
            return;
        if (arc_start >= arc_end)
            arc_end += 360.0;

        int half_line_width_hor = line_width_hor / 2;
        int half_line_width_vert = line_width_vert / 2;

        if (radius_x < half_line_width_hor)
        {
            half_line_width_hor = radius_x;
            line_width_hor = half_line_width_hor * 2;
        }

        if (radius_y < half_line_width_vert)
        {
            half_line_width_vert = radius_y;
            line_width_vert = half_line_width_vert * 2;
        }

        int line_width_max = max(line_width_hor, line_width_vert);
        int half_line_width_max = line_width_max / 2;
        wxPoint upper_left_max(center.x - radius_x - half_line_width_max,
                               center.y - radius_y - half_line_width_max);
        wxSize ellipse_size_max(2 * radius_x + line_width_max, 2 * radius_y + line_width_max);
        wxPoint center_point(ellipse_size_max.GetWidth() / 2, ellipse_size_max.GetHeight() / 2);

        double current_angle = ToRadians(arc_start);
        double end_radian_angle = ToRadians(arc_end);
        double enddot_angle = current_angle;
        double delta_angle = static_cast<double>(DOT_SIZE) / sqrt(radius_x * radius_x + radius_y * radius_y);
        if (delta_angle < ZERO_TOLERANCE)
            return;

        wxBitmap mono_bitmap(ellipse_size_max.GetWidth(), ellipse_size_max.GetHeight(), 1);
        wxMemoryDC monoDC(mono_bitmap);
        monoDC.SetPen(*wxWHITE_PEN);
        monoDC.SetBrush(*wxWHITE_BRUSH);
        monoDC.SetBackground(*wxBLACK_BRUSH);
        monoDC.Clear();

        unsigned int current_pattern_mask = 1;
        bool dot_status = line_pattern & current_pattern_mask;
        bool is_done = false;
        while (!is_done)
        {
            enddot_angle += delta_angle;
            if (enddot_angle >= end_radian_angle)
            {
                enddot_angle = end_radian_angle;
                is_done = true;
            }

            current_pattern_mask <<= 1;
            if (!current_pattern_mask)
                current_pattern_mask = 1;

            if (dot_status)
            {
                if (!(line_pattern & current_pattern_mask) || is_done)
                {
                    monoDC.SetPen(*wxWHITE_PEN);
                    monoDC.SetBrush(*wxWHITE_BRUSH);

                    int current_line_width = ceil(CountCurrentLineWidthDbl(current_angle, line_width_hor, line_width_vert));
                    wxSize ellipse_size_out(2 * radius_x + current_line_width, 2 * radius_y + current_line_width);
                    wxPoint upper_left_out(center_point.x - ellipse_size_out.GetWidth() / 2,
                                           center_point.y - ellipse_size_out.GetHeight() / 2);
                    monoDC.DrawEllipticArc(upper_left_out, ellipse_size_out, ToDegrees(current_angle), ToDegrees(enddot_angle));
                    current_angle = enddot_angle;
                    dot_status = false;
                }
            }
            else
            {
                if (line_pattern & current_pattern_mask)
                {
                    current_angle = enddot_angle;
                    dot_status = true;
                }
            }
        }

        monoDC.SetPen(*wxBLACK_PEN);
        monoDC.SetBrush(*wxBLACK_BRUSH);

        wxSize ellipse_size_in(2 * radius_x - line_width_hor, 2 * radius_y - line_width_vert);
        wxPoint upper_left_in(center_point.x - ellipse_size_in.GetWidth() / 2,
                              center_point.y - ellipse_size_in.GetHeight() / 2);
        //monoDC.DrawEllipticArc(upper_left_in, ellipse_size_in, arc_start, arc_end);
        monoDC.DrawEllipse(upper_left_in, ellipse_size_in);
        monoDC.SelectObject(wxNullBitmap);

        wxBitmap tone_bitmap(ellipse_size_max.GetWidth(), ellipse_size_max.GetHeight(), target_dc.GetDepth());
        monoDC.SelectObject(tone_bitmap);
        monoDC.SetPen(target_dc.GetPen());
        monoDC.SetBrush(target_dc.GetBrush());
        monoDC.SetBackground(target_dc.GetBrush());
        monoDC.Clear();
        monoDC.SelectObject(wxNullBitmap);
        wxMask mask(mono_bitmap);
        tone_bitmap.SetMask(&mask);
        target_dc.DrawBitmap(tone_bitmap, upper_left_max, true);
    }

    void DrawDashDotEllipticArc(wxDC &target_dc, wxPoint center, int radius_x, int radius_y,
                                wxPoint start_arc_point, wxPoint end_arc_point, int line_width_hor,
                                int line_width_vert, unsigned int line_pattern)
    {
        // Расчет граничных углов дуги
        double arc_start = ConvertTO2PI(atan2(center.y - start_arc_point.y, start_arc_point.x - center.x));
        double arc_end = ConvertTO2PI(atan2(center.y - end_arc_point.y, end_arc_point.x - center.x));

        DrawDashDotEllipticArc(target_dc, center, radius_x, radius_y, ToDegrees(arc_start), ToDegrees(arc_end),
                               line_width_hor, line_width_vert, line_pattern);
    }

    void DrawDashDotEllipticArc(wxDC &target_dc, wxRect covered_rect, wxPoint start_arc_point, wxPoint end_arc_point,
                                int line_width_hor, int line_width_vert, unsigned int line_pattern)
    {
        // Расчет координат центра и радиусов эллипса
        int radius_x = (covered_rect.width - 1) / 2;
        int radius_y = (covered_rect.height - 1) / 2;
        wxPoint center(covered_rect.x + radius_x, covered_rect.y + radius_y);
        // Расчет граничных углов дуги
        double arc_start = ConvertTO2PI(atan2(start_arc_point.y, start_arc_point.x));
        double arc_end = ConvertTO2PI(atan2(end_arc_point.y, end_arc_point.x));

        DrawDashDotEllipticArc(target_dc, center, radius_x, radius_y, ToDegrees(arc_start), ToDegrees(arc_end),
                               line_width_hor, line_width_vert, line_pattern);
    }

    void DrawDashDotRect(wxDC &target_dc, wxRect rect, int line_width_hor, int line_width_vert,
                         unsigned int line_pattern)
    {
        wxPoint point1{rect.x, rect.y};
        wxPoint point2{rect.x + rect.width - 1, rect.y};
        wxPoint point3{rect.x + rect.width - 1, rect.y + rect.height - 1};
        wxPoint point4{rect.x, rect.y + rect.height - 1};

        DrawDashDotLine(target_dc, point1, point2, line_width_hor, line_width_vert, line_pattern);
        DrawDashDotLine(target_dc, point2, point3, line_width_hor, line_width_vert, line_pattern);
        DrawDashDotLine(target_dc, point3, point4, line_width_hor, line_width_vert, line_pattern);
        DrawDashDotLine(target_dc, point4, point1, line_width_hor, line_width_vert, line_pattern);
    }

    ContourSideDetectType IsPointOnContour(DrawContext& draw_context, wxPoint dev_test_point,
                                           const SelectContourData& select_contour)
    { // Здесь точка dev_test_point должна быть задана в пространстве (системе координат) целевого устройства
        ContourSideDetectType result = ContourSideDetectType::CONTOUR_NOT_INTERSECT;
        if (!select_contour.is_contour_builded)
            return result;
        auto [left_side, right_side, up_side, down_side] = GetSelectingContourSides(draw_context, select_contour);

        if (left_side.Contains(dev_test_point))
        {
            if (up_side.Contains(dev_test_point))
                result = ContourSideDetectType::CONTOUR_INTERSECT_LEFT_TOP;
            else if (down_side.Contains(dev_test_point))
                result = ContourSideDetectType::CONTOUR_INTERSECT_LEFT_BOTTOM;
            else
                result = ContourSideDetectType::CONTOUR_INTERSECT_LEFT;
        }
        else if (right_side.Contains(dev_test_point))
        {
            if (up_side.Contains(dev_test_point))
                result = ContourSideDetectType::CONTOUR_INTERSECT_RIGHT_TOP;
            else if (down_side.Contains(dev_test_point))
                result = ContourSideDetectType::CONTOUR_INTERSECT_RIGHT_BOTTOM;
            else
                result = ContourSideDetectType::CONTOUR_INTERSECT_RIGHT;
        }
        else if (up_side.Contains(dev_test_point))
        {
            if (left_side.Contains(dev_test_point))
                result = ContourSideDetectType::CONTOUR_INTERSECT_LEFT_TOP;
            else if (right_side.Contains(dev_test_point))
                result = ContourSideDetectType::CONTOUR_INTERSECT_RIGHT_TOP;
            else
                result = ContourSideDetectType::CONTOUR_INTERSECT_TOP;
        }
        else if (down_side.Contains(dev_test_point))
        {
            if (left_side.Contains(dev_test_point))
                result = ContourSideDetectType::CONTOUR_INTERSECT_LEFT_BOTTOM;
            else if (right_side.Contains(dev_test_point))
                result = ContourSideDetectType::CONTOUR_INTERSECT_RIGHT_BOTTOM;
            else
                result = ContourSideDetectType::CONTOUR_INTERSECT_BOTTOM;
        }

        return result;
    }

    void RestoreContourBackgroud(DrawContext& draw_context, SelectContourData& select_contour)
    {
        if (!select_contour.is_contour_builded)
            return;
        auto [left_side, right_side, up_side, down_side] =
            GetSelectingContourSides(draw_context, select_contour);

        select_contour.is_contour_builded = false;
        wxMemoryDC memoDC;
        memoDC.SelectObject(select_contour.save_left);
        draw_context.target_dc_ptr->Blit(left_side.GetLeftTop(), left_side.GetSize(), &memoDC, wxPoint(0, 0));
        memoDC.SelectObject(select_contour.save_right);
        draw_context.target_dc_ptr->Blit(right_side.GetLeftTop(), right_side.GetSize(), &memoDC, wxPoint(0, 0));
        memoDC.SelectObject(select_contour.save_top);
        draw_context.target_dc_ptr->Blit(up_side.GetLeftTop(), up_side.GetSize(), &memoDC, wxPoint(0, 0));
        memoDC.SelectObject(select_contour.save_down);
        draw_context.target_dc_ptr->Blit(down_side.GetLeftTop(), down_side.GetSize(), &memoDC, wxPoint(0, 0));
        memoDC.SelectObject(wxNullBitmap);
    }

    void DrawSelectingContour(DrawContext& draw_context, SelectContourData& select_contour)
    {
        auto [left_side, right_side, up_side, down_side] =
            GetSelectingContourSides(draw_context, select_contour);
        // Сначала сохраним фон, находящийся под линиями сторон выделяющего контура
        // Сохранение выполняется в области растровых изображений, связанные с полями
        // объекта select_contour - save_left, save_right, save_top и save_down.
        wxMemoryDC memDC;
        select_contour.save_left.Create(left_side.GetWidth(), left_side.GetHeight());
        memDC.SelectObject(select_contour.save_left);
        memDC.Blit(wxPoint(0, 0), left_side.GetSize(),
                   draw_context.target_dc_ptr, left_side.GetLeftTop());

        select_contour.save_right.Create(right_side.GetWidth(), right_side.GetHeight());
        memDC.SelectObject(select_contour.save_right);
        memDC.Blit(wxPoint(0, 0), right_side.GetSize(),
                   draw_context.target_dc_ptr, right_side.GetLeftTop());

        select_contour.save_top.Create(up_side.GetWidth(), up_side.GetHeight());
        memDC.SelectObject(select_contour.save_top);
        memDC.Blit(wxPoint(0, 0), up_side.GetSize(),
                   draw_context.target_dc_ptr, up_side.GetLeftTop());

        select_contour.save_down.Create(down_side.GetWidth(), down_side.GetHeight());
        memDC.SelectObject(select_contour.save_down);
        memDC.Blit(wxPoint(0, 0), down_side.GetSize(),
                   draw_context.target_dc_ptr, down_side.GetLeftTop());

        //Далее описан упрощённый, но быстрый способ рисования рамки выделяющего контура.
        //draw_context.target_dc_ptr->SetPen(*wxWHITE_PEN);
        //draw_context.target_dc_ptr->SetBrush(*wxTRANSPARENT_BRUSH);
        //draw_context.target_dc_ptr->DrawRectangle(left_side);
        //draw_context.target_dc_ptr->DrawRectangle(right_side);
        //draw_context.target_dc_ptr->DrawRectangle(up_side);
        //draw_context.target_dc_ptr->DrawRectangle(down_side);
        //select_contour.is_contour_builded = true;
        //return;

        // Теперь рисуем собственно выделяющий прямоугольник. Он состоит из 4 штриховых линий
        // толщиной SELECT_CONTOUR_WIDTH. Сначала рисуем боковины.
        wxBitmap draw_bmp;
        draw_bmp.Create(left_side.GetWidth(), left_side.GetHeight());
        memDC.SelectObject(draw_bmp);
        memDC.SetPen(*wxWHITE_PEN);
        memDC.SetBrush(*wxWHITE_BRUSH);
        DrawDashDotVLine(memDC, wxPoint(SELECT_CONTOUR_WIDTH / 2, SELECT_CONTOUR_WIDTH / 2), left_side.GetHeight(),
                         SELECT_CONTOUR_WIDTH, DOTTED_RAPID_LINE_MASK);

        draw_context.target_dc_ptr->Blit(left_side.GetLeftTop(), left_side.GetSize(),
                                         &memDC, wxPoint(0, 0), wxXOR);
        draw_context.target_dc_ptr->Blit(right_side.GetLeftTop(), right_side.GetSize(),
                                         &memDC, wxPoint(0, 0), wxXOR);
        memDC.SelectObject(wxNullBitmap);
        // А теперь отрисовываем верх и низ.
        draw_bmp.Create(up_side.GetWidth(), up_side.GetHeight());
        memDC.SelectObject(draw_bmp);
        memDC.SetPen(*wxWHITE_PEN);
        memDC.SetBrush(*wxWHITE_BRUSH);
        DrawDashDotHLine(memDC, wxPoint(SELECT_CONTOUR_WIDTH / 2, SELECT_CONTOUR_WIDTH / 2), up_side.GetWidth(),
                         SELECT_CONTOUR_WIDTH, DOTTED_RAPID_LINE_MASK);

        draw_context.target_dc_ptr->Blit(up_side.GetLeftTop(), up_side.GetSize(),
                                         &memDC, wxPoint(0, 0), wxXOR);
        draw_context.target_dc_ptr->Blit(down_side.GetLeftTop(), down_side.GetSize(),
                                         &memDC, wxPoint(0, 0), wxXOR);
        // Обозначим вершины прямоугольника небольшими "ручками".
        wxSize handle_size(left_side.GetWidth(), down_side.GetHeight());
        wxRect left_top_handle(left_side.GetTopLeft(), handle_size);
        wxRect right_top_handle(right_side.GetTopLeft(), handle_size);
        wxRect left_bottom_handle(down_side.GetTopLeft(), handle_size);
        wxRect right_bottom_handle(wxPoint(down_side.GetRight() - handle_size.GetWidth() + 1,
                                           down_side.GetBottom() - handle_size.GetHeight() + 1),
                                   handle_size);
        draw_context.target_dc_ptr->SetPen(*wxWHITE_PEN);
        draw_context.target_dc_ptr->SetBrush(*wxBLACK_BRUSH);
        draw_context.target_dc_ptr->DrawRectangle(left_top_handle);
        draw_context.target_dc_ptr->DrawRectangle(right_top_handle);
        draw_context.target_dc_ptr->DrawRectangle(left_bottom_handle);
        draw_context.target_dc_ptr->DrawRectangle(right_bottom_handle);
        // Рисование завершено, устанавливаем новый статус is_contour_builded и выходим
        select_contour.is_contour_builded = true;
    }
} // namespace DrawEntities
