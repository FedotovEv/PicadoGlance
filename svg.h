#pragma once

#include <cstdint>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <variant>

namespace svg
{
    using namespace std::literals;

    enum class ResourceType
    {
        SVG_RESOURCE_UNKNOWN = 0,
        SVG_RESOURCE_PATTERN
    };

    struct Point
    {
        Point() = default;
        Point(double x, double y) : x(x), y(y)
        {}

        double x = 0;
        double y = 0;
    };

    struct StringPoint
    {
        StringPoint() = default;
        StringPoint(std::string x_str, std::string y_str) :
            x(std::move(x_str)), y(std::move(y_str))
        {}

        std::string x;
        std::string y;
    };

    std::string EscapeXMLSymbols(const std::string& arg_str);

    /*
     * Вспомогательная структура, хранящая контекст для вывода SVG-документа с отступами.
     * Хранит ссылку на поток вывода, текущее значение и шаг отступа при выводе элемента
     */
    struct RenderContext
    {
        RenderContext(std::ostream& out) : out(out)
        {}

        RenderContext(std::ostream& out, int indent_step, int indent = 0)
            : out(out), indent_step(indent_step), indent(indent)
        {}

        RenderContext Indented() const
        {
            return {out, indent_step, indent + indent_step};
        }

        void RenderIndent() const
        {
            for (int i = 0; i < indent; ++i)
                out.put(' ');
        }

        std::ostream& out;
        int indent_step = 0;
        int indent = 0;
    };

    /*
     * Абстрактный базовый класс Object служит для унифицированного хранения
     * конкретных тегов SVG-документа
     * Реализует паттерн "Шаблонный метод" для вывода содержимого тега
     */
    class Object
    {
    public:
        void Render(const RenderContext& context) const;
        virtual ~Object() = default;

    private:
        virtual void RenderObject(const RenderContext& context) const = 0;
    };

    struct Rgb
    {
        Rgb(uint8_t red_ = 0, uint8_t green_ = 0, uint8_t blue_ = 0) :
            red(red_), green(green_), blue(blue_)
        {}

        uint8_t red;
        uint8_t green;
        uint8_t blue;
    };

    struct Rgba
    {
        Rgba(uint8_t red_ = 0, uint8_t green_ = 0, uint8_t blue_ = 0, double opacity_ = 1) :
            red(red_), green(green_), blue(blue_), opacity(opacity_)
        {}

        uint8_t red;
        uint8_t green;
        uint8_t blue;
        double opacity;
    };

    using Color = std::variant<std::monostate, std::string, Rgb, Rgba>;

    struct ColorHandler
    {
        std::string operator()(std::monostate);
        std::string operator()(const Rgb& rgb);
        std::string operator()(const Rgba& rgba);
        std::string operator()(const std::string& string_color);
    };

    inline const std::string NoneColor = "none";
    inline const std::string line_cap_value_[] = {"", "butt", "round", "square"};
    inline const std::string line_join_value_[] =
        {"", "arcs", "bevel", "miter", "miter-clip", "round"};

    enum class StrokeLineCap
    {
        NO_LINE_CAP = 0,
        BUTT,
        ROUND,
        SQUARE
    };

    enum class StrokeLineJoin
    {
        NO_LINE_JOIN = 0,
        ARCS,
        BEVEL,
        MITER,
        MITER_CLIP,
        ROUND
    };

    enum class StrokeDashArray
    {
        LINE_CONTINUOUS = 0,
        LINE_DOTTED,
        LINE_DASHED
    };

    enum class TextAnchorType
    {
        TEXT_ANCHOR_UNKNOWN = 0,
        TEXT_ANCHOR_START = 1,
        TEXT_ANCHOR_MIDDLE,
        TEXT_ANCHOR_END,
    };

    inline const std::vector<int> line_dotted_dasharray{10, 2};
    inline const std::vector<int> line_dashed_dasharray{10};

    std::ostream& operator<<(std::ostream& out, StrokeLineCap stc);
    std::ostream& operator<<(std::ostream& out, StrokeLineJoin slj);

    template <typename Owner>
    class PathProps
    {
    public:
        PathProps() = default;

        Owner& SetFillColor(Color color)
        {
            fill_color_ = std::move(color);
            return static_cast<Owner&>(*this);
        }

        Owner& SetStrokeColor(Color color)
        {
            stroke_color_ = std::move(color);
            return static_cast<Owner&>(*this);
        }

        Owner& SetStrokeWidth(double width)
        {
            width_ = width;
            return static_cast<Owner&>(*this);
        }

        Owner& SetStrokeLineCap(StrokeLineCap line_cap)
        {
            line_cap_ = line_cap;
            return static_cast<Owner&>(*this);
        }

        Owner& SetStrokeLineJoin(StrokeLineJoin line_join)
        {
            line_join_ = line_join;
            return static_cast<Owner&>(*this);
        }

        Owner& SetStrokeDashArray(StrokeDashArray dash_cfg)
        {
            const std::vector<int>* dash_list_ptr = nullptr;
            dash_array_.clear();
            switch (dash_cfg)
            {
            case StrokeDashArray::LINE_DOTTED:
                dash_list_ptr = &line_dotted_dasharray;
                break;
            case StrokeDashArray::LINE_DASHED:
                dash_list_ptr = &line_dashed_dasharray;
                break;
            default:
                return static_cast<Owner&>(*this);
            }
            for (int dash_val : *dash_list_ptr)
                dash_array_.push_back(dash_val);
            return static_cast<Owner&>(*this);
        }

        Owner& SetStrokeDashArray(std::initializer_list<int> dash_list)
        {
            dash_array_.clear();
            for (int dash_val : dash_list)
                dash_array_.push_back(dash_val);
            return static_cast<Owner&>(*this);
        }

    protected:
        ~PathProps() = default;

        void RenderAttrs(std::ostream& out) const
        {
            const std::string fill_col = std::visit(ColorHandler{}, fill_color_);
            const std::string stroke_col = std::visit(ColorHandler{}, stroke_color_);

            if (fill_col.size())
                out << " fill=\"" << fill_col << "\"";
            if (stroke_col.size())
                out << " stroke=\"" << stroke_col << "\"";
            if (width_ > 0)
                out << " stroke-width=\"" << width_ << "\"";
            if (line_cap_ != StrokeLineCap::NO_LINE_CAP)
                out << " stroke-linecap=\"" << line_cap_value_[static_cast<int>(line_cap_)] << "\"";
            if (line_join_ != StrokeLineJoin::NO_LINE_JOIN)
                out << " stroke-linejoin=\"" << line_join_value_[static_cast<int>(line_join_)] << "\"";
            if (dash_array_.size())
            {
                out << " stroke-dasharray=\"";
                size_t dash_cnt = 0;
                for (int dash_val : dash_array_)
                {
                    out << dash_val;
                    if (++dash_cnt < dash_array_.size())
                        out << ',';
                }
                out << "\"";
            }
        }

    private:

        Color fill_color_;
        Color stroke_color_;
        double width_ = -1;
        StrokeLineCap line_cap_ = StrokeLineCap::NO_LINE_CAP;
        StrokeLineJoin line_join_ = StrokeLineJoin::NO_LINE_JOIN;
        std::vector<int> dash_array_;
    };

    class Document;
    class Pattern final : public Object
    {
    public:
        Pattern& SetId(const std::string& id);
        Pattern& SetPatternUnits(const std::string& pattern_units);
        Pattern& SetWidth(double width);
        Pattern& SetHeight(double height);
        Pattern& SetWidthHeight(Point width_height);
        std::string GetId()
        {
            return id_;
        }

        // Добавляет в шаблон новый элемент типа svg::Object
        virtual void AddPtr(std::unique_ptr<Object>&& obj);

        template <typename T>
        void Add(const T& object)
        {
            AddPtr(std::make_unique<T>(object));
        }

    private:
        void RenderObject(const RenderContext& context) const override;

        std::string id_;
        std::string pattern_units_ = "userSpaceOnUse";
        double width_ = 1;
        double height_ = 1;
        std::vector<std::shared_ptr<Object>> objects_list_;
    };

    /*
     * Класс Circle моделирует элемент <circle> для отображения круга
     * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/circle
     */
    class Circle final : public Object, public PathProps<Circle>
    {
    public:
        Circle& SetCenter(Point center);
        Circle& SetRadius(double radius);

    private:
        void RenderObject(const RenderContext& context) const override;

        Point center_;
        double radius_ = 1.0;
    };

    /*
     * Класс Polyline моделирует элемент <polyline> для отображения ломаных линий
     * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/polyline
     */
    class Polyline final : public Object, public PathProps<Polyline>
    {
    public:
        // Добавляет очередную вершину к ломаной линии
        Polyline& AddPoint(Point point);

    private:
        virtual void RenderObject(const RenderContext& context) const override;

        std::vector<Point> poly_points_; //Массив точек ломаной
    };

   /*
     * Класс Polygon моделирует элемент <polygone> для отображения замкнутых
     * контуров, состоящих из ломаных линий.
     * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/polygon
     */
    class Polygon final : public Object, public PathProps<Polygon>
    {
    public:
        // Добавляет очередную вершину к ломаному контуру
        Polygon& AddPoint(Point point);

    private:
        virtual void RenderObject(const RenderContext& context) const override;

        std::vector<Point> poly_points_; //Массив вершин полигона
    };

    /*
     * Класс Ellipse моделирует элемент <ellipse> для отображения эллипса
     * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/ellipse
     */
    class Ellipse final : public Object, public PathProps<Ellipse>
    {
    public:
        Ellipse& SetCenter(Point center);
        Ellipse& SetRadiusX(double radius);
        Ellipse& SetRadiusY(double radius);
        Ellipse& SetRadiuses(Point radiuses);

    private:
        void RenderObject(const RenderContext& context) const override;

        Point center_;
        double radius_x_ = 1.0;
        double radius_y_ = 1.0;
    };

    /*
     * Класс Rectangle моделирует элемент <rect> для отображения прямоугольника
     * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/rect
     */
    class Rectangle final : public Object, public PathProps<Rectangle>
    {
    public:
        Rectangle& SetTopLeftCorner(Point top_left_corner);
        Rectangle& SetWidth(double width);
        Rectangle& SetHeight(double height);
        Rectangle& SetWidthHeight(Point width_height);
        Rectangle& SetCornerRadiusX(double corner_radius);
        Rectangle& SetCornerRadiusY(double corner_radius);
        Rectangle& SetCornerRadiuses(Point corner_radiuses);

    private:
        void RenderObject(const RenderContext& context) const override;

        Point top_left_corner_;
        double width_ = 1.0;
        double height_ = 1.0;
        double corner_radius_x_ = 1.0;
        double corner_radius_y_ = 1.0;
    };

    /*
     * Класс Line моделирует элемент <line> для отображения отрезка
     * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/line
     */
    class Line final : public Object, public PathProps<Line>
    {
    public:
        Line& SetPoint(int point_number, Point point);

    private:
        void RenderObject(const RenderContext& context) const override;

        Point point1_;
        Point point2_;
    };

    /*
     * Класс EllipticArc использует механизм путей <path> для рисования дуги эллипса
     * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/path
     */
    class EllipticArc final : public Object, public PathProps<EllipticArc>
    {
    public:
        EllipticArc& SetStartPoint(Point start_point);
        EllipticArc& SetEndPoint(Point end_point);
        EllipticArc& SetRadiusX(double radius);
        EllipticArc& SetRadiusY(double radius);
        EllipticArc& SetRadiuses(Point radiuses);
        EllipticArc& SetXAxisRotation(double x_axis_rotation);
        EllipticArc& SetLargeArcFlag(bool large_arc);
        EllipticArc& SetSweepFlag(bool sweep);

    private:
        void RenderObject(const RenderContext& context) const override;

        Point start_point_;
        Point end_point_;
        double radius_x_ = 1.0;
        double radius_y_ = 1.0;
        double x_axis_rotation_ = 0;
        bool large_arc_ = false;
        bool sweep_ = false;
    };

    /*
     * Класс Text моделирует элемент <text> для отображения текста
     * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/text
     */
    class Text final : public Object, public PathProps<Text>
    {
    public:
        // Задаёт координаты опорной точки (атрибуты x и y)
        Text& SetPosition(Point pos);
        // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
        Text& SetOffset(Point offset);
        Text& SetOffset(StringPoint string_offset);
        // Задаёт размеры шрифта (атрибут font-size)
        Text& SetFontSize(uint32_t size);
        // Устанавливает угол наклона направляющей линии вывода текста
        Text& SetRotateAngle(double rotate_angle);
        // Установка признака зеркальности текста
        Text& SetMirrorAttr(bool is_mirror_text);
        // Устанавливает угол поворота очертаний символов относительно направляющей линии (атрибут rotate)
        Text& SetRotateGlyphAngle(double rotate_glyph_angle);
        // Устанавливает выравнивание текста по горизонтали (в продольном направлении)
        Text& SetTextAnchor(TextAnchorType text_anchor);
        // Задаёт название шрифта (атрибут font-family)
        Text& SetFontFamily(const std::string& font_family);
        // Задаёт толщину шрифта (атрибут font-weight)
        Text& SetFontWeight(const std::string& font_weight);
        // Задаёт текстовое содержимое объекта (отображается внутри тега text)
        Text& SetData(const std::string& data);

    private:
        virtual void RenderObject(const RenderContext& context) const override;

        Point pos_ = {0, 0}; // Координаты опорной точки (атрибуты x и y)
        // offset_ - смещение относительно опорной точки (атрибуты dx, dy)
        std::variant<Point, StringPoint> offset_ = Point(0, 0);
        uint32_t font_size_ = 1; // Размеры шрифта (атрибут font-size)
        std::optional<double> rotate_angle_;
        bool is_mirror_text_ = false;
        std::optional<double> rotate_glyph_angle_;
        TextAnchorType text_anchor_ = TextAnchorType::TEXT_ANCHOR_UNKNOWN;
        std::string font_family_; // Название шрифта (атрибут font-family)
        std::string font_weight_; // Толщина шрифта (атрибут font-weight)
        std::string data_; // Текстовое содержимое объекта (отображается внутри тега text)
    };

    class ObjectContainer
    {
    public:
        virtual ~ObjectContainer()
        {}

        // Добавляет в svg-документ объект-наследник svg::Object
        virtual void AddPtr(std::unique_ptr<Object>&& obj) = 0;

        template <typename T>
        void Add(const T& object)
        {
            AddPtr(std::make_unique<T>(object));
        }
    };

    class Document : public ObjectContainer
    {
    public:
        // Добавляет в svg-документ объект-наследник svg::Object
        virtual void AddPtr(std::unique_ptr<Object>&& obj) override;
        void Render(std::ostream& out) const;
        // Функция может проверять наличие ресурса типа resource_type с идентификатором id,
        // а также создавать его, если такового ещё нет и операция создания ресурса разрешена
        // установкой параметра is_create в "истину".
        // Если ресурс наличествует или создан, в первом члене пары возвращается указатель на него.
        // Второй член пары отражает результаты проверки наличия ресурса в документе. Если на
        // момент вызова функции ресурс отсутствовал, во втором члене пары возвращается "ложь".
        // Если при вызове функции ресурс уже существовал - возвращается "истина".
        std::pair<std::weak_ptr<Object>, bool> ResourceManager(ResourceType resource_type,
                                                    const std::string& id, bool is_create);

    private:
        std::vector<std::shared_ptr<Object>> objects_list_;
        Color viewport_fill_color_;
    };

    class Drawable
    {
    public:
        virtual ~Drawable()
        {}

        virtual void Draw(ObjectContainer& container) const = 0;
    };

}  // namespace svg
