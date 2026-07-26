#ifndef HEADER_HANDLER_PDIF
#define HEADER_HANDLER_PDIF

#pragma once

#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <variant>
#include <any>
#include <unordered_map>
#include <optional>
#include <memory>

#include "PCADFile.h"
#include "PCADViewDraw.h"
#include "ApertureFile.h"

namespace HandlerPDIF
{
    class PDIFFileWorkshop : public FileWorkshop
    {
        // Делаем дружественными данному классу-загрузчику все классы-обработчики узловых команд, чтобы дать им доступ к
        // внутреннему состоянию загрузчика, которое меняется в процессе его работы.
        friend class NodeCOMPONENTHandler;
        friend class NodeCOMPDEFHandler;
        friend class NodeNHandler;
        friend class NodeIHandler;
        friend class NodePHandler;
        friend class NodePKGHandler;
        friend class NodeSPKGHandler;
        friend class NodeModeHandler;
        friend class NodeVWHanlder;
        friend class NodePDIFvrevHandler;
        friend class NodeDBvrevHandler;
        friend class NodeDBunitHandler;
        friend class NodeLyrstrHandler;
        friend class NodeLyrphidHandler;
        friend class NodeSsymtblHandler;
        friend class NodePolyapGlbHandler;
        friend class NodePolyapLocHandler;
        friend class NodeDBtypeHandler;
        friend class NodePtHandler;
        friend class NodeLqHandler;
        friend class NodePlocHandler;
        friend class NodeVHandler;
        friend class NodeNnHandler;
        friend class NodeAHandler;
        friend class NodeArcHandler;
        friend class NodeCircleHandler;
        friend class NodeLineHandler;
        friend class NodeRectangleHandler;
        friend class NodeTextHandler;
        friend class NodeFillRectHandler;
        friend class NodePolyHandler;
        friend class NodePolyOlHandler;
        friend class NodePolyPvHandler;
        friend class NodePolyCvHandler;
        friend class NodeCNHandler;
        friend class NodeIPTHandler;
        friend class NodeRdHandler;
        friend class NodePnHandler;
        friend class NodeOrgHandler;
        friend class NodeTyHandler;
        friend class NodeSmdHandler;
        friend class NodeJmpHandler;
        friend class NodeAtHandler;
        friend class NodePlHandler;
        friend class NodeScHandler;
        friend class NodeRoHandler;
        friend class NodeMrHandler;
        friend class NodeNlHandler;
        friend class NodePsHandler;
        friend class NodePaHandler;
        friend class NodeUnCompHandler;
        friend class NodeUnNetHandler;
        friend class NodeAtHandler;
        friend class NodeLyHandler;
        friend class NodeLsHandler;
        friend class NodeWdHandler;
        friend class NodeTsHandler;
        friend class NodeTjHandler;
        friend class NodeTrHandler;
        friend class NodeTmHandler;
        friend class NodeSnaHandler;
        friend class NodeSpHandler;
        friend class NodeApnHandler;
        friend class NodeRdlHandler;
        friend class NodePnlHandler;
        friend class NodeSdHandler;
        friend class NodePidHandler;

    public:
        PDIFFileWorkshop();
        virtual std::string GetFileWorkshopDescription() const override
        {
            return "PCAD PDF(PDIF-OUT) файл взаимообмена информацией";
        }

        virtual std::vector<std::pair<std::string, std::string>> GetFileExtensions() const override
        {
            using namespace std::literals;

            std::pair<std::string, std::string> extension_pair =
                {"Базы данных PCAD в формате взаимообмена информацией PDIF (*.pdf)"s, "*.pdf"s};
            return {std::move(extension_pair)};
        }

        virtual FileWorkshop::LoadFileResult LoadPCADFile(std::istream& istr,
                                FileWorkshop::AdditionalLoadInfo& additional_load_info) override;

    private:
        // Статические настройки, определяющие радиус видимого изображения точечных объектов (via, межсоединений или переходных отверстий).
        static int VIA_POINT_RADIUS_SCH, VIA_POINT_RADIUS_PCB;

        static std::optional<int> CheckIntValue(int64_t test_value, int limit_value = 0)
        {
            if (limit_value <= 0)
            { // Проверка числа test_value на принадлежность полному диапазону типа int.
                if (test_value < INT_MIN || test_value > INT_MAX)
                    return {};
                else
                    return static_cast<int>(test_value);
            }
            else
            { // Проверка числа test_value на неотрицательность и непревышение граничного значения limit_value.
                if (test_value < 0 || test_value > limit_value)
                    return {};
                else
                    return static_cast<int>(test_value);
            }
        }

        static std::optional<LineType> FindLineType(const std::string& line_type_sign);         // Тип линии.
        // Ориентация (курс направляющей линии) и зеркальность текста.
        static std::optional<TextOrientation> FindTextOrientation(int text_rotation_index, bool is_text_mirror);
        static std::optional<TextAlign> FindTextAlign(const std::string& text_align_sign);      // Выравнивание текста.

        enum class ValueType
        {
            ARG_VALUE_NONE = 0, // Необрабатываемый параметр, который должен быть пропущен.
            ARG_VALUE_INT,
            ARG_VALUE_STRING,
            ARG_VALUE_DOUBLE,
            ARG_VALUE_NON_EXIST = std::numeric_limits<int>::max()
        };

        struct DataBlockDesc
        { // Структура, описывающая устройство набора последовательных значений (блока аргументов), входящего как нечто целое и неделимое в общий список
          // параметров какого-либо узла (терминального либо нетерминального) общего дерева PDIF-документа.
            std::vector<ValueType> data_types; // Состав блока по типам входящих в него значений аргументов.
            bool is_repetable = false;         // Может ли данный блок повторяться в списке параметров многократно? Повторяемым может быть только последний
                                               // блок последовательности.

            size_t size() const noexcept
            {
                return data_types.size();
            }

            ValueType value_type(size_t i) const noexcept
            {
                return i < data_types.size() ? data_types[i] : ValueType::ARG_VALUE_NON_EXIST;
            }
        };

        enum class LexemCategory
        {
            PDIF_LEX_UNKNOWN = 0,
            // Набор лексически значимых скобок - открывающие и закрывающие фигурные и квадратные скобки.
            PDIF_LEX_OPEN_FIG_BRACKET,
            PDIF_LEX_CLOSE_FIG_BRACKET,
            PDIF_LEX_OPEN_SQUARE_BRACKET,
            PDIF_LEX_CLOSE_SQUARE_BRACKET,
            // Лексемы-значения - строковые и числовые (целочисленные или дробные).
            PDIF_LEX_STRING_VALUE,
            PDIF_LEX_INT_VALUE,
            PDIF_LEX_DOUBLE_VALUE,
            // Лексема - какое-либо ключевое слово из известных разборщику.
            PDIF_LEX_KEYWORD
        };

        enum class PDIFKeywords
        { // Перечисление, определяющее список допустимых лексем-ключевых слов формата PDIF, поддерживаемых и
          // обрабатываемых на текущее время данным загрузчиком. PDIF-база является древовидной и состоит из внутренних узлов (разделов, нетерминалов)
          // и листьев (ключей, терминалов).
            PDIF_KEY_ANY = -1,
            PDIF_KEY_UNKNOWN = 0,
            // Далее перечислены константы ключевых слов, обозначающих разделы и подразделы документа PDF, по которым распределена вся его
            // содержательная начинка. Имена таких лексем состоят только из больших латинских букв, они являются нетерминалами (внутренними узлами)
            // дерева базы данных
            PDIF_KEY_COMPONENT, // Раздел COMPONENT - глобальный раздел, содержащий всю содержательную информацию базы данных, экспортированной в данный файл.
            PDIF_KEY_ENVIRONMENT, // Раздел ENVIRONMENT - раздел хранения настроек переменных окружения редактора (его общесистемных настроек).
            PDIF_KEY_USER, // USER - раздел с определениями специальных настроек какой-либо программы (или сразу нескольких программ), создавшей исходную
                           // базу данных.
                           // Это может быть либо редактор из состава комплекса PCAD (PC-CAPS, PC-PLACE или PC_CARDS), либо любая другая программа, создавшая
                           // либо обрабатывавшая данный PDIF-файл и посчитавшая нужным сохранить в этом разделе свои данные.
            PDIF_KEY_VIEW,      // VIEW - подраздел раздела USER, содержащий данные, используемые редакторами комплекса PCAD.
            PDIF_KEY_DETAIL,    // DETAIL - главный раздел проекта, содержащий корпус всей его основной информации.
            PDIF_KEY_SYMBOL,    // SYMBOL - второй главный раздел проекта, содержащий все данные описания библиотечного элемента при работе редакторов
                                // в символьном режиме (SYMB).
            PDIF_KEY_ANNOTATE,  // ANNOTATE - раздел, содержащий описание графических и разметочных элементов рисунка.
            PDIF_KEY_SUBCOMP,   // SUBCOMP - общий раздел хранения информации обо всех библиотечных компонентах, задействованных в схеме.
            PDIF_KEY_COMPDEF,   // COMP_DEF - подраздел раздела SUBCOMP для хранения информации о каком-либо отдельном компоненте, используемом в данном
                                // устройстве (печатной плате).
            PDIF_KEY_PINDEF,    // PIN_DEF - подраздел раздела COMP_DEF для хранения информации о выводах (пинах, "ножках"), принадлежащих радиокомпоненту,
                                // содержащему данный позраздел.
            PDIF_KEY_NETDEF,    // NET_DEF - подраздел раздела DETAIL для хранения информации о проводящих цепях - соединениях в схеме или токопроводящих диниях на плате.
            PDIF_KEY_PIC,       // PIC - графическое описание объектов.
            PDIF_KEY_I,         // I - подраздел раздела SUBCOMP с описанием отдельного включения некоторого библиотечного компонента в схему устройства.
            PDIF_KEY_N,         // N - подраздел раздела NET_DEF с описанием некоторой проводящей цепи (связи в схеме или проводника, проходящего по печатной плате).
            PDIF_KEY_P,         // P - подраздел раздела PIN_DEF с описанием отдельного вывода ("ножки") определённого компонента.
            PDIF_KEY_DG,        // DG - подраздел раздела NET_DEF/N с описанием некоторой проводящей цепи (связи в схеме или проводника, проходящего по печатной плате).
            PDIF_KEY_ATR,       // ATR - раздел атрибутов компонента.
            PDIF_KEY_IN,        // IN - секция (подраздел) хранения внутренних (предопределённых) атрибутов компонента.
            PDIF_KEY_EX,        // EX - секция (подраздел) хранения внешних (заданных пользователем) атрибутов компонента.
            PDIF_KEY_PKG,       // PKG - Информация об упаковке компонента в составе УГО или схемы (поддерево узла).
            PDIF_KEY_SPKG,      // SPKG - Информация об упаковке компонента в составе конструктива или на плате (поддерево узла ).
            PDIF_KEY_CN,        // CN - Подсекция I с информацией о соединении выводов вставленного экземпляра элемента с цепями схемы (или платы).
            PDIF_KEY_IPT,       // IPT - Подсекция I с данными об индивидуальных (для данной вставки) типах ножек (применяется только при описании плат).
            PDIF_KEY_ASG,       // ASG - Дополнительная упаковочная информация о свойствах вставленной копии компонента (применяется только для схем).
            // Далее перечислены константы терминальных лексем (листьев) дерева базы данных. Их правила именования таковы - несколько первых букв латинские
            // заглавные, остальные - латинские строчные.
            PDIF_KEY_MODE, // Mode - режим редактирования, в котором создавалась исходная база данных (конструктивно-символьный SYMB или плато-схематический DETL).
            PDIF_KEY_VW,   // Vw - положение и размер окон отображения (текуего и сохранённых) картинки в момент создания исходного файла базы данных.
            PDIF_KEY_PDIF_VREV, // PDIFvrev - версия (ревизия) PDIF-формата.
            PDIF_KEY_DB_VREV, // DBvrev - версия (ревизия) формата самой базы данных (её двоичного исходника).
            PDIF_KEY_DB_UNIT, // DBunit - единица измерений координат, используемая в файле.
            PDIF_KEY_LYRSTR,  // Lyrstr - таблица доступных слоёв - список слоёв, используемых компонентом.
            PDIF_KEY_LYRPHID, // Lyrphid - определение спаривания слоёв и их доступности для трассировкию
            PDIF_KEY_SSYMTBL, // Ssymtbl - задание свойств контактов, которые служат как переходные отверстия между различными слоями.
            PDIF_KEY_POLYAP,  // Polyap - ширина апертуры для рисования границ полигона.
            PDIF_KEY_POLY,    // Poly - рисующий раздел, создающий на изоюражении комплексный полигон (с возможными внутренними пустотами).
            PDIF_KEY_DBTYPE,  // DBtype - тип базы данных - схемный ("Schematic") или конструктивный ("PC-Board") элемент.
            // Характеристические терминалы, определяющие некоторые параметры отдельного вывода ("ножки") некоторого радиокомпонента.
            PDIF_KEY_PT,      // Pt - символьный (текстовый, для УГО символа) или номерной (для конструктива символа) тип данного вывода радиокомпонента.
            PDIF_KEY_LQ,      // Lq - код эквивалентности (взаимозаменяемости) данного вывода радиокомпонента.
            PDIF_KEY_PLOC,    // Ploc - точка расположения вывода.
            // "Низовые" (примитивные) составляющие элементы проводящей цепи.
            PDIF_KEY_W,       // W - Проводящая ломаная (набор смежных проводящих прямолинейных отрезков).
            PDIF_KEY_V,       // V - Переходное отверстие (на плате) или точка пересечения фрагментов одной цепи (на схеме).
            PDIF_KEY_NN,      // Nn - Положение видимого имени цепи.
            // Отдельные графические примитивы.
            PDIF_KEY_A,       // A - "старый" тип дуги.
            PDIF_KEY_ARC,     // Arc - "новый" тип дуги.
            PDIF_KEY_C,       // C - полная окружность.
            PDIF_KEY_L,       // L - линия (прямолинейный отрезок).
            PDIF_KEY_R,       // R - прямоугольник.
            PDIF_KEY_T,       // T - текстовая строка.
            PDIF_KEY_FR,      // Fr - заполненный прямоугольник.
            // Группа узлов описания полигона.
            PDIF_KEY_OL,      // Ol - описание контура полигона.
            PDIF_KEY_PV,      // Pv - полигональная пустота (выемка) внутри многоугольника.
            PDIF_KEY_CV,      // Cv - круглая выемка внутри многоугольника.
            // Ключевые слова установочных команд.
            PDIF_KEY_LY,      // Ly - выбор слоя для размещения последующих графических примитивов.
            PDIF_KEY_LS,      // Ls - назначение типа линии.
            PDIF_KEY_WD,      // Wd - ширина линии, которой будет обрисован контур примитива.
            PDIF_KEY_TS,      // Ts - высота текста (высота глифов используемого шрифта)
            PDIF_KEY_TJ,      // Tj - выравнивание текста.
            PDIF_KEY_TR,      // Tr - вращение (угол поворота) текста.
            PDIF_KEY_TM,      // Tm - зеркальность текста.
            // Ключевые слова, маркирующие узлы с атрибутами прочих сущностей, которым принадлежит данное атрибутное поддерево.
            PDIF_KEY_ORG,     // Org - Точка привязки символа.
            PDIF_KEY_TY,      // Ty - Тип компонента.
            PDIF_KEY_SMD,     // Smd - Прибор, монтируемый на поверхность (только для печатной платы).
            PDIF_KEY_JMP,     // Jmp - "Jumper" (только для печатной платы).
            PDIF_KEY_PL,      // Pl - Координаты местоположения.
            PDIF_KEY_SC,      // Sc - Масштабные коэффициенты по X и Y.
            PDIF_KEY_RO,      // Ro - Угол вращения.
            PDIF_KEY_MR,      // Mr - Статус зеркальности (только для базы данных принципиальной схемы).
            PDIF_KEY_NL,      // Nl - Местоположение имени копии компонента.
            PDIF_KEY_PS,      // Ps - Сторона размещения компонентов (только для печатной платы).
            PDIF_KEY_PA,      // Pa - Угол установки компонента.
            PDIF_KEY_UN,      // Un - Присваиваемое пользователем имя (только для PC-CAPS).
            PDIF_KEY_AT,      // At - текстовый атрибут пользователя.
            // Ключи особых свойств вставки радиокомпонента.
            PDIF_KEY_RD,          // Rd - свойства конструкторского обозначения (текст и координаты местоположения) для вставленного экземпляра радиокомпонента.
            PDIF_KEY_PN,          // Pn - свойства некоторого отдельного вывода вставленной копии компонента (его наименование и координаты положения).
            // Ключевые слова узлов, описывающих упаковку компонента в корпусе (соответствие выводов радиокомпонента ножкам корпуса).
            // Ключи узлов команды SPKG (корпусировка конструктива).
            PDIF_KEY_SNA,     // Sna - имя секции (только для PC-CARDS).
            PDIF_KEY_SP,      // Sp - парность ножек (только цифровая).
            PDIF_KEY_APN,     // Apn - алфавитно-цифровой номер ножки.
            // Ключи узлов команды PKG (корпусировка УГО).
            PDIF_KEY_RDL,     // Rdl - место показа позиционного обозначения (для режима SYMB).
            PDIF_KEY_PNL,     // Pnl - места для показа номеров ножке.
            PDIF_KEY_SD,      // Sd - секция  упаковки  (физического прибора и номера выводов.
            PDIF_KEY_PID      // Pid - упаковочный ID (только для PC-CAPS).
        };

        enum class PDIFOps
        { // Перечисление, содержащее список поддерживаемых "команд" PDIF-базы данных. "Команда" - код конкретного обработчика для некоторого узла дерева PDIF-документа.
            PDIFOP_NOPE = 0,
            PDIFOP_MODE,       // Mode - выбор режима (подтипа) БД - компонентный (описание отдельных компонент) или сборочный (описание всего устройства).
            PDIFOP_VW,         // Vw - положение и размер окон отображения (текуего и сохранённых) картинки в момент создания исходного файла базы данных.
            PDIFOP_PDIF_VREV,  // PDIFvrev - версия (ревизия) PDIF-формата.
            PDIFOP_DB_VREV,    // DBvrev - версия (ревизия) формата самой базы данных (её двоичного исходника).
            PDIFOP_DB_UNIT,    // DBunit - единица измерений координат, используемая в файле.
            PDIFOP_LYRSTR,     // Lyrstr - таблица доступных слоёв - список слоёв, используемых компонентом.
            PDIFOP_LYRPHID,    // Lyrphid - определение спаривания слоёв и их доступности для трассировкию
            PDIFOP_SSYMTBL,    // Ssymtbl - задание свойств контактов, которые служат как переходные отверстия между различными слоями.
            PDIFOP_POLYAP_GLB, // Polyap - глобальная ширина апертуры для рисования границ полигона.
            PDIFOP_POLYAP_LOC, // Polyap - локальная ширина апертуры для рисования границ полигона.
            PDIFOP_DBTYPE,     // DBtype - тип базы данных - схемный ("Schematic") или конструктивный ("PC-Board") элемент.
            // Команды открытия-закрытия контейнерных узлов.
            PDIFOP_CONT_COMPONENT,  // COMPONENT - корневой контейнер документа.
            PDIFOP_CONT_COMPDEF,    // SYMBOL или COMP_DEF - контейнер описания вложенного радиокомпонента.
            PDIFOP_CONT_N,          // N - контейнерное описание проводящей цепи схемы или платы.
            PDIFOP_CONT_I,          // I - контейнерное описание вставки радиокомпонента (его конкретной единичной копии).
            PDIFOP_CONT_P,          // P - контейнерное описание отдельного вывода ("ножки") радиокомпонента.
            PDIFOP_CONT_PKG,        // PKG - контейнер с упаковочной информацией для УГО радиокомпонента.
            PDIFOP_CONT_SPKG,       // SPKG - контейнер с упаковочной информацией конструктива радиокомпонента.
            // Команды конфигурации вставки экземпляра элемента.
            PDIFOP_INS_CN,          // CN - сведения о соединении выводов экземпляра компонента с цепями схемы или платы.
            PDIFOP_INS_IPT,         // IPT - переназначение типов ножек для конкретного экземпляра установленного на плату радиокомпонента.
            PDIFOP_INS_RD,          // Rd - свойства конструкторского обозначения (текст и координаты местоположения) для вставленного экземпляра радиокомпонента.
            PDIFOP_INS_PN,          // Pn - свойства некоторого отдельного вывода вставленной копии компонента (его наименование и координаты положения).
            //  Отдельные иллюстративные графические примитивы.
            PDIFOP_ANNOTATE_POLY,       // Poly - команда рисования свободного (уединённого) составного полигона.
            PDIFOP_PIC_POLY,            // Poly - команда рисования привязанного составного полигона.
            PDIFOP_ANNOTATE_A_ARC,      // A - Создание свободной (отъединённой) A-дуги.
            PDIFOP_PIC_A_ARC,           // A - Создание привязанной A-дуги.
            PDIFOP_ANNOTATE_ARC_ARC,    // Arc - Создание свободной (отъединённой) ARC-дуги.
            PDIFOP_PIC_ARC_ARC,         // Arc - Создание привязанной ARC-дуги.
            PDIFOP_ANNOTATE_CIRCLE,     // C - Создание свободной (отъединённой) окружности.
            PDIFOP_PIC_CIRCLE,          // C - Создание привязанной окружности.
            PDIFOP_ANNOTATE_LINE,       // L - свободная ломаная линия (набор последовательных смежных прямолинейных отрезков).
            PDIFOP_PIC_LINE,            // L - привязанная ломаная линия (набор последовательных смежных прямолинейных отрезков).
            PDIFOP_ANNOTATE_RECT,       // R - свободный прямоугольник.
            PDIFOP_PIC_RECT,            // R - привязанный прямоугольник.
            PDIFOP_ANNOTATE_TEXT,       // T - свободная текстовая строка.
            PDIFOP_PIC_TEXT,            // T - привязанная текстовая строка.
            PDIFOP_ANNOTATE_FILL_RECT,  // Fr - свободный заполненный прямоугольник.
            PDIFOP_PIC_FILL_RECT,       // Fr - привязанный заполненный прямоугольник.
            // Команды установки разнообразных атрибутов различных сущностей базы PDIF-данных.
            PDIFOP_ATR_IN_ORG,         // Org - Точка привязки символа.
            PDIFOP_ATR_IN_TY,          // Ty - Тип компонента.
            PDIFOP_ATR_IN_SMD,         // Smd - Прибор, монтируемый на поверхность (только для печатной платы).
            PDIFOP_ATR_IN_JMP,         // Jmp - "Jumper" (только для печатной платы).
            PDIFOP_ATR_IN_PL,          // Pl - Координаты местоположения.
            PDIFOP_ATR_IN_SC,          // Sc - Масштабные коэффициенты по X и Y.
            PDIFOP_ATR_IN_RO,          // Ro - Угол вращения.
            PDIFOP_ATR_IN_MR,          // Mr - Статус зеркальности (только для базы данных принципиальной схемы).
            PDIFOP_ATR_IN_NL,          // Nl - Местоположение имени копии компонента.
            PDIFOP_ATR_IN_PS,          // Ps - Сторона размещения компонентов (только для печатной платы).
            PDIFOP_ATR_IN_PA,          // Pa - Угол установки компонента.
            PDIFOP_ATR_IN_UN_COMP,     // Un - Присваиваемое пользователем имя вставленного компонента (только для PC-CAPS).
            PDIFOP_ATR_IN_UN_NET,      // Un - Присваиваемое пользователем имя цепи (только для PC-CAPS).
            PDIFOP_ATR_EX_AT,          // At - текстовый атрибут пользователя.
            // Компоненты полигона (команды подраздела Poly).
            PDIFOP_POLY_OL,          // Ol - описание контура полигона.
            PDIFOP_POLY_PV,          // Pv - полигональная пустота (выемка) внутри многоугольника.
            PDIFOP_POLY_CV,          // Cv - круглая выемка внутри многоугольника.
            // Характеристики параметров отдельного вывода ("ножки") некоторого радиокомпонента.
            PDIFOP_PIN_PT,           // Pt - символьный (для УГО символа) или номерной (для конструктива символа) тип вывода радиокомпонента.
            PDIFOP_PIN_LQ,           // Lq - код эквивалентности (взаимозаменяемости) данного вывода радиокомпонента.
            PDIFOP_PIN_PLOC,         // Ploc - точка расположения вывода.
            // Специфические образующие элементы проводящей цепи.
            PDIFOP_NET_LINE,         // L - проводящая ломаная линия (набор смежных проводящих прямолинейных отрезков) в составе электропроводной цепи.
            PDIFOP_NET_POLY,         // Poly - команда создания электропроводного составного полигона в составе проводящей цепи.
            PDIFOP_NET_V,            // V - Переходное отверстие (на плате) или точка пересечения фрагментов одной цепи (на схеме).
            PDIFOP_NET_NN,           // Nn - Положение видимого имени цепи.
            // Настроечные (установочные) команды, указывающие различные режимы и параметры построения последующих примитивов
            // некоторого подраздела.
            PDIFOP_SET_LY,           // Ly - выбор слоя для размещения последующих графических примитивов.
            PDIFOP_SET_LS,           // Ls - назначение типа линии.
            PDIFOP_SET_WD,           // Wd - ширина линии, которой будет обрисован контур примитива.
            PDIFOP_SET_TS,           // Ts - высота текста (высота глифов используемого шрифта)
            PDIFOP_SET_TJ,           // Tj - выравнивание текста.
            PDIFOP_SET_TR,           // Tr - вращение (угол поворота) текста.
            PDIFOP_SET_TM,           // Tm - зеркальность текста.
            // Сведения об упаковке радиокомпонента.
            // Ключи узлов команды SPKG (корпусировка конструктива).
            PDIFOP_PKG_SNA,          // Sna - имя секции (только для PC-CARDS).
            PDIFOP_PKG_SP,           // Sp - парность ножек (только цифровая).
            PDIFOP_PKG_APN,          // Apn - алфавитно-цифровой номер ножки.
            // Ключи узлов команды PKG (корпусировка УГО).
            PDIFOP_PKG_RDL,          // Rdl - место показа позиционного обозначения.
            PDIFOP_PKG_PNL,          // Pnl - места для показа номеров ножке.
            PDIFOP_PKG_SD,           // Sd - секция  упаковки  (физического прибора и номера выводов.
            PDIFOP_PKG_PID           // Pid - упаковочный ID (только для PC-CAPS).
        };

        using TreePathType = std::vector<PDIFKeywords>;
        struct TreeNodeDesc
        { // Описание структуры какого-либо возможного и корректного узла дерева PDIF-базы данных, предположительно поддерживаемого загрузчиком.
            PDIFKeywords key = PDIFKeywords::PDIF_KEY_UNKNOWN;  // Ключевая лексема, идентифицирующая тип данного узла.
            bool is_terminal = true;                            // Является ли этот узел дерева терминальным (листом). Если поле == true, это
                                                                // терминальный узел дерева (лист).
            bool is_setting = false;                            // Является ли данный узел установочным или операционным. Если поле == true, Узел
                                                                // является установочным и настраивает какую-либо переменную состояния редактора -
                                                                //  - устанавливает её в некоторое новое определённое аргументами значение. Операционный
                                                                // же узел представляет собой законченное описание некоторого примитива базы данных -
                                                                // - явдяется операцией создания этого примитива.
            TreePathType tree_path;                             // Маршрут, по которому должен располагаться данный узел. Представляет собой необходимый
                                                                // список вышестоящих разделов дерева, образующих маршрут от его вершины до данного узла.
            std::vector<DataBlockDesc> data_blocks;             // Описание параметров (аргументов) узла. Аргументы узла обязаны следовать в той же
                                                                // последовательности, в каком расположены их дескрипторы в данном массиве.
            PDIFOps opcode = PDIFOps::PDIFOP_NOPE;              // Опкод, обозначающий операцию, соответствующую данному узлу дерева.
        };

        using ArgumentDataType = std::variant<std::monostate, int64_t, double, std::string>; // Тип для хранения значения аргумента.

        // Структуры описания реального узла, сформированного (или формируемого) при разборе данных загружаемой PDIF-базы.
        // Структура хранения текущих настроек загрузки, выполненных посредством установочных команд обрабатываемого PDIF-потока.
        struct NodeSettings
        {
            static int constexpr MAX_LINE_WIDTH = 10000; // Максимальная толщина линии в DBU.
            static int constexpr MAX_TEXT_HEIGHT = 10000; // Максимальная толщина линии в DBU.

            int layer_number = 0;                       // Текущий слой, на котором будут создаваться примитивы.
            LineType line_type = LineType::LINE_SOLID;  // Тип используемой линии.
            int line_width = 1;                         // Её ширина (толщина).
            int text_height = 1;                        // Высота текста (высота составляющих его глифов в DBU).
            TextOrientation text_orient = TextOrientation::TEXT_LEFT_RIGHT; // Ориентация (поворот) и зеркальность текста.
            TextAlign text_align = TextAlign::TEXT_LEFT_UP;                 // Выравнивание текста относительно его якорной точки.
            // Вспомогательные поля, служащие для определения комплексного поля text_orient.
            int text_rotation_index = 0;
            bool is_text_mirror = false;
        };

        enum class NodeSpecType
        {
            NODE_TYPE_NONE = 0,
            NODE_TYPE_POLYGONE,         // Контейнерный узел с описанием многоугольника.
            NODE_TYPE_RADIO_COMPONENT,  // Контейнер дескриптора радиокомпонента.
            NODE_TYPE_NET_DESC,         // Контейнер описания токопроводящей цепи.
            NODE_TYPE_COMP_INSERTION,   // Контейнер с информацией о вставке экземпляра компонента в схему или на плату.
            NODE_TYPE_PIN_DEF,          // Структура описания отдельного вывода радиокомпонента.
            NODE_TYPE_PKG,              // Контейнер упаковочной информации УГО.
            NODE_TYPE_SPKG              // Контейнер упаковочной информации физического конструктива.
        };

        struct NodeSpecInfo
        {
            using HandlerSpecDataDeleter = decltype([](void* spec_pointer) -> void
            {
                if (spec_pointer)
                    ::operator delete(spec_pointer);
            });

            NodeSpecType type = NodeSpecType::NODE_TYPE_NONE;
            // Указатель на блок специальных данных, который может использоваться обработчиком узла для своих внутренних целей.
            std::unique_ptr<void, HandlerSpecDataDeleter> handler_spec_data;
        };

        struct TreeNodeData
        {
            PDIFKeywords key = PDIFKeywords::PDIF_KEY_ANY;  // Ключевое слово, идентифицирующее данный узел.
            std::string key_text;                           // Текст ключевого слова, которым был маркирован данный узел дерева.
            bool is_setting = false;                        // Фактически, указывает тип скобки, открывшей данный узел, - фигурной или квадратной.
            const TreeNodeDesc* node_desc = nullptr;        // Указатель на шаблон (шаблонный описатель) типа данного узла.
            TreePathType tree_path;                         // Действительный маршрут, по которому располагается данный узел.
            std::vector<ArgumentDataType> args;             // Истинное значение аргументов узла, извлечённое из потока.
            NodeSettings node_settings;                     // Оперативные настройки, изменяемые установочными командами PDIF-потока.
            //
            NodeSpecInfo spec_info;                         // Данные, сохраняемые в теле узла его специальным обработчиком.
        };

        class TreeNodeHandler
        { // Обобщенный абстрактный класс-интерфейс (родоначальник иерархии всех подобных конкретных классов, создаваемых для
          // определённых узлов) обработчика событий жизненного цикла древесных узлов дерева PDIF-базы данных.
        public:
            TreeNodeHandler(PDIFFileWorkshop* p_workshop = nullptr) : m_workshop(p_workshop)
            {}
            virtual ~TreeNodeHandler()
            {}
            virtual std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) = 0;   // Обработчик события создания (открытия) узла.
            virtual std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) = 0;  // Обработчик события завершения (закрытия) узла.

            void SetWorkshop(PDIFFileWorkshop* p_workshop = nullptr) noexcept
            {
                m_workshop = p_workshop;
            }

            PDIFFileWorkshop* GetWorkshop() const noexcept
            {
                return m_workshop;
            }

        protected:
            PDIFFileWorkshop* m_workshop = nullptr;
        };

        // Семейство конкретных классов обработки узлов дерева (исполнения связанных с ними узловых команд).

        class NodeModeHandler : public TreeNodeHandler // Mode - выбор режима (подтипа) БД.
        {
        public:
            NodeModeHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        class NodeVWHanlder : public TreeNodeHandler // Vw - положение и размер окон отображения.
        {
        public:
            NodeVWHanlder(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        class NodePDIFvrevHandler : public TreeNodeHandler  // PDIFvrev - версия (ревизия) PDIF-формата.
        {
        public:
            NodePDIFvrevHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        class NodeDBvrevHandler : public TreeNodeHandler  // DBvrev - версия (ревизия) формата двоичного исходника базы данных.
        {
        public:
            NodeDBvrevHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        class NodeDBunitHandler : public TreeNodeHandler  // DBunit - единица измерений координат, используемая в файле.
        {
        public:
            NodeDBunitHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        class NodeLyrstrHandler : public TreeNodeHandler // Lyrstr - таблица доступных слоёв - список слоёв, используемых компонентом.
        {
        public:
            NodeLyrstrHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        class NodeLyrphidHandler : public TreeNodeHandler // Lyrphid - определение спаривания слоёв и их доступности для трассировкию
        {
        public:
            NodeLyrphidHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        class NodeSsymtblHandler : public TreeNodeHandler  // Ssymtbl - задание свойств контактов, которые служат как переходные отверстия между различными слоями.
        {
        public:
            NodeSsymtblHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        class NodeDBtypeHandler : public TreeNodeHandler  // DBtype - тип базы данных - схемный ("Schematic") или конструктивный ("PC-Board") элемент.
        {
        public:
            NodeDBtypeHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        // Классы-обработчики узлов-контейнеров, которые требуют какой-либо особой обработки при открытии и/или закрытии.

        class NodeCOMPONENTHandler : public TreeNodeHandler // COMPONENT - корневой контейнер PDIF-документа.
        {
        public:
            NodeCOMPONENTHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        class NodeCOMPDEFHandler : public TreeNodeHandler // COMP_DEF - контейнер описания вложенного радиокомпонента.
        {
        public:
            NodeCOMPDEFHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        class NodeNHandler : public TreeNodeHandler // N - контейнерное описание проводящей цепи схемы или платы.
        {
        public:
            NodeNHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        class NodeIHandler : public TreeNodeHandler // I - контейнерное описание вставки радиокомпонента (его конкретной единичной копии).
        {
        public:
            NodeIHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        class NodePHandler : public TreeNodeHandler // P - контейнерное описание отдельного вывода некоторого библиотечного радиокомпонента.
        {
        public:
            NodePHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        class NodePKGHandler : public TreeNodeHandler     // PKG - контейнер с упаковочной информацией для УГО радиокомпонента.
        {
        public:
            struct PKGHelper : public ComponentPKGSectDef
            {
                size_t current_pnl_index = 0;
            };

            NodePKGHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        class NodeSPKGHandler : public TreeNodeHandler    // SPKG - контейнер с упаковочной информацией конструктива радиокомпонента.
        {
        public:
            struct SPKGHelper : public ComponentSPKGSectDef
            {
                std::vector<std::string> sect_names;
            };

            NodeSPKGHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        // ----- Конец объявления объектов-обработчиков контейнерных узлов PDIF-базы.

        // Характеристики параметров отдельного вывода ("ножки") некоторого радиокомпонента.
        class NodePtHandler : public TreeNodeHandler    // Pt - символьный (для УГО символа) или номерной (для конструктива символа) тип вывода радиокомпонента.
        {
        public:
            NodePtHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        class NodeLqHandler : public TreeNodeHandler    // Lq - код эквивалентности (взаимозаменяемости) данного вывода радиокомпонента.
        {
        public:
            NodeLqHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        class NodePlocHandler : public TreeNodeHandler  // Ploc - точка расположения вывода.
        {
        public:
            NodePlocHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        // Базовые элементы проводящей цепи (не подходящие под другие определения типовых графических элементов).
        class NodeVHandler : public TreeNodeHandler // V - переходное отверстие на печатной плате.
        {
        public:
            NodeVHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        class NodeNnHandler : public TreeNodeHandler  // Nn - Положение видимого имени цепи.
        {
        public:
            NodeNnHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        // Экземпляры обработчиков создания примитивных фигур - основных видимых элементов графики схем и печатных плат.
        class NodePolyapGlbHandler : public TreeNodeHandler // Polyap - глобальная ширина апертуры для рисования границ полигона.
        {
        public:
            NodePolyapGlbHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        class NodePolyapLocHandler : public TreeNodeHandler // Polyap - локальная ширина апертуры для рисования границ полигона.
        {
        public:
            NodePolyapLocHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        class NodePolyHandler : public TreeNodeHandler       // Poly - команда рисования составного полигона (как свободного, так и привязанного).
        {
        public:
            struct PolyContour
            {
                bool is_hatched = false;
                std::vector<wxPoint> points;
            };

            struct PolyNodeSpecInfo
            {
                std::vector<ObjCVoid> circ_voids; // Список круглых выемок.
                std::vector<ObjPVoid> poly_voids; // Список многоугольных пустот.
                PolyContour           contour;    // Внешний контур (габарит) многоугольника.
                int                   polyap = 0; // Ширина апертуры, используемая для рисовки контура данного полигона.
            };

            NodePolyHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;

        private:
            // Флаг, указывающий на то, что данная фигура (полигон) "привязана" к некоторому компоненту и входит в его состав.
            // Если флаг == false, то фигура является свободной и добавляется непосредственно к массиву графэлементов PCAD-документа.
            bool is_bonded_shape = false;
        };

        // Классы-обработчики вложенных служебных команд общего описания многоугольника (полигона).

        class NodePolyOlHandler : public TreeNodeHandler       // Ol - описание внешнего контура (габарита) полигона.
        {
        public:
            NodePolyOlHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        class NodePolyPvHandler : public TreeNodeHandler       // Pv - многоугольная пустота полигона.
        {
        public:
            NodePolyPvHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        class NodePolyCvHandler : public TreeNodeHandler       // Cv - описание круглой выемки полигона.
        {
        public:
            NodePolyCvHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        // Классы-обработчики узлов, соответствующим примитивным графическим объектам.

        class NodeAHandler : public TreeNodeHandler         // A - Создание свободной (отъединённой) или привязанной (присоединённой) A-дуги.
        {
        public:
            NodeAHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        class NodeArcHandler : public TreeNodeHandler       // Arc - Создание ARC-дуги (обоих возможных типов - свободной или присоединённой).
        {
        public:
            NodeArcHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        class NodeCircleHandler : public TreeNodeHandler    // C - Создание окружности.
        {
        public:
            NodeCircleHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        class NodeLineHandler : public TreeNodeHandler    // L - Создание прямолинейного отрезка.
        {
        public:
            NodeLineHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        class NodeRectangleHandler : public TreeNodeHandler   // R - свободный или привязанный прямоугольник.
        {
        public:
            NodeRectangleHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        class NodeTextHandler : public TreeNodeHandler            // T - свободная или привязанная текстовая строка.
        {
        public:
            NodeTextHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        class NodeFillRectHandler : public TreeNodeHandler            // Fr - заполненный прямоугольник.
        {
        public:
            NodeFillRectHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        // ------ Набор обработчиков разбора блока атрибутов различных сущностей.

        class NodeOrgHandler : public TreeNodeHandler       // Org - Точка привязки символа.
        {
        public:
            NodeOrgHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        class NodeTyHandler : public TreeNodeHandler        // Ty - Тип компонента.
        {
        public:
            NodeTyHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        class NodeSmdHandler : public TreeNodeHandler       // Smd - Прибор, монтируемый на поверхность (только для печатной платы).
        {
        public:
            NodeSmdHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        class NodeJmpHandler : public TreeNodeHandler       // Jmp - "Jumper" (только для печатной платы).
        {
        public:
            NodeJmpHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        class NodeAtHandler : public TreeNodeHandler        // At - текстовый атрибут пользователя.
        {
        public:
            NodeAtHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        class NodePlHandler : public TreeNodeHandler        // Pl - Координаты местоположения.
        {
        public:
            NodePlHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        class NodeScHandler : public TreeNodeHandler        // Sc - Масштабные коэффициенты по X и Y.
        {
        public:
            NodeScHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        class NodeRoHandler : public TreeNodeHandler        // Ro - Угол вращения.
        {
        public:
            NodeRoHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        class NodeMrHandler : public TreeNodeHandler        // Mr -  Статус зеркальности (только для базы данных принципиальной схемы).
        {
        public:
            NodeMrHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        class NodeNlHandler : public TreeNodeHandler        // Nl - Местоположение имени копии компонента.
        {
        public:
            NodeNlHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        class NodePsHandler : public TreeNodeHandler        // Ps - Сторона размещения компонентов (только для печатной платы).
        {
        public:
            NodePsHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        class NodePaHandler : public TreeNodeHandler        // Pa - Угол установки компонента.
        {
        public:
            NodePaHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        class NodeUnCompHandler : public TreeNodeHandler        // Un - Присваиваемое пользователем имя (для вставки компонента, только для PC-CAPS).
        {
        public:
            NodeUnCompHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        class NodeUnNetHandler : public TreeNodeHandler        // Un - Присваиваемое пользователем имя (для проводящей цепи, только для PC-CAPS).
        {
        public:
            NodeUnNetHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        // ------ Узлы вставки радиокомпонент.
        // CN - сведения о соединении выводов экземпляра компонента с цепями схемы или платы.
        class NodeCNHandler : public TreeNodeHandler
        {
        public:
            NodeCNHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        // IPT - переназначение типов ножек для конкретного экземпляра установленного на плату радиокомпонента.
        class NodeIPTHandler : public TreeNodeHandler
        {
        public:
            NodeIPTHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        // Rd - свойства конструкторского обозначения (текст и координаты местоположения) для вставленного экземпляра радиокомпонента.
        class NodeRdHandler : public TreeNodeHandler
        {
        public:
            NodeRdHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        // Pn - свойства некоторого отдельного вывода вставленной копии компонента (его наименование и координаты положения).
        class NodePnHandler : public TreeNodeHandler
        {
        public:
            NodePnHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        // Объекты-обработчики установочных команд.
        class NodeLyHandler : public TreeNodeHandler        // Ly - выбор слоя размещения примитивов.
        {
        public:
            NodeLyHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        class NodeLsHandler : public TreeNodeHandler        // Ls - назначение типа линии.
        {
        public:
            NodeLsHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        class NodeWdHandler : public TreeNodeHandler        // Wd - указание толщины линии.
        {
        public:
            NodeWdHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        // Группа объектов-обработчиков команд настройки параметров текстового примитива.
        class NodeTsHandler : public TreeNodeHandler        // Ts - высота текста (высота глифов используемого шрифта)
        {
        public:
            NodeTsHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        class NodeTjHandler : public TreeNodeHandler        // Tj - выравнивание текста.
        {
        public:
            NodeTjHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        class NodeTrHandler : public TreeNodeHandler        // Tr - вращение (угол поворота) текста.
        {
        public:
            NodeTrHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        class NodeTmHandler : public TreeNodeHandler        // Tm - зеркальность текста.
        {
        public:
            NodeTmHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        // Обработчики блоков, содержащих сведения об упаковке радиокомпонента.
        // Объекты, связанные с узлами команды SPKG (корпусировка конструктива).
        class NodeSnaHandler : public TreeNodeHandler          // Sna - имя секции (только для PC-CARDS).
        {
        public:
            NodeSnaHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        class NodeSpHandler : public TreeNodeHandler           // Sp - парность ножек (только цифровая).
        {
        public:
            NodeSpHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        class NodeApnHandler : public TreeNodeHandler          // Apn - алфавитно-цифровой номер ножки.
        {
        public:
            NodeApnHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        // Объекты, связанные с узлами команды PKG (корпусировка УГО).
        class NodeRdlHandler : public TreeNodeHandler          // Rdl - место показа позиционного обозначения.
        {
        public:
            NodeRdlHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        class NodePnlHandler : public TreeNodeHandler          // Pnl - места для показа номеров ножке.
        {
        public:
            NodePnlHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        class NodeSdHandler : public TreeNodeHandler           // Sd - секция упаковки (физического прибора и номера выводов.
        {
        public:
            NodeSdHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        class NodePidHandler : public TreeNodeHandler           // Pid - упаковочный ID (только для PC-CAPS).
        {
        public:
            NodePidHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data) override;
            std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data) override;
        };

        // -----------------------------------------------------

        struct LexemData
        { // Обобщённый описатель лексем, поток которых возникает после лексического разбора входного символьного потока PDIF-документа.
            LexemCategory cat = LexemCategory::PDIF_LEX_UNKNOWN;
            PDIFKeywords key = PDIFKeywords::PDIF_KEY_UNKNOWN;
            std::string text;
            ArgumentDataType data;
        };

        struct PDIFOpDetailed
        { // Детализированная PDIF-операция, извлеченная из входного потока при разборе загружаемой базы данных.
            PDIFOps opcode = PDIFOps::PDIFOP_NOPE;  // Опкод операции, учитывающий ключевое слово и расположение операции в дереве.
            bool is_setting = false;                // Вспомогательный атрибут, разделяющий оперативные и установочные операции.
            std::vector<ArgumentDataType> arguments;     // Извлечённые из входного потока и декодированные аргументы операции.
            std::vector<PDIFKeywords> tree_path;         // Маршрут, по которому обнаружен данный узел в исходной базе.
        };

        enum PictureMeasureUnits
        {
            PDIF_MEASURE_UNIT_INCH = 2,
            PDIF_MEASURE_UNIT_MM = 7
        };

        enum PictureGraphEditor
        {
            PDIF_EDITOR_PCCARDS = 1,
            PDIF_EDITOR_PCCAPS = 10
        };

        // Словарь преобразования ключевого слова в соответствующую ему номерную константу перечисления PDIFKeywords.
        static const std::unordered_map<std::string_view, PDIFKeywords> keyword_to_enum_value;
        // Словарь, отражающий константы ключевых слов в описания кодируемых им узлов дерева PDIF-базы данных.
        static const std::unordered_multimap<PDIFKeywords, TreeNodeDesc> keyword_to_node_desc;

        // Экземпляры классов-обработчиков узловых команд узлов PDIF-дерева.
        // Экземпляры для обслуживания контейнерных узлов.
        NodeCOMPONENTHandler node_component_handler;    // COMPONENT - корневой узел всей древовидной структуры PDIF-документа.
        NodeCOMPDEFHandler node_comp_def_handler;       // COMP_DEF - контейнер описания вложенного радиокомпонента.
        NodeNHandler node_n_handler;                    // N - контейнерное описание проводящей цепи схемы или платы.
        NodeIHandler node_i_handler;                    // I - контейнерное описание вставки радиокомпонента (его конкретной единичной копии).
        NodePHandler node_p_handler;                    // P - контейнерное описание некоторого вывода радиокомпонента.
        NodePKGHandler node_pkg_handler;                // PKG - контейнер с упаковочной информацией для УГО радиокомпонента.
        NodeSPKGHandler node_spkg_handler;              // SPKG - контейнер с упаковочной информацией конструктива радиокомпонента.
        // Обработчики глобальных параметров PDIF-документа, принадлежащих к его среде.
        NodeModeHandler node_mode_handler;                    // Выбор режима (подтипа) БД
        NodeVWHanlder node_vw_handler;                        // Vw - положение и размер окон отображения.
        NodePDIFvrevHandler node_pdif_vrev_handler;           // PDIFvrev - версия (ревизия) PDIF-формата.
        NodeDBvrevHandler node_db_vrev_handler;               // DBvrev - версия (ревизия) формата двоичного исходника базы данных.
        NodeDBunitHandler node_db_unit_handler;               // DBunit - единица измерений координат, используемая в файле.
        NodeLyrstrHandler node_lyrstr_handler;                // Lyrstr - таблица доступных слоёв - список слоёв, используемых компонентом.
        NodeLyrphidHandler node_lyrphid_handler;              // Lyrphid - определение спаривания слоёв и их доступности для трассировкию
        NodeSsymtblHandler node_ssymtbl_handler;              // Ssymtbl - задание свойств контактов, которые служат как переходные отверстия между различными слоями.
        NodePolyapGlbHandler node_polyap_glb_handler;         // Polyap(glob) - глобальная ширина апертуры для рисования границ полигона.
        NodePolyapLocHandler node_polyap_loc_hanlder;         // Polyap(loc) - локальная ширина апертуры для рисования границ полигона.
        NodeDBtypeHandler node_dbtype_handler;                // DBtype - тип базы данных - схемный ("Schematic") или конструктивный ("PC-Board") элемент.
        // Параметры выводов ("ножек") радиокомпонентов.
        NodePtHandler node_pt_handler;                      // Pt - назначение типа (строкового или числового) вывода.
        NodeLqHandler node_lq_handler;                      // Lq - код эквивалентности выводов.
        NodePlocHandler node_ploc_handler;                  // Ploc - координаты точки местоположения вывода.
        // Базовые элементы проводящих цепей.
        NodeVHandler node_v_handler;                        // V - переходное отверстие на печатной плате.
        NodeNnHandler node_nn_handler;                      // Nn - Положение видимого имени цепи.
        // Конструкторы примитивных (простейших) графических объектов. Один и тот же обработчик обслуживает все команды создания соответствующего примитива в
        // любом контектсе (как свободные фигуры, так и привязанные).
        NodeAHandler node_a_handler;                          // A - Создание A-дуги.
        NodeArcHandler node_arc_handler;                      // Arc - Создание ARC-дуги.
        NodeCircleHandler node_circle_handler;                // C - Создание окружности.
        NodeLineHandler node_line_handler;                    // L - Создание прямолинейного отрезка.
        NodeRectangleHandler node_rect_handler;               // R - Прямоугольник.
        NodeTextHandler node_text_handler;                    // T - Текстовая строка.
        NodeFillRectHandler node_fill_rect_handler;           // Fr - Заполненный прямоугольник.
        NodePolyHandler node_poly_hanlder;                    // Poly - команда рисования составного полигона.
        // Составляющие многоугольника.
        NodePolyOlHandler node_poly_ol_handler;               // Ol - описание внешнего контура (габарита) полигона.
        NodePolyPvHandler node_poly_pv_hanlder;               // Pv - многоугольная пустота полигона.
        NodePolyCvHandler node_poly_cv_handler;               // Cv - описание круглой выемки полигона.
        // Объекты-обработчики узлов параметров вставки радиокомпонент.
        NodeCNHandler node_cn_handler;      // CN - сведения о соединении выводов экземпляра компонента с цепями схемы или платы.
        NodeIPTHandler node_ipt_handler;    // IPT - переназначение типов ножек для конкретного экземпляра установленного на плату радиокомпонента.
        NodeRdHandler node_rd_handler;      // Rd - позиционное обозначение экземпляра компонента - его текст и положение.
        NodePnHandler node_pn_handler;      // Pn - обозначение вывода элемента - текст и координаты точки размещения.
        // Инфраструктура и образующий её набор обработчиков для получения и установки различных атрибутов, дополняющих описание
        // некоторых сущностей загружаемой базы PCAD-данных.
        NodeOrgHandler node_org_handler;            // Org - Точка привязки символа.
        NodeTyHandler node_ty_handler;              // Ty - Тип компонента.
        NodeSmdHandler node_smd_handler;            // Smd - Прибор, монтируемый на поверхность (только для печатной платы).
        NodeJmpHandler node_jmp_handler;            // Jmp - "Jumper" (только для печатной платы).
        NodePlHandler node_pl_handler;              // Pl - Координаты местоположения.
        NodeScHandler node_sc_handler;              // Sc - Масштабные коэффициенты по X и Y.
        NodeRoHandler node_ro_handler;              // Ro - Угол вращения.
        NodeMrHandler node_mr_handler;              // Mr - Статус зеркальности (только для базы данных принципиальной схемы).
        NodeNlHandler node_nl_handler;              // Nl - Местоположение имени копии компонента.
        NodePsHandler node_ps_handler;              // Ps - Сторона размещения компонентов (только для печатной платы).
        NodePaHandler node_pa_handler;              // Pa - Угол установки компонента.
        NodeUnCompHandler node_un_comp_handler;     // Un - Присваиваемое пользователем имя вставленного компонента (только для PC-CAPS).
        NodeUnNetHandler node_un_net_handler;       // Un - Присваиваемое пользователем имя проводящей цепи (только для PC-CAPS).
        NodeAtHandler node_at_handler;              // At - текстовый атрибут пользователя.
        // Обработчики установочных команд (команд настройки атрибутов рисования).
        NodeLyHandler node_layer_select_handler;    // Ly - выбор слоя размещения примитивов.
        NodeLsHandler node_line_type_handler;       // Ls - назначение типа линии.
        NodeWdHandler node_line_width_handler;      // Wd - указание толщины линии.
        // Группа объектов-обработчиков команд настройки параметров текстового примитива.
        NodeTsHandler node_text_height_handler;     // Ts - высота текста (высота глифов используемого шрифта).
        NodeTjHandler node_text_justify_hanlder;    // Tj - выравнивание текста.
        NodeTrHandler node_text_rotation_handler;   // Tr - вращение (угол поворота) текста.
        NodeTmHandler node_text_mirror_handler;     // Tm - зеркальность текста.
        // Экземпляры объектов обработки данных упаковки или корпусировки конструктива (команды SPKG или PKG соответственно).
        // Вложенные узлы команды SPKG (корпусировка конструктива).
        NodeSnaHandler node_sna_handler;            // Sna - имя секции (только для PC-CARDS).
        NodeSpHandler node_sp_handler;              // Sp - парность ножек (только цифровая).
        NodeApnHandler node_apn_handler;            // Apn - алфавитно-цифровой номер ножки.
        // Экземпляры объектов обработки данных корпусировки УГО (команды PKG).
        NodeRdlHandler node_rdl_handler;            // Rdl - место показа позиционного обозначения.
        NodePnlHandler node_pnl_handler;            // Pnl - места для показа номеров ножке.
        NodeSdHandler node_sd_handler;              // Sd - секция  упаковки  (физического прибора и номера выводов.
        NodePidHandler node_pid_handler;            // Pid - упаковочный ID (только для PC-CAPS).

        // Словарь, содержащий указатели на объекты-обработчики различных узлов дерева PDIF-базы (точнее,
        // объекты-исполнители соответствующих им команд).
        const std::unordered_map<PDIFOps, TreeNodeHandler*> opcode_to_handler
        {
            // Обработчики обслуживания контейнерных узлов.
            {PDIFOps::PDIFOP_CONT_COMPONENT &node_component_handler},   // COMPONENT - корневой узел всей древовидной структуры PDIF-документа.
            {PDIFOps::PDIFOP_CONT_COMPDEF, &node_comp_def_handler},     // COMP_DEF - контейнер описания вложенного радиокомпонента.
            {PDIFOps::PDIFOP_CONT_N, &node_n_handler},                  // N - контейнерное описание проводящей цепи схемы или платы.
            {PDIFOps::PDIFOP_CONT_I, &node_i_handler},                  // I - контейнерное описание вставки радиокомпонента (его конкретной единичной копии).
            {PDIFOps::PDIFOP_CONT_P, &node_p_handler},                  // P - контейнерное описание вывода ("ножки") радиоэлемента.
            {PDIFOps::PDIFOP_CONT_PKG, &node_pkg_handler},              // PKG - контейнер с упаковочной информацией для УГО радиокомпонента.
            {PDIFOps::PDIFOP_CONT_SPKG, &node_spkg_handler},            // SPKG - контейнер с упаковочной информацией конструктива радиокомпонента.
            // Обработчики параметров среды PDIF-документа.
            {PDIFOps::PDIFOP_MODE, &node_mode_handler},                 // Выбор режима (подтипа) БД.
            {PDIFOps::PDIFOP_VW, &node_vw_handler},                     // Vw - положение и размер окон отображения.
            {PDIFOps::PDIFOP_PDIF_VREV, &node_pdif_vrev_handler},       // PDIFvrev - версия (ревизия) PDIF-формата.
            {PDIFOps::PDIFOP_DB_VREV, &node_db_vrev_handler},           // DBvrev - версия (ревизия) формата двоичного исходника базы данных.
            {PDIFOps::PDIFOP_DB_UNIT, &node_db_unit_handler},           // DBunit - единица измерений координат, используемая в файле.
            {PDIFOps::PDIFOP_LYRSTR, &node_lyrstr_handler},             // Lyrstr - таблица доступных слоёв - список слоёв, используемых компонентом.
            {PDIFOps::PDIFOP_LYRPHID, &node_lyrphid_handler},           // Lyrphid - определение спаривания слоёв и их доступности для трассировкию
            {PDIFOps::PDIFOP_SSYMTBL, &node_ssymtbl_handler},           // Ssymtbl - задание свойств контактов, которые служат как переходные отверстия между различными слоями.
            {PDIFOps::PDIFOP_POLYAP_GLB, &node_polyap_glb_handler},     // Polyap(glob) - глобальная ширина апертуры для рисования границ полигона.
            {PDIFOps::PDIFOP_POLYAP_LOC, &node_polyap_loc_hanlder},     // Polyap(loc) - локальная ширина апертуры для рисования границ полигона.
            {PDIFOps::PDIFOP_DBTYPE, &node_dbtype_handler},             // DBtype - тип базы данных - схемный ("Schematic") или конструктивный ("PC-Board") элемент.
            // Команды создания отдельных примитивных графических элементов.
            {PDIFOps::PDIFOP_ANNOTATE_POLY, &node_poly_hanlder},
            {PDIFOps::PDIFOP_PIC_POLY, &node_poly_hanlder},
            {PDIFOps::PDIFOP_ANNOTATE_A_ARC, &node_a_handler},
            {PDIFOps::PDIFOP_PIC_A_ARC, &node_a_handler},
            {PDIFOps::PDIFOP_ANNOTATE_ARC_ARC, &node_arc_handler},
            {PDIFOps::PDIFOP_PIC_ARC_ARC, &node_arc_handler},
            {PDIFOps::PDIFOP_ANNOTATE_CIRCLE, &node_circle_handler},
            {PDIFOps::PDIFOP_PIC_CIRCLE, &node_circle_handler},
            {PDIFOps::PDIFOP_ANNOTATE_LINE, &node_line_handler},
            {PDIFOps::PDIFOP_PIC_LINE, &node_line_handler},
            {PDIFOps::PDIFOP_ANNOTATE_RECT, &node_rect_handler},
            {PDIFOps::PDIFOP_PIC_RECT, &node_rect_handler},
            {PDIFOps::PDIFOP_ANNOTATE_TEXT, &node_text_handler},
            {PDIFOps::PDIFOP_PIC_TEXT, &node_text_handler},
            {PDIFOps::PDIFOP_ANNOTATE_FILL_RECT, &node_fill_rect_handler},
            {PDIFOps::PDIFOP_PIC_FILL_RECT, &node_fill_rect_handler},
            // Обработчики описания компонентов многоугольника.
            {PDIFOps::PDIFOP_POLY_OL, &node_poly_ol_handler},
            {PDIFOps::PDIFOP_POLY_PV, &node_poly_pv_hanlder},
            {PDIFOps::PDIFOP_POLY_CV, &node_poly_cv_handler},
            // Экземпляры обработчиков-извлекателей различного рода "атрибутов", дополняющих описание некоторых сущностей загружаемой базы PCAD-данных.
            {PDIFOps::PDIFOP_ATR_IN_ORG, &node_org_handler},       // Org - Точка привязки символа.
            {PDIFOps::PDIFOP_ATR_IN_TY, &node_ty_handler},         // Ty - Тип компонента.
            {PDIFOps::PDIFOP_ATR_IN_SMD, &node_smd_handler},       // Smd - Прибор, монтируемый на поверхность (только для печатной платы).
            {PDIFOps::PDIFOP_ATR_IN_JMP, &node_jmp_handler},       // Jmp - "Jumper" (только для печатной платы).
            {PDIFOps::PDIFOP_ATR_IN_PL, &node_pl_handler},         // Pl - Координаты местоположения.
            {PDIFOps::PDIFOP_ATR_IN_SC, &node_sc_handler},         // Sc - Масштабные коэффициенты по X и Y.
            {PDIFOps::PDIFOP_ATR_IN_RO, &node_ro_handler},         // Ro - Угол вращения.
            {PDIFOps::PDIFOP_ATR_IN_MR, &node_mr_handler},         // Mr - Статус зеркальности (только для базы данных принципиальной схемы).
            {PDIFOps::PDIFOP_ATR_IN_NL, &node_nl_handler},         // Nl - Местоположение имени копии компонента.
            {PDIFOps::PDIFOP_ATR_IN_PS, &node_ps_handler},         // Ps - Сторона размещения компонентов (только для печатной платы).
            {PDIFOps::PDIFOP_ATR_IN_PA, &node_pa_handler},         // Pa - Угол установки компонента.
            {PDIFOps::PDIFOP_ATR_IN_UN_COMP, &node_un_comp_handler}, // Un - Присваиваемое пользователем имя вставленного компонента (только для PC-CAPS).
            {PDIFOps::PDIFOP_ATR_IN_UN_NET, &node_un_net_handler},   // Un - Присваиваемое пользователем имя проводящей цепи (только для PC-CAPS).
            {PDIFOps::PDIFOP_ATR_EX_AT, &node_at_handler},         // At - "внешний" текстовый атрибут сущности.
            // Параметры выводов ("ножек") радиокомпонентов.
            {PDIFOps::PDIFOP_PIN_PT, &node_pt_handler},            // Pt - назначение типа (строкового или числового) вывода.
            {PDIFOps::PDIFOP_PIN_LQ, &node_lq_handler},            // Lq - код эквивалентности выводов.
            {PDIFOps::PDIFOP_PIN_PLOC, &node_ploc_handler},        // Ploc - координаты точки местоположения вывода.
            // Специфические образующие элементы проводящей цепи.
            {PDIFOps::PDIFOP_NET_V, &node_v_handler},           // V - Переходное отверстие (на плате) или точка пересечения фрагментов одной цепи (на схеме).
            {PDIFOps::PDIFOP_NET_NN, &node_nn_handler},         // Nn - Положение видимого имени цепи.
            // Пары соответствия для назначения обработчиков настроечным командам PDIF-базы.
            {PDIFOps::PDIFOP_SET_LY, &node_layer_select_handler},
            {PDIFOps::PDIFOP_SET_LS, &node_line_type_handler},
            {PDIFOps::PDIFOP_SET_WD, &node_line_width_handler},
            // Обработчики для команд настройки параметров текстового примитива.
            {PDIFOps::PDIFOP_SET_TS, &node_text_height_handler},
            {PDIFOps::PDIFOP_SET_TJ, &node_text_justify_hanlder},
            {PDIFOps::PDIFOP_SET_TR, &node_text_rotation_handler},
            {PDIFOps::PDIFOP_SET_TM, &node_text_mirror_handler},
            // Сведения об упаковке радиокомпонента.
            // Ключи узлов команды SPKG (корпусировка конструктива).
            {PDIFOps::PDIFOP_PKG_SNA, &node_sna_handler},        // Sna - имя секции (только для PC-CARDS).
            {PDIFOps::PDIFOP_PKG_SP, &node_sp_handler},          // Sp - парность ножек (только цифровая).
            {PDIFOps::PDIFOP_PKG_APN, &node_apn_handler},        // Apn - алфавитно-цифровой номер ножки.
            // Ключи узлов команды PKG (корпусировка УГО).
            {PDIFOps::PDIFOP_PKG_RDL, &node_rdl_handler},        // Rdl - место показа позиционного обозначения.
            {PDIFOps::PDIFOP_PKG_PNL, &node_pnl_handler},        // Pnl - места для показа номеров ножке.
            {PDIFOps::PDIFOP_PKG_SD, &node_sd_handler},          // Sd - секция  упаковки  (физического прибора и номера выводов.
            {PDIFOps::PDIFOP_PKG_PID, &node_pid_handler}         // Pid - упаковочный ID (только для PC-CAPS).
        };

        // Ниже находятся динамические поля класса PDIFFileWorkshop, изменяющиеся в процессе загрузки PDIF-базы и
        // отражающие текущее состояние загрузчика.
        // Далее следует список массивов, накапливающих информацию, полученную из PDIF-потока в процессе его обработки.
        std::vector<TreeNodeData> node_stack;   // Текущая ветвь дерева PDIF-документа, анализируемая в данный момент.
        // Объект-накопитель, содержащий и собирающий информацию, получаемую при разборе загружаемого PDIF-документа.
        PCADFileSource load_file_data;

        // Поля, определяющие текущее состояние лексического разборщика.
        // is_wait_keyword - флаг, установка которого в "ИСТИНУ" свидетельствует о том, что мы ожидаем ключевое
        // слово для идентификации вновь созданного узла дерева.
        bool is_wait_keyword = false;

        // Вспомогательные методы лексического разборщика.
        bool SkipSpaces(std::istream& istr) const;
        bool SkipComment(std::istream& istr) const;
        // Основной метод лексического разбора входного PDIF-пакета.
        LexemData GetNextLexem(std::istream& istr);
        // Функция поиска подходящего узла, описание которого в достаточной степени совпадает с заданных аргументом find_node.
        const TreeNodeDesc* FindNodeDesc(const TreeNodeDesc& find_node) const;
        // Функции преобразования координат в единицах базы данных ("точечных" координат) в "логические" координаты (координаты на
        // графической плоскости, то есть той поверхности, на которой определены графические примитивы изображения).
        wxPoint ConvPntToLog(int pict_x, int pict_y) const;
        wxPoint ConvPntToLog(wxPoint pict) const;
        wxRect ConvRectToLog(int pict_ldx, int pict_ldy, int pict_rux, int pict_ruy) const;
        wxRect ConvRectToLog(wxRect pict_rect) const;
        // Проверка совпадения маршрута concrete_path шаблону pattern_path.
        bool IsPDIFPathCorrespond(const TreePathType& concrete_path, const TreePathType& pattern_path) const;
        int FindLayerByName(const std::string& find_layer_name) const;
        // Вспомогательные функции, применяемые обработчиками событий узлов для ориентации в текущем состоянии узлового стека
        // (в своём текущем положении в дереве базы данных).
        bool IsPrevNodeContainer(TreeNodeData* node_data, PDIFKeywords key = PDIFKeywords::PDIF_KEY_ANY) const;
        bool IsPrevNodeAttributed(TreeNodeData* node_data) const;
        // Поиск самого последнего (ближе всего к верхущке стека) узла дерева, лежащего на его текущей обходимой в данной момент ветке.
        TreeNodeData* FindNodeByType(NodeSpecType find_type) const;
        // Проверка соответствия "хвоста" (суффикса) маршрута concrete_path требованиям трафарета pattern_tail_path.
        bool CheckNodeStackTail(const TreePathType& concrete_path, const TreePathType& pattern_tail_path) const;
    };
} // namespace HandlerPDIF
#endif // header guard
