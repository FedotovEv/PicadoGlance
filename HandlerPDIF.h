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
        friend class NodeModeHandler;
        friend class NodeVWHanlder;
        friend class NodePDIFvrevHandler;
        friend class NodeDBvrevHandler;
        friend class NodeDBunitHandler;
        friend class NodeLyrstrHandler;
        friend class NodeLyrphidHandler;
        friend class NodeSsymtblHandler;
        friend class NodePolyapGlbHandler;
        friend class NodePolyapLochandler;
        friend class NodeAnnotatePolyHandler;
        friend class NodePicPolyHandler;
        friend class NodeDBtypeHandler;
        friend class NodeAnnotateAHandler;
        friend class NodePicAHandler;
        friend class NodeAnnotateArcHandler;
        friend class NodePicArcHandler;
        friend class NodeAnnotateCircleHandler;
        friend class NodePicCircleHandler;
        friend class NodeAnnotateLineHandler;
        friend class NodePicLineHandler;
        friend class NodeAnnotateRectangleHandler;
        friend class NodePicRectangleHandler;
        friend class NodeAnnotateTextHandler;
        friend class NodePicTextHandler;
        friend class NodeAnnotateFillRectHandler;
        friend class NodePicFillRectHandler;
        friend class NodeAtHandler;

    public:
        PDIFFileWorkshop();
        virtual std::string GetFileWorkshopDescription() const override
        {
            return "PCAD PDF(PDIF-OUT) файл взаимообмена информацией";
        }

        virtual std::vector<std::pair<std::string, std::string>> GetFileExtensions() const override
        {
            using namespace std;

            pair<string, string> extension_pair = {"Базы данных PCAD в формате взаимообмена информацией PDIF (*.pdf)"s, "*.pdf"s};
            return {move(extension_pair)};
        }

        virtual FileWorkshop::LoadFileResult LoadPCADFile(std::istream& istr,
                                FileWorkshop::AdditionalLoadInfo& additional_load_info) override;

    private:
        static std::optional<int> CheckIntValue(int64_t test_value, int limit_value = 0)
        {
            if (limit_value <= 0)
            {
                if (test_value < INT_MIN || test_value > INT_MAX)
                    return {};
                else
                    return static_cast<int>(test_value);
            }
            else
            {
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
            PDIF_KEY_VIEW, // VIEW - подраздел раздела USER, содержащий данные, используемые редакторами комплекса PCAD.
            PDIF_KEY_DETAIL,    // DETAIL - главный раздел проекта, содержащий корпус всей его основной информации.
            PDIF_KEY_SYMBOL,    // SYMBOL - второй главный раздел проекта, содержащий все данные описания библиотечного элемента при работе редакторов
                                // в символьном режиме (SYMB).
            PDIF_KEY_ANNOTATE,  // ANNOTATE - раздел, содержащий описание графических и разметочных элементов рисунка.
            PDIF_KEY_SUBCOMP,   // SUBCOMP - общий раздел хранения информации обо всех библиотечных компонентах, задействованных в схеме.
            PDIF_KEY_COMPDEF,   // COMP_DEF - подраздел раздела SUBCOMP для хранения информации о каком-либо отдельном компоненте, используемом в данном
                                // устройстве (печатной плате).
            PDIF_KEY_PIC,       // PIC - графическое описание объектов.
            PDIF_KEY_I,         // I - подраздел раздела SUBCOMP с описанием отдельного включения некоторого библиотечного компонентиа в схему устройства.
            PDIF_KEY_ATR,       // ATR - раздел атрибутов компонента.
            PDIF_KEY_IN,        // IN - секция (подраздел) хранения внутренних (предопределённых) атрибутов компонента.
            PDIF_KEY_EX,        // EX - секция (подраздел) хранения внешних (заданных пользователем) атрибутов компонента.
            // Далее перечислены константы терминальных лексем (листьев) дерева базы данных. Их правила именования таковы - первая буква латинская заглавная,
            // остальные - латинские строчные.
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
            // Отдельные графические примитивы.
            PDIF_KEY_A,       // A - "старый" тип дуги.
            PDIF_KEY_ARC,     // Arc - "новый" тип дуги.
            PDIF_KEY_C,       // C - полная окружность.
            PDIF_KEY_L,       // L - линия (прямолинейный отрезок).
            PDIF_KEY_R,       // R - прямоугольник.
            PDIF_KEY_T,       // T - текстовая строка.
            PDIF_KEY_FR,      // Fr - заполненный прямоугольник.
            //
            PDIF_KEY_AT,      // At - текстовый атрибут пользователя.
            //
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
            PDIF_KEY_TM       // Tm - зеркальность текста.
        };

        enum class PDIFOps
        { // Перечисление, содержащее список поддерживаемых "команд" PDIF-базы данных. "Команда"
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
            //  Отдельные графические примитивы.
            PDIFOP_ANNOTATE_POLY,    // Poly - команда рисования свободного (уединённого) составного полигона.
            PDIFOP_PIC_POLY,         // Poly - команда рисования привязанного составного полигона.
            PDIFOP_ANNOTATE_A_ARC,   // A - Создание свободной (отъединённой) A-дуги.
            PDIFOP_PIC_A_ARC,        // A - Создание привязанной A-дуги.
            PDIFOP_ANNOTATE_ARC_ARC, // Arc - Создание свободной (отъединённой) ARC-дуги.
            PDIFOP_PIC_ARC_ARC,      // Arc - Создание привязанной ARC-дуги.
            PDIFOP_ANNOTATE_CIRCLE,  // C - Создание свободной (отъединённой) окружности.
            PDIFOP_PIC_CIRCLE,       // C - Создание привязанной окружности.
            PDIFOP_ANNOTATE_LINE,    // L - свободная линия (прямолинейный отрезок).
            PDIFOP_PIC_LINE,         // L - привязанная линия (прямолинейный отрезок).
            PDIFOP_ANNOTATE_RECT,    // R - свободный прямоугольник.
            PDIFOP_PIC_RECT,         // R - привязанный прямоугольник.
            PDIFOP_ANNOTATE_TEXT,    // T - свободная текстовая строка.
            PDIFOP_PIC_TEXT,         // T - привязанная текстовая строка.
            PDIFOP_ANNOTATE_FILL_RECT, // Fr - свободный заполненный прямоугольник.
            PDIFOP_PIC_FILL_RECT,      // Fr - привязанный заполненный прямоугольник.
            //
            PDIFOP_EX_ATTR,          // At - текстовый атрибут пользователя.
            // Компоненты полигона (команды подраздела Poly).
            PDIFOP_POLY_OL,          // Ol - описание контура полигона.
            PDIFOP_POLY_PV,          // Pv - полигональная пустота (выемка) внутри многоугольника.
            PDIFOP_POLY_CV,          // Cv - круглая выемка внутри многоугольника.
            // Настроечные (установочные) команды, указывающие различные режимы и параметры построения последующих примитивов некоторого подраздела.
            PDIFOP_SET_LY,      // Ly - выбор слоя для размещения последующих графических примитивов.
            PDIFOP_SET_LS,      // Ls - назначение типа линии.
            PDIFOP_SET_WD,      // Wd - ширина линии, которой будет обрисован контур примитива.
            PDIFOP_SET_TS,      // Ts - высота текста (высота глифов используемого шрифта)
            PDIFOP_SET_TJ,      // Tj - выравнивание текста.
            PDIFOP_SET_TR,      // Tr - вращение (угол поворота) текста.
            PDIFOP_SET_TM       // Tm - зеркальность текста.
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
            NODE_TYPE_POLYGONE
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
            std::string key_text;               // Текст ключевого слова, которым был маркирован данный узел дерева.
            bool is_setting = false;            // Фактически, указывает тип скобки, открывшей данный узел, - фигурной или квадратной.
            const TreeNodeDesc* node_desc = nullptr;  // Указатель на шаблон (шаблонный описатель) типа данного узла.
            TreePathType tree_path;             // Действительный маршрут, по которому располагается данный узел.
            std::vector<ArgumentDataType> args; // Истинное значение аргументов узла, извлечённое из потока.
            NodeSettings node_settings;         // Оперативные настройки, изменяемые установочными командами PDIF-потока.
            //
            NodeSpecInfo spec_info;             // Данные, сохраняемые в теле узла его специальным обработчиком.
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
            virtual std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data);
            virtual std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data);
        };

        class NodeVWHanlder : public TreeNodeHandler // Vw - положение и размер окон отображения.
        {
        public:
            NodeVWHanlder(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            virtual std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data);
            virtual std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data);
        };

        class NodePDIFvrevHandler : public TreeNodeHandler  // PDIFvrev - версия (ревизия) PDIF-формата.
        {
        public:
            NodePDIFvrevHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            virtual std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data);
            virtual std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data);
        };

        class NodeDBvrevHandler : public TreeNodeHandler  // DBvrev - версия (ревизия) формата двоичного исходника базы данных.
        {
        public:
            NodeDBvrevHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            virtual std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data);
            virtual std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data);
        };

        class NodeDBunitHandler : public TreeNodeHandler  // DBunit - единица измерений координат, используемая в файле.
        {
        public:
            NodeDBunitHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            virtual std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data);
            virtual std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data);
        };

        class NodeLyrstrHandler : public TreeNodeHandler // Lyrstr - таблица доступных слоёв - список слоёв, используемых компонентом.
        {
        public:
            NodeLyrstrHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            virtual std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data);
            virtual std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data);
        };

        class NodeLyrphidHandler : public TreeNodeHandler // Lyrphid - определение спаривания слоёв и их доступности для трассировкию
        {
        public:
            NodeLyrphidHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            virtual std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data);
            virtual std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data);
        };

        class NodeSsymtblHandler : public TreeNodeHandler  // Ssymtbl - задание свойств контактов, которые служат как переходные отверстия между различными слоями.
        {
        public:
            NodeSsymtblHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            virtual std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data);
            virtual std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data);
        };

        class NodePolyapGlbHandler : public TreeNodeHandler // Polyap - глобальная ширина апертуры для рисования границ полигона.
        {
        public:
            NodePolyapGlbHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            virtual std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data);
            virtual std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data);
        };

        class NodePolyapLocHandler : public TreeNodeHandler // Polyap - локальная ширина апертуры для рисования границ полигона.
        {
        public:
            NodePolyapLocHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            virtual std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data);
            virtual std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data);
        };

        class NodeAnnotatePolyHandler : public TreeNodeHandler       // Poly - команда рисования свободного составного полигона.
        {
        public:
            NodeAnnotatePolyHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            virtual std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data);
            virtual std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data);
        };

        class NodePicPolyHandler : public TreeNodeHandler       // Poly - команда рисования привязанного составного полигона.
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

            NodePicPolyHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            virtual std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data);
            virtual std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data);
        };

        // Классы-обработчики вложенных служебных команд общего описания многоугольника (полигона).

        class NodePolyOlHandler : public TreeNodeHandler       // Ol - описание внешнего контура (габарита) полигона.
        {
        public:
            NodePolyOlHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            virtual std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data);
            virtual std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data);
        };

        class NodePolyPvHandler : public TreeNodeHandler       // Pv - многоугольная пустота полигона.
        {
        public:
            NodePolyPvHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            virtual std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data);
            virtual std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data);
        };

        class NodePolyCvHandler : public TreeNodeHandler       // Cv - описание круглой выемки полигона.
        {
        public:
            NodePolyCvHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            virtual std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data);
            virtual std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data);
        };

        // ------------------------------------

        class NodeDBtypeHandler : public TreeNodeHandler      // DBtype - тип базы данных - схемный ("Schematic") или конструктивный ("PC-Board") элемент.
        {
        public:
            NodeDBtypeHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            virtual std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data);
            virtual std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data);
        };

        // Классы-обработчики узлов, соответствующим примитивным графическим объектам.

        class NodeAnnotateAHandler : public TreeNodeHandler         // A - Создание свободной (отъединённой) A-дуги.
        {
        public:
            NodeAnnotateAHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            virtual std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data);
            virtual std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data);
        };

        class NodePicAHandler : public TreeNodeHandler           // A - Создание привязанной A-дуги.
        {
        public:
            NodePicAHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            virtual std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data);
            virtual std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data);
        };

        class NodeAnnotateArcHandler : public TreeNodeHandler       // Arc - Создание свободной (отъединённой) ARC-дуги.
        {
        public:
            NodeAnnotateArcHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            virtual std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data);
            virtual std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data);
        };

        class NodePicArcHandler : public TreeNodeHandler         // Arc - Создание привязанной ARC-дуги.
        {
        public:
            NodePicArcHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            virtual std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data);
            virtual std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data);
        };

        class NodeAnnotateCircleHandler : public TreeNodeHandler    // C - Создание свободной (отъединённой) окружности.
        {
        public:
            NodeAnnotateCircleHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            virtual std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data);
            virtual std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data);
        };

        class NodePicCircleHandler : public TreeNodeHandler      // C - Создание привязанной окружности.
        {
        public:
            NodePicCircleHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            virtual std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data);
            virtual std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data);
        };

        class NodeAnnotateLineHandler : public TreeNodeHandler    // L - Создание свободного (отъединённого) прямолинейного отрезка.
        {
        public:
            NodeAnnotateLineHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            virtual std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data);
            virtual std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data);
        };

        class NodePicLineHandler : public TreeNodeHandler      // L - Создание привязанного линейного отрезка.
        {
        public:
            NodePicLineHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            virtual std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data);
            virtual std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data);
        };

        class NodeAnnotateRectangleHandler : public TreeNodeHandler            // R - свободный прямоугольник.
        {
        public:
            NodeAnnotateRectangleHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            virtual std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data);
            virtual std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data);
        };

        class NodePicRectangleHandler : public TreeNodeHandler             // R - привязанный прямоугольник.
        {
        public:
            NodePicRectangleHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            virtual std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data);
            virtual std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data);
        };

        class NodeAnnotateTextHandler : public TreeNodeHandler            // T - свободная текстовая строка.
        {
        public:
            NodeAnnotateTextHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            virtual std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data);
            virtual std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data);
        };

        class NodePicTextHandler : public TreeNodeHandler             // T - привязанная текстовая строка.
        {
        public:
            NodePicTextHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            virtual std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data);
            virtual std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data);
        };

        class NodeAnnotateFillRectHandler : public TreeNodeHandler            // Fr - свободный заполненный прямоугольник.
        {
        public:
            NodeAnnotateFillRectHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            virtual std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data);
            virtual std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data);
        };

        class NodePicFillRectHandler : public TreeNodeHandler            // Fr - привязанный заполненный прямоугольник.
        {
        public:
            NodePicFillRectHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            virtual std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data);
            virtual std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data);
        };

        // ------------------------------------

        class NodeAtHandler : public TreeNodeHandler        // At - текстовый атрибут пользователя.
        {
        public:
            NodeAtHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            virtual std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data);
            virtual std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data);
        };

        class NodeLyHandler : public TreeNodeHandler        // Ly - выбор слоя размещения примитивов.
        {
        public:
            NodeLyHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            virtual std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data);
            virtual std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data);
        };

        class NodeLsHandler : public TreeNodeHandler        // Ls - назначение типа линии.
        {
        public:
            NodeLsHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            virtual std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data);
            virtual std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data);
        };

        class NodeWdHandler : public TreeNodeHandler        // Wd - указание толщины линии.
        {
        public:
            NodeWdHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            virtual std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data);
            virtual std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data);
        };

        // Группа объектов-обработчиков команд настройки параметров текстового примитива.
        class NodeTsHandler : public TreeNodeHandler        // Ts - высота текста (высота глифов используемого шрифта)
        {
        public:
            NodeTsHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            virtual std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data);
            virtual std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data);
        };

        class NodeTjHandler : public TreeNodeHandler        // Tj - выравнивание текста.
        {
        public:
            NodeTjHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            virtual std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data);
            virtual std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data);
        };

        class NodeTrHandler : public TreeNodeHandler        // Tr - вращение (угол поворота) текста.
        {
        public:
            NodeTrHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            virtual std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data);
            virtual std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data);
        };

        class NodeTmHandler : public TreeNodeHandler        // Tm - зеркальность текста.
        {
        public:
            NodeTmHandler(PDIFFileWorkshop* p_workshop = nullptr) : TreeNodeHandler(p_workshop)
            {}
            virtual std::optional<ErrorInfo> HandleOpenNode(TreeNodeData* node_data);
            virtual std::optional<ErrorInfo> HandleCloseNode(TreeNodeData* node_data);
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

        struct RadioComponentDesc
        {
            std::string comp_name; // Имя описываемого компонента (радиодетали).

        };

        struct RadioComponentInsertion
        {
            std::string comp_name;  // Имя вставленного радиокомпонента.

        };

        // Словарь преобразования ключевого слова в соответствующую ему номерную константу перечисления PDIFKeywords.
        static const std::unordered_map<std::string_view, PDIFKeywords> keyword_to_enum_value;
        // Словарь, отражающий константы ключевых слов в описания кодируемых им узлов дерева PDIF-базы данных.
        static const std::unordered_multimap<PDIFKeywords, TreeNodeDesc> keyword_to_node_desc;

        // Экземпляры классов-обработчиков узловых команд узлов PDIF-дерева.

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
        NodeAnnotatePolyHandler node_poly_annot_hanlder;      // Poly - команда рисования свободного составного полигона.
        NodePicPolyHandler node_poly_pic_hanlder;             // Poly - команда рисования связанного составного полигона.
        NodeDBtypeHandler node_dbtype_handler;                // DBtype - тип базы данных - схемный ("Schematic") или конструктивный ("PC-Board") элемент.
        // Конструкторы примитивных (простейших) графических объектов.
        NodeAnnotateAHandler node_a_annot_handler;            // A - Создание свободной (отъединённой) A-дуги.
        NodePicAHandler node_a_pic_handler;                   // A - Создание привязанной A-дуги.
        NodeAnnotateArcHandler node_arc_annot_handler;        // Arc - Создание свободной (отъединённой) ARC-дуги.
        NodePicArcHandler node_arc_pic_handler;               // Arc - Создание привязанной ARC-дуги.
        NodeAnnotateCircleHandler node_circle_annot_handler;  // C - Создание свободной (отъединённой) окружности.
        NodePicCircleHandler node_circle_pic_handler;         // C - Создание привязанной окружности.
        NodeAnnotateLineHandler node_line_annot_handler;      // L - Создание свободного (отъединённого) прямолинейного отрезка.
        NodePicLineHandler  node_line_pic_handler;            // L - Создание привязанного линейного отрезка.
        NodeAnnotateRectangleHandler node_rect_annot_handler; // R - свободный прямоугольник.
        NodePicRectangleHandler node_rect_pic_handler;        // R - привязанный прямоугольник.
        NodeAnnotateTextHandler node_text_annot_handler;      // T - свободная текстовая строка.
        NodePicTextHandler node_text_pic_handler;             // T - привязанная текстовая строка.
        NodeAnnotateFillRectHandler node_fill_rect_annot_handler; // Fr - свободный заполненный прямоугольник.
        NodePicFillRectHandler node_fill_rect_pic_handler;        // Fr - привязанный заполненный прямоугольник.
        //
        NodeAtHandler node_at_handler;                        // At - текстовый атрибут пользователя.
        NodePolyOlHandler node_poly_ol_handler;               // Ol - описание внешнего контура (габарита) полигона.
        NodePolyPvHandler node_poly_pv_hanlder;               // Pv - многоугольная пустота полигона.
        NodePolyCvHandler node_poly_cv_handler;               // Cv - описание круглой выемки полигона.
        // Обработчики установочных команд (команд настройки атрибутов рисования).
        NodeLyHandler node_layer_select_handler;              // Ly - выбор слоя размещения примитивов.
        NodeLsHandler node_line_type_handler;                 // Ls - назначение типа линии.
        NodeWdHandler node_line_width_handler;                // Wd - указание толщины линии.
        // Группа объектов-обработчиков команд настройки параметров текстового примитива.
        NodeTsHandler node_text_height_handler;               // Ts - высота текста (высота глифов используемого шрифта).
        NodeTjHandler node_text_justify_hanlder;              // Tj - выравнивание текста.
        NodeTrHandler node_text_rotation_handler;             // Tr - вращение (угол поворота) текста.
        NodeTmHandler node_text_mirror_handler;               // Tm - зеркальность текста.

        // Словарь, содержащий указатели на объекты-обработчики различных узлов дерева PDIF-базы (точнее,
        // объекты-исполнители соответствующих им команд).
        const std::unordered_map<PDIFOps, TreeNodeHandler*> opcode_to_handler
        {
            {PDIFOps::PDIFOP_MODE, &node_mode_handler},
            {PDIFOps::PDIFOP_VW, &node_vw_handler},
            {PDIFOps::PDIFOP_PDIF_VREV, &node_pdif_vrev_handler},
            {PDIFOps::PDIFOP_DB_VREV, &node_db_vrev_handler},
            {PDIFOps::PDIFOP_DB_UNIT, &node_db_unit_handler},
            {PDIFOps::PDIFOP_LYRSTR, &node_lyrstr_handler},
            {PDIFOps::PDIFOP_LYRPHID, &node_lyrphid_handler},
            {PDIFOps::PDIFOP_SSYMTBL, &node_ssymtbl_handler},
            {PDIFOps::PDIFOP_POLYAP_GLB, &node_polyap_glb_handler},
            {PDIFOps::PDIFOP_POLYAP_LOC, &node_polyap_loc_hanlder},
            {PDIFOps::PDIFOP_DBTYPE, &node_dbtype_handler},
            // Команды создания отдельных примитивных графических элементов.
            {PDIFOps::PDIFOP_ANNOTATE_POLY, &node_poly_annot_hanlder},
            {PDIFOps::PDIFOP_PIC_POLY, &node_poly_pic_hanlder},
            {PDIFOps::PDIFOP_ANNOTATE_A_ARC, &node_a_annot_handler},
            {PDIFOps::PDIFOP_PIC_A_ARC, &node_a_pic_handler},
            {PDIFOps::PDIFOP_ANNOTATE_ARC_ARC, &node_arc_annot_handler},
            {PDIFOps::PDIFOP_PIC_ARC_ARC, &node_arc_pic_handler},
            {PDIFOps::PDIFOP_ANNOTATE_CIRCLE, &node_circle_annot_handler},
            {PDIFOps::PDIFOP_PIC_CIRCLE, &node_circle_pic_handler},
            {PDIFOps::PDIFOP_ANNOTATE_LINE, &node_line_annot_handler},
            {PDIFOps::PDIFOP_PIC_LINE, &node_line_pic_handler},
            {PDIFOps::PDIFOP_ANNOTATE_RECT, &node_rect_annot_handler},
            {PDIFOps::PDIFOP_PIC_RECT, &node_rect_pic_handler},
            {PDIFOps::PDIFOP_ANNOTATE_TEXT, &node_text_annot_handler},
            {PDIFOps::PDIFOP_PIC_TEXT, &node_text_pic_handler},
            {PDIFOps::PDIFOP_ANNOTATE_FILL_RECT, &node_fill_rect_annot_handler},
            {PDIFOps::PDIFOP_PIC_FILL_RECT, &node_fill_rect_pic_handler},
            // Обработчики описания компонентов многоугольника.
            {PDIFOps::PDIFOP_POLY_OL, &node_poly_ol_handler},
            {PDIFOps::PDIFOP_POLY_PV, &node_poly_pv_hanlder},
            {PDIFOps::PDIFOP_POLY_CV, &node_poly_cv_handler},
            //
            {PDIFOps::PDIFOP_EX_ATTR, &node_at_handler},
            // Пары соответствия для назначения обработчиков настроечным командам PDIF-базы.
            {PDIFOps::PDIFOP_SET_LY, &node_layer_select_handler},
            {PDIFOps::PDIFOP_SET_LS, &node_line_type_handler},
            {PDIFOps::PDIFOP_SET_WD, &node_line_width_handler},
            // Обработчики для команд настройки параметров текстового примитива.
            {PDIFOps::PDIFOP_SET_TS, &node_text_height_handler},
            {PDIFOps::PDIFOP_SET_TJ, &node_text_justify_hanlder},
            {PDIFOps::PDIFOP_SET_TR, &node_text_rotation_handler},
            {PDIFOps::PDIFOP_SET_TM, &node_text_mirror_handler}
        };

        // Ниже находятся динамические поля класса PDIFFileWorkshop, изменяющиеся в процессе загрузки PDIF-базы и
        // отражающие текущее состояние загрузчика.
        // Далее следует список массивов, накапливающих информацию, полученную из PDIF-потока в процессе его обработки.
        std::vector<TreeNodeData> node_stack;   // Текущая ветвь дерева PDIF-документа, анализируемая в данный момент.
        std::vector<GraphObj*> graph_objects;   // Массив графических элементов файла.
        std::vector<LayerDesc> layers;          // Массив описателей слоёв изображения.
        std::vector<CrossLayerPinhole> cross_layers_pinholes;   // Вектор описателей межслойных переходных отверстий.
        std::vector<RadioComponentDesc> radio_components;       // Массив описателей используемых в проекте компонентов.
        // Массив с информацией о вставках (применениях) компонент из списка radio_components в описанной в PDIF-файле
        // электронной схеме устройства.
        std::vector<RadioComponentInsertion> radio_comp_inserts;
        // Прочие поля, подлежащие заполнению при загрузке PDIF-файла.
        FileDefValues file_values;

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
        wxPoint ConvPntToLog(int pict_x, int pict_y) const;
        wxPoint ConvPntToLog(wxPoint pict) const;
        wxRect ConvRectToLog(int pict_ldx, int pict_ldy, int pict_rux, int pict_ruy) const;
        wxRect ConvRectToLog(wxRect pict_rect) const;
        bool IsPDIFPathCorrespond(const TreePathType& concrete_path, const TreePathType& pattern_path) const;
        int FindLayerByName(const std::string& find_layer_name) const;
        bool IsPrevNodeContainer(TreeNodeData* node_data) const;
    };
} // namespace HandlerPDIF
