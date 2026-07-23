#pragma once
#include <string>
#include <variant>
#include <wx/colour.h>
#include <wx/gdicmn.h>
#include "PCADViewDraw.h"

std::string ConvertSizeToString(double size_value, DrawContext& draw_context);
std::string ConvertPointToString(wxPoint pnt, DrawContext& draw_context);
std::string ConvertRectToString(const wxRect& rect, DrawContext& draw_context);
std::string ConvertColorToString(const SelColorType& col);
std::string ConvertRectToStringShort(const wxRect& rect, DrawContext& draw_context);
std::string ConvertRectToStringShortEx(const wxRect& rect, DrawContext& draw_context);
std::string ConvertColorToStringShort(const SelColorType& col);
