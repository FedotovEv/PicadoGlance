#include <iostream>
#include <sstream>
#include <iomanip>
#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <variant>
#include <typeinfo>

#include "svg.h"

namespace svg
{
    using namespace std::literals;

    const char* TEXT_ANCHOR_VALUES[] = {"", "start", "middle", "end"};

    std::string EscapeXMLSymbols(const std::string& arg_str)
    {
        std::string result;
        for (char c : arg_str)
            switch(c)
            {
                case '"':
                    result += "&quot;";
                    break;
                case '\'':
                    result += "&apos;";
                    break;
                case '<':
                    result += "&lt;";
                    break;
                case '>':
                    result += "&gt;";
                    break;
                case '&':
                    result += "&amp;";
                    break;
                default:
                    result += c;
            }
        return result;
    }

    std::ostream& operator<<(std::ostream& out, StrokeLineCap stc)
    {
        out << line_cap_value_[static_cast<int>(stc)];
        return out;
    }

    std::ostream& operator<<(std::ostream& out, StrokeLineJoin slj)
    {
        out << line_join_value_[static_cast<int>(slj)];
        return out;
    }

    // ---------- ColorHandler ------------------

    std::string ColorHandler::operator()(std::monostate)
    {
        return {};
    }

    std::string ColorHandler::operator()(const Rgb& rgb)
    {
        const std::string r = std::to_string(rgb.red);
        const std::string g = std::to_string(rgb.green);
        const std::string b = std::to_string(rgb.blue);

        return "rgb("s + r + ","s + g + ","s + b + ")"s;
    }

    std::string ColorHandler::operator()(const Rgba& rgba)
    {
        const std::string r = std::to_string(rgba.red);
        const std::string g = std::to_string(rgba.green);
        const std::string b = std::to_string(rgba.blue);
        std::ostringstream alpha;
        alpha << rgba.opacity;

        return "rgba("s + r + ","s + g + ","s + b + ","s + alpha.str() + ")"s;
    }

    std::string ColorHandler::operator()(const std::string& string_color)
    {
        return string_color;
    }

    // ---------- Document ------------------

    void Document::AddPtr(std::unique_ptr<Object>&& obj)
    {
        objects_list_.push_back(std::move(obj));
    }

    void Document::Render(std::ostream& out) const
    {
        const std::string viewport_fill_col = std::visit(ColorHandler{}, viewport_fill_color_);

        out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" << std::endl;
        out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\"";
        if (viewport_fill_col.size())
            out << " viewport-fill=\"" << viewport_fill_col << "\"";
        out << ">" << std::endl;

        size_t pattern_count = 0;
        for (auto& object_ptr : objects_list_)
            if (typeid(*object_ptr) == typeid(Pattern))
                ++pattern_count;
        if (pattern_count)
        {
            out << "<defs>" << std::endl;
            for (auto& object_ptr : objects_list_)
                if (typeid(*object_ptr) == typeid(Pattern))
                    object_ptr->Render(out);
            out << "</defs>" << std::endl;
        }

        for (auto& object_ptr : objects_list_)
            if (typeid(*object_ptr) != typeid(Pattern))
                object_ptr->Render(out);

        out << "</svg>" << std::endl;
    }

    std::pair<std::weak_ptr<Object>, bool> Document::ResourceManager(ResourceType resource_type,
                                                    const std::string& id, bool is_create)
    {
        for (auto& object_ptr : objects_list_)
        {
            if (resource_type == ResourceType::SVG_RESOURCE_PATTERN)
            {
                Pattern* pattern_ptr = dynamic_cast<Pattern*>(object_ptr.get());
                if (pattern_ptr && pattern_ptr->GetId() == id)
                    return {object_ptr, true};
            }
            else
            {
                return {{}, false};
            }
        }
        if (!is_create || resource_type == ResourceType::SVG_RESOURCE_UNKNOWN)
            return {{}, false};

        if (resource_type == ResourceType::SVG_RESOURCE_PATTERN)
        {
            std::unique_ptr<Pattern> object_ptr = std::make_unique<Pattern>();
            object_ptr->SetId(id);
            objects_list_.push_back(std::move(object_ptr));
        }
        return {objects_list_.back(), false};
    }

    // ---------- Object ------------------

    void Object::Render(const RenderContext& context) const
    {
        context.RenderIndent();

        // Делегируем вывод тега своим подклассам
        RenderObject(context);

        context.out << std::endl;
    }

    // ---------- Pattern ------------------

    Pattern& Pattern::SetId(const std::string& id)
    {
        id_ = id;
        return *this;
    }

    Pattern& Pattern::SetPatternUnits(const std::string& pattern_units)
    {
        pattern_units_ = pattern_units;
        return *this;
    }

    Pattern& Pattern::SetWidth(double width)
    {
        width_ = width;
        return *this;
    }

    Pattern& Pattern::SetHeight(double height)
    {
        height_ = height;
        return *this;
    }

    Pattern& Pattern::SetWidthHeight(Point width_height)
    {
        width_ = width_height.x;
        height_ = width_height.y;
        return *this;
    }

    void Pattern::AddPtr(std::unique_ptr<Object>&& obj)
    {
        objects_list_.push_back(std::move(obj));
    }

    void Pattern::RenderObject(const RenderContext& context) const
    {
        auto& out = context.out;
        out << "<pattern id=\"" << id_ << "\" patternUnits=\"" << pattern_units_ << "\" ";
        out << "width=\"" << width_ << "\" height=\"" << height_ << "\">" << std::endl;

        RenderContext new_context = context.Indented();
        for (auto& object_ptr : objects_list_)
            object_ptr->Render(new_context);

        context.RenderIndent();
        out << "</pattern>";
    }

    // ---------- Circle ------------------

    Circle& Circle::SetCenter(Point center)
    {
        center_ = center;
        return *this;
    }

    Circle& Circle::SetRadius(double radius)
    {
        radius_ = radius;
        return *this;
    }

    void Circle::RenderObject(const RenderContext& context) const
    {
        auto& out = context.out;
        out << "<circle cx=\"" << center_.x << "\" cy=\"" << center_.y << "\" ";
        out << "r=\"" << radius_ << '"';
        RenderAttrs(out);
        out << "/>";
    }

    // ---------- Ellipse ------------------

    Ellipse& Ellipse::SetCenter(Point center)
    {
        center_ = center;
        return *this;
    }

    Ellipse& Ellipse::SetRadiusX(double radius)
    {
        radius_x_ = radius;
        return *this;
    }

    Ellipse& Ellipse::SetRadiusY(double radius)
    {
        radius_y_ = radius;
        return *this;
    }

    Ellipse& Ellipse::SetRadiuses(Point radiuses)
    {
        radius_x_ = radiuses.x;
        radius_y_ = radiuses.y;
        return *this;
    }

    void Ellipse::RenderObject(const RenderContext& context) const
    {
        auto& out = context.out;
        out << "<ellipse cx=\"" << center_.x << "\" cy=\"" << center_.y << "\" ";
        out << "rx=\"" << radius_x_ << "\" ry=\"" << radius_y_ << '"';
        RenderAttrs(out);
        out << "/>";
    }

    // ---------- Rectangle ------------------

    Rectangle& Rectangle::SetTopLeftCorner(Point top_left_corner)
    {
        top_left_corner_ = top_left_corner;
        return *this;
    }

    Rectangle& Rectangle::SetWidth(double width)
    {
        width_ = width;
        return *this;
    }

    Rectangle& Rectangle::SetHeight(double height)
    {
        height_ = height;
        return *this;
    }

    Rectangle& Rectangle::SetWidthHeight(Point width_height)
    {
        width_ = width_height.x;
        height_ = width_height.y;
        return *this;
    }

    Rectangle& Rectangle::SetCornerRadiusX(double corner_radius)
    {
        corner_radius_x_ = corner_radius;
        return *this;
    }

    Rectangle& Rectangle::SetCornerRadiusY(double corner_radius)
    {
        corner_radius_y_ = corner_radius;
        return *this;
    }

    Rectangle& Rectangle::SetCornerRadiuses(Point corner_radiuses)
    {
        corner_radius_x_ = corner_radiuses.x;
        corner_radius_x_ = corner_radiuses.y;
        return *this;
    }

    void Rectangle::RenderObject(const RenderContext& context) const
    {
        auto& out = context.out;
        out << "<rect x=\"" << top_left_corner_.x << "\" y=\"" << top_left_corner_.y << "\" ";
        out << "width=\"" << width_ << "\" height=\"" << height_ << "\" ";
        out << "rx=\"" << corner_radius_x_ << "\" ry=\"" << corner_radius_y_ << '"';
        RenderAttrs(out);
        out << "/>";
    }

    // ------------ Line --------------------

    Line& Line::SetPoint(int point_number, Point point)
    {
        if (point_number == 1)
            point1_ = point;
        else if (point_number == 2)
            point2_ = point;
        return *this;
    }

    void Line::RenderObject(const RenderContext& context) const
    {
        auto& out = context.out;
        out << "<line x1=\"" << point1_.x << "\" y1=\"" << point1_.y << "\" ";
        out << "x2=\"" << point2_.x << "\" y2=\"" << point2_.y << '"';
        RenderAttrs(out);
        out << "/>";
    }

    // ----------- EllipticArc -----------------

    EllipticArc& EllipticArc::SetStartPoint(Point start_point)
    {
        start_point_ = start_point;
        return *this;
    }

    EllipticArc& EllipticArc::SetEndPoint(Point end_point)
    {
        end_point_ = end_point;
        return *this;
    }

    EllipticArc& EllipticArc::SetRadiusX(double radius)
    {
        radius_x_ = radius;
        return *this;
    }

    EllipticArc& EllipticArc::SetRadiusY(double radius)
    {
        radius_y_ = radius;
        return *this;
    }

    EllipticArc& EllipticArc::SetRadiuses(Point radiuses)
    {
        radius_x_ = radiuses.x;
        radius_y_ = radiuses.y;
        return *this;
    }

    EllipticArc& EllipticArc::SetXAxisRotation(double x_axis_rotation)
    {
        x_axis_rotation_ = x_axis_rotation;
        return *this;
    }

    EllipticArc& EllipticArc::SetLargeArcFlag(bool large_arc)
    {
        large_arc_ = large_arc;
        return *this;
    }

    EllipticArc& EllipticArc::SetSweepFlag(bool sweep)
    {
        sweep_ = sweep;
        return *this;
    }

    void EllipticArc::RenderObject(const RenderContext& context) const
    {
        auto& out = context.out;
        out << std::noboolalpha << "<path d=\"M ";
        out << start_point_.x << ' ' << start_point_.y << ' ';
        out << "A " << radius_x_ << ' ' << radius_y_ << ' ';
        out << x_axis_rotation_ << ' ' << large_arc_ << ' ' << sweep_ << ' ';
        out << end_point_.x << ' ' << end_point_.y << '"';
        RenderAttrs(out);
        out << "/>";
    }

    // ---------- Polyline ------------------

    Polyline& Polyline::AddPoint(Point point)
    {
        poly_points_.push_back(std::move(point));
        return *this;
    }

    void Polyline::RenderObject(const RenderContext& context) const
    {
        context.out << "<polyline points=\"";
        size_t point_number = 0;
        for (const Point& point : poly_points_)
        {
            context.out << point.x << ',' << point.y;
            if (point_number < poly_points_.size() - 1)
                context.out << ' ';
            ++point_number;
        }
        context.out << '"';
        RenderAttrs(context.out);
        context.out << "/>";
    }

    // ---------- Polygon ------------------

    Polygon& Polygon::AddPoint(Point point)
    {
        poly_points_.push_back(std::move(point));
        return *this;
    }

    void Polygon::RenderObject(const RenderContext& context) const
    {
        context.out << "<polygon points=\"";
        size_t point_number = 0;
        for (const Point& point : poly_points_)
        {
            context.out << point.x << ',' << point.y;
            if (point_number < poly_points_.size() - 1)
                context.out << ' ';
            ++point_number;
        }
        context.out << '"';
        RenderAttrs(context.out);
        context.out << "/>";
    }

    // ---------- Text ------------------

    Text& Text::SetPosition(Point pos)
    {
        pos_ = pos;
        return *this;
    }

    Text& Text::SetOffset(Point offset)
    {
        offset_ = offset;
        return *this;
    }

    Text& Text::SetOffset(StringPoint string_offset)
    {
        offset_ = std::move(string_offset);
        return *this;
    }

    Text& Text::SetFontSize(uint32_t size)
    {
        font_size_ = size;
        return *this;
    }

    Text& Text::SetFontFamily(const std::string& font_family)
    {
        font_family_ = font_family;
        return *this;
    }

    Text& Text::SetFontWeight(const std::string& font_weight)
    {
        font_weight_ = font_weight;
        return *this;
    }

    Text& Text::SetRotateAngle(double rotate_angle)
    {
        rotate_angle_ = rotate_angle;
        return *this;
    }

    Text& Text::SetMirrorAttr(bool is_mirror_text)
    {
        is_mirror_text_ = is_mirror_text;
        return *this;
    }

    Text& Text::SetRotateGlyphAngle(double rotate_glyph_angle)
    {
        rotate_glyph_angle_ = rotate_glyph_angle;
        return *this;
    }

    Text& Text::SetTextAnchor(TextAnchorType text_anchor)
    {
        text_anchor_ = text_anchor;
        return *this;
    }

    Text& Text::SetData(const std::string& data)
    {
        data_ = data;
        return *this;
    }

    void Text::RenderObject(const RenderContext& context) const
    {
        std::string transform_string;

        context.out << "<text x=\"" << pos_.x << "\" y=\"" << pos_.y << '"';
        if (std::holds_alternative<StringPoint>(offset_))
        {
            StringPoint string_offset = std::get<StringPoint>(offset_);
            context.out << " dx=\"" << string_offset.x << "\" dy=\"" << string_offset.y << '"';
        }
        else
        {
            Point double_offset = std::get<Point>(offset_);
            context.out << " dx=\"" << double_offset.x << "\" dy=\"" << double_offset.y << '"';
        }
        if (rotate_glyph_angle_.has_value())
            context.out << " rotate=\"" << rotate_glyph_angle_.value() << '"';
        if (font_size_ > 0)
            context.out << " font-size=\"" << font_size_ << '"';
        if (font_family_.size() > 0)
            context.out << " font-family=\"" << font_family_ << '"';
        if (font_weight_.size() > 0)
            context.out << " font-weight=\"" << font_weight_ << '"';
        if (text_anchor_ != TextAnchorType::TEXT_ANCHOR_UNKNOWN)
            context.out << " text-anchor=\"" <<
                           TEXT_ANCHOR_VALUES[static_cast<int>(text_anchor_)] << '"';
        if (rotate_angle_.has_value())
        {
            std::ostringstream ostr;
            ostr << " rotate(" << rotate_angle_.value() << ", ";
            ostr << pos_.x << ", " << pos_.y << ")";
            transform_string += ostr.str();
        }
        if (is_mirror_text_)
        {
            std::ostringstream ostr;
            if (transform_string.size())
                ostr << ' ';
            ostr << "translate(" << pos_.x << ", " << pos_.y << ")";
            ostr << " scale(-1,1) translate(" << -pos_.x << ", " << -pos_.y << ")";
            transform_string += ostr.str();
        }
        if (transform_string.size())
            context.out << " transform=\"" << transform_string << '"';

        RenderAttrs(context.out);
        context.out << '>';
        context.out << EscapeXMLSymbols(data_) << "</text>";
    }
}  // namespace svg
