#pragma once
#include <functional>
#include <any>
#include <wx/defs.h>
#include <wx/colour.h>
#include <wx/pen.h>
#include <wx/brush.h>
#include <wx/gdicmn.h>
#include <wx/palette.h>

#include "PCADViewDraw.h"
#include "PCADViewerApp.h"

namespace DrawEntities
{
    class wxPointDouble
    {
    public:
        double x, y;

        wxPointDouble() : x(0), y(0)
        {}
        wxPointDouble(double xx, double yy) : x(xx), y(yy)
        {}

        bool operator==(const wxPointDouble& p) const
        {
            return (abs(x - p.x) < ZERO_TOLERANCE) &&
                   (abs(y - p.y) < ZERO_TOLERANCE);
        }

        bool operator!=(const wxPointDouble& p) const
        {
            return !(*this == p);
        }

        wxPointDouble operator+(const wxPointDouble& p) const
        {
            return wxPointDouble(x + p.x, y + p.y);
        }

        wxPointDouble operator-(const wxPointDouble& p) const
        {
            return wxPointDouble(x - p.x, y - p.y);
        }

        wxPointDouble& operator+=(const wxPointDouble& p)
        {
            x += p.x;
            y += p.y;
            return *this;
        }

        wxPointDouble& operator-=(const wxPointDouble& p)
        {
            x -= p.x;
            y -= p.y;
            return *this;
        }

        wxPointDouble operator-() const
        {
            return wxPointDouble(-x, -y);
        }
    };

    int CountHalfWidth(int full_width);
    int CountCurrentLineWidth(double current_angle, int line_width_hor, int line_width_vert);
    void DrawDashDotLine(wxDC &target_dc, const wxPoint start_point, const wxPoint end_point,
                         int line_width_hor, int line_width_vert, unsigned int line_pattern);
    void DrawDashDotLine(wxDC &target_dc, const wxPoint start_point, const wxPoint end_point,
                         int line_width, unsigned int line_pattern);
    void DrawDashDotHLine(wxDC &target_dc, const wxPoint start_point, int line_length,
                          int line_width, unsigned int line_pattern);
    void DrawDashDotVLine(wxDC &target_dc, const wxPoint start_point, int line_length,
                          int line_width, unsigned int line_pattern);
    void DrawDashDotEllipticArc(wxDC &target_dc, wxPoint center, int radius_x, int radius_y,
                                double arc_start, double arc_end, int line_width_hor,
                                int line_width_vert, unsigned int line_pattern);
    void DrawDashDotEllipticArc(wxDC &target_dc, wxPoint center, int radius_x, int radius_y,
                                wxPoint start_arc_point, wxPoint end_arc_point, int line_width_hor,
                                 int line_width_vert, unsigned int line_pattern);
    void DrawDashDotEllipticArc(wxDC &target_dc, wxRect covered_rect, wxPoint start_arc_point,
                                wxPoint end_arc_point, int line_width_hor, int line_width_vert,
                                unsigned int line_pattern);
    void DrawDashDotRect(wxDC &target_dc, wxRect rect, int line_width_hor, int line_width_vert,
                         unsigned int line_pattern);
    void RestoreContourBackgroud(DrawContext& draw_context, SelectContourData& select_contour);
    void DrawSelectingContour(DrawContext& draw_context, SelectContourData& select_contour);
    ContourSideDetectType IsPointOnContour(DrawContext& draw_context, wxPoint dev_test_point,
                                           const SelectContourData& select_contour);
} // namespace DrawEntities
