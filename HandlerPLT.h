#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <any>

#include "PCADFile.h"
#include "PCADViewDraw.h"
#include "ApertureFile.h"

namespace HandlerPLT
{
    class PLT4FileWorkshop : public FileWorkshop
    {
    public:
        PLT4FileWorkshop() = default;
        virtual std::string GetFileWorkshopDescription() const override
        {
            return "PCAD PLT4 файл";
        }

        virtual std::vector<std::pair<std::string, std::string>> GetFileExtensions() const override
        {
            using namespace std;

            pair<string, string> extension_pair = {"Файлы изображений PCAD PLT4 (*.plt)"s, "*.plt"s};
            return {move(extension_pair)};
        }

        virtual FileWorkshop::LoadFileResult LoadPCADFile(std::istream& istr,
                                FileWorkshop::AdditionalLoadInfo& additional_load_info) override;

    private:
        enum PictureMeasureUnitS
        {
            PLT4_MEASURE_UNIT_INCH = 2,
            PLT4_MEASURE_UNIT_MM = 7
        };

        enum PictureGraphEditor
        {
            PLT4_EDITOR_PCCARDS = 1,
            PLT4_EDITOR_PCCAPS = 10
        };

        //Дескрипторы различных примитивов PLT-файла
        enum PictureGraphSignature
        {
            PLT4_GRAPH_COLOR_PEN_OBJ = 0x80A9,
            PLT4_GRAPH_LINE_OBJ  = 0x80A8,
            PLT4_GRAPH_RECT_OBJ  = 0x80A7,
            PLT4_GRAPH_FRECT_OBJ = 0x80A6,
            PLT4_GRAPH_CIRC_OBJ  = 0x80A5,
            PLT4_GRAPH_ARC_OBJ = 0x807A,
            PLT4_GRAPH_TEXT_OBJ = 0x80A2,
            PLT4_GRAPH_FLASH_OBJ = 0x8095,
            PLT4_GRAPH_POLY_OBJ = 0x80A3,
        };

        enum PolygonGraphSignature
        {
            PLT4_GRAPH_CVOID_OBJ = 0x807F,
            PLT4_GRAPH_PVOID_OBJ = 0x807E,
            PLT4_GRAPH_END_POLY_POINTS = 0x8080
        };

        enum PictureLineType
        {
            PLT4_GRAPH_LINE_SOLID = 0,
            PLT4_GRAPH_LINE_DOTTED = 1,
            PLT4_GRAPH_LINE_DASHED = 2
        };

        enum PictureTextOrient
        {
            PLT4_GRAPH_TEXT_LEFT_RIGHT = 0,
            PLT4_GRAPH_TEXT_DOWN_UP = 1,
            PLT4_GRAPH_TEXT_RIGHT_LEFT = 2,
            PLT4_GRAPH_TEXT_UP_DOWN = 3,
            PLT4_GRAPH_TEXT_LEFT_RIGHT_MIRROR = 4,
            PLT4_GRAPH_TEXT_DOWN_UP_MIRROR = 5,
            PLT4_GRAPH_TEXT_RIGHT_LEFT_MIRROR = 6,
            PLT4_GRAPH_TEXT_UP_DOWN_MIRROR = 7
        };

        enum PictureTextAlign
        {
            PLT4_GRAPH_TEXT_CENTER_DOWN = 25,   // Точка привязки текста по центру снизу
            PLT4_GRAPH_TEXT_CENTER_UP = 26,
            PLT4_GRAPH_TEXT_CENTER_CENTER = 27,
            PLT4_GRAPH_TEXT_LEFT_DOWN = 33,
            PLT4_GRAPH_TEXT_LEFT_UP = 34,
            PLT4_GRAPH_TEXT_LEFT_CENTER = 35,
            PLT4_GRAPH_TEXT_RIGHT_DOWN = 41,
            PLT4_GRAPH_TEXT_RIGHT_UP = 42,
            PLT4_GRAPH_TEXT_RIGHT_CENTER = 43   // Точка привязки текста справа по центру
        };

        enum PictureFlashOrient
        {
            PLT4_GRAPH_FLASH_NORMAL = 0,
            PLT4_GRAPH_FLASH_90_DEGREES = 1,
            PLT4_GRAPH_FLASH_180_DEGREES = 2,
            PLT4_GRAPH_FLASH_270_DEGREES = 3
        };

        enum PictureFillType
        {
            PLT4_GRAPH_SOLID_FILL = 1,
            PLT4_GRAPH_HATCH_FILL = 2
        };

        // Далее следуют структуры, применяемые в PLT-файлах в качестве файлового заголовка
        // и для описания хранящихся в нём примитивов.
        #pragma pack(push, 1)
        struct LayerInfoS
        {
            char layer_name[6]; // имя слоя. Если короче 6 символов, то заканчивается нулевым символом
            unsigned char layer_color; // цвет, назначенный слою
            unsigned char layer_num; // порядковый номер слоя в списке
        };

        struct PPoint  // структура для хранения точек полигонов
        {
            short xp;
            short yp;
        };

        struct FixPLTHeadSt
        {
            char file_desc[4];      // дескриптор файла
            short ldx;              // X координата левого нижнего угла рисунка
            short ldy;              // Y координата левого нижнего угла рисунка
            short rux;              // X координата правого верхнего угла рисунка
            short ruy;              // Y координата правого верхнего угла рисунка
            unsigned char measure_unit; // единицы измерения длины в чертеже (2 - дюймы, 7 - миллиметры)
            unsigned char reserved1;
            unsigned char editor_type;  //тип редактора-создателя файла(10-PCCAPS, 1-PCCARDS)
            unsigned char reserved2;
            short layer_count;      // количество включенных слоев
            short reserved3;;
        };

        struct ObjSelColorS // Пpимитив "номеp цветного карандаша"
        {
            unsigned char reserved1;  //нулевой байт
            unsigned char pen_color;  //номер цветного карандаша ( от 1 до 15 )
        };

        struct ObjLineS //Пpимитив "линия"
        {
            unsigned char line_type;  // тип линии (0 - сплошная линия (SOLID),
                                       // 1 - штриховая(DOTTED), 2 - пунктирная(DASHED))
            unsigned char line_width; // ширина линии
            short begin_x;    // начальная координата X
            short begin_y;    // начальная координата Y
            short end_x;    // конечная координата X
            short end_y;    // конечная координата Y
        };

        struct ObjRectS // Пpимитив "пpямоугольник"
        {
            unsigned char line_type;  // тип линии (0 - сплошная линия (SOLID),
                                      // 1 - штриховая(DOTTED), 2 - пунктирная(DASHED))
            unsigned char line_width; // ширина линии
            short rect_left_dn_x; // кооpдината X левого нижнего угла пpямоугольника
            short rect_left_dn_y; // кооpдината Y левого нижнего угла пpямоугольника
            short rect_right_up_x; // кооpдината X пpавого веpхнего угла пpямоугольника
            short rect_right_up_y; // кооpдината Y пpавого веpхнего угла пpямоугольника
        };

        struct ObjFRecS // Пpимитив "закрашенный пpямоугольник":
        {
            unsigned char fill_color;    // номеp цвета (1...15)
            unsigned char reserved1;  // нулевой байт
            short rect_left_dn_x; // кооpдината X левого нижнего угла пpямоугольника
            short rect_left_dn_y; // кооpдината Y левого нижнего угла пpямоугольника
            short rect_right_up_x; // кооpдината X пpавого веpхнего угла пpямоугольника
            short rect_right_up_y; // кооpдината Y пpавого веpхнего угла пpямоугольника
        };

        struct ObjCircS // Пpимитив "окpужность"
        {
            unsigned char line_type;     // тип линии
            unsigned char line_width;    // ширина линии
            short circle_center_x;     // кооpдината Х центpа
            short circle_center_y;     // кооpдината Y центpа
            short circle_center_radius;    // pадиус
            unsigned char dummy[6]; // шесть нулевых байт
        };

        struct ObjArcS // Примитив "дуга" или "дуга по трем точкам"
        {
            unsigned char line_type;    // тип линии
            unsigned char line_width;   // ширина линии
            short arc_center_x;     // кооpдината Х центpа
            short arc_center_y;     // кооpдината Y центpа
            short arc_begin_x;     // координата X начала дуги
            short arc_begin_y;     // координата Y начала дуги
            short arc_end_x;     // координата X конца дуги
            short arc_end_y;     // координата Y конца дуги
        };

        struct ObjTextS // Пpимитив "текст"
        {
            short reserved1;  // два нулевых байта
            short text_x;     // координата X точки привязки текста
            short text_y;     // координата Y точки привязки текста
            unsigned char text_orientation; // ориентация текста
            unsigned char text_height_lo_byte; // младший байт высоты текста
            unsigned char text_height_hi_byte; // старший байт высоты текста
            unsigned char text_align; // расположение (выравнивание) текста
        };

        struct ObjFlashS // Пpимитив "засветка" (вспышка)
        {
            unsigned char aperture_number; // номеp апеpтуpы
            unsigned char flash_orientation;  // ориентация засветки
            short flash_x;   // координата X засветки
            short flash_y;   // координата Y засветки
        };

        struct ObjPolyS // Пpимитив "полигон"
        {
            unsigned char hatch_type;   // тип заполнения контура полигона
                                        // (hatch_type = 1-сплошная заливка(SOLID),
                                        // hatch_type = 2-штриховая заливка(HATCH))
            unsigned char aperture_width; //ширина (толщина) апертуры
        };

        struct ObjCVoidS // Примитив "круглая выборка" из полигона
        {
            short void_center_x;  // координата X центра окружности
            short void_center_y;  // координата Y центра окружности
            unsigned short void_radius;  // радиус окружности
            unsigned short reserved2;    // два нулевых байта
        };
        #pragma pack(pop)

        static constexpr const char* DEFAULT_APR_FILE = "PCPHOTO.APR";
        static constexpr const char* DEFAULT_GAP_FILE = "APE45.GAP";
        static constexpr const char* PLT4_FILE_DESC = "\x4e\x80\x04";
        static constexpr const int PLT4_FILE_DESC_LEN = 4;

        FixPLTHeadSt fix_plt_head_;

        wxPoint ConvPntToLog(int pict_x, int pict_y) const;
        wxPoint ConvPntToLog(wxPoint pict) const;
        wxRect ConvRectToLog(int pict_ldx, int pict_ldy, int pict_rux, int pict_ruy) const;
        wxRect ConvRectToLog(wxRect pict_rect) const;
        void FillFileValues(FileDefValues& fdv, FileWorkshop::AdditionalLoadInfo& additional_load_info) const;
        // Набор функций, преобразующих внутрифайловое представление некоторых характеристик
        // графических примитивов в их универсальную, общепрограммную форму.
        LineType GetLineType(unsigned char line_type) const;
        TextOrientation GetTextOrient(unsigned char text_orientation) const;
        TextAlign GetTextAlign(unsigned char text_align) const;
        FlashOrientation GetFlashOrient(unsigned char flash_orientation) const;
        FillType GetHatchType(unsigned char hatch_type) const;
        int DetectLayerNumber(int current_col_index, const std::vector<LayerDesc>& layers) const;
        // Набор функций для считывания из файла блоков информации, описывающих определённые структуры изображения
        std::string ReadTextData(std::istream& istr) const;
        std::vector<wxPoint> GetPolyVertexes(std::istream& istr, uint32_t& spec_sign) const;
        std::vector<LayerDesc> ReadLayerInfo(std::istream& istr, PCADLoadError& process_error_code) const;
    };
} // namespace HandlerPLT
