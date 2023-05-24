#pragma once

#include <string>
#include <vector>
#include <variant>
#include <wx/colour.h>
#include <wx/pen.h>
#include <wx/brush.h>
#include <wx/gdicmn.h>
#include <wx/palette.h>
#include <wx/dcmirror.h>

#include "macroses.h"
#include "svg.h"
#include "ApertureProvider.h"

//Константы типов линий
enum class LineType
{
    LINE_SOLID = 0,  //сплошная линия
    LINE_DOTTED,     //штриховая
    LINE_DASHED,     //пунктирная
};

//Константы типов заливки полигона
enum class FillType
{
    POLY_FILL_SOLID = 1,  //сплошная заливка
    POLY_FILL_HATCH       //штриховая заливка
};

enum class TextOrientation
{
    TEXT_LEFT_RIGHT = 0,
    TEXT_DOWN_UP,
    TEXT_RIGHT_LEFT,
    TEXT_UP_DOWN,
    TEXT_LEFT_RIGHT_MIRROR,
    TEXT_DOWN_UP_MIRROR,
    TEXT_RIGHT_LEFT_MIRROR,
    TEXT_UP_DOWN_MIRROR,
};

enum class TextAlign
{
    // Имя константы состоит из двух ключей расположения точки привязки
    // относительно текста. Первый ключ определяет выравнивание текста по
    // горизонтали, а второй - по вертикали.
    TEXT_CENTER_DOWN = 0, // Точка привязки текста по центру снизу
    TEXT_CENTER_UP,
    TEXT_CENTER_CENTER,
    TEXT_LEFT_DOWN,
    TEXT_LEFT_UP,
    TEXT_LEFT_CENTER,
    TEXT_RIGHT_DOWN,
    TEXT_RIGHT_UP,
    TEXT_RIGHT_CENTER     // Точка привязки текста справа по центру
};

enum class TextCoordAlign
{
    TEXTCOORD_NOOP_WIDTH = 0,
    TEXTCOORD_NOOP_HEIGHT,
    TEXTCOORD_PLUS_HALFWIDTH,
    TEXTCOORD_PLUS_WIDTH,
    TEXTCOORD_MINUS_HALFWIDTH,
    TEXTCOORD_MINUS_WIDTH,
    TEXTCOORD_PLUS_HALFHEIGHT,
    TEXTCOORD_PLUS_HEIGHT,
    TEXTCOORD_MINUS_HALFHEIGHT,
    TEXTCOORD_MINUS_HEIGHT
};

struct TextSignature
{
    TextOrientation text_orientation;
    TextAlign text_align;
    bool is_horizontal;
    bool is_mirror;
    bool is_reverse;
    TextCoordAlign coord_x;
    TextCoordAlign coord_y;
    double text_angle;
};

enum class FlashOrientation
{
    FLASH_NORMAL = 0,
    FLASH_90_DEGREES,
    FLASH_180_DEGREES,
    FLASH_270_DEGREES
};

enum class CanvasSizeCode
{
    CANVAS_SIZE_NOT_STANDART = 0,
    CANVAS_SIZE_A0,
    CANVAS_SIZE_A1,
    CANVAS_SIZE_A2,
    CANVAS_SIZE_A3,
    CANVAS_SIZE_A4,
    CANVAS_SIZE_A5,
    CANVAS_SIZE_A6,
    CANVAS_SIZE_A7,
    CANVAS_SIZE_A8
};

struct SVGRect
{
    svg::Point left_up;
    double width;
    double height;
};

using SelColorType = std::variant<unsigned char, wxColor>;

struct CanvasContext
{
    bool is_canvas = false;
    CanvasSizeCode canvas_size_code;
    // Размеры выделения хранятся в поле canvas_size в "абсолютных" DBU текущего
    //  изображения, т. е. они не масштабируются при изменении масштаба картинки.
    wxSize canvas_size = wxSize(100000, 200000);
    wxPoint canvas_position;
    wxColor canvas_color;
    double canvas_ratio; // Пропорция отношения ширины холста к его высоте.
                         // Используется при is_fixed_ratio == true.
    bool is_portrait = true;
    bool is_metric = true;
    bool is_fixed_ratio = false;
};

struct SelectContourData
{
    bool is_contour_builded = false;
    wxRect contour_rect; //выделяющий контур в области (на поверхности) исходного изображения
    wxBitmap save_left;
    wxBitmap save_right;
    wxBitmap save_top;
    wxBitmap save_down;
    ContourSideDetectType contour_catch = ContourSideDetectType::CONTOUR_NOT_INTERSECT;
};

struct StdWXObjects
{
    StdWXObjects();
    // Инструменты прямых цветов
    std::vector<wxColor> std_colors;
    std::vector<wxPen> std_pens_1;
    std::vector<wxBrush> std_brushes_solid;
    std::vector<wxBrush> std_brushes_hatch;
    // Инструменты инверсных цветов
    std::vector<wxColor> inv_std_colors;
    std::vector<wxPen> inv_std_pens_1;
    std::vector<wxBrush> inv_std_brushes_solid;
    std::vector<wxBrush> inv_std_brushes_hatch;

    static const wxSize MinCanvasSizeInMM;
    static const wxSize MinCanvasSizeInPoints;
    static const wxSize MaxCanvasSizeInMM;
    static const wxSize MaxCanvasSizeInPoints;
    static const wxSize std_canvas_size[];

    wxSize GetStandartCanvasSize(CanvasSizeCode canvas_size_code, bool is_metric) const
    {
        wxSize canvas_size = std_canvas_size[static_cast<int>(canvas_size_code)];
        if (is_metric)
            return canvas_size; // Возвращаем результат в миллиметрах
        else // Возвращаем результат в милах - тысячных дюйма
            return wxSize(canvas_size.GetWidth() * MILS_IN_MILLIMETER,
                          canvas_size.GetHeight() * MILS_IN_MILLIMETER);
    }

    static uint32_t ToLong(wxColor wxc)
    {
        uint32_t red = wxc.Red();
        uint32_t green = wxc.Green();
        uint32_t blue = wxc.Blue();
        uint32_t alpha = wxc.Alpha();
        return (alpha << 24) | (blue << 16) | (green << 8) | red;
    }

    static wxColor FromLong(uint32_t color_long)
    {
        unsigned char red = color_long & 0xFF;
        unsigned char green = (color_long >> 8) & 0xFF;
        unsigned char blue = (color_long >> 16) & 0xFF;
        unsigned char alpha = (color_long >> 24) & 0xFF;
        return wxColor(red, green, blue, alpha);
    }

    static bool CompareColorLong(wxColor pixel_col, uint32_t long_col)
    {
        return (ToLong(pixel_col) & COLOR_24BITS_MASK) == (long_col & COLOR_24BITS_MASK);
    }

    static wxColor InverseColor(wxColor col_arg)
    {
        return wxColor(255 - col_arg.Red(), 255 - col_arg.Green(), 255 - col_arg.Blue());
    }
};

class PCADFile;

namespace chr
{
    class CHRProcessor;
}

struct FileDefValues;

struct DrawContext
{
    DrawContext();
    DrawContext(wxDC* arg_target_dc, bool is_create_recognize_dc = false);
    DrawContext(const DrawContext& other_context);
    DrawContext(DrawContext&& rhs);

    PCADFile* pcad_doc_ptr = nullptr;
    wxDC* target_dc_ptr = nullptr;
    wxSize target_dc_size;
    chr::CHRProcessor* chr_processor_ptr = nullptr;
    bool is_use_chr_font = false; // Флажок применения chr-текторисователя.
    wxFont context_font; // Текущий шрифт, атрибуты которого (вся информация о шрифте, кроме высоты)
                         // будут использоваться для вывода текстовых элементов на изображение.
    // Блок полей определения инструментов рисования - перьев и кистей.
    // Текущие цвет, перо и кисть устанавливаются, как правило, при "рисовании" примитива выбора цвета
    wxColor color;  // Текущий цвет рисуемых объектов. Такой цвет приобретают также перо и кисть
                    // (за одним исключением - примитив "закрашенный прямоугольник").
    wxColor inv_color; // Обращённый цвет для вывода отмеченных объектов.
    wxColor background_color; // Фоновый цвет изображения
    wxPen pen_1;    // Перо текущего цвета толщины 1 единица
    wxPen inv_pen_1;    // Перо инверсного цвета толщины 1 единица
    wxPen pen_width;    // Перо текущего цвета толщины line_width_
    wxPen inv_pen_width;  // Перо обращённого цвета толщины line_width_
    wxBrush brush_solid;  // Сплошная кисть текущего цвета
    wxBrush inv_brush_solid;  // Сплошная кисть инверсного цвета
    wxBrush brush_hatch;  // Штриховая кисть текущего цвета
    wxBrush inv_brush_hatch;  // Штриховая кисть обращённого цвета
    wxBrush brush_temp;   // Временная кисть, которая используется для примитива "закрашенный прямоугольник".
    wxPen pen_recognize;
    wxBrush brush_recognize;
    wxBrush background_brush_recognize;
    bool is_use_selected; // Если поле установлено в "истину", отмеченные элементы будут выводиться "превращенным"
                          // способом (специальным выделением).
    MeasureUnitTypeData measure_unit_type; // Тип единиц измерения, которые будут использоваться при
                                           // вводе-выводе координат и размеров.
    TextExportStyleType text_export_style; // Переключатель стиля форматирования координат и расстояний при их
                                           // преобразовании в текстовую форму.
    // -----------------------
    // Блок полей обеспечения масштабной проекции рисунка на целевую поверхность
    double scale_x; // Масштаб по оси x
    double scale_y; // Масштаб по оси y
    wxRect project_rect;  // Область проецирования - прямоугольник в области исходного
                          // PCAD-изображения, который надлежит спроецировать и
                          // вывести на поверхность рисовки *target_dc_ptr.
                          // Он задаётся в логической системе координат (системе
                          // коодинат PCAD-изображения). В этой системе, как и в традиционной экранной,
                          // ось y (ось ординат) направлена вниз.
    wxRect clipping_rect; // Область отсечения - прямоугольник в области исходного PCAD-изображения,
                          // внутри которого лежит разрешённая область рисовки, а за её пределами
                          // изображение отсекается (не выводится). Он задаётся также в логической системе координат.
    // -----------------------
    // Дополнительная поверхность рисования, служащая для установки соответствия между точкой на целевой поверхности
    // и конкретным элементом изображения.
    wxBitmap recognize_bitmap;  // recognize_bitmap - вспомогательный 32-битный графический слой для
                                // отождествления точек растра с определённым примитивом рисунка.
    wxMemoryDC recognize_dc;    // recognize_dc - дополнительный контекст устройства для рисования на recognize_bitmap.

    DrawContext& operator=(const DrawContext& rhs);
    void Copier(const DrawContext& rhs);
    void InitDrawContextByColor(const SelColorType& set_color);
    void InitColorPenBrush();
    void InitThisAppFields();
    void ResetTargetDC(wxDC* arg_target_dc_ptr, bool is_create_recognize_dc = false);
    void ResetTargetSize(wxSize new_target_size, bool is_from_target_dc = false);
    void ResetRecognizeDC(wxSize new_dc_size);
    void ResetProjectRect(const wxRect& new_project_rect);
    // Преобразует точку в координатном пространстве PCAD-изображения в точку в системе координат целевой поверхности target_dc.
    wxPoint ConvertPointToDevice(wxPoint pict_point);
    wxRect ConvertRectToDevice(wxRect pict_rect);
    // Преобразует точку на целевой поверхности target_dc в точку в координатном пространстве PCAD-изображения.
    wxPoint ConvertPointFromDevice(wxPoint pict_point);
    wxRect ConvertRectFromDevice(wxRect pict_rect);
};

struct GraphObjGlobalConfig
{
    OptionsData* options_data_ptr = nullptr; // Указатель на структуру глобальной конфигурации
    wxFontData* font_data_ptr = nullptr; // Указатель на текущий системный шрифт
    chr::CHRProcessor* chr_proc_ptr = nullptr; // Указатель на используемый в системе CHR-обработчик
};

struct GraphObjAttributes
{
    bool is_selected = false; // Флаг отметки примитива
};

class GraphObj
{
public:
    GraphObj(int layer_number) : layer_number_(layer_number)
    {
        graph_object_ordinal_ = ++last_graph_object_ordinal_;
    }

    virtual ~GraphObj()
    {}

    void InitDrawContextByLayerNum(DrawContext& draw_context) const;
    virtual wxRect DrawObject(DrawContext& draw_context) const = 0;
    virtual std::string GetObjectLongText(DrawContext& draw_context) const = 0;
    virtual std::string GetObjectShortText(DrawContext& draw_context) const = 0;
    virtual void DrawObjectSVG(svg::Document& svg_doc, DrawContext& draw_context) const = 0;
    virtual void ShiftObject(int shift_direction_x, int shift_direction_y) = 0;

    wxRect GetFrameRect() const
    {
        return frame_rect_;
    }

    uint32_t GetGraphObjectOrdinal() const
    {
        return graph_object_ordinal_;
    }

    void SetGraphObjAttributes(const GraphObjAttributes& set_attributes) const
    {
        graph_obj_attributes_ = set_attributes;
    }

    GraphObjAttributes GetGraphObjAttributes() const
    {
        return graph_obj_attributes_;
    }

    int GetLayerNumber()
    {
        return layer_number_;
    }

    static void SetGlobalConfigPtrs(GraphObjGlobalConfig glob_cfg)
    {
        glob_cfg_ = glob_cfg;
    }

protected:
    void SetPenBrushExt(DrawContext& draw_context, int pen_width = 1) const;
    void ClearPenBrush(DrawContext& draw_context) const;
    static svg::Point ToSVGPoint(wxPoint wxp)
    {
        return svg::Point(wxp.x, wxp.y);
    }

    static svg::Color ToSVGColor(wxColor color)
    {
        return svg::Rgb(color.Red(), color.Green(), color.Blue());
    }

    static svg::StrokeDashArray ToSVGDashArray(LineType line_type)
    {
        switch(line_type)
        {
        case LineType::LINE_DASHED:
            return svg::StrokeDashArray::LINE_DASHED;
        case LineType::LINE_DOTTED:
            return svg::StrokeDashArray::LINE_DOTTED;
        default:
            return svg::StrokeDashArray::LINE_CONTINUOUS;
        }
    }

    static std::string ToSVGFontFamily(wxFontFamily wx_font_family)
    {
        switch (wx_font_family)
        {
        case wxFONTFAMILY_DECORATIVE:
            return "fantasy";
        case wxFONTFAMILY_ROMAN:
            return "serif";
        case wxFONTFAMILY_SCRIPT:
            return "cursive";
        case wxFONTFAMILY_SWISS:
            return "sans-serif";
        case wxFONTFAMILY_MODERN:
        case wxFONTFAMILY_TELETYPE:
            return "monospace";
        default:
            return "serif";
        }
    }

    static std::string ToSVGFontWeight(wxFontWeight wx_font_weight)
    {
        switch (wx_font_weight)
        {
        case wxFONTWEIGHT_LIGHT:
            return "lighter";
        case wxFONTWEIGHT_BOLD:
            return "bold";
        case wxFONTWEIGHT_MAX:
            return "bolder";
        case wxFONTWEIGHT_NORMAL:
        default:
            return "normal";
        }
    }

    wxRect frame_rect_; // Ограничительный прямоугольник, описанный вокруг фигуры
    uint32_t graph_object_ordinal_; // Уникальный номер графического объекта
    int layer_number_ = -1; // Номер слоя, к которому относится примитив.
                            // Значение -1 указывает, что объект внеслоевой.
    mutable GraphObjAttributes graph_obj_attributes_; // Некоторая дополнительная информация,
                                                      // хранящаяся вместе с примитивом.
    inline static GraphObjGlobalConfig glob_cfg_; // Коллекция указателей на различные структуры,
                                           // описывающие нужные нам настройки глобальной конфигурации.

private:
    inline static uint32_t last_graph_object_ordinal_ = 0;
};

class ContourGraphObj : public GraphObj
{
public:
    ContourGraphObj(int layer_number, LineType line_type, int line_width) :
                    GraphObj(layer_number), line_type_(line_type), line_width_(line_width)
    {}

    virtual ~ContourGraphObj()
    {}

protected:
    double CountLineWidthDbl(DrawContext& draw_context, wxPoint first_point, wxPoint last_point) const;
    int CountLineWidth(DrawContext& draw_context, wxPoint first_point, wxPoint last_point) const;
    uint32_t GetLinePattern() const;
    void CountHorVertWidthDbl(DrawContext& draw_context, double* line_width_hor_ptr, double* line_width_vert_ptr) const;
    void CountHorVertWidth(DrawContext& draw_context, int* line_width_hor_ptr, int* line_width_vert_ptr) const;
    void SetPenBrushInt(DrawContext& draw_context, int pen_width = 1) const;

    LineType line_type_;     // тип линии
    int line_width_;         // ширина линии
};

class ObjSelColor : public GraphObj // Объект установки текущего цвета
{
public:
    ObjSelColor(int layer_number, unsigned char set_color, const FileDefValues& file_values);
    ObjSelColor(int layer_number, wxColor set_color, const FileDefValues& file_values);

    virtual ~ObjSelColor() = default;
    virtual wxRect DrawObject(DrawContext& draw_context) const override;
    virtual std::string GetObjectLongText(DrawContext& draw_context) const override;
    virtual std::string GetObjectShortText(DrawContext& draw_context) const override;
    virtual void DrawObjectSVG(svg::Document& svg_doc, DrawContext& draw_context) const override;
    virtual void ShiftObject(int shift_direction_x, int shift_direction_y) override
    {}

private:
    // Цвет устанавливается либо прямо, либо с использованием индекса палитры
    SelColorType set_color_;
};

class ObjLine : public ContourGraphObj //Объект "линия"
{
public:
    ObjLine(int layer_number, LineType line_type, int line_width, wxPoint point1, wxPoint point2);
    virtual ~ObjLine() = default;
    virtual wxRect DrawObject(DrawContext& draw_context) const override;
    virtual std::string GetObjectLongText(DrawContext& draw_context) const override;
    virtual std::string GetObjectShortText(DrawContext& draw_context) const override;
    virtual void DrawObjectSVG(svg::Document& svg_doc, DrawContext& draw_context) const override;
    virtual void ShiftObject(int shift_direction_x, int shift_direction_y) override;

private:
    wxPoint point1_; // Начальная точка отрезка
    wxPoint point2_; // Конечная точка отрезка
};

class ObjRect : public ContourGraphObj // Объект "пpямоугольник"
{
public:
    ObjRect(int layer_number, LineType line_type, int line_width, wxRect rect);
    virtual ~ObjRect() = default;
    virtual wxRect DrawObject(DrawContext& draw_context) const override;
    virtual std::string GetObjectLongText(DrawContext& draw_context) const override;
    virtual std::string GetObjectShortText(DrawContext& draw_context) const override;
    virtual void DrawObjectSVG(svg::Document& svg_doc, DrawContext& draw_context) const override;
    virtual void ShiftObject(int shift_direction_x, int shift_direction_y) override;

private:
    wxRect rect_; // координаты прямоугольника
};

class ObjFillRect : public GraphObj // Пpимитив "закрашенный пpямоугольник":
{
public:
    ObjFillRect(int layer_number, unsigned char rect_color, wxRect rect) :
                GraphObj(layer_number), rect_color_(rect_color), rect_(rect)
    {
        frame_rect_ = rect_;
    }

    ObjFillRect(int layer_number, wxColor rect_color, wxRect rect) :
                GraphObj(layer_number), rect_color_(rect_color), rect_(rect)
    {
        frame_rect_ = rect_;
    }

    virtual ~ObjFillRect() = default;
    virtual wxRect DrawObject(DrawContext& draw_context) const override;
    virtual std::string GetObjectLongText(DrawContext& draw_context) const override;
    virtual std::string GetObjectShortText(DrawContext& draw_context) const override;
    virtual void DrawObjectSVG(svg::Document& svg_doc, DrawContext& draw_context) const override;
    virtual void ShiftObject(int shift_direction_x, int shift_direction_y) override;

private:
    SelColorType rect_color_;
    wxRect rect_; // координаты прямоугольника
};

class ObjCirc : public ContourGraphObj // Пpимитив "окpужность"
{
public:
    ObjCirc(int layer_number, LineType line_type, int line_width, wxPoint center, int radius);
    virtual ~ObjCirc() = default;
    virtual wxRect DrawObject(DrawContext& draw_context) const override;
    virtual std::string GetObjectLongText(DrawContext& draw_context) const override;
    virtual std::string GetObjectShortText(DrawContext& draw_context) const override;
    virtual void DrawObjectSVG(svg::Document& svg_doc, DrawContext& draw_context) const override;
    virtual void ShiftObject(int shift_direction_x, int shift_direction_y) override;

private:
    wxPoint center_;         // центр окружности
    int radius_;             // pадиус
};

class ObjArc : public ContourGraphObj // Примитив "дуга окружности" или "дуга по трем точкам"
{
public:
    ObjArc(int layer_number, LineType line_type, int line_width, wxPoint arc_center, wxPoint arc_begin,
           wxPoint arc_end);
    virtual ~ObjArc() = default;
    virtual wxRect DrawObject(DrawContext& draw_context) const override;
    virtual std::string GetObjectLongText(DrawContext& draw_context) const override;
    virtual std::string GetObjectShortText(DrawContext& draw_context) const override;
    virtual void DrawObjectSVG(svg::Document& svg_doc, DrawContext& draw_context) const override;
    virtual void ShiftObject(int shift_direction_x, int shift_direction_y) override;

private:
    wxPoint arc_center_;     // Центр окружности
    wxPoint arc_begin_;      // Точка начала дуги
    wxPoint arc_end_;        // Точка конца дуги
};

class ObjText : public GraphObj // Пpимитив "текст"
{
public:
    ObjText(int layer_number, wxPoint text_point, TextOrientation text_orientation,
            int text_height, TextAlign text_align, std::string text);
    virtual ~ObjText() = default;
    virtual wxRect DrawObject(DrawContext& draw_context) const override;
    virtual std::string GetObjectLongText(DrawContext& draw_context) const override;
    virtual std::string GetObjectShortText(DrawContext& draw_context) const override;
    virtual void DrawObjectSVG(svg::Document& svg_doc, DrawContext& draw_context) const override;
    virtual void ShiftObject(int shift_direction_x, int shift_direction_y) override;

private:
    wxPoint text_point_;     // точка привязки текста
    TextOrientation text_orientation_; // ориентация текста
    int text_height_; // высота текста
    TextAlign text_align_; // расположение (выравнивание) текста
    std::string text_;
};

class ObjFlash : public GraphObj // Пpимитив "засветка" (вспышка)
{
public:
    ObjFlash(int layer_number, wxPoint flash_point, FlashOrientation flash_orientation,
             int aperture_number, const FileDefValues& file_values,
             const aperture::ApertureProvider& aperture_provider);
    virtual ~ObjFlash() = default;
    virtual wxRect DrawObject(DrawContext& draw_context) const override;
    virtual std::string GetObjectLongText(DrawContext& draw_context) const override;
    virtual std::string GetObjectShortText(DrawContext& draw_context) const override;
    virtual void DrawObjectSVG(svg::Document& svg_doc, DrawContext& draw_context) const override;
    virtual void ShiftObject(int shift_direction_x, int shift_direction_y) override;
    void RecountFrameRect(const FileDefValues& file_values, const aperture::ApertureProvider& aperture_provider);

private:
    int aperture_number_; // номеp апеpтуpы
    wxPoint flash_point_; // координата засветки
    FlashOrientation flash_orientation_;  // ориентация засветки
};

class ObjCVoid : public GraphObj // Примитив "круглая выборка" из полигона
{
public:
    ObjCVoid(wxPoint void_center, int void_radius);
    virtual ~ObjCVoid() = default;
    virtual wxRect DrawObject(DrawContext& draw_context) const override;
    virtual std::string GetObjectLongText(DrawContext& draw_context) const override;
    virtual std::string GetObjectShortText(DrawContext& draw_context) const override;
    virtual void DrawObjectSVG(svg::Document& svg_doc, DrawContext& draw_context) const override;
    virtual void ShiftObject(int shift_direction_x, int shift_direction_y) override;

private:
    wxPoint void_center_;
    int void_radius_;  // радиус окружности
};

class ObjPVoid : public GraphObj // Примитив "многоугольная выборка" из полигона
{
public:
    ObjPVoid(std::vector<wxPoint> points);
    virtual ~ObjPVoid() = default;
    virtual wxRect DrawObject(DrawContext& draw_context) const override;
    virtual std::string GetObjectLongText(DrawContext& draw_context) const override;
    virtual std::string GetObjectShortText(DrawContext& draw_context) const override;
    virtual void DrawObjectSVG(svg::Document& svg_doc, DrawContext& draw_context) const override;
    virtual void ShiftObject(int shift_direction_x, int shift_direction_y) override;

private:
    std::vector<wxPoint> points_; //  Список точек полигона
};

class ObjPoly : public GraphObj // Пpимитив "полигон"
{
public:
    ObjPoly(int layer_number, FillType hatch_type, int aperture_width, std::vector<wxPoint> points,
            std::vector<ObjCVoid> cvoids, std::vector<ObjPVoid> pvoids);
    virtual ~ObjPoly() = default;
    virtual wxRect DrawObject(DrawContext& draw_context) const override;
    virtual std::string GetObjectLongText(DrawContext& draw_context) const override;
    virtual std::string GetObjectShortText(DrawContext& draw_context) const override;
    virtual void DrawObjectSVG(svg::Document& svg_doc, DrawContext& draw_context) const override;
    virtual void ShiftObject(int shift_direction_x, int shift_direction_y) override;

private:
    FillType hatch_type_;   // тип заполнения контура полигона
    int aperture_width_; // ширина (толщина) апертуры
    std::vector<wxPoint> points_; //  Список точек полигона
    std::vector<ObjCVoid> cvoids_; // Список круглых выемок из полигона
    std::vector<ObjPVoid> pvoids_; // Список многоугольных выемок из полигона
};

inline double ConvertTO2PI(double angle)
{ // Конвертирует представление углов из диапазона [-M_PI, M_PI] (функция atan2()) в диапазон [0, 2 * M_PI].
    if (angle < 0)
        return 2 * M_PI + angle;
    else
        return angle;
}

inline double ToDegrees(double radians)
{ // Преобразует радианы в градусы
    return radians * (180.0 / M_PI);
}

inline double ToRadians(double degrees)
{ // Преобразует градусы в радианы
    return (degrees / 180.0) * M_PI;
}
