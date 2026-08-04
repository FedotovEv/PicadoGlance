#ifndef HEADER_F7C55BF1835B913E
#define HEADER_F7C55BF1835B913E

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <variant>
#include <any>

#include "macroses.h"
#include "PCADViewDraw.h"
#include "ApertureProvider.h"

// Описание единичного "внешнего" текстового атрибута (командный узел "At").
struct TextExtAttr
{
    std::string key;        // Ключ (имя) атрибута.
    std::string value;      // Значение (содержимое) атрибута.
    wxPoint attr_coords;    // Точка его привязки (координаты якоря).
};

// Тип, содержащий набор "внешних" атрибутов, привязанных к некоторой сущности PCAD-документа.
using ExAttrsCollection = std::unordered_map<std::string, TextExtAttr>;

enum LayerAttributesValues
{
    LAYER_ON_AVL_ACT_OFF = 0,   // Слой выключен (невидим)
    LAYER_ON_AVL_ACT_ON = 1,    // Слой включён (отображается)
    LAYER_ON_AVL_ACT_AVL = 2,   // Слой активен (доступен для редактирования)
    LAYER_ON_AVL_ACT_ACT = 3    // Рабочий слой (выбран для редактирования)
};

enum LayerTraceableValues
{
    LAYER_NOT_TRACEABLE = 0,    // Трассировка на слое не выполняется.
    LAYER_IS_TRACEABLE = 1      // Слой доступен для трассировки.
};

struct LayerAttributes
{
    unsigned layer_on_avl_act : 2;
    unsigned layer_is_traceable : 1;
    operator int()
    {
        return (int)layer_on_avl_act + ((int)layer_is_traceable << 2);
    }
};

struct LayerDesc : public ExAttrsCollection
{
    static const LayerDesc LAYER_DESC_INVALID;

    std::string layer_name;     // Имя (название) слоя.
    int layer_number;           // Номер слоя в списке VLYR (отсчитывается от нуля).
    int layer_color = 0;        // Индекс в палитре, которым должна выводиться графика слоя.
    wxColor layer_wx_color;     // Прямой цвет, который будет применяться для рисования графики слоя.
    int layer_pair_number = -1; // Номер слоя в списке VLYR (отсчитывается от нуля), парного данному.
                                // Равен -1, если слоя, парного этому, не существует.
    // Флаги включения, активности и трассировочности (доступности для трассирования) слоя.
    mutable LayerAttributes layer_attributes = {LAYER_ON_AVL_ACT_OFF, LAYER_NOT_TRACEABLE};
};

// Структура-описатель межслойного переходного отверстия.
enum class CrossLayerHoleType
{
    PINHOLE_UNKNOWN = 0,
    PINHOLE_THROUGH,        // Сквозное.
    PINHOLE_INTERLAYER      // Межслойное.
};

struct CrossLayerPinhole : public ExAttrsCollection
{
    int pin_number = -1;        // Номер переходного отверстия (от 0 до 255), описанного в данной структуре.
    CrossLayerHoleType pin_type = CrossLayerHoleType::PINHOLE_UNKNOWN; // Тип переходного отверстия.
    int layer_from = -1;        // Номера (индексы, базируются к нулю) слоёв, который соединяет межслойное переходное отверстие.
    int layer_to = -1;          // Для сквозного оба поля устанавливаются в -1.
};

enum FileFlags
{
    FILE_FLAG_UNIT_INCHES = 1,      // Единица измерения в файле - дюймы, иначе - миллиметры.
                                    // Этот флаг имеет смысл в сочетании с полем DBU_in_measure_unit
                                    // структуры FileDefValues, указывающим плотность DBU в файле, т. е.
                                    // количество DBU на дюйм или миллиметр.
    FILE_FLAG_EDITOR_PCCAPS = 2,    // Файл создан в редакторе PCCAPS, иначе - в PCCARDS или PCPLACE.
    FILE_FLAG_DETL = 4              // Файл описывает схему или плату целиком (режим DETL), в противном
                                    // случае он содержит описание отдельного компонента (режим SYMB).
};

class GraphObj;
class FileWorkshop;

struct FileDefValues
{
    wxString file_signature; // Строка, кратко описывающая тип и содержание файла.
    wxString database_name;  // Строка, содержащая внутреннее имя загруженной базы данных (если оно доступно из её внутреннего содержимого).
    const FileWorkshop* file_workshop = nullptr; // Указатель на "цех файловой фабрики",
                                                 // открывший данный файл
    wxRect frame_rect;  // Предельные логические координаты загруженного
                        // изображения (прямоугольные границы картинки,
                        // её размер находится в полях width и height).
    uint32_t file_flags = 0;    // Поле битовых флагов, опеределяющих отдельные
                                // бинарные признаки изображения из группы FileFlags.
    int DBU_in_measure_unit = 100; // Количество координатных единиц DBU в основной
                                   // измерительной единице изображения (миллиметры
                                   // или дюймы, что определяется флагом FILE_FLAG_INCHES).
    std::filesystem::path picture_filepath; // Путь к файлу загруженной картинки.

    // Описание пользовательских окон, текущего (используемого в момент создания загруженного файла) и сохранённых.
    wxPoint current_window_center = {0, 0}; // Центр текущего окна.
    double current_window_scale = 1.0;      // Его масштаб.
    std::vector<wxRect> user_windows;       // Окна просмотра, сохранённые пользователем в файле.
    //---------------
    int polyap = 0;                 // Ширина апертуры, которой по умолчанию (при отсутствии локального определения)
                                    // рисуется габарит полигона (его внешний контур).

    void Clear() noexcept
    {
        (*this) = {};
    }
};

struct PinLabelDef
{ // Описание текстовой подписи вывода - текстовой метки, содержащей, как правило, его номер.
    wxPoint pin_name_coords;                    // Якорная точка размещения текстовой подписи.
    int layer_number = -1;                      // Номер слоя в списке VLYR (отсчитывается от нуля), на котором размещена текстовая метка вывода.
    int text_height = 1;                        // Высота текста (высота составляющих его глифов в DBU).
    TextOrientation text_orient = TextOrientation::TEXT_LEFT_RIGHT; // Ориентация (поворот) и зеркальность текста.
    TextAlign text_align = TextAlign::TEXT_LEFT_UP;                 // Выравнивание текста относительно его якорной точки.

    void Clear() noexcept
    {
        (*this) = {};
    }
};

// Тип вывода (для УГО символа) или тип контакта (для ножки прибора) может быть как строкой (в первом случае), так и числом (во втором случае).
using PinType = std::variant<int, std::string>;

struct ComponentPinDef : public ExAttrsCollection
{  // Описание отдельного вывода некоторого радиоэлемента (секция PIN_DEF PDIF-документа).
    std::string pin_al_number;   // "Алфавитно-цифровой номер" ножки прибора (задается только для конструктива).
    std::string pin_name;        // Имя вывода, задаётся как для УГО (имя вывода логического вентиля), так и для
                                 // физического устройства (конструктива).
    int layer_number = -1;       // Номер слоя в списке VLYR (отсчитывается от нуля), на котором размещено условное изображение вывода.
    // pin_type - тип вывода. Для режима УГО - текстовая строка - один из вариантов "INPUT", "OUTPUT", "I/O", "OC", "OE",
    // "TRI", "AN",  "7", "8", "9", "10", "11", "12", "13", "14", "15", и. т. д.
    // Для конструктива элемента тип ножки (целое число) от 1 до 50 или от 101 до 255.
    PinType pin_type = 0;
    int equive_code = 0;        // Код эквивалентности вывода. Не имеющие эквивалентов содержат в этом поле 0.
    // Точка размещения вывода (в DBU базы данных). Здесь эти координаты относительные и "справочные" - заданы относительно реперной
    // точки компонента и не учитывают .
    wxPoint pin_coords;
    PinLabelDef pin_label;      // Сведения о текстовой метке, сопровождающей данный вывод.

    void Clear() noexcept
    {
        (*this) = {};
    }
};

// Словарь, отражающий логическое имя вывода на "алфавитно-цифровой номер" ножки некоторой секции физического прибора, которая ему
// соответствует "в железе". Ключ - имя вывода логического вентиля (pin_name), значение - "алфавитно-цифровой номер" ножки (pin_al_number).
using PinNameToALNumber = std::unordered_map<std::string, std::string>;
// Словарь, ключ которого есть имя какой-либо секции (как правило, из диапазона A-Z). Значение - упаковочные данные этой секции.
using SectNameToPackInfo = std::unordered_map<std::string, PinNameToALNumber>;

struct ComponentPKGSectDef
{ // Структура описания секционирования радиокомпонента при описании его УГО.
    // Словарь, объявленный ниже, содержит в качестве ключа имя секции (как правило, однобуквенное в диапазоне A-Z), а в качестве
    // значения - подсловарь (ассоциативный массив "нижнего уровня") преобразования логических имён выводов вентиля (его УГО) в
    // соответствующие им "алфавитно-цифровые номера" ножек физического прибора.
    SectNameToPackInfo pin_pkg_data;

    void Clear() noexcept
    {
        (*this) = {};
    }
};

struct ComponentSPKGSectDef
{ // Структура описания секционирования радиокомпонента для его конструктива (сведения о секциях в составе реального прибора).
    // Реальный прибор может быть неоднородным (гетерогенным) и иметь конструктивно различные секции. Каждый такой тип (группа)
    // секций описывается одним элементом нижеследующего вектора.
    // ------------------
    // Массив sect_spkg_data содержит упаковочные данные различных секций физического устройства. Каждый отдельный его элемент
    // соответствует группе однородных (схемотехнически и конструктивно) одинаковых секций. Элемент - словарь, ключ которого
    // суть имя какой-либо секции (как правило, из диапазона A-Z), принадлежащей к данной однородной группе. А значение - упаковочные
    // данные этой секции - подсловарь типа PinNameToALNumber. Для него, в свою очередь, ключ - логическое имя вывода, а значение -
    // - "алфавитно-цифровой номер" соответствющей ножки конструктива для указанной секции.
    std::vector<SectNameToPackInfo> sect_spkg_data;

    void Clear() noexcept
    {
        (*this) = {};
    }
};

using ComponentSectDefVar = std::variant<ComponentPKGSectDef, ComponentSPKGSectDef>;

struct ComponentSectDef : public ComponentSectDefVar
{
    void Clear() noexcept;
};

struct RefDesDef
{ // Структура описания свойств "конструкторского обозначения" (порядкового идентификатора) радиокомпонента.
    wxPoint pos;                         // Относительное (заданное относительно опорной точки радиоэлемента) "справочное" положение
                                         // конструкторского обозначения данного радиоэлемента. Описание же конкретного текстового
                                         // элемента с текстом конструкторского обозначения (его содержимого и правильных координат)
                                         // задаётся отдельно для каждой его вставленной копии в их узлах Rd.
    int layer_number = -1;                      // Номер слоя в списке VLYR (отсчитывается от нуля), на котором размещена текстовая метка вывода.
    int text_height = 1;                        // Высота текста (высота составляющих его глифов в DBU).
    TextOrientation text_orient = TextOrientation::TEXT_LEFT_RIGHT; // Ориентация (поворот) и зеркальность текста.
    TextAlign text_align = TextAlign::TEXT_LEFT_UP;                 // Выравнивание текста относительно его якорной точки.

    void Clear() noexcept              // Функция-член общей очистки данных этой структуры.
    {
        (*this) = {};
    }
};

class RadioComponentDesc : public ExAttrsCollection
{  // Класс, хранящий описание отдельного радиоэлемента, встроенного в базу данных PCAD-документа.
private:
    std::string comp_name_;                     // Имя описываемого компонента (радиодетали).
    std::vector<GraphObj*> graph_objects_;      // Массив графических элементов очертания радиоэлемента.
    std::vector<ComponentPinDef> pins_;         // Массив определений выводов данного радиоэлемента.
    // Элементы упаковочной информации библиотечного радиоэлемента.
    std::string package_id_;                    // Упаковочный идентификатор - имя файла конструктива для данного УГО. Определяется только
                                                // для УГО элемента (определение содержится в секции PKG) и связывает УГО с соответствующим
                                                // ему конструктивом радиокомпонента.
    RefDesDef refdes_;                          // Определитель "справочных" свойств конструкторского обозначения компонента (при вставке
                                                // его экземпляра они могут измениться).
    ComponentSectDef sections_def_;             // Данные о секционном составе радиокомпонента.
    // --------- Параметры прибора, назначаемые его внутренними атрибутами (в подсекции ATR/IN).
    wxPoint org_pos_;                           // Якорная точка привязки библиотечного элемента (назначается только для основного символа базы).
    int ty_id_ = 0;                             // Идентификационный код типа компонента (ID).
    bool is_smd_ = false;                       // Признак планарного радиокомпонента.
    bool is_jumper_ = false;                    // Признак радиокомпонента-перемычки.

public:
    struct SourceData
    { // Структура с полным комплектом данных для конструирования объекта.
        std::string comp_name;                  // Имя описываемого компонента (радиодетали).

        std::vector<GraphObj*> graph_objects;   // Массив графических элементов очертания радиоэлемента.
        std::vector<ComponentPinDef> pins;      // Массив определений выводов данного радиоэлемента.
        std::string package_id;                 // Упаковочный идентификатор - имя файла конструктива для данного УГО.
        RefDesDef refdes;                       // Определитель "справочных" свойств конструкторского обозначения компонента.
        ComponentSectDef sections_def;          // Данные о секционном составе радиокомпонента.
        // --------- Параметры прибора, назначаемые его внутренними атрибутами (в подсекции ATR/IN).
        wxPoint org_pos;                        // Якорная точка привязки библиотечного элемента (назначается только для основного символа базы).
        int ty_id = 0;                          // Идентификационный код типа компонента (ID).
        bool is_smd = false;                    // Признак планарного радиокомпонента.
        bool is_jumper = false;                 // Признак радиокомпонента-перемычки.
        // ---------
        ExAttrsCollection ex_attr_collection;   // Промежуточное хранилище набора "внешних" атрибутов радиокомпонента.
    };

    RadioComponentDesc() = default;
    RadioComponentDesc(SourceData&& radio_component_data);
    RadioComponentDesc(const RadioComponentDesc& other) = delete;
    RadioComponentDesc(RadioComponentDesc&& other) noexcept;
    ~RadioComponentDesc() noexcept;

    RadioComponentDesc& operator=(const RadioComponentDesc& other) = delete;
    RadioComponentDesc& operator=(RadioComponentDesc&& other) noexcept;

    void Clear() noexcept;              // Функция-член общей очистки данных этой структуры.

    const std::string GetName() const   // Извлечение имени радиокомпонента.
    {
        return comp_name_;
    }

    // Получение имени файла, содержащего конструктив радиокомпонента для данного УГО (хранящегося в этом PCAD-документе).
    const std::string GetPackageID() const
    {
        return package_id_;
    }

    int GetTyID() const // Получение идентификационного кода типа данного компонента.
    {
        return ty_id_;
    }

    // Функции-члены обзора списка графических элементов радиокомпонента.
    size_t size() const
    {
        return graph_objects_.size();
    }

    decltype(graph_objects_)::const_iterator begin() const
    {
        return graph_objects_.cbegin();
    }

    decltype(graph_objects_)::const_iterator end() const
    {
        return graph_objects_.cend();
    }

    decltype(graph_objects_)::const_reverse_iterator rbegin() const
    {
        return graph_objects_.crbegin();
    }

    decltype(graph_objects_)::const_reverse_iterator rend() const
    {
        return graph_objects_.crend();
    }

    // Функции-члены обслуживания списка выводов радиокомпонента.
    size_t pins_size() const
    {
        return pins_.size();
    }

    decltype(pins_)::const_iterator pins_begin() const
    {
        return pins_.cbegin();
    }

    decltype(pins_)::const_iterator pins_end() const
    {
        return pins_.cend();
    }

    decltype(pins_)::const_reverse_iterator pins_rbegin() const
    {
        return pins_.crbegin();
    }

    decltype(pins_)::const_reverse_iterator pins_rend() const
    {
        return pins_.crend();
    }

    // Некоторые поисковые запросы для выбора графических элементов, очерчивающих контуры радиокомпонента.
    const GraphObj* ScanForGraphObject(uint32_t graph_object_ordinal) const;
    const GraphObj* ScanForGraphObject(wxColor graph_obj_ord_as_color) const;
};

struct RadioComponentInsertion : public ExAttrsCollection
{ // Структура, описывающая единичную вставку некоторого встроенного радиоэлемента
public:
    struct SourceData
    { // Структура источника конструирования, то есть структура, содержащая все данные, необходимые для
      // создания полнофункционального объекта RadioComponentInsertion.
        std::string comp_name;              // Имя библиотечного образца вставленного радиокомпонента.
        std::string insertion_name;         // Собственное имя вставленной копии.
        GraphObj* refdes_obj = nullptr;     // Конструкторское (индивидуальное порядковое) обозначение вставленной копии радиокомпонента.
                                            // Хранится в виде графического объекта класса ObjText.
        std::vector<GraphObj*> pin_labels;  // Набор текстовых графических элементов (типа ObjText), соответствующих подписям выводов
                                            // копии вставленного радиокомпонента.
        // Данные о подсоединении выводов компонента (первый член каждой пары) к токопроводящим цепям схемы или платы
        // (второй член каждой пары).
        std::vector<std::pair<std::string, std::string>> connect_info;
        // Данные о переназначении типов ножек (контактов) для данного экземпляра компонента (IPT-данные).
        // При отсутствии такового переназначения этот массив пуст. Иначе первый член каждой пары - имя ножки, второй - вновь назначенный ей тип.
        std::vector<std::pair<std::string, PinType>> pin_type_info;
        // --------- Параметры вставки, назначаемые её внутренними атрибутами (в подсекции ATR/IN).
        bool is_mirror = false;                 // Признак зеркальности вставленного экземпляра.
        bool on_top_side = true;                // Признак размещения экземпляра на верхней стороне печатной платы (на стороне деталей).
                                                // Если данный признак == "ЛОЖЬ", экземпляр компонента устанавливается на её нижнюю сторону
                                                // (сторону проводников).
        bool is_user_ins_name = false;          // Флаг присвоения экземпляру компонента пользовательского имени insertion_name. Если
                                                // флаг == "ЛОЖЬ", в данный момент это имя автоматически присвоенное.
        wxPoint place_pos;                      // Точка местоположения данной вставки (координаты точки вставки данного экземпляра компонента
                                                // в схему или на плату).
        double scale_x = 1.0;                   // Горизонтальный масштаб вставленной копии (масштаб по оси x).
        double scale_y = 1.0;                   // Вертикальный масштаб вставленной копии (масштаб по оси y).
        int rotate_factor = 0;                  // Фактор нормального поворота вставленной копии компонента (угол, соответствующий этому
                                                // фактору == 90 * rotate_factor градусов по часовой стрелке).
        std::optional<wxPoint> ins_name_pos;    // Положение якорной точки частного имени данной копии компонента (хранится в insertion_name),
                                                // если оно видимо.
        double set_angle = 0.0;                 // Угол установки копии компонента в целых градусах в диапазоне от 0 до 89 градусов.
        // ---------
        ExAttrsCollection ex_attr_collection;   // Промежуточное хранилище набора "внешних" атрибутов операции вставки.
    };

    RadioComponentInsertion() = default;
    RadioComponentInsertion(SourceData&& component_insert_data);
    RadioComponentInsertion(const RadioComponentInsertion& other) = delete;
    RadioComponentInsertion(RadioComponentInsertion&& other) noexcept;
    ~RadioComponentInsertion() noexcept;

    RadioComponentInsertion& operator=(const RadioComponentInsertion& other) = delete;
    RadioComponentInsertion& operator=(RadioComponentInsertion&& other) noexcept;

    void Clear() noexcept ; // Функция-член общей очистки данных этой структуры.

private:
    struct PinNetConnectInfo
    {
        std::string pin_name;
        int pin_index = -1;
        std::string net_name;
        int net_index = -1;
    };

    std::string comp_name_;              // Имя библиотечного образца вставленного радиокомпонента.
    std::string insertion_name_;         // Собственное имя вставленной копии.
    int comp_number_ = -1;               // Номер (индекс, отсчитываемый от нуля) вставленного радиоэлемента, к которому
                                         // относится данная запись. Значение -1 указывает на фиктивную вставку.
    GraphObj* refdes_obj_ = nullptr;     // Конструкторское (индивидуальное порядковое) обозначение вставленной копии радиокомпонента.
                                         // Хранится в виде графического объекта класса ObjText.
    std::vector<GraphObj*> pin_labels_;  // Набор текстовых графических элементов (типа ObjText), соответствующих подписям выводов
                                         // копии вставленного радиокомпонента.
    // Данные о подсоединении выводов компонента к токопроводящим цепям схемы или платы.
    std::vector<PinNetConnectInfo> connect_info_;
    // Данные о переназначении типов ножек (контактов) для данного экземпляра компонента (IPT-данные).
    std::vector<std::pair<std::string, PinType>> pin_type_info_;
    // --------- Параметры вставки, назначаемые её внутренними атрибутами (в подсекции ATR/IN).
    bool is_mirror_ = false;                // Признак зеркальности вставленного экземпляра.
    bool on_top_side_ = true;               // Признак размещения экземпляра на верхней стороне печатной платы (на стороне деталей).
    bool is_user_ins_name_ = false;         // Флаг присвоения экземпляру компонента пользовательского имени.
    wxPoint place_pos_;                     // Точка местоположения данной вставки (координаты точки вставки данного экземпляра компонента
                                            // в схему или на плату).
    double scale_x_ = 1.0;                  // Горизонтальный масштаб вставленной копии (масштаб по оси x).
    double scale_y_ = 1.0;                  // Вертикальный масштаб вставленной копии (масштаб по оси y).
    int rotate_factor_ = 0;                 // Фактор нормального поворота вставленной копии компонента.

    std::optional<wxPoint> ins_name_pos_;   // Положение якорной точки частного имени данной копии компонента (хранится в insertion_name),
                                            // если оно видимо.
    double set_angle_ = 0.0;                // Угол установки копии компонента в целых градусах в диапазоне от 0 до 89 градусов.
                                            // При вычислении действительной ориентации копии комбинируется с rotate_factor_.
};

struct NetDefDesc : public ExAttrsCollection
{ // Структура описания некоторой проводящей цепи, существующей в схеме или на печатной плате.
private:
    std::string net_name_;                  // Имя описываемой проводящей цепи.
    std::vector<GraphObj*> net_parts_;      // Массив электропроводных элементов цепи. Среди них могут присутствовать точки
                                            // (переходные отверстия и межсоединения), ломаные многосегментные линии и полигоны.
    // --------- Параметры цепи, назначаемые её внутренними атрибутами (в подсекции ATR/IN).
    bool is_user_net_name_ = false;         // Флаг присвоения данной цепи пользовательского имени.

public:
    struct SourceData
    { // Структура с полным комплектом данных для конструирования объекта.
        std::string net_name;                   // Имя описываемого компонента (радиодетали).
        std::vector<GraphObj*> net_parts;       // Массив электропроводных элементов цепи.
        // --------- Параметры цепи, назначаемые её внутренними атрибутами (в подсекции ATR/IN).
        bool is_user_net_name = false;          // Флаг присвоения этой цепи некоторого пользовательского имени. Если флаг == "ЛОЖЬ",
                                                // в данный момент это имя сгенерировано автоматически.
        // ---------
        ExAttrsCollection ex_attr_collection;   // Промежуточное хранилище набора "внешних" атрибутов цепи.
    };

    NetDefDesc() = default;
    NetDefDesc(SourceData&& net_def_data);
    NetDefDesc(const NetDefDesc& other) = delete;
    NetDefDesc(NetDefDesc&& other) noexcept;
    ~NetDefDesc() noexcept;

    NetDefDesc& operator=(const NetDefDesc& other) = delete;
    NetDefDesc& operator=(NetDefDesc&& other) noexcept;

    void Clear() noexcept;     // Функция-член общей очистки данных этой структуры.

    // Функции-члены обзора списка элементов цепи.
    size_t size() const
    {
        return net_parts_.size();
    }

    decltype(net_parts_)::const_iterator begin() const
    {
        return net_parts_.cbegin();
    }

    decltype(net_parts_)::const_iterator end() const
    {
        return net_parts_.cend();
    }

    decltype(net_parts_)::const_reverse_iterator rbegin() const
    {
        return net_parts_.crbegin();
    }

    decltype(net_parts_)::const_reverse_iterator rend() const
    {
        return net_parts_.crend();
    }

    // Некоторые поисковые запросы для выбора требуемых звеньев данной цепи.
    const GraphObj* ScanForGraphObject(uint32_t graph_object_ordinal) const;
    const GraphObj* ScanForGraphObject(wxColor graph_obj_ord_as_color) const;
};

// Промежуточный тип для сбора информации в процессе загрузки файла и последующей передачи её в объект документа.
struct PCADFileSource
{
    std::vector<GraphObj*> graph_objects;                       // Массив графических элементов файла.
    std::vector<LayerDesc> layers;                              // Массив описателей слоёв изображения.
    std::vector<CrossLayerPinhole> cross_layers_pinholes;       // Массив со свойствами межслойных переходных отверстий.
    std::vector<RadioComponentDesc> radio_components;           // Массив с описаниями радиоэлементов, встроенных в данный файл.
    std::vector<RadioComponentInsertion> radio_comp_inserts;    // Массив описаний актов вставки радиоэлементов в схему файла.
    std::vector<NetDefDesc> nets;                               // Массив определений токопроводящих цепей схемы или платы.

    FileDefValues file_values;                                  // Общее описание характеристик файла.
};

class PCADFile
{
private:
    void PCADFileClear();
    void InitLayersColor();

    std::vector<GraphObj*> graph_objects_;                      // Массив графических элементов файла.
    std::vector<LayerDesc> layers_;                             // Массив описателей слоёв изображения.
    std::vector<CrossLayerPinhole> cross_layers_pinholes_;      // Массив со свойствами межслойных переходных отверстий.
    std::vector<RadioComponentDesc> radio_components_;          // Массив с описаниями радиоэлементов, встроенных в данный файл.
    std::vector<RadioComponentInsertion> radio_comp_inserts_;   // Массив описаний актов вставки радиоэлементов в схему файла.
    std::vector<NetDefDesc> nets_;                              // Массив определений токопроводящих цепей схемы или платы.
    FileDefValues file_values_;                                 // Общее описание характеристик файла.

    aperture::ApertureProvider& aperture_provider_;             // Ссылка на диспетчер апертур.

public:
    PCADFile(aperture::ApertureProvider& aperture_provider);
    PCADFile(const PCADFile& other) = delete;
    PCADFile(PCADFile&& other);
    // Упрощённый конструктор PCAD-документа, воспринимающий только графическую информацию загруженного файла.
    PCADFile(std::vector<GraphObj*> graph_objects, std::vector<LayerDesc> layers,
             aperture::ApertureProvider& aperture_provider, FileDefValues file_values);
    // Полноценный конструктор PCAD-документа, передающий ему всю существующую информацию, которая может быть
    // загружена из файла.
    PCADFile(PCADFileSource&& data_source, aperture::ApertureProvider& aperture_provider);
    ~PCADFile();

    PCADFile& operator=(const PCADFile& other) = delete;
    PCADFile& operator=(PCADFile&& other);

    void DrawFile(DrawContext& draw_context, const CanvasContext& canvas_context, SelectContourData& contour_data);
    void ShiftDocument(int shift_direction_x, int shift_direction_y);
    aperture::UsingApertureType GetApertureType() const
    {
        return aperture_provider_.GetApertureType();
    }

    void SetApertureType(aperture::UsingApertureType using_aperture_type) const;
    size_t size() const
    {
        return graph_objects_.size();
    }

    decltype(graph_objects_)::const_iterator begin() const
    {
        return graph_objects_.cbegin();
    }

    decltype(graph_objects_)::const_iterator end() const
    {
        return graph_objects_.cend();
    }

    decltype(graph_objects_)::const_reverse_iterator rbegin() const
    {
        return graph_objects_.crbegin();
    }

    decltype(graph_objects_)::const_reverse_iterator rend() const
    {
        return graph_objects_.crend();
    }

    size_t layers_size() const
    {
        return layers_.size();
    }

    decltype(layers_)::const_iterator layers_begin() const
    {
        return layers_.cbegin();
    }

    decltype(layers_)::const_iterator layers_end() const
    {
        return layers_.cend();
    }

    decltype(layers_)::const_reverse_iterator layers_rbegin() const
    {
        return layers_.crbegin();
    }

    decltype(layers_)::const_reverse_iterator layers_rend() const
    {
        return layers_.crend();
    }

    size_t flashes_size() const;
    std::vector<aperture::FlashDesc>::const_iterator flashes_begin() const;
    std::vector<aperture::FlashDesc>::const_iterator flashes_end() const;
    std::vector<aperture::FlashDesc>::const_reverse_iterator flashes_rbegin() const;
    std::vector<aperture::FlashDesc>::const_reverse_iterator flashes_rend() const;
    aperture::FlashDesc GetFlashDesc(int flash_num) const;
    aperture::ApertureFileType GetApertureFileType() const
    {
        return aperture_provider_.GetApertureFileType();
    }

    wxString GetApertureFileName() const
    {
        return aperture_provider_.GetApertureFileName();
    }

    std::filesystem::path GetApertureFilePath() const
    {
        return aperture_provider_.GetApertureFilePath();
    }

    wxRect GetFrameRect() const
    {
        return file_values_.frame_rect;
    }

    uint32_t GetFileFlags() const
    {
        return file_values_.file_flags;
    }

    FileDefValues GetFileDefValues() const
    {
        return file_values_;
    }

    LayerDesc GetLayerDesc(int layer_num) const
    {
        if (layer_num >= 0 && layer_num < static_cast<int>(layers_.size()))
            return layers_[layer_num];
        else //  Слой по умолчанию с невозможными параметрами
            return LayerDesc::LAYER_DESC_INVALID;
    }

    void SetLayerAttributes(int layer_num, LayerAttributes layer_attributes) const
    {
        if (layer_num >= 0 && layer_num < static_cast<int>(layers_.size()))
            layers_[layer_num].layer_attributes = layer_attributes;
    }

    LayerAttributes GetLayerAttributes(int layer_num) const
    {
        if (layer_num >= 0 && layer_num < static_cast<int>(layers_.size()))
            return layers_[layer_num].layer_attributes;
        else
            return LayerAttributes{0};
    }

    wxString GetPictureFileName() const
    {
        return wxString(file_values_.picture_filepath.string().c_str(), wxConvUTF8);
    }

    std::filesystem::path GetPictureFilePath() const
    {
        return file_values_.picture_filepath;
    }

    const GraphObj* ScanForGraphObject(uint32_t graph_object_ordinal) const;
    const GraphObj* ScanForGraphObject(wxColor graph_obj_ord_as_color) const;
    // Функции преобразования размеров и координат точек из единиц DBU в два вида
    // абсолютных геометрических единиц - дюймы и миллиметры.
    double ConvertDBUToInch(double DBU_size) const;
    double ConvertDBUToMM(double DBU_size) const;
    double ConvertInchToDBU(double inch_size) const;
    double ConvertMMToDBU(double mm_size) const;
};

class FileWorkshop
{
public:

    static constexpr char FILE_CREATE_MSG[] = wxTRANSLATE("Ошибка создания файла");
    static constexpr char FILE_OPEN_MSG[] = wxTRANSLATE("Ошибка открытия файла");
    static constexpr char FILE_READ_MSG[] = wxTRANSLATE("Ошибка чтения файла");
    static constexpr char FILE_WRITE_MSG[] = wxTRANSLATE("Ошибка записи файла");
    static constexpr char FILE_SEEK_MSG[] = wxTRANSLATE("Ошибка позиционирования файлового указателя");
    static constexpr char FILE_BAD_FILE_FORMAT_MSG[] = wxTRANSLATE("Неверный формат файла");
    static constexpr char FILE_BAD_FILE_HEADER_MSG[] = wxTRANSLATE("Неверный формат заголовка файла");
    static constexpr char FILE_BAD_RECORD_FORMAT_MSG[] = wxTRANSLATE("Ошибка структуры записи файла");
    static constexpr char FILE_BAD_FIELD_FORMAT_MSG[] = wxTRANSLATE("Ошибка типа или значения поля записи файла");
    static constexpr char MEMORY_ALLOC_MSG[] = wxTRANSLATE("Ошибка при выделении памяти");
    static constexpr char OPEN_CLOSE_BRACKETS_MSG[] = wxTRANSLATE("Некорректная последовательность скобок");
    static constexpr char NODE_NOT_OPENED_MSG[] = wxTRANSLATE("Узел не открыт");
    static constexpr char NUMERIC_VALUE_NOT_ALLOWED_MSG[] = wxTRANSLATE("Здесь числовое значение недопустимо");
    static constexpr char PARAMS_QUANTITY_MSG[] = wxTRANSLATE("Неверное количество параметров");
    static constexpr char PARAM_TYPE_MISMATCH_MSG[] = wxTRANSLATE("Несоответствие типа параметра");
    static constexpr char PARAM_INVALID_VALUE_MSG[] = wxTRANSLATE("Недопустимое значение параметра");
    static constexpr char PARAM_COMAMND_UNACCEPTABLE_HERE_MSG[] = wxTRANSLATE("Это команда здесь недопустима");

    static std::string GetErrMess(PCADLoadError err_code)
    {
        switch (err_code)
        {
        case PCADLoadError::LOAD_FILE_CREATE_ERROR:
            return std::string(wxGetTranslation(FILE_CREATE_MSG).mb_str());
        case PCADLoadError::LOAD_FILE_OPEN_ERROR:
            return std::string(wxGetTranslation(FILE_OPEN_MSG).mb_str());
        case PCADLoadError::LOAD_FILE_READ_ERROR:
            return std::string(wxGetTranslation(FILE_READ_MSG).mb_str());
        case PCADLoadError::LOAD_FILE_WRITE_ERROR:
            return std::string(wxGetTranslation(FILE_WRITE_MSG).mb_str());
        case PCADLoadError::LOAD_FILE_SEEK_ERROR:
            return std::string(wxGetTranslation(FILE_SEEK_MSG).mb_str());
        case PCADLoadError::LOAD_FILE_BAD_FILE_FORMAT:
            return std::string(wxGetTranslation(FILE_BAD_FILE_FORMAT_MSG).mb_str());
        case PCADLoadError::LOAD_FILE_BAD_FILE_HEADER:
            return std::string(wxGetTranslation(FILE_BAD_FILE_HEADER_MSG).mb_str());
        case PCADLoadError::LOAD_FILE_BAD_RECORD_FORMAT:
            return std::string(wxGetTranslation(FILE_BAD_RECORD_FORMAT_MSG).mb_str());
        case PCADLoadError::LOAD_FILE_BAD_FIELD_FORMAT:
            return std::string(wxGetTranslation(FILE_BAD_FIELD_FORMAT_MSG).mb_str());
        case PCADLoadError::LOAD_FILE_MEMORY_ERROR:
            return std::string(wxGetTranslation(MEMORY_ALLOC_MSG).mb_str());
        case PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY:
            return std::string(wxGetTranslation(PARAMS_QUANTITY_MSG).mb_str());
        case PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE:
            return std::string(wxGetTranslation(PARAM_TYPE_MISMATCH_MSG).mb_str());
        case PCADLoadError::LOAD_FILE_INCORRECT_PARAM_VALUE:
            return std::string(wxGetTranslation(PARAM_INVALID_VALUE_MSG).mb_str());
        case PCADLoadError::LOAD_FILE_COMMAND_UNACCEPTABLE_HERE:
            return std::string(wxGetTranslation(PARAM_COMAMND_UNACCEPTABLE_HERE_MSG).mb_str());
        default:
            return {};
        }
    }

    static std::string FormErrMess(PCADLoadError err_code, const std::string& add_err_mess)
    {
        std::string result_errmess = GetErrMess(err_code);
        if (!add_err_mess.empty())
            result_errmess += " - " + add_err_mess;
        return result_errmess;
    }

    using LoadFileResult = std::pair<PCADFile, PCADLoadError>;
    // Пара с детальной информацией об ошибках, возникших при загрузке файла - код ошибки и текст с подробной информацией о ней.
    using ErrorInfo = std::pair<PCADLoadError, std::string>;
    struct AdditionalLoadInfo
    {
        std::filesystem::path picture_file_path;
        // Сслыка на поставщика информации об апертурах, который будет использоваться загружаемым документом.
        aperture::ApertureProvider& aperture_provider;
        // Указывает загрузчику, следует ли ему перезагружать или переключать текущее множество засветок,
        // если этого требует формат загружаемого файла. При значении "ложь" загрузчику запрещается каким-либо
        // образом изменять текущий набор апертур, а нужно приспособиться к его имеющемуся состоянию.
        bool is_control_aperture_data = false;
        // Список ошибок, возникших при загрузке файла.
        std::vector<ErrorInfo> load_errors;

        void AddError(ErrorInfo&& error_info)
        {
            load_errors.push_back(std::move(error_info));
        }

        void AddError(PCADLoadError err_code)
        {
            load_errors.push_back({err_code, std::string()});
        }

        // Функция возвращает текстовое сообщение об ошибке error_info, представляющее собой комбинацию из типовой
        // компоненты (соответствующей коду ошибки) и специальной компоненты, хранящейся в строковой части пары.
        std::string GetErrMess(const ErrorInfo& error_info) const
        {
            std::string common_message = FileWorkshop::GetErrMess(error_info.first);
            if (error_info.second.empty())
                return common_message;
            else
                return common_message + " - " + error_info.second;
        }

        std::string GetErrMess(size_t error_index) const
        {
            if (error_index >= load_errors.size())
                return {};
            else
                return GetErrMess(load_errors[error_index]);
        }

        std::string GetSpecErrMess(size_t error_index) const
        {
            if (error_index >= load_errors.size())
                return {};
            else
                return load_errors[error_index].second;
        }

        size_t error_size() const noexcept
        {
            return load_errors.size();
        }
    };

    virtual ~FileWorkshop()
    {}

    virtual std::string GetFileWorkshopDescription() const = 0;
    virtual std::vector<std::pair<std::string, std::string>> GetFileExtensions() const = 0;
    virtual LoadFileResult LoadPCADFile(std::istream& istr,
                                        AdditionalLoadInfo& additional_load_info) = 0;
};

class PCADFileFactory
{
private:
    std::vector<FileWorkshop*> file_workshops_ptr_;

public:
    void RegisterFileWorkshop(FileWorkshop* file_workshop_ptr)
    {
        for (FileWorkshop* scan_workshop_ptr : file_workshops_ptr_)
            if (scan_workshop_ptr == file_workshop_ptr)
                return;
        file_workshops_ptr_.push_back(file_workshop_ptr);
    }

    const FileWorkshop* GetFileWorkshop(int workshop_number)
    {
        if (workshop_number >= 0 && workshop_number < static_cast<int>(file_workshops_ptr_.size()))
            return file_workshops_ptr_[workshop_number];
        else
            return nullptr;
    }

    size_t size()
    {
        return file_workshops_ptr_.size();
    }

    decltype(file_workshops_ptr_)::const_iterator begin() const
    {
        return file_workshops_ptr_.cbegin();
    }

    decltype(file_workshops_ptr_)::const_iterator end() const
    {
        return file_workshops_ptr_.cend();
    }
};
#endif // header guard

