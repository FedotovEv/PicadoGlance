#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <array>
#include <any>
#include <cctype>
#include <algorithm>

#include "HandlerPDIF.h"
#include "PCADFile.h"
#include "PCADViewDraw.h"
#include "ApertureFile.h"
#include <wx/colour.h>
#include <wx/gdicmn.h>

using namespace std;
using namespace aperture;

namespace HandlerPDIF
{
    const unordered_map<string_view, PDIFFileWorkshop::PDIFKeywords> PDIFFileWorkshop::keyword_to_enum_value
    {
        // Имена контейнеров - разделы и подразделы.
        {"COMPONENT"sv, PDIFKeywords::PDIF_KEY_COMPONENT},
        {"ENVIRONMENT"sv, PDIFKeywords::PDIF_KEY_ENVIRONMENT},
        {"USER"sv, PDIFKeywords::PDIF_KEY_USER},
        {"VIEW"sv, PDIFKeywords::PDIF_KEY_VIEW},
        {"DETAIL"sv, PDIFKeywords::PDIF_KEY_DETAIL},
        {"SYMBOL"sv, PDIFKeywords::PDIF_KEY_SYMBOL},
        {"ANNOTATE"sv, PDIFKeywords::PDIF_KEY_ANNOTATE},
        {"SUBCOMP"sv, PDIFKeywords::PDIF_KEY_SUBCOMP},
        {"COMP_DEF"sv, PDIFKeywords::PDIF_KEY_COMPDEF},
        {"PIC"sv, PDIFKeywords::PDIF_KEY_PIC},
        {"IN"sv, PDIFKeywords::PDIF_KEY_IN},
        {"EX"sv, PDIFKeywords::PDIF_KEY_EX},
        {"I"sv, PDIFKeywords::PDIF_KEY_I},
        {"ATR"sv, PDIFKeywords::PDIF_KEY_ATR},
        // Исполнительские команды.
        {"Mode"sv, PDIFKeywords::PDIF_KEY_MODE},
        {"Vw"sv, PDIFKeywords::PDIF_KEY_VW},
        {"PDIFvrev"sv, PDIFKeywords::PDIF_KEY_PDIF_VREV},
        {"DBvrev"sv, PDIFKeywords::PDIF_KEY_DB_VREV},
        {"DBunit"sv, PDIFKeywords::PDIF_KEY_DB_UNIT},
        {"Lyrstr"sv, PDIFKeywords::PDIF_KEY_LYRSTR},
        {"Lyrphid"sv, PDIFKeywords::PDIF_KEY_LYRPHID},
        {"Ssymtbl"sv, PDIFKeywords::PDIF_KEY_SSYMTBL},
        {"Polyap"sv, PDIFKeywords::PDIF_KEY_POLYAP},
        {"Poly"sv, PDIFKeywords::PDIF_KEY_POLY},
        {"DBtype"sv, PDIFKeywords::PDIF_KEY_DBTYPE},
        // Индивидуальные графические примитивы, более или менее соответствующие отдельным рисуемым объектам заголовка PCADViewDraw.h.
        {"A"sv, PDIFKeywords::PDIF_KEY_A},
        {"Arc"sv, PDIFKeywords::PDIF_KEY_ARC},
        {"C"sv, PDIFKeywords::PDIF_KEY_C},
        {"L"sv, PDIFKeywords::PDIF_KEY_L},
        {"R"sv, PDIFKeywords::PDIF_KEY_R},
        {"T"sv, PDIFKeywords::PDIF_KEY_T},
        {"Fr"sv, PDIFKeywords::PDIF_KEY_FR},
        //
        {"At"sv, PDIFKeywords::PDIF_KEY_AT},
        // Контур и пустоты многоугольника.
        {"Ol"sv, PDIFKeywords::PDIF_KEY_OL},
        {"Pv"sv, PDIFKeywords::PDIF_KEY_PV},
        {"Cv"sv, PDIFKeywords::PDIF_KEY_CV},
        // Группа ключевых слов, соответствующих настроечным (установочным) командам.
        {"Ly"sv, PDIFKeywords::PDIF_KEY_LY},
        {"Ls"sv, PDIFKeywords::PDIF_KEY_LS},
        {"Wd"sv, PDIFKeywords::PDIF_KEY_WD},
        {"Ts"sv, PDIFKeywords::PDIF_KEY_TS},
        {"Tj"sv, PDIFKeywords::PDIF_KEY_TJ},
        {"Tr"sv, PDIFKeywords::PDIF_KEY_TR},
        {"Tm"sv, PDIFKeywords::PDIF_KEY_TM}
    };

    // Словарь описания поддерживаемых блоков (разделов и терминальных элементов), которые могут содержаться в PDIF-данных и будут
    // обработаны загрузчиком. Прочие блоки, не описанные в этом словаре, при загрузке пропускаются.
    // В начале инициализационного списка размещены разделы и подразделы, являющиеся нетерминальными контейнерами и не выполняющие
    // никаких команд.
    const unordered_multimap<PDIFFileWorkshop::PDIFKeywords, PDIFFileWorkshop::TreeNodeDesc> PDIFFileWorkshop::keyword_to_node_desc
    {
        {PDIFKeywords::PDIF_KEY_COMPONENT,
            {
                PDIFKeywords::PDIF_KEY_COMPONENT,          // Ключ "COMPONENT".
                false,                                     // Нетерминал.
                false,                                     // Не является установочной операцией.
                {},                                        // Находится в корне иерархии.
                {
                    // Имя компонента, обязательная единичная строка.
                    {{ValueType::ARG_VALUE_STRING}, false}
                },
                PDIFOps::PDIFOP_NOPE                       // "Чистая" секция, не выполняет никаких операций.
            }
        },
        {PDIFKeywords::PDIF_KEY_USER,
            {
                PDIFKeywords::PDIF_KEY_USER,             // Ключ "USER".
                false,                                   // Нетерминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе "COMPONENT".
                {PDIFKeywords::PDIF_KEY_COMPONENT},
                {},                                      // Не имеет аргументов.
                PDIFOps::PDIFOP_NOPE                     // Только подраздел, вмещает нижележащие структуры, но не выполняет никаких операций.
            }
        },
        {PDIFKeywords::PDIF_KEY_VIEW,
            {
                PDIFKeywords::PDIF_KEY_VIEW,             // Ключ "VIEW".
                false,                                   // Нетерминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе "COMPONENT/USER".
                {PDIFKeywords::PDIF_KEY_COMPONENT, PDIFKeywords::PDIF_KEY_USER},
                {},                                      // Не имеет аргументов.
                PDIFOps::PDIFOP_NOPE                     // "Чистый" подраздел.
            }
        },
        {PDIFKeywords::PDIF_KEY_DETAIL,
            {
                PDIFKeywords::PDIF_KEY_DETAIL,           // Ключ "DETAIL".
                false,                                   // Нетерминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе "COMPONENT".
                {PDIFKeywords::PDIF_KEY_COMPONENT},
                {},                                      // Не имеет аргументов.
                PDIFOps::PDIFOP_NOPE                     // "Чистый" подраздел.
            }
        },
        {PDIFKeywords::PDIF_KEY_SYMBOL,
            {
                PDIFKeywords::PDIF_KEY_SYMBOL,           // Ключ "SYMBOL".
                false,                                   // Нетерминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе "COMPONENT".
                {PDIFKeywords::PDIF_KEY_COMPONENT},
                {},                                      // Не имеет аргументов.
                PDIFOps::PDIFOP_NOPE                     // "Чистый" подраздел.
            }
        },
        {PDIFKeywords::PDIF_KEY_PIC,
            {
                PDIFKeywords::PDIF_KEY_PIC,              // Ключ "PIC".
                false,                                   // Нетерминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе "...".
                {PDIFKeywords::PDIF_KEY_ANY},
                {},                                      // Не имеет аргументов.
                PDIFOps::PDIFOP_NOPE                     // "Чистый" подраздел.
            }
        },
        {PDIFKeywords::PDIF_KEY_ANNOTATE,
            {
                PDIFKeywords::PDIF_KEY_ANNOTATE,         // Ключ "ANNOTATE".
                false,                                   // Нетерминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе "COMPONENT".
                {PDIFKeywords::PDIF_KEY_COMPONENT},
                {},                                      // Не имеет аргументов.
                PDIFOps::PDIFOP_NOPE                     // "Чистый" подраздел.
            }
        },
        {PDIFKeywords::PDIF_KEY_ATR,
            {
                PDIFKeywords::PDIF_KEY_ATR,              // Ключ "ATR".
                false,                                   // Нетерминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе "COMPONENT/..".
                {PDIFKeywords::PDIF_KEY_COMPONENT, PDIFKeywords::PDIF_KEY_ANY},
                {},                                      // Не имеет аргументов.
                PDIFOps::PDIFOP_NOPE                     // Только подраздел, содержит "внутренние" атрибуты компонента.
            }
        },
        {PDIFKeywords::PDIF_KEY_IN,
            {
                PDIFKeywords::PDIF_KEY_IN,               // Ключ "IN".
                false,                                   // Нетерминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе "../ATR".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_ATR},
                {},                                      // Не имеет аргументов.
                PDIFOps::PDIFOP_NOPE                     // Только подраздел, содержит "внутренние" атрибуты компонента.
            }
        },
        {PDIFKeywords::PDIF_KEY_EX,
            {
                PDIFKeywords::PDIF_KEY_EX,               // Ключ "EX".
                false,                                   // Нетерминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе "../ATR".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_ATR},
                {},                                      // Не имеет аргументов.
                PDIFOps::PDIFOP_NOPE                     // Только подраздел, содержит "внешние" (пользовательские) атрибуты компонента.
            }
        },
        {PDIFKeywords::PDIF_KEY_COMPDEF,
            {
                PDIFKeywords::PDIF_KEY_COMPDEF,            // Ключ "COMP_DEF".
                false,                                     // Нетерминал.
                false,                                     // Не является установочной операцией.
                // Находится в разделе "../DETAIL/SUBCOMP".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_DETAIL, PDIFKeywords::PDIF_KEY_SUBCOMP},
                {
                    // Имя компонента, обязательная единичная строка.
                    {{ValueType::ARG_VALUE_STRING}, false}
                },
                PDIFOps::PDIFOP_NOPE                       // "Чистая" секция, не выполняет никаких операций.
            }
        },
        {PDIFKeywords::PDIF_KEY_I,
            {
                PDIFKeywords::PDIF_KEY_I,                  // Ключ "I".
                false,                                     // Нетерминал.
                false,                                     // Не является установочной операцией.
                // Находится в разделе "../DETAIL/SUBCOMP".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_DETAIL, PDIFKeywords::PDIF_KEY_SUBCOMP},
                {
                    // Два обязательных аргумента - имя файла и имя компонента.
                    {{ValueType::ARG_VALUE_STRING, ValueType::ARG_VALUE_STRING}, false}
                },
                PDIFOps::PDIFOP_NOPE                       // "Чистая" секция, не выполняет никаких операций.
            }
        },

        // ------------ Описатели терминальных команд. ----------------
        {PDIFKeywords::PDIF_KEY_MODE,
            {
                PDIFKeywords::PDIF_KEY_MODE,                 // Ключ "Mode".
                true,                                        // Терминал.
                false,                                       // Не является установочной операцией.
                // Находится в разделе "COMPONENT/USER/VIEW".
                {PDIFKeywords::PDIF_KEY_COMPONENT, PDIFKeywords::PDIF_KEY_USER, PDIFKeywords::PDIF_KEY_VIEW},
                {
                    // Один строковый аргумент - наименование режима (подтипа) базы данных.
                    {{ValueType::ARG_VALUE_STRING}, false}
                },
                PDIFOps::PDIFOP_MODE                         // Выбор подтипа базы данных.
            }
        },
        {PDIFKeywords::PDIF_KEY_VW,
            {
                PDIFKeywords::PDIF_KEY_VW,               // Ключ "Vw".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе "COMPONENT/USER/VIEW".
                {PDIFKeywords::PDIF_KEY_COMPONENT, PDIFKeywords::PDIF_KEY_USER, PDIFKeywords::PDIF_KEY_VIEW},
                {
                    // Настройки текущего окна - обязательный группа в единственном экземпляре.
                    {{ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_DOUBLE}, false},
                    // Запомненные пользователем окна - необязательные группы, которые могут присутствовать в любом количестве.
                    {{ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT}, true}
                }, //
                PDIFOps::PDIFOP_VW                       // Установка параметров имеющихся в системе окон отображения картинки.
            }
        },
        {PDIFKeywords::PDIF_KEY_PDIF_VREV,
            {
                PDIFKeywords::PDIF_KEY_PDIF_VREV,       // Ключ "PDIFvrev".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе "COMPONENT/ENVIRONMENT".
                {PDIFKeywords::PDIF_KEY_COMPONENT, PDIFKeywords::PDIF_KEY_ENVIRONMENT},
                {
                    // Версия формата PDIF-базы данных, хранимая как число с плавающей точкой.
                    {{ValueType::ARG_VALUE_DOUBLE}, false}
                },
                PDIFOps::PDIFOP_PDIF_VREV               // Команда определения версии формата загружаемого PDIF-файла.
            }
        },
        {PDIFKeywords::PDIF_KEY_DB_VREV,
            {
                PDIFKeywords::PDIF_KEY_DB_VREV,          // Ключ "DBvrev".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе "COMPONENT/ENVIRONMENT".
                {PDIFKeywords::PDIF_KEY_COMPONENT, PDIFKeywords::PDIF_KEY_ENVIRONMENT},
                {
                    // Версия формата базы данных, хранимая как число с плавающей точкой.
                    {{ValueType::ARG_VALUE_DOUBLE}, false}
                },
                PDIFOps::PDIFOP_DB_VREV                 // Команда определения версии формата исходной двоичной базы данных.
            }
        },
        {PDIFKeywords::PDIF_KEY_DB_UNIT,
            {
                PDIFKeywords::PDIF_KEY_DB_UNIT,          // Ключ "DBunit".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе "COMPONENT/ENVIRONMENT".
                {PDIFKeywords::PDIF_KEY_COMPONENT, PDIFKeywords::PDIF_KEY_ENVIRONMENT},
                {
                    // Наименование единиц измерения длины, применяемой в базе данных.
                    {{ValueType::ARG_VALUE_STRING}, false}
                },
                PDIFOps::PDIFOP_DB_UNIT        // Команда указания единицы измерения координат базы.
            }
        },
        {PDIFKeywords::PDIF_KEY_LYRSTR,
            {
                PDIFKeywords::PDIF_KEY_LYRSTR,           // Ключ "Lyrstr".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе "COMPONENT/ENVIRONMENT".
                {PDIFKeywords::PDIF_KEY_COMPONENT, PDIFKeywords::PDIF_KEY_ENVIRONMENT},
                {
                    // Слои изображения - произвольное количество групп, состоящих из имени слоя и назначенного ему цвета.
                    {{ValueType::ARG_VALUE_STRING, ValueType::ARG_VALUE_INT}, true}
                },
                PDIFOps::PDIFOP_LYRSTR         // Список имеющихся слоёв изображения.
            }
        },
        {PDIFKeywords::PDIF_KEY_LYRPHID,
            {
                PDIFKeywords::PDIF_KEY_LYRPHID,          // Ключ "Lyrphid".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе "COMPONENT/ENVIRONMENT".
                {PDIFKeywords::PDIF_KEY_COMPONENT, PDIFKeywords::PDIF_KEY_ENVIRONMENT},
                {   // Тройка чисел, определяющая парование слоёв и доступность их для трассировки соединений.
                    {{ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT}, true}
                },
                PDIFOps::PDIFOP_LYRPHID     // Установка спаривания слоёв и возможности их использования при трассировке.
            }
        },
        {PDIFKeywords::PDIF_KEY_SSYMTBL,
            {
                PDIFKeywords::PDIF_KEY_SSYMTBL,          // Ключ "SSymtbl".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе "COMPONENT/ENVIRONMENT".
                {PDIFKeywords::PDIF_KEY_COMPONENT, PDIFKeywords::PDIF_KEY_ENVIRONMENT},
                {   // Свойства контактов, служащих переходными между слоями платы. Четыре целых числа - характеристика одного такого контакта.
                    {{ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT}, true}
                },
                PDIFOps::PDIFOP_SSYMTBL     // Таблица свойств контактов, используемых для формирования переходных отверстий.
            }
        },
        {PDIFKeywords::PDIF_KEY_POLYAP,
            {
                PDIFKeywords::PDIF_KEY_POLYAP,           // Ключ "Polyap".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе "COMPONENT/ENVIRONMENT".
                {PDIFKeywords::PDIF_KEY_COMPONENT, PDIFKeywords::PDIF_KEY_ENVIRONMENT},
                {   // Установка глобальной ширины апертуры для рисования границ полигона.
                    {{ValueType::ARG_VALUE_INT}, false}
                },
                PDIFOps::PDIFOP_POLYAP_GLB          // Установка глобальной ширины апертуры для рисовки полигонов (применяется при отсутствии локальной настройки).
            }
        },
        {PDIFKeywords::PDIF_KEY_POLYAP,
            {
                PDIFKeywords::PDIF_KEY_POLYAP,           // Ключ "Polyap".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе ".../Poly".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_POLY},
                {   // Установка глобальной ширины апертуры для рисования границ полигона.
                    {{ValueType::ARG_VALUE_INT}, false}
                },
                PDIFOps::PDIFOP_POLYAP_LOC          // Местная установка ширины апертуры для рисовки данного полигона.
            }
        },
        {PDIFKeywords::PDIF_KEY_POLY,
            {
                PDIFKeywords::PDIF_KEY_POLY,             // Ключ "Poly".
                false,                                   // Нетерминал (подраздел).
                false,                                   // Не является установочной операцией.
                // Находится в разделе ".../PIC".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_PIC},
                {}, // Не имеет аргументов.
                PDIFOps::PDIFOP_PIC_POLY    // Создание привязанного многоугольника - многоугольника в составе описания
                                            // УГО какого-либо конструктивного элемента (.../PIC).
            }
        },
        {PDIFKeywords::PDIF_KEY_POLY,
            {
                PDIFKeywords::PDIF_KEY_POLY,             // Ключ "Poly".
                false,                                   // Нетерминал (подраздел).
                false,                                   // Не является установочной операцией.
                // Находится в разделе "DETAIL/ANNOTATE".
                {PDIFKeywords::PDIF_KEY_DETAIL, PDIFKeywords::PDIF_KEY_ANNOTATE},
                {}, // Не имеет аргументов.
                PDIFOps::PDIFOP_ANNOTATE_POLY       // Создание свободного (уединённого) многоугольника - иллюстративного элемента изображения.
            }
        },
        // Подкоманды команды "Poly", которая также выступает как подраздел.
        {PDIFKeywords::PDIF_KEY_OL,
            {
                PDIFKeywords::PDIF_KEY_OL,               // Ключ "Ol".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе "../Poly".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_POLY},
                {
                    {{ValueType::ARG_VALUE_INT}, false}, // Тип заполнения внутренней области полигона - закраска или штриховка.
                    {{ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT}, true} // Пары координат точек ломаной линии контура полигона.
                },
                PDIFOps::PDIFOP_POLY_OL                  // Описание внешнего контура (габарита) многоугольника.
            }
        },
        {PDIFKeywords::PDIF_KEY_CV,
            {
                PDIFKeywords::PDIF_KEY_CV,               // Ключ "Cv".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе "../Poly".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_POLY},
                {   // Тройка параметров круговой выемки - пара координат её центра и радиус.
                    {{ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT}, false}
                },
                PDIFOps::PDIFOP_POLY_CV                  // Описание внутренней круглой пустоты многоугольника.
            }
        },
        {PDIFKeywords::PDIF_KEY_PV,
            {
                PDIFKeywords::PDIF_KEY_PV,               // Ключ "Pv".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе "../Poly".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_POLY},
                {   // Произвольное количество вершин контура пустоты, каждая из которых задана парой своих координат.
                    {{ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT}, true}
                },
                PDIFOps::PDIFOP_POLY_PV                  // Описание внутренней полигональной пустоты многоугольника.
            }
        },
        //
        {PDIFKeywords::PDIF_KEY_DBTYPE,
            {
                PDIFKeywords::PDIF_KEY_DBTYPE,           // Ключ "DBtype".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе "COMPONENT".
                {PDIFKeywords::PDIF_KEY_COMPONENT},
                {   // Обязательный параметр - типа проекта.
                    {{ValueType::ARG_VALUE_STRING}, false}
                },
                PDIFOps::PDIFOP_DBTYPE          // Установка типа проекта - схема или конструктив.
            }
        },
        {PDIFKeywords::PDIF_KEY_A,
            {
                PDIFKeywords::PDIF_KEY_A,                // Ключ "A".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе "DETAIL/ANNOTATE".
                {PDIFKeywords::PDIF_KEY_DETAIL, PDIFKeywords::PDIF_KEY_ANNOTATE},
                { // Аргументы - обязательная пятёрка чисел - пара координат (X, Y), радиус окружности, а также начальный и конечный угол дуги в градусах.
                    {{ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_DOUBLE, ValueType::ARG_VALUE_DOUBLE}, false}
                },
                PDIFOps::PDIFOP_ANNOTATE_A_ARC          // Создание свободной (отъединённой) A-дуги - иллюстративного элемента изображения.
            }
        },
        {PDIFKeywords::PDIF_KEY_A,
            {
                PDIFKeywords::PDIF_KEY_A,                // Ключ "A".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе ".../PIC".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_PIC},
                { // Аргументы - обязательная пятёрка чисел - пара координат (X, Y), радиус окружности, а также начальный и конечный угол дуги в градусах.
                    {{ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_DOUBLE, ValueType::ARG_VALUE_DOUBLE}, false}
                },
                PDIFOps::PDIFOP_PIC_A_ARC          // Создание привязанной A-дуги - компонента описания УГО конструктива.
            }
        },
        {PDIFKeywords::PDIF_KEY_ARC,
            {
                PDIFKeywords::PDIF_KEY_ARC,              // Ключ "Arc".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе "DETAIL/ANNOTATE".
                {PDIFKeywords::PDIF_KEY_DETAIL, PDIFKeywords::PDIF_KEY_ANNOTATE},
                { // Аргументы - обязательная шестёрка чисел - три пары координат (X, Y) центра, начала и конца дуги.
                    {{ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT}, false}
                },
                PDIFOps::PDIFOP_ANNOTATE_ARC_ARC          // Создание свободной (отъединённой) ARC-дуги - иллюстративного элемента изображения.
            }
        },
        {PDIFKeywords::PDIF_KEY_ARC,
            {
                PDIFKeywords::PDIF_KEY_ARC,              // Ключ "Arc".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе ".../PIC".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_PIC},
                { // Аргументы - обязательная шестёрка чисел - три пары координат (X, Y) центра, начала и конца дуги.
                    {{ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT}, false}
                },
                PDIFOps::PDIFOP_PIC_ARC_ARC          // Создание привязанной ARC-дуги - компонента описания УГО конструктива.
            }
        },
        {PDIFKeywords::PDIF_KEY_AT,
            {
                PDIFKeywords::PDIF_KEY_AT,               // Ключ "At".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе ".../ATR/EX".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_ATR, PDIFKeywords::PDIF_KEY_EX},
                { // Аргументы - обязательный набор из двух строк (имя, значение) и пары координат (X, Y) точки привязки.
                    {{ValueType::ARG_VALUE_STRING, ValueType::ARG_VALUE_STRING, ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT}, false}
                },
                PDIFOps::PDIFOP_EX_ATTR          // Указание "внешних" атрибутов компонента (радиодетали).
            }
        },
        {PDIFKeywords::PDIF_KEY_C,
            {
                PDIFKeywords::PDIF_KEY_C,                // Ключ "C".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе "DETAIL/ANNOTATE".
                {PDIFKeywords::PDIF_KEY_DETAIL, PDIFKeywords::PDIF_KEY_ANNOTATE},
                { // Аргументы - координаты центра (X, Y) и радиус окружности.
                    {{ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT}, false}
                },
                PDIFOps::PDIFOP_ANNOTATE_CIRCLE          // Создание свободной (отъединённой) окружности - иллюстративного элемента изображения.
            }
        },
        {PDIFKeywords::PDIF_KEY_C,
            {
                PDIFKeywords::PDIF_KEY_C,                // Ключ "C".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе ".../PIC".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_PIC},
                { // Аргументы - координаты центра (X, Y) и радиус окружности.
                    {{ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT}, false}
                },
                PDIFOps::PDIFOP_PIC_CIRCLE              // Создание привязанной окружности - компонента описания УГО конструктива.
            }
        },
        {PDIFKeywords::PDIF_KEY_L,
            {
                PDIFKeywords::PDIF_KEY_L,                // Ключ "L".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе "DETAIL/ANNOTATE".
                {PDIFKeywords::PDIF_KEY_DETAIL, PDIFKeywords::PDIF_KEY_ANNOTATE},
                { // Аргументы - набор координатных пар (X, Y) вершин ломаной.
                    {{ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT}, true}
                },
                PDIFOps::PDIFOP_ANNOTATE_LINE            // Создание свободной (отъединённой) ломаной - иллюстративного элемента изображения.
            }
        },
        {PDIFKeywords::PDIF_KEY_L,
            {
                PDIFKeywords::PDIF_KEY_L,                // Ключ "L".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе ".../PIC".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_PIC},
                { // Аргументы - набор координатных пар (X, Y) вершин ломаной.
                    {{ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT}, true}
                },
                PDIFOps::PDIFOP_PIC_LINE              // Создание привязанной ломаной линии - компонента описания УГО конструктива.
            }
        },
        {PDIFKeywords::PDIF_KEY_R,
            {
                PDIFKeywords::PDIF_KEY_R,                // Ключ "R".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе "DETAIL/ANNOTATE".
                {PDIFKeywords::PDIF_KEY_DETAIL, PDIFKeywords::PDIF_KEY_ANNOTATE},
                { // Аргументы - две координатные пары (X, Y) диагональных вершин прямоугольника.
                    {{ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT}, false}
                },
                PDIFOps::PDIFOP_ANNOTATE_RECT            // Создание свободного пустотелого прямоугольника - иллюстративного элемента изображения.
            }
        },
        {PDIFKeywords::PDIF_KEY_R,
            {
                PDIFKeywords::PDIF_KEY_R,                // Ключ "R".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе ".../PIC".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_PIC},
                { // Аргументы - две координатные пары (X, Y) диагональных вершин прямоугольника.
                    {{ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT}, false}
                },
                PDIFOps::PDIFOP_PIC_RECT              // Создание привязанного незакрашенного прямоугольника- компонента описания УГО конструктива.
            }
        },
        {PDIFKeywords::PDIF_KEY_T,
            {
                PDIFKeywords::PDIF_KEY_T,                // Ключ "T".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе "DETAIL/ANNOTATE".
                {PDIFKeywords::PDIF_KEY_DETAIL, PDIFKeywords::PDIF_KEY_ANNOTATE},
                { // Аргументы - содержимок надписи и координатная пара (X, Y) точки привязки текста.
                    {{ValueType::ARG_VALUE_STRING, ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT}, false}
                },
                PDIFOps::PDIFOP_ANNOTATE_TEXT            // Создание свободной текстовой надписи - иллюстративного элемента изображения.
            }
        },
        {PDIFKeywords::PDIF_KEY_T,
            {
                PDIFKeywords::PDIF_KEY_T,                // Ключ "T".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе ".../PIC".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_PIC},
                { // Аргументы - содержимок надписи и координатная пара (X, Y) точки привязки текста.
                    {{ValueType::ARG_VALUE_STRING, ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT}, false}
                },
                PDIFOps::PDIFOP_PIC_TEXT              // Создание привязанного текстового элемента - компонента описания УГО конструктива.
            }
        },
        {PDIFKeywords::PDIF_KEY_FR,
            {
                PDIFKeywords::PDIF_KEY_FR,               // Ключ "Fr".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе "DETAIL/ANNOTATE".
                {PDIFKeywords::PDIF_KEY_DETAIL, PDIFKeywords::PDIF_KEY_ANNOTATE},
                { // Аргументы - две координатные пары (X, Y) диагональных вершин прямоугольника.
                    {{ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT}, false}
                },
                PDIFOps::PDIFOP_ANNOTATE_FILL_RECT       // Создание свободного закрашенного прямоугольника - иллюстративного элемента изображения.
            }
        },
        {PDIFKeywords::PDIF_KEY_FR,
            {
                PDIFKeywords::PDIF_KEY_FR,               // Ключ "Fr".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе ".../PIC".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_PIC},
                { // Аргументы - две координатные пары (X, Y) диагональных вершин прямоугольника.
                    {{ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT}, false}
                },
                PDIFOps::PDIFOP_PIC_FILL_RECT            // Создание привязанного закрашенного прямоугольника- компонента описания УГО конструктива.
            }
        },
        // Настроечные команды.
        {PDIFKeywords::PDIF_KEY_LY,
            {
                PDIFKeywords::PDIF_KEY_LY,               // Ключ "Ly".
                true,                                    // Терминал.
                true,                                    // Является установочной операцией.
                // Находится в разделе "..." (может располагаться в любом разделе).
                {PDIFKeywords::PDIF_KEY_ANY},
                { // Единственный аргумент - имя устанавливаемого слоя.
                    {{ValueType::ARG_VALUE_STRING}, false}
                },
                PDIFOps::PDIFOP_SET_LY                    // Установка активного слоя изображения.
            }
        },
        {PDIFKeywords::PDIF_KEY_LS,
            {
                PDIFKeywords::PDIF_KEY_LS,               // Ключ "Ls".
                true,                                    // Терминал.
                true,                                    // Является установочной операцией.
                // Находится в разделе "..." (может располагаться в любом разделе).
                {PDIFKeywords::PDIF_KEY_ANY},
                { // Единственный аргумент - текстовое обозначение типа выбираемой линии.
                    {{ValueType::ARG_VALUE_STRING}, false}
                },
                PDIFOps::PDIFOP_SET_LS                    // Установка типа линии для рисования линейных геометрических примитивов.
            }
        },
        {PDIFKeywords::PDIF_KEY_WD,
            {
                PDIFKeywords::PDIF_KEY_WD,               // Ключ "Wd".
                true,                                    // Терминал.
                true,                                    // Является установочной операцией.
                // Находится в разделе "..." (может располагаться в любом разделе).
                {PDIFKeywords::PDIF_KEY_ANY},
                { // Единственный аргумент - устанавливаемая ширина линии.
                    {{ValueType::ARG_VALUE_INT}, false}
                },
                PDIFOps::PDIFOP_SET_WD                    // Установка ширины линии для рисования линейных геометрических примитивов.
            }
        },
        {PDIFKeywords::PDIF_KEY_TS,
            {
                PDIFKeywords::PDIF_KEY_TS,               // Ключ "Ts".
                true,                                    // Терминал.
                true,                                    // Является установочной операцией.
                // Находится в разделе "..." (может располагаться в любом разделе).
                {PDIFKeywords::PDIF_KEY_ANY},
                { // Единственный аргумент - высота текста (глифов шрифта) в единицах базы данных DBU.
                    {{ValueType::ARG_VALUE_INT}, false}
                },
                PDIFOps::PDIFOP_SET_TS                    // Установка высоты (размера) последующих текстовых примитивов.
            }
        },
        {PDIFKeywords::PDIF_KEY_TJ,
            {
                PDIFKeywords::PDIF_KEY_TJ,               // Ключ "Tj".
                true,                                    // Терминал.
                true,                                    // Является установочной операцией.
                // Находится в разделе "..." (может располагаться в любом разделе).
                {PDIFKeywords::PDIF_KEY_ANY},
                { // Единственный обязательный аргумент - строковый код типа выравнивание последующего текста.
                    {{ValueType::ARG_VALUE_STRING}, false}
                },
                PDIFOps::PDIFOP_SET_TJ                    // Установка типа выравнивания текстовых примитивов.
            }
        },
        {PDIFKeywords::PDIF_KEY_TR,
            {
                PDIFKeywords::PDIF_KEY_TR,               // Ключ "Tr".
                true,                                    // Терминал.
                true,                                    // Является установочной операцией.
                // Находится в разделе "..." (может располагаться в любом разделе).
                {PDIFKeywords::PDIF_KEY_ANY},
                { // Единственный обязательный аргумент - индекс N угла поворота текста. Истинный угол будет равен 90 * N градусов.
                    {{ValueType::ARG_VALUE_INT}, false}
                },
                PDIFOps::PDIFOP_SET_TR                    // Установка угла вращения текстовых примитивов.
            }
        },
        {PDIFKeywords::PDIF_KEY_TM,
            {
                PDIFKeywords::PDIF_KEY_TM,               // Ключ "Tm".
                true,                                    // Терминал.
                true,                                    // Является установочной операцией.
                // Находится в разделе "..." (может располагаться в любом разделе).
                {PDIFKeywords::PDIF_KEY_ANY},
                { // Единственный обязательный аргумент - наличие зеркального отражения текста. "N" - нормальный режим рисования, "Y" - зеркалирование.
                    {{ValueType::ARG_VALUE_STRING}, false}
                },
                PDIFOps::PDIFOP_SET_TR                    // Установка угла вращения текстовых примитивов.
            }
        }
    };

    PDIFFileWorkshop::PDIFFileWorkshop() :
        node_mode_handler(this),
        node_vw_handler(this),
        node_pdif_vrev_handler(this),
        node_db_vrev_handler(this),
        node_db_unit_handler(this),
        node_lyrstr_handler(this),
        node_lyrphid_handler(this),
        node_ssymtbl_handler(this),
        node_polyap_glb_handler(this),
        node_polyap_loc_hanlder(this),
        node_poly_annot_hanlder(this),
        node_poly_pic_hanlder(this),
        node_dbtype_handler(this),
        node_a_annot_handler(this),
        node_a_pic_handler(this),
        node_arc_annot_handler(this),
        node_arc_pic_handler(this),
        node_circle_annot_handler(this),
        node_circle_pic_handler(this),
        node_at_handler(this),
        node_poly_ol_handler(this),
        node_poly_pv_hanlder(this),
        node_poly_cv_handler(this),
        node_layer_select_handler(this),
        node_line_type_handler(this),
        node_line_width_handler(this),
        node_text_height_handler(this),
        node_text_justify_hanlder(this),
        node_text_rotation_handler(this),
        node_text_mirror_handler(this)
    {}

    wxPoint PDIFFileWorkshop::ConvPntToLog(int pict_x, int pict_y) const
    {
        return {pict_x, -pict_y};
    }

    wxPoint PDIFFileWorkshop::ConvPntToLog(wxPoint pict) const
    {
        return {pict.x, -pict.y};
    }

    wxRect PDIFFileWorkshop::ConvRectToLog(int pict_ldx, int pict_ldy, int pict_rux, int pict_ruy) const
    {
        if (pict_ldx > pict_rux)
            swap(pict_ldx, pict_rux);
        if (pict_ldy > pict_ruy)
            swap(pict_ldy, pict_ruy);
        return {pict_ldx, -pict_ruy, pict_rux - pict_ldx + 1, pict_ruy - pict_ldy + 1};
    }

    wxRect PDIFFileWorkshop::ConvRectToLog(wxRect pict_rect) const
    { // Полагаем, что в точке (pict_rect.x, pict_rect.y) хранятся координаты левого нижнего угла прямоугольника.
        int pict_ruy = pict_rect.y + pict_rect.height - 1;
        return {pict_rect.x, -pict_ruy, pict_rect.width, pict_rect.height};
    }

    // Метод проверки совпадения некоторого конкретного маршрута concrete_path маршрутному шаблону pattern_path.
    bool PDIFFileWorkshop::IsPDIFPathCorrespond(const TreePathType& concrete_path, const TreePathType& pattern_path) const
    {
        size_t test_pos = 0;
        for (; test_pos < concrete_path.size() && test_pos < pattern_path.size(); ++test_pos)
        {
            switch (pattern_path[test_pos])
            {
            case PDIFKeywords::PDIF_KEY_ANY:
            {
                // Ищем ближайший значащий элемент трафарета.
                size_t next_meaning_pattern_pos = test_pos;
                while (next_meaning_pattern_pos < pattern_path.size())
                {
                    PDIFKeywords scan_key = pattern_path[next_meaning_pattern_pos];
                    if (scan_key != PDIFKeywords::PDIF_KEY_ANY && scan_key != PDIFKeywords::PDIF_KEY_UNKNOWN)
                        break;
                }
                if (next_meaning_pattern_pos >= pattern_path.size())
                    // Значащих элементов в хвосте трафарета более не содержится. Проверяемый маршрут, таким образом,
                    // удовлетворяет требованиям трафарета, завершаем проверку с истинным результатом.
                    return true;
                // Следующий значащий элемент трафарета находится по индексу next_meaning_pattern_pos. Пробуем найти
                // положение хвоста (суффикса) трафарета в ещё непроверенном остатке маршрута concrete_path (начиная с
                // его звена с индексом test_pos).
                TreePathType pattern_tail{pattern_path.begin() + next_meaning_pattern_pos, pattern_path.end()};
                for (size_t i = test_pos; i < concrete_path.size(); ++ i)
                {
                    TreePathType concrete_tail{concrete_path.begin() + i, concrete_path.end()};
                    if (IsPDIFPathCorrespond(concrete_tail, pattern_tail))
                        // Хвост трафарета pattern_tail укладывается (соответствует) некоторому концевому участку конкрета
                        // (начиная от элемента с индексом i). Так что и в целом конкрет соответствует требованиям шаблона.
                        return true;
                }
                // Хвосты трафарета и проверяемого маршрута совместить не удалось, конкретный маршрут не
                // соответствует трафарету.
                return false;
            }
            case PDIFKeywords::PDIF_KEY_UNKNOWN:
                // Шаблонный заполнитель, соответствующий любому ключевому слову в данном элементе маршрута.
                // Так что здесь нам подходит любой тип текущего звена проверяемого маршрута concrete_path, никаких
                // специальных проверок на соответствие для данного элемента не требуется.
                break; // Данное звено маршрута соответствует требованиям трафарета, продолжаем проверку.
            default:
                if (pattern_path[test_pos] != concrete_path[test_pos])
                    return false; // Выявлено несовпадение проверяемого маршрута трафарету.
                else
                    break;  // Данное звено маршрута совпадает с трафаретом, продолжаем проверку.
            }
        }
        // Проверяемый маршрут полностью соответствует некоторому префиксу (начальному участку) трафарета.
        // Их полное совпадение будет иметь место, если оставшийся хвост трафарета не содержит обязательных частей.
        for (; test_pos < pattern_path.size(); ++test_pos)
        {
            if (pattern_path[test_pos] != PDIFKeywords::PDIF_KEY_ANY)
                // Хвост трафарета содержит какую-то значащую часть, которой в проверяемой части нет. Так что проверяемый
                // маршрут не соответствует трафарету.
                return false;
        }
        // Хвост трафарета не содержит обязательных частей (только PDIF_KEY_ANY-макросы), так что его можно считать
        // соответствующим проверяемому объекту.
        return true;
    }

    // Функция поиска команды, дескриптор которой существенным образом совпадает с содержимым аргумента find_node.
    // При сравнении принимаются во внимание следующие поля:
    const PDIFFileWorkshop::TreeNodeDesc* PDIFFileWorkshop::FindNodeDesc(const TreeNodeDesc& find_node) const
    {
        auto equal_it_pair = keyword_to_node_desc.equal_range(find_node.key);
        for (auto current_eq_it = equal_it_pair.first; current_eq_it != equal_it_pair.second; ++current_eq_it)
            if (IsPDIFPathCorrespond(find_node.tree_path, current_eq_it->second.tree_path))
                // Удалось обнаружить тип узла из числа описанных "в узловой энциклопедии" keyword_to_node_desc,
                // подходящий нам по ключевому слову и трафарету допустимого маршрута.
                return &(current_eq_it->second);
        // Найти подходящий типаж узла не удалось, возвращаем NULL.
        return nullptr;
    }

    bool PDIFFileWorkshop::SkipSpaces(istream& istr) const
    {
        while (istr && isspace(istr.get()));
        if (istr)
            istr.unget(); // Непробельный символ найден, вернём его в поток.
        return bool(istr);
    }

    bool PDIFFileWorkshop::SkipComment(istream& istr) const
    {
        char c = '\0';
        // Проверка на возможное наличие комментария в строке.
        istr.get(c);
        if (c == '%')
        { // Это комментарий. Он простирается до ближайшего конца строки.
            while (istr && c != '\r' && c != '\n')
                istr.get(c);
            return false; // Вернём результат, предписывающий повторить цикл поиска начала истинной лексемы сначала.
        }
        else
        {  // Это не начало комментария.
            if (istr)
            { // Конец данных не достигнут, вернём считанный символ в поток (он есть первый символ какой-то
              // полинной лексемы) и вернём результат, предписывающий продолжить работу.
                istr.unget();
                return true;
            }
            else
            { // Конец данных достигнут, вернём ответ, который повторит цикл поиска начала лексемы.
                return false;
            }
        }
    }

    // Метод лексического разбора входного потока - разделение его на лексемы.
    PDIFFileWorkshop::LexemData PDIFFileWorkshop::GetNextLexem(istream& istr)
    {
        // Список специальных литералов, не обязательно отделяющихся пробелами от предыдущих и последующих лексем.
        static constexpr array SPEC_PREDEF_LEXEM{"{"sv, "}"sv, "["sv, "]"sv};
        static constexpr size_t SPEC_PREDEF_LEXEM_MAXLEN = 1;

        string acc_text;
        LexemData result;
        char c;

        // Ищем первый символ, с которого будет начинаться следующая лексема (пропускаем пробельные символы и комментарии).
        while (true)
        {
            if (!SkipSpaces(istr))
            {  // Непробельных символов более нет, данные во входном потоке закончились, выходим без результата.
                is_wait_keyword = false;
                return result;
            }
            if (SkipComment(istr))
                break; // Найден подлинный начальный символ лексемы, выходим из цикла и приступаем к её извлечению и анализу.
        }

        // Считываем байты из потока до следующего пробельного символа, знаменующего конец очередной лексемы.
        bool now_in_quoted_string = false, is_obvious_string = false;
        while (true)
        {
            istr.get(c);
            if (!istr)
                break;

            if (now_in_quoted_string)
            { // Мы в данный момент не находимся в пределах закавыченного "сырого литерала".
              // Если лексема начинается кавычкой, то это "сырой литерал" и к нему присоединяются все символы потока без анализа,
              // вплоть до закрывающей парной кавычки.
                if (c == '\"')
                    now_in_quoted_string = false;
                else
                    acc_text += c;
            }
            else
            { // Мы в данный момент не находимся в пределах закавыченного "сырого литерала".
                if (c == '\\')
                { // Символ отмены. Пропускаем его и включаем в состав лексемы следующий за ним в потоке.
                    istr.get(c);
                    if (!istr)
                        break;
                    if (c == 'b')
                        c = ' '; // Последовательность "\b" - это пробел.
                    is_obvious_string = true;
                    acc_text += c;
                }
                else
                { // Это обыкновенный символ. Он может быть либо пробельным (в таком случае он завершает лексему), либо
                  // открывающей кавычкой (в таком случае он открывает "сырой литерал"), либо любой иной символ, просто
                  // включаемый в тело лексемы.
                    if (isspace(c))
                        break;
                    else if (c == '\"')
                        is_obvious_string = now_in_quoted_string = true;
                    else
                    {
                        acc_text += c;
                        // Проверяем, относится ли полученная к данному моменту лексема к специальным лексемам, не требующим
                        // пробела для своего завершения и считающихся законченными сразу же, как только они сформировались.
                        if (!is_obvious_string && acc_text.size() <= SPEC_PREDEF_LEXEM_MAXLEN)
                        {
                            if (auto spec_lexem_it = find(SPEC_PREDEF_LEXEM.begin(), SPEC_PREDEF_LEXEM.end(), acc_text);
                                spec_lexem_it != SPEC_PREDEF_LEXEM.end())
                                break; // Это именно такая особенная лексема. Она полностью завершена, выходим.
                        }
                    }
                }
            }
        }

        // Любое значение, заключенное в двойные кавычки (содержащее какой-либо сырой литерал), будем рассматривать, как строку.
        if (is_obvious_string)
        {
            is_wait_keyword = false;
            result.data = acc_text;
            result.cat = LexemCategory::PDIF_LEX_STRING_VALUE;
            return result;
        }

        // В acc_text выделена очередная лексема входного потока. Проанализируем её содержимое.
        result.text = acc_text;
        if (acc_text == "{"sv)
        {
            is_wait_keyword = true;
            result.cat = LexemCategory::PDIF_LEX_OPEN_FIG_BRACKET;
            return result;
        }
        else if (acc_text == "}"sv)
        {
            is_wait_keyword = false;
            result.cat = LexemCategory::PDIF_LEX_CLOSE_FIG_BRACKET;
            return result;
        }
        else if (acc_text == "["sv)
        {
            is_wait_keyword = true;
            result.cat = LexemCategory::PDIF_LEX_OPEN_SQUARE_BRACKET;
            return result;
        }
        else if (acc_text == "]"sv)
        {
            is_wait_keyword = false;
            result.cat = LexemCategory::PDIF_LEX_CLOSE_SQUARE_BRACKET;
            return result;
        }
        else if (is_wait_keyword)
        {
            for (pair<string_view, PDIFKeywords> test_keyword_pair : keyword_to_enum_value)
            {
                if (test_keyword_pair.first == acc_text)
                { // Лексема - это ключевое слово.
                    is_wait_keyword = false;
                    result.cat = LexemCategory::PDIF_LEX_KEYWORD;
                    result.key = test_keyword_pair.second;
                    return result;
                }
            }
        }

        is_wait_keyword = false;
        char conv_str_arg[128];
        strncpy(conv_str_arg, acc_text.c_str(), size(conv_str_arg) - 1);
        conv_str_arg[size(conv_str_arg) - 1] = '\0';
        char *conv_text_end;

        // Выделенная лексема - не скобки и не ключевое слово. Следовательно, это какой-то литерал, тип
        // которого мы сейчас и определим.
        double double_lex_value = strtod(conv_str_arg, &conv_text_end);
        if (conv_text_end - conv_str_arg == static_cast<int>(acc_text.size()))
        {
            result.cat = LexemCategory::PDIF_LEX_DOUBLE_VALUE;
            result.data = double_lex_value;
            return result;
        }

        int64_t long_int_lex_value = strtoll(conv_str_arg, &conv_text_end, 10);
        if (conv_text_end - conv_str_arg == static_cast<int>(acc_text.size()))
        {
            result.cat = LexemCategory::PDIF_LEX_INT_VALUE;
            result.data = long_int_lex_value;
            return result;
        }

        // Значение нечисловое - значит, строка.
        result.cat = LexemCategory::PDIF_LEX_STRING_VALUE;
        result.data = acc_text;
        return result;
    }

    // Функция поиска (и определения номера) слоя с именем find_layer_name в списке слоёв layers.
    int PDIFFileWorkshop::FindLayerByName(const std::string& find_layer_name) const
    {
        auto layers_it = find_if(layers.begin(), layers.end(), [&find_layer_name](const LayerDesc& layer_desc) -> bool
            {
                return layer_desc.layer_name == find_layer_name;
            });
        return layers_it == layers.end() ? -1 : layers_it - layers.begin();
    }

    // Метод определения типа линии по его символьной сигнатуре.
    optional<LineType> PDIFFileWorkshop::FindLineType(const std::string& line_type_sign)
    {
        if (line_type_sign == "SOLID"s)
            return LineType::LINE_SOLID;
        else if (line_type_sign == "DASHED")
            return LineType::LINE_DASHED;
        else if (line_type_sign == "DOTTED")
            return LineType::LINE_DOTTED;
        else
            return {};
    }

    // Определение типа комплексной ориентации текста.
    optional<TextOrientation> PDIFFileWorkshop::FindTextOrientation(int text_rotation_index, bool is_text_mirror)
    {
        switch (text_rotation_index)
        {
        case 0:
            if (is_text_mirror)
                return TextOrientation::TEXT_LEFT_RIGHT_MIRROR;
            else
                return TextOrientation::TEXT_LEFT_RIGHT;
        case 1:
            if (is_text_mirror)
                return TextOrientation::TEXT_DOWN_UP_MIRROR;
            else
                return TextOrientation::TEXT_UP_DOWN;
        case 2:
            if (is_text_mirror)
                return TextOrientation::TEXT_RIGHT_LEFT_MIRROR;
            else
                return TextOrientation::TEXT_RIGHT_LEFT;
        case 3:
            if (is_text_mirror)
                return TextOrientation::TEXT_UP_DOWN_MIRROR;
            else
                return TextOrientation::TEXT_DOWN_UP;
        default:
            return {};
        }
    }

    // Выравнивание текста.
    optional<TextAlign> PDIFFileWorkshop::FindTextAlign(const std::string& text_align_sign)
    {
        if (text_align_sign.size() != 2)
            return {};

        if (text_align_sign == "CB"s)
            return TextAlign::TEXT_CENTER_DOWN;
        else if (text_align_sign == "CT"s)
            return TextAlign::TEXT_CENTER_UP;
        else if (text_align_sign == "CC"s)
            return TextAlign::TEXT_CENTER_CENTER;
        else if (text_align_sign == "LB"s)
            return TextAlign::TEXT_LEFT_DOWN;
        else if (text_align_sign == "LT"s)
            return TextAlign::TEXT_LEFT_UP;
        else if (text_align_sign == "LC"s)
            return TextAlign::TEXT_LEFT_CENTER;
        else if (text_align_sign == "RB"s)
            return TextAlign::TEXT_RIGHT_DOWN;
        else if (text_align_sign == "RT"s)
            return TextAlign::TEXT_RIGHT_UP;
        else if (text_align_sign == "RC"s)
            return TextAlign::TEXT_RIGHT_CENTER;
        else
            return {};
    }

    // Метод выполняет проверку того факта, что предверхний (предпоследний) элемент стека узлов является подразделом (контейнером).
    bool PDIFFileWorkshop::IsPrevNodeContainer([[maybe_unused]] TreeNodeData* node_data) const
    {
        int prev_stack_index = static_cast<int>(node_stack.size()) - 2; // Индекс предпоследнего  узла, который нас вмещает.
        const TreeNodeData* prev_node_data = prev_stack_index >= 0 ? &(node_stack[prev_stack_index]) : nullptr;
        return (prev_node_data && prev_node_data->node_desc && !prev_node_data->node_desc->is_terminal);
    }

    // Основной метод загрузки данных из PDIF-потока.
    FileWorkshop::LoadFileResult PDIFFileWorkshop::LoadPCADFile(istream& istr, FileWorkshop::AdditionalLoadInfo& additional_load_info)
    {
        // Внутренняя лямбда, формирующая текущий маршрут (до текущего разбираемого узла) в форме объекта класса TreePathType -
        // - последовательности (вектора) идентификационных констант поддерживаемых загрузчиком ключевых слов.
        auto current_tree_path = [this]() -> TreePathType
        {
            TreePathType tree_result;
            for (const TreeNodeData& node_data : node_stack)
                tree_result.push_back(node_data.key);
            return tree_result;
        };

        // Определение требуемого типа аргумента с индексом arg_index.
        auto select_arg_type = [this](const TreeNodeDesc* node_desc, int arg_index) -> ValueType
        {
            if (!node_desc->data_blocks.size())
                // Узел вообще не содержит аргументов, о чем мы и сообщаем вызывающему коду.
                return ValueType::ARG_VALUE_NON_EXIST;

            int scan_arg_index = 0, scan_arg_block = 0, scan_index_in_block = 0;
            while (true)
            {
                const DataBlockDesc& current_data_block_desc = node_desc->data_blocks[scan_arg_block];
                if (scan_arg_index == arg_index)
                    // Мы добрались до описателя типа параметра с порядковым индексом arg_index. Возвращаем его установленный тип.
                    return current_data_block_desc.data_types[scan_index_in_block];
                // Вычисляем координаты следующего эдемента списка аргументов для шаблона узла node_desc.
                ++scan_arg_index;
                if (++scan_index_in_block >= static_cast<int>(current_data_block_desc.size()))
                {
                    scan_index_in_block = 0;
                    if (!current_data_block_desc.is_repetable)
                    {
                        if (++scan_arg_block >= static_cast<int>(node_desc->data_blocks.size()))
                            return ValueType::ARG_VALUE_NON_EXIST; // Следующего аргумента в узле node_desc не предусмотрено.
                    }
                }
            }
        };

        // Лямбда проверки соответствия типа параметра с индексом arg_index для узла с шаблоном node_desc его истинному
        // типу arg_type.
        auto param_type_check = [this, &select_arg_type](const TreeNodeDesc* node_desc, ValueType arg_type, int arg_index) -> bool
        {
            // Выясняем требуемый тип данных для параметра с порядковым номером (индексом) arg_index.
            switch (select_arg_type(node_desc, arg_index))
            {
            case ValueType::ARG_VALUE_NONE: // Необрабатываемый параметр, который должен быть пропущен.
                return true; // Для пропускаемого параметра истинный его тип значения не имеет.
            case ValueType::ARG_VALUE_INT: // В целое значение допускается сохранять только целочисленные величины.
                return arg_type == ValueType::ARG_VALUE_INT;
            case ValueType::ARG_VALUE_STRING:
                // Для сохранения в строку подходит любой исходный содержательный тип параметра.
                return (arg_type == ValueType::ARG_VALUE_INT ||
                        arg_type == ValueType::ARG_VALUE_DOUBLE ||
                        arg_type == ValueType::ARG_VALUE_STRING);
            case ValueType::ARG_VALUE_DOUBLE: // В качестве дробного числа могут восприниматься любые числа любых числовых типов.
                return (arg_type == ValueType::ARG_VALUE_INT) || (arg_type == ValueType::ARG_VALUE_DOUBLE);
            default:
                return false;
            }
        };

        // Обобщённая лямбда-функция для преобразования значения аргумента param_value к типу ValueType в виде вариантной
        // переменной типа ArgumentDataType.
        auto convert_param_to_arg = [this](ValueType arg_type, auto param_value) -> ArgumentDataType
        {
            using param_type_t = decay_t<decltype(param_value)>;

            switch (arg_type)
            {
            case ValueType::ARG_VALUE_INT:
                if constexpr (is_same_v<param_type_t, string>)
                    return {};
                else if constexpr (is_same_v<param_type_t, int64_t>)
                    return param_value;
                else if constexpr (is_same_v<param_type_t, double>)
                    return lround(param_value);
                else
                    return {};
            case ValueType::ARG_VALUE_STRING:
                if constexpr (is_same_v<param_type_t, string>)
                    return param_value;
                else if constexpr (is_same_v<param_type_t, int64_t> || is_same_v<param_type_t, double>)
                    return to_string(param_value);
                else
                    return {};
            case ValueType::ARG_VALUE_DOUBLE:
                if constexpr (is_same_v<param_type_t, string>)
                    return {};
                else if constexpr (is_same_v<param_type_t, int64_t>)
                    return static_cast<double>(param_value);
                else if constexpr (is_same_v<param_type_t, double>)
                    return param_value;
                else
                    return {};
            default:
                return {};
            }
        };

        // Инициализация состояния загрузчика - сброс его к начальному положению.
        graph_objects.clear();
        layers.clear();
        cross_layers_pinholes.clear();
        radio_components.clear();
        radio_comp_inserts.clear();
        file_values = {};
        node_stack.clear();
        is_wait_keyword = false;
        additional_load_info.load_errors.clear();

        // Заполним поля файлового описания file_values, зависящие только от типа загружаемого файла.
        file_values.file_signature = "PDIF"; // Строка, кратко описывающая тип и содержание файла
        file_values.file_workshop = this; // Указатель на "цех файловой фабрики", открывший данный файл

        // Основной цикл разбора входного потока PDIF-данных.
        int skip_node_level = -1;  // Параметр режима пропуска неподдерживаемых узлов PDIF-дерева.
        while (true)
        {
            LexemData currrent_lexem = GetNextLexem(istr);
            if (currrent_lexem.cat == LexemCategory::PDIF_LEX_UNKNOWN)
                break; // Лексемы во входном потоке закончились - выходим.

            if (skip_node_level >= 0)
            { // Мы находимся в режиме пропуска узлов (они принадлежат к неподдерживаемой загрузчиком ветке).
              // В этом режиме нам нужно отслеживать только уровень вложенности, пока он не станет равным skip_node_level.
                switch (currrent_lexem.cat)
                {
                case LexemCategory::PDIF_LEX_OPEN_FIG_BRACKET:
                    [[fallthrough]];
                case LexemCategory::PDIF_LEX_OPEN_SQUARE_BRACKET: // Открытие (создания) нового узла дерева.
                    node_stack.push_back({});
                    break;
                case LexemCategory::PDIF_LEX_CLOSE_FIG_BRACKET:
                    [[fallthrough]];
                case LexemCategory::PDIF_LEX_CLOSE_SQUARE_BRACKET:
                    // Закрытие (завершение обработки) активного узла дерева, описание которого находится в последнем элементе node_stack.
                    node_stack.pop_back();
                    if (static_cast<int>(node_stack.size()) <= skip_node_level)
                        // Все неподдерживаемые узлы, которые следовало пропустить, завершены. Выходим из режима пропуска.
                        skip_node_level = -1;
                    break;
                default:
                    break;
                }
                continue;
            }

            // Производим обработку выделенной из потока лексемы.
            switch (currrent_lexem.cat)
            {
            case LexemCategory::PDIF_LEX_OPEN_FIG_BRACKET:
                [[fallthrough]];
            case LexemCategory::PDIF_LEX_OPEN_SQUARE_BRACKET:
            {// Открытие (создания) нового узла дерева.
                node_stack.push_back({});
                TreeNodeData& use_node = node_stack.back();
                use_node.is_setting = (currrent_lexem.cat == LexemCategory::PDIF_LEX_OPEN_SQUARE_BRACKET);
                use_node.tree_path = current_tree_path();
                break;
            }
            case LexemCategory::PDIF_LEX_CLOSE_FIG_BRACKET:
                [[fallthrough]];
            case LexemCategory::PDIF_LEX_CLOSE_SQUARE_BRACKET:
            { // Закрытие (завершение обработки) активного узла дерева, описание которого находится в последнем элементе node_stack.
                if (!node_stack.size())
                { // Стек текущей ветви пуст - баланс открывающих и закрывающих скобок нарушен.
                    additional_load_info.AddError(PCADLoadError::LOAD_FILE_BAD_FILE_FORMAT);
                    return {PCADFile{additional_load_info.aperture_provider}, PCADLoadError::LOAD_FILE_BAD_FILE_FORMAT};
                }
                TreeNodeData& use_node = node_stack.back();
                if (use_node.is_setting != (currrent_lexem.cat == LexemCategory::PDIF_LEX_CLOSE_SQUARE_BRACKET))
                { // Несоответствие типов скобок, открывающих и закрывающих данный узел.
                    additional_load_info.AddError({PCADLoadError::LOAD_FILE_BAD_FILE_FORMAT, node_stack.back().key_text});
                    return {PCADFile{additional_load_info.aperture_provider}, PCADLoadError::LOAD_FILE_BAD_FILE_FORMAT};
                }
                // С парностью скобок пока всё в порядке. Вызываем завершающую операцию и удаляем полностью обработанный
                // узел дерева.
                if (use_node.node_desc && opcode_to_handler.count(use_node.node_desc->opcode))
                { // Закрывается какой-то поддерживаемый и обрабатываемый узел дерева.
                    if (auto error_info = opcode_to_handler.at(use_node.node_desc->opcode)->HandleCloseNode(&use_node); error_info)
                        additional_load_info.AddError(move(error_info).value());
                }
                node_stack.pop_back(); // Узел полностью обработан и закрыт - удаляем его из стека.
                break;
            }
            // Лексема - какое-либо ключевое слово из известных разборщику.
            case LexemCategory::PDIF_LEX_KEYWORD:
            {
                if (!node_stack.size())
                { // Указание параметра (в данном случае, ключевого слова-идентификатора) вне какого-либо узла недопустимо.
                    additional_load_info.AddError(PCADLoadError::LOAD_FILE_BAD_FILE_FORMAT);
                    return {PCADFile{additional_load_info.aperture_provider}, PCADLoadError::LOAD_FILE_BAD_FILE_FORMAT};
                }
                TreeNodeData& use_node = node_stack.back();
                if (use_node.key != PDIFKeywords::PDIF_KEY_ANY && use_node.key != PDIFKeywords::PDIF_KEY_UNKNOWN)
                    break;  // Узел уже был идентифицирован и открыт ранее.

                // Заполняем известные поля шаблона нужного узла.
                TreeNodeDesc find_node_template;
                find_node_template.key = currrent_lexem.key;
                find_node_template.is_setting = use_node.is_setting;
                find_node_template.tree_path = use_node.tree_path;
                if (const TreeNodeDesc* suitable_node_desc = FindNodeDesc(find_node_template))
                {  // Удалось обнаружить шаблон узла, отвечающий требованиям. Выполняем набор открывающих узел операций.
                    use_node.key = currrent_lexem.key;
                    use_node.key_text = currrent_lexem.text;
                    use_node.node_desc = suitable_node_desc;
                    if (opcode_to_handler.count(use_node.node_desc->opcode))
                    {  // Открывается какой-то поддерживаемый и обрабатываемый узел дерева.
                        if (auto error_info = opcode_to_handler.at(use_node.node_desc->opcode)->HandleOpenNode(&use_node); error_info)
                        { // При открытии узла возникла ошибка. Пропускаем как этот узел, так и все вложенные в него.
                            additional_load_info.AddError(move(error_info).value());
                            skip_node_level = static_cast<int>(node_stack.size()) - 1; // Включаем режим пропуска узлов, пока данный узел не закроется.
                        }
                    }
                }
                break;
            }
            // Лексемы-значения - строковые и числовые (целочисленные или дробные).
            case LexemCategory::PDIF_LEX_STRING_VALUE:
            { // Здесь следует разобрать два случая - если ключевое слово текущего узла уже определено (в этом случае мы
              // просто добавляем очередной извлечённый аргумент в список аргументов узла) и если ключевое слово узла ещё
              // не задано - в таком случае извлечённая из потока строка является ключом узла, но таким ключом, который мы
              // (возможно, пока) не поддерживаем.
                if (!node_stack.size())
                { // Указание параметра вне какого-либо узла недопустимо.
                    additional_load_info.AddError(PCADLoadError::LOAD_FILE_BAD_FILE_FORMAT);
                    return {PCADFile{additional_load_info.aperture_provider}, PCADLoadError::LOAD_FILE_BAD_FILE_FORMAT};
                }

                TreeNodeData& use_node = node_stack.back();
                const string& string_value = get<string>(currrent_lexem.data);
                if (use_node.key == PDIFKeywords::PDIF_KEY_ANY || use_node.key == PDIFKeywords::PDIF_KEY_UNKNOWN ||
                    !use_node.node_desc)
                {   // Это неизвестное нам имя или тип узла - переходим в режим пропуска данных, пока он не заверщится.
                    use_node.key_text = string_value;
                    skip_node_level = static_cast<int>(node_stack.size()) - 1;
                    break;
                }

                // Тип (шаблон) узла ранее был найден, так что есть возможность продолжать обработку параметра.
                // сверить истинный и надлежащий тип очередного параметра.
                int next_arg_index = static_cast<int>(use_node.args.size());
                if (!param_type_check(use_node.node_desc, ValueType::ARG_VALUE_STRING, next_arg_index))
                { // Истинный тип параметра не совпадает с требуемым - ошибка.
                    additional_load_info.AddError(PCADLoadError::LOAD_FILE_BAD_FILE_FORMAT);
                    return {PCADFile{additional_load_info.aperture_provider}, PCADLoadError::LOAD_FILE_BAD_FILE_FORMAT};
                }

                // Истинный тип параметра соответствует требуемому и может быть в него преобразован. Сохраняем его
                // в use_node.args как очередной его элемент.
                use_node.args.push_back(convert_param_to_arg(select_arg_type(use_node.node_desc, next_arg_index), string_value));
                break;
            }
            case LexemCategory::PDIF_LEX_INT_VALUE:
                [[fallthrough]];
            case LexemCategory::PDIF_LEX_DOUBLE_VALUE:
            {
                if (!node_stack.size())
                { // Указание параметра вне какого-либо узла недопустимо.
                    additional_load_info.AddError(PCADLoadError::LOAD_FILE_BAD_FILE_FORMAT);
                    return {PCADFile{additional_load_info.aperture_provider}, PCADLoadError::LOAD_FILE_BAD_FILE_FORMAT};
                }

                TreeNodeData& use_node = node_stack.back();
                if (use_node.key == PDIFKeywords::PDIF_KEY_ANY || use_node.key == PDIFKeywords::PDIF_KEY_UNKNOWN ||
                    !use_node.node_desc)
                { // Числовой параметр задан для узла, который предварительно не был идентифицирован ключевым словом (известным
                  // нам или неизвестным, не имеет значения). Считаем подобное ошибкой структуры PDIF-потока.
                    additional_load_info.AddError(PCADLoadError::LOAD_FILE_BAD_FILE_FORMAT);
                    return {PCADFile{additional_load_info.aperture_provider}, PCADLoadError::LOAD_FILE_BAD_FILE_FORMAT};
                }

                // Тип (шаблон) узла ранее был найден, так что есть возможность продолжать обработку параметра.
                //сверить истинный и надлежащий тип очередного параметра.
                int next_arg_index = static_cast<int>(use_node.args.size());
                if (!param_type_check(use_node.node_desc, ValueType::ARG_VALUE_STRING, next_arg_index))
                { // Истинный тип параметра не совпадает с требуемым - ошибка.
                    additional_load_info.AddError(PCADLoadError::LOAD_FILE_BAD_FILE_FORMAT);
                    return {PCADFile{additional_load_info.aperture_provider}, PCADLoadError::LOAD_FILE_BAD_FILE_FORMAT};
                }

                // Истинный тип параметра соответствует требуемому и может быть в него преобразован. Сохраняем его
                // в use_node.args как очередной его элемент.
                ValueType target_value_type = select_arg_type(use_node.node_desc, next_arg_index);
                if (currrent_lexem.cat == LexemCategory::PDIF_LEX_INT_VALUE)
                {
                    int64_t int_value = get<int64_t>(currrent_lexem.data);
                    use_node.args.push_back(convert_param_to_arg(target_value_type, int_value));
                }
                else if (currrent_lexem.cat == LexemCategory::PDIF_LEX_DOUBLE_VALUE)
                {
                    double double_value = get<double>(currrent_lexem.data);
                    use_node.args.push_back(convert_param_to_arg(target_value_type, double_value));
                }
                break;
            }
            default:
                break;
            }
        }

        if (node_stack.size())
        { // Стек текущей ветви не пуст - баланс открывающих и закрывающих скобок также нарушен.
            additional_load_info.AddError(PCADLoadError::LOAD_FILE_BAD_FILE_FORMAT);
            return {PCADFile{additional_load_info.aperture_provider}, PCADLoadError::LOAD_FILE_BAD_FILE_FORMAT};
        }

        return {PCADFile(move(graph_objects), move(layers), additional_load_info.aperture_provider,
                         move(file_values)), PCADLoadError::LOAD_FILE_NO_ERROR};
    }

    // Определения методов конкретных классов-обработчиов командных узлов дерева PDIF-базы данных.

    // Mode - выбор режима (подтипа) БД.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeModeHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeModeHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        if (node_data->args.size() != 1)
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}}; // Должен быть строго один строковый параметр.
        if (!holds_alternative<string>(node_data->args[0]))
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};   // Параметр оказался не строковым.

        const string& mode_name = get<string>(node_data->args[0]);
        if (mode_name == "DETL"s)
            GetWorkshop()->file_values.file_flags |= FileFlags::FILE_FLAG_DETL;
        else if (mode_name == "SYMB"s)
            GetWorkshop()->file_values.file_flags &= (~FileFlags::FILE_FLAG_DETL);
        else
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_VALUE, {}}};   // Недопустмое значение параметра (не SYMB и не DETL).

        return {};
    }

    // Vw - положение и размер окон отображения.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeVWHanlder::HandleOpenNode(TreeNodeData* node_data)
    {
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeVWHanlder::HandleCloseNode(TreeNodeData* node_data)
    {
        if (node_data->args.size() < 3)
            // В составе параметров должно быть описано хотя бы текущее окно, установленное в момент создания (сохранения) файла.
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}};

        size_t window_count = (node_data->args.size() - 3) / 5;
        if (node_data->args.size() != (window_count * 5 + 3))
            // На каждое дополнительное сохранённое окно должно быть ещё по 5 параметров на штуку.
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}};

        for (size_t i = 0; i < node_data->args.size(); ++i)
        {
            const ArgumentDataType& cur_arg = node_data->args[i];
            if (i != 2)
            { // Все параметры узла, кроме второго, должны быть целочисленными.
                if (!holds_alternative<int64_t>(cur_arg))
                    return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};
            }
            else
            { // Второй параметр должен являться числом с плавающей точкой.
                if (!holds_alternative<double>(cur_arg))
                    return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};
            }
        }

        GetWorkshop()->file_values.current_window_center =
            {get<int64_t>(node_data->args[0]), get<int64_t>(node_data->args[1])};           // Центр рабочего окна.
        GetWorkshop()->file_values.current_window_scale = get<double>(node_data->args[2]);  // Его масштаб.

        for (size_t current_window = 0, arg_number = 3; current_window < window_count; ++current_window, arg_number += 5)
        {
            int64_t real_window_number = get<int64_t>(node_data->args[arg_number]),
                    dlx = get<int64_t>(node_data->args[arg_number + 1]),
                    dly = get<int64_t>(node_data->args[arg_number + 2]),
                    urx = get<int64_t>(node_data->args[arg_number + 3]),
                    ury = get<int64_t>(node_data->args[arg_number + 4]);

            int64_t ulx = dlx, uly = ury, win_width = urx - dlx, win_height = ury - dly;

            vector<wxRect>& uwin = GetWorkshop()->file_values.user_windows;
            if (real_window_number >= static_cast<int>(uwin.size()))
                uwin.resize(real_window_number + 1);
            uwin.push_back({ulx, uly, win_width, win_height});
        }

        return {};
    }

    // PDIFvrev - версия (ревизия) PDIF-формата.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodePDIFvrevHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodePDIFvrevHandler::HandleCloseNode(TreeNodeData* node_data)
    { // Ревизию PDIF-формата мы не обрабатываем - просто пропускаем.
        return {};
    }

    // DBvrev - версия (ревизия) формата двоичного исходника базы данных.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeDBvrevHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeDBvrevHandler::HandleCloseNode(TreeNodeData* node_data)
    { // Версия исхолника (из которого получен загружаемый PDIF-файл) нас также пока не интересует.
        return {};
    }

    // DBunit - единица измерений координат, используемая в файле.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeDBunitHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeDBunitHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        if (node_data->args.size() != 1)
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}}; // Должен быть строго один строковый параметр.
        if (!holds_alternative<string>(node_data->args[0]))
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};   // Параметр оказался не строковым.

        const string& db_unit_name = get<string>(node_data->args[0]);
        if (db_unit_name == "MIL"s)
            GetWorkshop()->file_values.file_flags |= FileFlags::FILE_FLAG_UNIT_INCHES;
        else if (db_unit_name == "CMM"s)
            GetWorkshop()->file_values.file_flags &= (~FileFlags::FILE_FLAG_UNIT_INCHES);
        else
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_VALUE, {}}};   // Недопустмое значение параметра (не MIL и не CMM).

        return {};
    }

    // Lyrstr - таблица доступных слоёв - список слоёв, используемых компонентом.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeLyrstrHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeLyrstrHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        if (node_data->args.size() % 2)
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}}; // Допускается только чётное количество параметров.

        size_t layer_count = node_data->args.size() / 2;
        for (size_t current_layer = 0, arg_number = 0; current_layer < layer_count; ++current_layer, arg_number += 2)
        {
            LayerDesc new_layer;
            if (!holds_alternative<string>(node_data->args[arg_number]))
                return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};   // Имя слоя должно быть строкой.
            new_layer.layer_name = get<string>(node_data->args[arg_number]);
            new_layer.layer_number = current_layer;

            if (!holds_alternative<int64_t>(node_data->args[arg_number + 1]))
                return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};   // Индекс цвета обязан быть целочисленным.
            if (optional<int> checked_val = CheckIntValue(get<int64_t>(node_data->args[arg_number + 1]), 15); checked_val)
                new_layer.layer_color = checked_val.value();
            else
                return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_VALUE, {}}};   // Индекс цвета вне допустимых пределов.

            new_layer.layer_wx_color = *wxWHITE;
            new_layer.layer_attributes = {LAYER_ON_AVL_ACT_ON, LAYER_NOT_TRACEABLE};

            GetWorkshop()->layers.push_back(move(new_layer));
        }
        return {};
    }

    // Lyrphid - определение спаривания слоёв и их доступности для трассировки.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeLyrphidHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeLyrphidHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        if (node_data->args.size() % 3)
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}}; // Количество параметров должно быть кратно 3 (по три числа на описываемый слой).

        int max_layer_number = static_cast<int>(GetWorkshop()->layers.size()) - 1;  // Максимальный индекс существующего слоя.
        if (!max_layer_number)
            return {};
        for (size_t arg_number = 0; arg_number < node_data->args.size(); arg_number += 3)
        {
            if (!holds_alternative<int64_t>(node_data->args[arg_number]) ||
                !holds_alternative<int64_t>(node_data->args[arg_number + 1]) ||
                !holds_alternative<int64_t>(node_data->args[arg_number + 2]))
                return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};   // Все члены тройки должны быть целыми.

            optional<int> use_layer_number = CheckIntValue(get<int64_t>(node_data->args[arg_number]), max_layer_number);
            optional<int> pair_layer_number = CheckIntValue(get<int64_t>(node_data->args[arg_number + 1]), max_layer_number);
            int64_t traceable_flag = get<int64_t>(node_data->args[arg_number + 2]);
            if (use_layer_number && pair_layer_number)
            {
                LayerDesc& use_layer_desc = GetWorkshop()->layers[use_layer_number.value()];
                use_layer_desc.layer_pair_number = pair_layer_number.value();
                use_layer_desc.layer_attributes.layer_is_traceable = (unsigned int)(traceable_flag == use_layer_number.value());
            }
            else
            {  // Индекс (номер) слоя указывает на несуществующий (не описанный ранее в Lyrstr) слой.
                return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_VALUE, {}}};
            }
        }
        return {};
    }

    // Ssymtbl - задание свойств контактов, которые служат как переходные отверстия между различными слоями.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeSsymtblHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeSsymtblHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        if (node_data->args.size() % 4)
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}}; // Количество параметров должно быть кратным 4 (по 4 числа на отверстие).

        int max_layer_number = static_cast<int>(GetWorkshop()->layers.size()) - 1;  // Максимальный индекс существующего слоя.
        size_t pinhole_count = node_data->args.size() / 4;
        for (size_t current_pinhole = 0, arg_number = 0; current_pinhole < pinhole_count; ++current_pinhole, arg_number += 4)
        {
            if (!holds_alternative<int64_t>(node_data->args[arg_number]) ||
                !holds_alternative<int64_t>(node_data->args[arg_number + 1]) ||
                !holds_alternative<int64_t>(node_data->args[arg_number + 2]) ||
                !holds_alternative<int64_t>(node_data->args[arg_number + 3]))
                return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};   // Все члены параметрической четвёрки должны быть целыми.

            optional<int> use_pin_number = CheckIntValue(get<int64_t>(node_data->args[arg_number]), 255);
            int64_t use_pin_type = get<int64_t>(node_data->args[arg_number + 1]);
            int64_t use_layer_from = get<int64_t>(node_data->args[arg_number + 2]);
            int64_t use_layer_to = get<int64_t>(node_data->args[arg_number + 3]);
            if (!use_pin_number || (use_pin_type != -1 && use_pin_type != -2))
                return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_VALUE, {}}}; // Недопустимый номер (индекс) переходного отверстия или его тип.

            CrossLayerPinhole new_cross_pinhole;
            new_cross_pinhole.pin_number = use_pin_number.value();
            new_cross_pinhole.pin_type =
                use_pin_type == -1 ? CrossLayerHoleType::PINHOLE_THROUGH : CrossLayerHoleType::PINHOLE_INTERLAYER;
            if (new_cross_pinhole.pin_type == CrossLayerHoleType::PINHOLE_INTERLAYER)
            {
                if (use_layer_from < 0 || use_layer_from > max_layer_number ||
                    use_layer_to < 0 || use_layer_to > max_layer_number)
                    // Недопустимый индекс входного или выходного слоя для межслойного переходного отверстия.
                    return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_VALUE, {}}};

                new_cross_pinhole.layer_from = use_layer_from;
                new_cross_pinhole.layer_to = use_layer_to;
            }
            GetWorkshop()->cross_layers_pinholes.push_back(move(new_cross_pinhole));
        }
        return {};
    }

    // Polyap - глобальная ширина апертуры для рисования границ полигона.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodePolyapGlbHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodePolyapGlbHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        if (node_data->args.size() != 1)
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}}; // Должен существовать только один параметр - ширина рисующей апертуры.
        if (!holds_alternative<int64_t>(node_data->args[0]))
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};   // Все члены параметрической четвёрки должны быть целыми.

        if (optional<int> use_poly_aperture = CheckIntValue(get<int64_t>(node_data->args[0]), 1024); use_poly_aperture)
            GetWorkshop()->file_values.polyap = use_poly_aperture.value();
        else
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_VALUE, {}}};
        return {};
    }

    // Polyap - локальная ширина апертуры для рисования границ полигона.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodePolyapLocHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodePolyapLocHandler::HandleCloseNode(TreeNodeData* node_data)
    {
       if (node_data->args.size() != 1)
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}}; // Должен существовать только один параметр - ширина рисующей апертуры.
        if (!holds_alternative<int64_t>(node_data->args[0]))
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};   // Все члены параметрической четвёрки должны быть целыми.

        if (optional<int> use_poly_aperture = CheckIntValue(get<int64_t>(node_data->args[0]), 1024); use_poly_aperture)
        {
            if (node_data->spec_info.type != NodeSpecType::NODE_TYPE_POLYGONE)
                return {{PCADLoadError::LOAD_FILE_COMAMND_UNACCEPTABLE_HERE, "Polyap"}};

            if (node_data->spec_info.handler_spec_data)
            {
                NodePicPolyHandler::PolyNodeSpecInfo* poly_spec_info =
                    (NodePicPolyHandler::PolyNodeSpecInfo*)(node_data->spec_info.handler_spec_data.get());
                poly_spec_info->polyap = use_poly_aperture.value();
            }
        }
        else
        {
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_VALUE, {}}};
        }
        return {};
    }

    // DBtype - тип базы данных - схемный ("Schematic") или конструктивный ("PC-Board") элемент.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeDBtypeHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeDBtypeHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        if (node_data->args.size() != 1)
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}};  // Должен быть строго один строковый параметр.
        if (!holds_alternative<string>(node_data->args[0]))
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};       // Параметр оказался не строковым.

        const string& db_type_name = get<string>(node_data->args[0]);
        if (db_type_name == "PC-Board"s)
        {
            GetWorkshop()->file_values.file_flags &= (~FileFlags::FILE_FLAG_EDITOR_PCCAPS);
            GetWorkshop()->file_values.DBU_in_measure_unit = 100; // Конструктив - 100 DBU на метрическую единицу (дюйм или миллиметр).
        }
        else if (db_type_name == "Schematic"s)
        {
            GetWorkshop()->file_values.file_flags |= FileFlags::FILE_FLAG_EDITOR_PCCAPS;
            GetWorkshop()->file_values.DBU_in_measure_unit = 10;  // Схема - 10 DBU на метрическую единицу (дюйм или миллиметр).
        }
        else
        {
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_VALUE, {}}};   // Недопустмое значение параметра (не Schematic и не PC-Board).
        }
        return {};
    }

    // Poly - команда рисования свободного составного полигона.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeAnnotatePolyHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        void* spec_info_ptr = ::operator new(sizeof(NodePicPolyHandler::PolyNodeSpecInfo), nothrow);
        if (!spec_info_ptr)
            return {{PCADLoadError::LOAD_FILE_MEMORY_ERROR, {}}};

        // Память под блок специфической информации, накопительно описывающей полигон, успешно выделена.
        node_data->spec_info.handler_spec_data.reset(spec_info_ptr);
        node_data->spec_info.type = NodeSpecType::NODE_TYPE_POLYGONE;
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeAnnotatePolyHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        NodePicPolyHandler::PolyNodeSpecInfo* poly_spec_info =
            (NodePicPolyHandler::PolyNodeSpecInfo*)(node_data->spec_info.handler_spec_data.get());
        FillType poly_fill_type = poly_spec_info->contour.is_hatched ? FillType::POLY_FILL_HATCH : FillType::POLY_FILL_SOLID;

        GraphObj* graph_obj_ptr = new ObjPoly
            (node_data->node_settings.layer_number, poly_fill_type, poly_spec_info->polyap,
             poly_spec_info->contour.points, poly_spec_info->circ_voids, poly_spec_info->poly_voids);
        GetWorkshop()->graph_objects.push_back(graph_obj_ptr);
        return {};
    }

    // Poly - команда рисования привязанного составного полигона.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodePicPolyHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        void* spec_info_ptr = ::operator new(sizeof(NodePicPolyHandler::PolyNodeSpecInfo), nothrow);
        if (!spec_info_ptr)
            return {{PCADLoadError::LOAD_FILE_MEMORY_ERROR, {}}};

        // Память под блок специфической информации, накопительно описывающей полигон, успешно выделена.
        node_data->spec_info.handler_spec_data.reset(spec_info_ptr);
        node_data->spec_info.type = NodeSpecType::NODE_TYPE_POLYGONE;
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodePicPolyHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        // Пока здесь заглушка без фактической реализации.
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodePolyOlHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodePolyOlHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        size_t argc = node_data->args.size();
        if (argc < 3 || (argc % 2) != 1)
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}};  // Параметров должны быть нечётное количество и не менее 3.

        // Все аргументы должны быть целочисленными.
        for (size_t i = 0; i < argc; ++i)
        {
            if (!holds_alternative<int64_t>(node_data->args[i]))
                return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};
        }

        if (node_data->spec_info.type != NodeSpecType::NODE_TYPE_POLYGONE)
            return {{PCADLoadError::LOAD_FILE_COMAMND_UNACCEPTABLE_HERE, "Poly::Ol"}};

        if (node_data->spec_info.handler_spec_data)
        {
            NodePicPolyHandler::PolyNodeSpecInfo* poly_spec_info =
                (NodePicPolyHandler::PolyNodeSpecInfo*)(node_data->spec_info.handler_spec_data.get());
            switch (get<int64_t>(node_data->args[0]))
            { // Тип заполнения многоугольника.
            case 1:
                poly_spec_info->contour.is_hatched = false;
            case 2:
                poly_spec_info->contour.is_hatched = true;
            default:
                poly_spec_info->contour.is_hatched = false;
            }
            // Переносим координаты вершин из аргументов команды в поле contour описателя строящегося полигона.
            poly_spec_info->contour.points.clear();
            for (size_t i = 1; i < argc; i += 2)
            {
                wxPoint log_contour_point =
                    GetWorkshop()->ConvPntToLog(get<int64_t>(node_data->args[i]), get<int64_t>(node_data->args[i + 1]));
                poly_spec_info->contour.points.push_back(log_contour_point);
            }
        }

        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodePolyPvHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodePolyPvHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        if (node_data->args.size() % 2)
            // Количество параметров должно быть четным (по паре координат на вершину).
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}};

        // Все аргументы должны быть целочисленными.
        for (size_t i = 0; i < node_data->args.size(); ++i)
        {
            if (!holds_alternative<int64_t>(node_data->args[i]))
                return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};
        }

        if (node_data->spec_info.type != NodeSpecType::NODE_TYPE_POLYGONE)
            return {{PCADLoadError::LOAD_FILE_COMAMND_UNACCEPTABLE_HERE, "Poly::Pv"}};

        if (node_data->spec_info.handler_spec_data)
        {
            NodePicPolyHandler::PolyNodeSpecInfo* poly_spec_info =
                (NodePicPolyHandler::PolyNodeSpecInfo*)(node_data->spec_info.handler_spec_data.get());
            // Создаем новую полигональную пустоту и сохранем её в описатель многоугольника.
            std::vector<wxPoint> void_points;
            // Перенос вершин из массива аргументов в вектор вершин создаваемой пустоты.
            for (size_t i = 0; i < node_data->args.size(); i += 2)
            {
                wxPoint log_void_point =
                    GetWorkshop()->ConvPntToLog(get<int64_t>(node_data->args[i]), get<int64_t>(node_data->args[i + 1]));
                void_points.push_back(log_void_point);
            }

            ObjPVoid new_poly_void(void_points);
            poly_spec_info->poly_voids.push_back(move(new_poly_void));
        }
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodePolyCvHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodePolyCvHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        if (node_data->args.size() != 3)
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}};  // Параметров должны быть строго 3.

        // Все аргументы должны быть целочисленными.
        for (size_t i = 0; i < node_data->args.size(); ++i)
        {
            if (!holds_alternative<int64_t>(node_data->args[i]))
                return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};
        }

        if (node_data->spec_info.type != NodeSpecType::NODE_TYPE_POLYGONE)
            return {{PCADLoadError::LOAD_FILE_COMAMND_UNACCEPTABLE_HERE, "Poly::Cv"}};

        if (node_data->spec_info.handler_spec_data)
        {
            NodePicPolyHandler::PolyNodeSpecInfo* poly_spec_info =
                (NodePicPolyHandler::PolyNodeSpecInfo*)(node_data->spec_info.handler_spec_data.get());
            // Создаем новую круговую пустоту и сохранем её в описатель многоугольника.
            wxPoint log_void_center =
                GetWorkshop()->ConvPntToLog(get<int64_t>(node_data->args[0]), get<int64_t>(node_data->args[1]));
            ObjCVoid new_circ_void(log_void_center, get<int64_t>(node_data->args[2]));
            poly_spec_info->circ_voids.push_back(move(new_circ_void));
        }
        return {};
    }

    // A - Создание свободной (отъединённой) A-дуги.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeAnnotateAHandler::HandleOpenNode(TreeNodeData* node_data)
    {
       return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeAnnotateAHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        if (node_data->args.size() != 5)
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}};  // Параметров должны быть строго 5.
        // Первые три параметра - координаты центра и радиус - целочисленные. Последние два - угловые пределы дуги - дробные.
        if (!holds_alternative<int64_t>(node_data->args[0]) ||
            !holds_alternative<int64_t>(node_data->args[1]) ||
            !holds_alternative<int64_t>(node_data->args[2]) ||
            !holds_alternative<double>(node_data->args[3]) ||
            !holds_alternative<double>(node_data->args[4]))
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};

        int arc_center_x = get<int64_t>(node_data->args[0]),
            arc_center_y = get<int64_t>(node_data->args[1]),
            arc_radius = get<int64_t>(node_data->args[2]);
        double arc_start_angle = get<double>(node_data->args[3]),
               arc_end_angle = get<double>(node_data->args[4]);

        wxPoint arc_center_log = GetWorkshop()->ConvPntToLog(arc_center_x, arc_center_y);

        int arc_begin_x = arc_radius * cos(arc_start_angle) + arc_center_x;
        int arc_begin_y = arc_radius * sin(arc_start_angle) + arc_center_y;
        wxPoint arc_begin_log = GetWorkshop()->ConvPntToLog(arc_begin_x, arc_begin_y);

        int arc_end_x = arc_radius * cos(arc_end_angle) + arc_center_x;
        int arc_end_y = arc_radius * sin(arc_end_angle) + arc_center_y;
        wxPoint arc_end_log = GetWorkshop()->ConvPntToLog(arc_end_x, arc_end_y);

        GraphObj* graph_obj_ptr = new ObjArc
            (node_data->node_settings.layer_number, node_data->node_settings.line_type,
             node_data->node_settings.line_width,
             arc_center_log, arc_begin_log, arc_end_log);
        GetWorkshop()->graph_objects.push_back(graph_obj_ptr);
        return {};
    }

    // A - Создание привязанной A-дуги.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodePicAHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodePicAHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        // Пока здесь заглушка без фактической реализации.
        return {};
    }

    // Arc - Создание свободной (отъединённой) ARC-дуги.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeAnnotateArcHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeAnnotateArcHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        if (node_data->args.size() != 6)
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}};  // Параметров должны быть строго 6.

        // Все аргументы должны быть целочисленными (три пары координат трёх кардинальных точек дуги).
        for (size_t i = 0; i < node_data->args.size(); ++i)
        {
            if (!holds_alternative<int64_t>(node_data->args[i]))
                return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};
        }

        int arc_center_x = get<int64_t>(node_data->args[0]),
            arc_center_y = get<int64_t>(node_data->args[1]);
        int arc_begin_x = get<int64_t>(node_data->args[2]),
            arc_begin_y = get<int64_t>(node_data->args[3]);
        int arc_end_x = get<int64_t>(node_data->args[4]),
            arc_end_y = get<int64_t>(node_data->args[5]);

        wxPoint arc_center_log = GetWorkshop()->ConvPntToLog(arc_center_x, arc_center_y);
        wxPoint arc_begin_log = GetWorkshop()->ConvPntToLog(arc_begin_x, arc_begin_y);
        wxPoint arc_end_log = GetWorkshop()->ConvPntToLog(arc_end_x, arc_end_y);

        GraphObj* graph_obj_ptr = new ObjArc
            (node_data->node_settings.layer_number, node_data->node_settings.line_type,
             node_data->node_settings.line_width, arc_center_log, arc_begin_log, arc_end_log);
        GetWorkshop()->graph_objects.push_back(graph_obj_ptr);
        return {};
    }

    // Arc - Создание привязанной ARC-дуги.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodePicArcHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodePicArcHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        // Пока здесь заглушка без фактической реализации.
        return {};
    }

    // C - Создание свободной (отъединённой) окружности.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeAnnotateCircleHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeAnnotateCircleHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        if (node_data->args.size() != 3)
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}};  // Параметров должны быть строго 3.

        // Все аргументы должны быть целочисленными (пара координат и радиус окружности).
        for (size_t i = 0; i < node_data->args.size(); ++i)
        {
            if (!holds_alternative<int64_t>(node_data->args[i]))
                return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};
        }

        int circle_center_x = get<int64_t>(node_data->args[0]),
            circle_center_y = get<int64_t>(node_data->args[1]),
            circle_radius = get<int64_t>(node_data->args[2]);
        wxPoint circle_center_log = GetWorkshop()->ConvPntToLog(circle_center_x, circle_center_y);

        GraphObj* graph_obj_ptr = new ObjCirc
            (node_data->node_settings.layer_number, node_data->node_settings.line_type,
             node_data->node_settings.line_width, circle_center_log, circle_radius);
        GetWorkshop()->graph_objects.push_back(graph_obj_ptr);
        return {};
    }

    // C - Создание привязанной окружности.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodePicCircleHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodePicCircleHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        // Пока здесь заглушка без фактической реализации.
        return {};
    }

    // L - Создание свободной (отъединённой) ломаной, являющейся цепью прямолинейных отрезков.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeAnnotateLineHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeAnnotateLineHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        if (node_data->args.size() > 0 && (node_data->args.size() % 2 == 0))
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}};  // Количество параметров должны быть четным.

        // Все аргументы должны быть целочисленными (набор из координатных пар последовательных вершин ломаной).
        for (size_t i = 0; i < node_data->args.size(); ++i)
        {
            if (!holds_alternative<int64_t>(node_data->args[i]))
                return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};
        }

        size_t span_count = node_data->args.size() / 2;
        int begin_span_vertex_x = get<int64_t>(node_data->args[0]),
            begin_span_vertex_y = get<int64_t>(node_data->args[1]);
        wxPoint begin_span_vertex_log = GetWorkshop()->ConvPntToLog(begin_span_vertex_x, begin_span_vertex_y);

        for (size_t span_index = 0; span_index < span_count; ++span_index)
        {
            size_t param_index = span_index * 2;
            int end_span_vertex_x = get<int64_t>(node_data->args[param_index]),
                end_span_vertex_y = get<int64_t>(node_data->args[param_index + 1]);
            wxPoint end_span_vertex_log = GetWorkshop()->ConvPntToLog(end_span_vertex_x, end_span_vertex_y);

            GraphObj* graph_obj_ptr = new ObjLine
                (node_data->node_settings.layer_number, node_data->node_settings.line_type,
                 node_data->node_settings.line_width, begin_span_vertex_log, end_span_vertex_log);
            GetWorkshop()->graph_objects.push_back(graph_obj_ptr);

            begin_span_vertex_log = end_span_vertex_log;
        }

        return {};
    }

    // L - Создание привязанной ломаной линии из прямолинейных отрезков.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodePicLineHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodePicLineHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        // Пока здесь заглушка без фактической реализации.
        return {};
    }

    // R - свободный прямоугольник.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeAnnotateRectangleHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeAnnotateRectangleHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        if (node_data->args.size() != 4)
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}};  // Параметров должны быть точно 4.

        // Все аргументы должны быть целочисленными (две пара координат противолежащих вершин прямоугольника).
        for (size_t i = 0; i < node_data->args.size(); ++i)
        {
            if (!holds_alternative<int64_t>(node_data->args[i]))
                return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};
        }

        int left_down_x = get<int64_t>(node_data->args[0]),
            left_down_y = get<int64_t>(node_data->args[1]);
        int right_up_x = get<int64_t>(node_data->args[2]),
            right_up_y = get<int64_t>(node_data->args[3]);
        wxRect rect_log = GetWorkshop()->ConvRectToLog(left_down_x, left_down_y, right_up_x, right_up_y);

        GraphObj* graph_obj_ptr = new ObjRect
            (node_data->node_settings.layer_number, node_data->node_settings.line_type,
             node_data->node_settings.line_width, rect_log);
        GetWorkshop()->graph_objects.push_back(graph_obj_ptr);

        return {};
    }

    // R - привязанный прямоугольник.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodePicRectangleHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodePicRectangleHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        // Пока здесь заглушка без фактической реализации.
        return {};
    }

    // T - свободная текстовая строка.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeAnnotateTextHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeAnnotateTextHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        if (node_data->args.size() != 3)
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}};  // Параметров должны быть точно 3.

        // Первый параметр текстовый (тело строки), а следующте два целочисленные (пара координат якорной точки).
        if (!holds_alternative<string>(node_data->args[0]) ||
            !holds_alternative<int64_t>(node_data->args[1]) ||
            !holds_alternative<int64_t>(node_data->args[2]))
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};

        string text_text = get<string>(node_data->args[0]);
        int text_anchor_x = get<int64_t>(node_data->args[1]),
            text_anchor_y = get<int64_t>(node_data->args[2]);
        wxPoint text_anchor_log = GetWorkshop()->ConvPntToLog(text_anchor_x, text_anchor_y);

        GraphObj* graph_obj_ptr = new ObjText
            (node_data->node_settings.layer_number, text_anchor_log,
             node_data->node_settings.text_orient, node_data->node_settings.text_height,
             node_data->node_settings.text_align, text_text);
        GetWorkshop()->graph_objects.push_back(graph_obj_ptr);

        return {};
    }

    // T - привязанная текстовая строка.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodePicTextHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodePicTextHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        // Пока здесь заглушка без фактической реализации.
        return {};
    }

    // Fr - свободный заполненный прямоугольник.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeAnnotateFillRectHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeAnnotateFillRectHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        if (node_data->args.size() != 4)
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}};  // Параметров должны быть точно 4.

        // Все аргументы должны быть целочисленными (две пары координат противоположных вершин прямоугольника).
        for (size_t i = 0; i < node_data->args.size(); ++i)
        {
            if (!holds_alternative<int64_t>(node_data->args[i]))
                return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};
        }

        int left_down_x = get<int64_t>(node_data->args[0]),
            left_down_y = get<int64_t>(node_data->args[1]);
        int right_up_x = get<int64_t>(node_data->args[2]),
            right_up_y = get<int64_t>(node_data->args[3]);
        wxRect rect_log = GetWorkshop()->ConvRectToLog(left_down_x, left_down_y, right_up_x, right_up_y);
        unsigned char rect_fill_color = node_data->node_settings.layer_number < static_cast<int>(GetWorkshop()->layers.size()) ?
            GetWorkshop()->layers[node_data->node_settings.layer_number].layer_color : 0;

        GraphObj* graph_obj_ptr = new ObjFillRect
            (node_data->node_settings.layer_number, rect_fill_color, rect_log);
        GetWorkshop()->graph_objects.push_back(graph_obj_ptr);

        return {};
    }

    // Fr - привязанный заполненный прямоугольник.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodePicFillRectHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodePicFillRectHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        // Пока здесь заглушка без фактической реализации.
        return {};
    }

    // At - текстовый атрибут пользователя. Он всегда привязанный, принадлежащий какому-то символу (компоненту) базы данных.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeAtHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeAtHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        // Пока здесь заглушка без фактической реализации.
        return {};
    }

    // Ly - выбор слоя размещения примитивов.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeLyHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        if (GetWorkshop()->IsPrevNodeContainer(node_data))
            return {};
        else
            return {{PCADLoadError::LOAD_FILE_COMAMND_UNACCEPTABLE_HERE, "Ly"}};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeLyHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        if (node_data->args.size() != 1)
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}};  // Настройка имеет только один обязательный параметр.
        // Этот аргумент - строка.
        if (!holds_alternative<string>(node_data->args[0]))
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};

        PDIFFileWorkshop* my_workshop = GetWorkshop();
        int prev_stack_index = static_cast<int>(my_workshop->node_stack.size()) - 2;
        if (int find_layer_number = my_workshop->FindLayerByName(get<string>(node_data->args[0]));
            prev_stack_index >= 0 && find_layer_number >= 0)
        {
            my_workshop->node_stack[prev_stack_index].node_settings.layer_number = find_layer_number;
            return {};
        }
        else
        {
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_VALUE, get<string>(node_data->args[0])}};
        }
    }

    // Ls - назначение типа линии.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeLsHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        if (GetWorkshop()->IsPrevNodeContainer(node_data))
            return {};
        else
            return {{PCADLoadError::LOAD_FILE_COMAMND_UNACCEPTABLE_HERE, "Ls"}};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeLsHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        if (node_data->args.size() != 1)
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}};  // Настройка имеет только один обязательный параметр.
        // Этот аргумент - строка.
        if (!holds_alternative<string>(node_data->args[0]))
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};

        PDIFFileWorkshop* my_workshop = GetWorkshop();
        int prev_stack_index = static_cast<int>(my_workshop->node_stack.size()) - 2;
        if (auto line_type = my_workshop->FindLineType(get<string>(node_data->args[0])); line_type)
        {
            my_workshop->node_stack[prev_stack_index].node_settings.line_type = line_type.value();
            return {};
        }
        else
        {
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_VALUE, get<string>(node_data->args[0])}};
        }
    }

    // Wd - указание толщины линии.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeWdHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        if (GetWorkshop()->IsPrevNodeContainer(node_data))
            return {};
        else
            return {{PCADLoadError::LOAD_FILE_COMAMND_UNACCEPTABLE_HERE, "Wd"}};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeWdHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        if (node_data->args.size() != 1)
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}};  // Настройка имеет только один обязательный параметр.
        // Этот аргумент - целое число.
        if (!holds_alternative<int64_t>(node_data->args[0]))
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};

        PDIFFileWorkshop* my_workshop = GetWorkshop();
        int prev_stack_index = static_cast<int>(my_workshop->node_stack.size()) - 2;
        if (
            optional<int> checked_line_width = my_workshop->CheckIntValue(get<int64_t>(node_data->args[0]), NodeSettings::MAX_LINE_WIDTH);
            checked_line_width)
        {
            my_workshop->node_stack[prev_stack_index].node_settings.line_width = checked_line_width.value();
            return {};
        }
        else
        {
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_VALUE, {}}};
        }
    }

    // Группа объектов-обработчиков команд настройки параметров текстового примитива.
    // Ts - высота текста (высота глифов используемого шрифта)
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeTsHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        if (GetWorkshop()->IsPrevNodeContainer(node_data))
            return {};
        else
            return {{PCADLoadError::LOAD_FILE_COMAMND_UNACCEPTABLE_HERE, "Ts"}};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeTsHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        if (node_data->args.size() != 1)
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}};  // Настройка имеет только один обязательный параметр.
        // Этот аргумент - целое число.
        if (!holds_alternative<int64_t>(node_data->args[0]))
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};

        PDIFFileWorkshop* my_workshop = GetWorkshop();
        int prev_stack_index = static_cast<int>(my_workshop->node_stack.size()) - 2;
        if (optional<int> checked_text_height = my_workshop->CheckIntValue(get<int64_t>(node_data->args[0]), NodeSettings::MAX_TEXT_HEIGHT);
            checked_text_height)
        {
            my_workshop->node_stack[prev_stack_index].node_settings.text_height = checked_text_height.value();
            return {};
        }
        else
        {
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_VALUE, {}}};
        }
    }

    // Tj - выравнивание текста.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeTjHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        if (GetWorkshop()->IsPrevNodeContainer(node_data))
            return {};
        else
            return {{PCADLoadError::LOAD_FILE_COMAMND_UNACCEPTABLE_HERE, "Tj"}};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeTjHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        if (node_data->args.size() != 1)
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}};  // Настройка имеет только один обязательный параметр.
        // Этот аргумент - строка.
        if (!holds_alternative<string>(node_data->args[0]))
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};

        PDIFFileWorkshop* my_workshop = GetWorkshop();
        int prev_stack_index = static_cast<int>(my_workshop->node_stack.size()) - 2;
        if (auto text_align = my_workshop->FindTextAlign(get<string>(node_data->args[0])); text_align && prev_stack_index >= 0)
        {
            my_workshop->node_stack[prev_stack_index].node_settings.text_align = text_align.value();
            return {};
        }
        else
        {
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_VALUE, get<string>(node_data->args[0])}};
        }
    }

    // Tr - вращение (угол поворота) текста.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeTrHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        if (GetWorkshop()->IsPrevNodeContainer(node_data))
            return {};
        else
            return {{PCADLoadError::LOAD_FILE_COMAMND_UNACCEPTABLE_HERE, "Tr"}};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeTrHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        if (node_data->args.size() != 1)
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}};  // Настройка имеет только один обязательный параметр.
        // Этот аргумент - целое число.
        if (!holds_alternative<int64_t>(node_data->args[0]))
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};

        PDIFFileWorkshop* my_workshop = GetWorkshop();
        int prev_stack_index = static_cast<int>(my_workshop->node_stack.size()) - 2;
        if (optional<int> checked_text_rotation = my_workshop->CheckIntValue(get<int64_t>(node_data->args[0]), 3);
            checked_text_rotation && prev_stack_index >= 0)
        {
            NodeSettings& node_settings = my_workshop->node_stack[prev_stack_index].node_settings;
            node_settings.text_rotation_index = checked_text_rotation.value();
            if (auto text_orient_checked = my_workshop->FindTextOrientation(node_settings.text_rotation_index, node_settings.is_text_mirror);
                text_orient_checked)
                node_settings.text_orient = text_orient_checked.value();
            return {};
        }
        else
        {
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_VALUE, {}}};
        }
    }

    // Tm - зеркальность текста.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeTmHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        if (GetWorkshop()->IsPrevNodeContainer(node_data))
            return {};
        else
            return {{PCADLoadError::LOAD_FILE_COMAMND_UNACCEPTABLE_HERE, "Tm"}};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeTmHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        if (node_data->args.size() != 1)
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}};  // Настройка имеет только один обязательный параметр.
        // Этот аргумент - строка.
        if (!holds_alternative<string>(node_data->args[0]))
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};

        std::string& text_mirror_flag = get<string>(node_data->args[0]);
        PDIFFileWorkshop* my_workshop = GetWorkshop();
        int prev_stack_index = static_cast<int>(my_workshop->node_stack.size()) - 2;
        NodeSettings& node_settings = my_workshop->node_stack[prev_stack_index].node_settings;

        if (text_mirror_flag == "N")
            node_settings.is_text_mirror = false;
        else if (text_mirror_flag == "Y")
            node_settings.is_text_mirror = true;
        else
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_VALUE, text_mirror_flag}};

        if (auto text_orient_checked = my_workshop->FindTextOrientation(node_settings.text_rotation_index, node_settings.is_text_mirror);
            text_orient_checked)
            node_settings.text_orient = text_orient_checked.value();
        return {};
    }
} // namespace HandlerPDIF
