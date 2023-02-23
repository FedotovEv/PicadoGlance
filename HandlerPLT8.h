#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <any>

#include "PCADFile.h"
#include "PCADViewDraw.h"
#include "ApertureFile.h"

namespace HandlerPLT8
{
    class PLT8FileWorkshop : public FileWorkshop
    {
    public:

        PLT8FileWorkshop() = default;
        virtual std::string GetFileWorkshopDescription() const override
        {
            return "PCAD8 PLT8 файл";
        }

        virtual std::vector<std::pair<std::string, std::string>> GetFileExtensions() const override
        {
            using namespace std;

            pair<string, string> extension_pair = {"Файлы изображений PCAD8 PLT8 (*.plt)"s, "*.plt"s};
            return {move(extension_pair)};
        }

        virtual FileWorkshop::LoadFileResult LoadPCADFile(std::istream& istr,
                                                          FileWorkshop::AdditionalLoadInfo& addit_load_info) override;

    private:

        template <typename T>
        static void ReverseQWord(T* src_qword_ptr)
        {
            char* chr_ptr = reinterpret_cast<char*>(src_qword_ptr);
            std::swap<char>(chr_ptr[0], chr_ptr[7]);
            std::swap<char>(chr_ptr[1], chr_ptr[6]);
            std::swap<char>(chr_ptr[2], chr_ptr[5]);
            std::swap<char>(chr_ptr[3], chr_ptr[4]);
        }

        template <typename T>
        static void ReverseDWord(T* src_dword_ptr)
        {
            char* chr_ptr = reinterpret_cast<char*>(src_dword_ptr);
            std::swap<char>(chr_ptr[0], chr_ptr[3]);
            std::swap<char>(chr_ptr[1], chr_ptr[2]);
        }

        template <typename T>
        static void ReverseWord(T* src_word_ptr)
        {
            char* chr_ptr = reinterpret_cast<char*>(src_word_ptr);
            std::swap<char>(chr_ptr[0], chr_ptr[1]);
        }

        enum PictureMeasureUnitS
        {
            PLT8_MEASURE_UNIT_INCH = 8,
            PLT8_MEASURE_UNIT_MM = 9
        };

        enum PictureGraphEditor
        {
            PLT8_EDITOR_PCCARDS = 1,
            PLT8_EDITOR_PCCAPS = 10
        };

        //Дескрипторы различных примитивов PLT-файла
        enum PictureGraphSignature : uint32_t
        {
            PLT8_GRAPH_COLOR_PEN_OBJ = 0x80000419,
            PLT8_GRAPH_LINE_OBJ  = 0x80000418,
            PLT8_GRAPH_RECT_OBJ  = 0x80000417,
            PLT8_GRAPH_FRECT_OBJ = 0x80000416,
            PLT8_GRAPH_CIRC_OBJ  = 0x80000415,
            PLT8_GRAPH_ARC_OBJ = 0x800003EA,
            PLT8_GRAPH_TEXT_OBJ = 0x80000412,
            PLT8_GRAPH_FLASH_OBJ = 0x80000405,
            PLT8_GRAPH_POLY_OBJ = 0x80000413
        };

        enum PolygonGraphSignature : uint64_t
        {
            PLT8_GRAPH_CVOID_OBJ = 0x800003EF00000000,
            PLT8_GRAPH_PVOID_OBJ = 0x800003EE00000000,
            PLT8_GRAPH_END_POLY_POINTS = 0x800003F000000000
        };

        enum PictureLineType
        {
            PLT8_GRAPH_LINE_SOLID = 0,
            PLT8_GRAPH_LINE_DOTTED = 1,
            PLT8_GRAPH_LINE_DASHED = 2
        };

        enum PictureTextOrient
        {
            PLT8_GRAPH_TEXT_LEFT_RIGHT = 0,
            PLT8_GRAPH_TEXT_DOWN_UP = 1,
            PLT8_GRAPH_TEXT_RIGHT_LEFT = 2,
            PLT8_GRAPH_TEXT_UP_DOWN = 3,
            PLT8_GRAPH_TEXT_LEFT_RIGHT_MIRROR = 4,
            PLT8_GRAPH_TEXT_DOWN_UP_MIRROR = 5,
            PLT8_GRAPH_TEXT_RIGHT_LEFT_MIRROR = 6,
            PLT8_GRAPH_TEXT_UP_DOWN_MIRROR = 7
        };

        enum PictureTextAlign
        {
            PLT8_GRAPH_TEXT_CENTER_DOWN = 0x1A,   // Точка привязки текста по центру снизу
            PLT8_GRAPH_TEXT_CENTER_UP = 0x19,
            PLT8_GRAPH_TEXT_CENTER_CENTER = 0x1B,
            PLT8_GRAPH_TEXT_LEFT_DOWN = 0x22,
            PLT8_GRAPH_TEXT_LEFT_UP = 0x21,
            PLT8_GRAPH_TEXT_LEFT_CENTER = 0x23,
            PLT8_GRAPH_TEXT_RIGHT_DOWN = 0x2A,
            PLT8_GRAPH_TEXT_RIGHT_UP = 0x29,
            PLT8_GRAPH_TEXT_RIGHT_CENTER = 0x2B   // Точка привязки текста справа по центру
        };

        enum PictureFlashOrient
        {
            PLT8_GRAPH_FLASH_NORMAL = 0,
            PLT8_GRAPH_FLASH_90_DEGREES = 1,
            PLT8_GRAPH_FLASH_180_DEGREES = 2,
            PLT8_GRAPH_FLASH_270_DEGREES = 3
        };

        enum PictureFillType
        {
            PLT8_GRAPH_SOLID_FILL = 1,
            PLT8_GRAPH_HATCH_FILL = 2
        };

        // Далее следуют структуры, применяемые в PLT-файлах в качестве файлового заголовка
        // и для описания хранящихся в нём примитивов.
        #pragma pack(push, 1)
        struct LayerInfoS
        {
            char layer_name[8]; // имя слоя. Правила преобразования имени описаны в спецификации на формат
            uint32_t layer_num; // порядковый номер слоя в списке
            uint32_t layer_color; // цвет, назначенный слою
        };

        struct PPoint  // структура для хранения точек полигонов
        {
            int32_t xp;
            int32_t yp;
        };

        struct FixPLTHeadSt
        {
            char file_desc[8];        // дескриптор файла
            int32_t ldx;              // X координата левого нижнего угла рисунка
            int32_t ldy;              // Y координата левого нижнего угла рисунка
            int32_t rux;              // X координата правого верхнего угла рисунка
            int32_t ruy;              // Y координата правого верхнего угла рисунка
            uint32_t measure_unit;    // единицы измерения длины в чертеже (2 - дюймы, 7 - миллиметры)
            uint32_t editor_type;     //тип редактора-создателя файла(10-PCCAPS, 1-PCCARDS)
            uint32_t layer_count;     // количество включенных слоев
            uint32_t reserved;

            void Read(std::istream& istr)
            {
                istr.read(reinterpret_cast<char*>(this), sizeof(*this));
                if (!istr)
                    return;
                ReverseDWord(&ldx);
                ReverseDWord(&ldy);
                ReverseDWord(&rux);
                ReverseDWord(&ruy);
                ReverseDWord(&measure_unit);
                ReverseDWord(&editor_type);
                ReverseDWord(&layer_count);
            }

            FixPLTHeadSt() = default;
            FixPLTHeadSt(std::istream& istr)
            {
                Read(istr);
            }
        };

        struct ObjSelColorS // Пpимитив "номеp цветного карандаша"
        {
            unsigned short pen_color;  //номер цветного карандаша ( от 1 до 15 )
            unsigned short reserved1;  //нулевое слово

            void Read(std::istream& istr)
            {
                istr.read(reinterpret_cast<char*>(this), sizeof(*this));
                if (!istr)
                    return;
                ReverseWord(&pen_color);
            }

            ObjSelColorS() = default;
            ObjSelColorS(std::istream& istr)
            {
                Read(istr);
            }

        };

        struct ObjLineS //Пpимитив "линия"
        {
            unsigned short line_width; // ширина линии
            unsigned short line_type;  // тип линии (0 - сплошная линия (SOLID),
                                       // 1 - штриховая(DOTTED), 2 - пунктирная(DASHED))

            int32_t begin_x;    // начальная координата X
            int32_t begin_y;    // начальная координата Y
            int32_t end_x;    // конечная координата X
            int32_t end_y;    // конечная координата Y

            void Read(std::istream& istr)
            {
                istr.read(reinterpret_cast<char*>(this), sizeof(*this));
                if (!istr)
                    return;
                ReverseWord(&line_width);
                ReverseWord(&line_type);
                ReverseDWord(&begin_x);
                ReverseDWord(&begin_y);
                ReverseDWord(&end_x);
                ReverseDWord(&end_y);
            }

            ObjLineS() = default;
            ObjLineS(std::istream& istr)
            {
                Read(istr);
            }
        };

        struct ObjRectS // Пpимитив "пpямоугольник"
        {
            unsigned short line_width; // ширина линии
            unsigned short line_type;  // тип линии (0 - сплошная линия (SOLID),
                                      // 1 - штриховая(DOTTED), 2 - пунктирная(DASHED))
            int32_t rect_left_dn_x; // кооpдината X левого нижнего угла пpямоугольника
            int32_t rect_left_dn_y; // кооpдината Y левого нижнего угла пpямоугольника
            int32_t rect_right_up_x; // кооpдината X пpавого веpхнего угла пpямоугольника
            int32_t rect_right_up_y; // кооpдината Y пpавого веpхнего угла пpямоугольника

            void Read(std::istream& istr)
            {
                istr.read(reinterpret_cast<char*>(this), sizeof(*this));
                if (!istr)
                    return;
                ReverseWord(&line_width);
                ReverseWord(&line_type);
                ReverseDWord(&rect_left_dn_x);
                ReverseDWord(&rect_left_dn_y);
                ReverseDWord(&rect_right_up_x);
                ReverseDWord(&rect_right_up_y);
            }

            ObjRectS() = default;
            ObjRectS(std::istream& istr)
            {
                Read(istr);
            }
        };

        struct ObjFRecS // Пpимитив "закрашенный пpямоугольник":
        {
            unsigned short reserved1;     // нулевой байт
            unsigned short fill_color;    // номеp цвета (1...15)
            int32_t rect_left_dn_x; // кооpдината X левого нижнего угла пpямоугольника
            int32_t rect_left_dn_y; // кооpдината Y левого нижнего угла пpямоугольника
            int32_t rect_right_up_x; // кооpдината X пpавого веpхнего угла пpямоугольника
            int32_t rect_right_up_y; // кооpдината Y пpавого веpхнего угла пpямоугольника

            void Read(std::istream& istr)
            {
                istr.read(reinterpret_cast<char*>(this), sizeof(*this));
                if (!istr)
                    return;
                ReverseWord(&fill_color);
                ReverseDWord(&rect_left_dn_x);
                ReverseDWord(&rect_left_dn_y);
                ReverseDWord(&rect_right_up_x);
                ReverseDWord(&rect_right_up_y);
            }

            ObjFRecS() = default;
            ObjFRecS(std::istream& istr)
            {
                Read(istr);
            }
        };

        struct ObjCircS // Пpимитив "окpужность"
        {
            unsigned short line_width;    // ширина линии
            unsigned short line_type;     // тип линии (0 - сплошная линия (SOLID),
                                          // 1 - штриховая(DOTTED), 2 - пунктирная(DASHED))
            int32_t circle_center_x;     // кооpдината Х центpа
            int32_t circle_center_y;     // кооpдината Y центpа
            int32_t circle_center_radius;    // pадиус
            uint32_t dummy[3];          // три нулевых двойных слова

            void Read(std::istream& istr)
            {
                istr.read(reinterpret_cast<char*>(this), sizeof(*this));
                if (!istr)
                    return;
                ReverseWord(&line_width);
                ReverseWord(&line_type);
                ReverseDWord(&circle_center_x);
                ReverseDWord(&circle_center_y);
                ReverseDWord(&circle_center_radius);
            }

            ObjCircS() = default;
            ObjCircS(std::istream& istr)
            {
                Read(istr);
            }
        };

        struct ObjArcS // Примитив "дуга" или "дуга по трем точкам"
        {
            unsigned short line_width;   // ширина линии
            unsigned short line_type;    // тип линии
            int32_t arc_center_x;     // кооpдината Х центpа
            int32_t arc_center_y;     // кооpдината Y центpа
            int32_t arc_begin_x;     // координата X начала дуги
            int32_t arc_begin_y;     // координата Y начала дуги
            int32_t arc_end_x;     // координата X конца дуги
            int32_t arc_end_y;     // координата Y конца дуги

            void Read(std::istream& istr)
            {
                istr.read(reinterpret_cast<char*>(this), sizeof(*this));
                if (!istr)
                    return;
                ReverseWord(&line_width);
                ReverseWord(&line_type);
                ReverseDWord(&arc_center_x);
                ReverseDWord(&arc_center_y);
                ReverseDWord(&arc_begin_x);
                ReverseDWord(&arc_begin_y);
                ReverseDWord(&arc_end_x);
                ReverseDWord(&arc_end_y);
            }

            ObjArcS() = default;
            ObjArcS(std::istream& istr)
            {
                Read(istr);
            }
        };

        struct ObjTextS // Пpимитив "текст"
        {
            uint32_t reserved1;  // нулевое двойное слово
            int32_t text_x;     // координата X точки привязки текста
            int32_t text_y;     // координата Y точки привязки текста
            unsigned short reserved2;
            unsigned text_orientation : 4; // ориентация текста (направление его вывода)
            unsigned text_align : 8;       // выравнивание (выключка) текста
            unsigned : 4;                  // Дополнение до целого слова
            uint32_t text_height; // высота текста

            void Read(std::istream& istr)
            {
                istr.read(reinterpret_cast<char*>(this), sizeof(*this));
                if (!istr)
                    return;
                ReverseDWord(&text_x);
                ReverseDWord(&text_y);
                ReverseWord(&reserved2 + 1);
                ReverseDWord(&text_height);
            }

            ObjTextS() = default;
            ObjTextS(std::istream& istr)
            {
                Read(istr);
            }
        };

        struct ObjFlashS // Пpимитив "засветка" (вспышка)
        {
            unsigned short flash_orientation;  // ориентация засветки
            unsigned short aperture_number; // номеp апеpтуpы
            int32_t flash_x;   // координата X засветки
            int32_t flash_y;   // координата Y засветки

            void Read(std::istream& istr)
            {
                istr.read(reinterpret_cast<char*>(this), sizeof(*this));
                if (!istr)
                    return;
                ReverseWord(&flash_orientation);
                ReverseWord(&aperture_number);
                ReverseDWord(&flash_x);
                ReverseDWord(&flash_y);
            }

            ObjFlashS() = default;
            ObjFlashS(std::istream& istr)
            {
                Read(istr);
            }
        };

        struct ObjPolyS // Пpимитив "полигон"
        {
            unsigned short aperture_width; //ширина (толщина) апертуры
            unsigned short hatch_type;   // тип заполнения контура полигона
                                        // (hatch_type = 1-сплошная заливка(SOLID), hatch_type = 2-штриховая заливка(HATCH))

            void Read(std::istream& istr)
            {
                istr.read(reinterpret_cast<char*>(this), sizeof(*this));
                if (!istr)
                    return;
                ReverseWord(&aperture_width);
                ReverseWord(&hatch_type);
            }

            ObjPolyS() = default;
            ObjPolyS(std::istream& istr)
            {
                Read(istr);
            }
        };

        struct ObjCVoidS // Примитив "круглая выборка" из полигона
        {
            int32_t void_center_x;  // координата X центра окружности
            int32_t void_center_y;  // координата Y центра окружности
            int32_t void_radius;    // радиус окружности
            uint32_t reserved;      // нулевое двойное слово

            void Read(std::istream& istr)
            {
                istr.read(reinterpret_cast<char*>(this), sizeof(*this));
                if (!istr)
                    return;
                ReverseDWord(&void_center_x);
                ReverseDWord(&void_center_y);
                ReverseDWord(&void_radius);
            }

            ObjCVoidS() = default;
            ObjCVoidS(std::istream& istr)
            {
                Read(istr);
            }
        };
        #pragma pack(pop)

        static constexpr const char* DEFAULT_APR_FILE = "PCPHOTO.APR";
        static constexpr const char* DEFAULT_GAP_FILE = "APERTURE.GAP";
        static constexpr const char* PLT8_FILE_DESC = "\x80\x00\x03\xBE\x00\x00\x00\x06";
        static constexpr const int PLT8_FILE_DESC_LEN = 8;

        FixPLTHeadSt fix_plt_head_;

        wxPoint ConvPntToLog(int pict_x, int pict_y) const;
        wxPoint ConvPntToLog(wxPoint pict) const;
        wxRect ConvRectToLog(int pict_ldx, int pict_ldy, int pict_rux, int pict_ruy) const;
        wxRect ConvRectToLog(wxRect pict_rect) const;
        void FillFileValues(FileDefValues& fdv, FileWorkshop::AdditionalLoadInfo& additional_load_info) const;
        // Набор функций, преобразующих внутрифайловое представление некоторых характеристик
        // графических примитивов в их универсальную, общепрограммную форму.
        LineType GetLineType(unsigned short line_type) const;
        TextOrientation GetTextOrient(unsigned char text_orientation) const;
        TextAlign GetTextAlign(unsigned char text_align) const;
        FlashOrientation GetFlashOrient(unsigned short flash_orientation) const;
        FillType GetHatchType(unsigned short hatch_type) const;
        int DetectLayerNumber(int current_col_index, const std::vector<LayerDesc>& layers) const;
        // Набор функций для считывания из файла блоков информации, описывающих определённые структуры изображения
        std::string ReadTextData(std::istream& istr) const;
        std::vector<wxPoint> GetPolyVertexes(std::istream& istr, uint64_t& spec_sign) const;
        std::vector<LayerDesc> ReadLayerInfo(std::istream& istr, PCADLoadError& process_error_code) const;
    };
} // namespace HandlerPLT8
