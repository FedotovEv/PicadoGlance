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
    // Радиус видимого изображения межсоединения в схеме.
    int PDIFFileWorkshop::VIA_POINT_RADIUS_SCH = 10;
    // Радиус видимого изображения переходного отверстия на плате.
    int PDIFFileWorkshop::VIA_POINT_RADIUS_PCB = 10;

    const unordered_map<string_view, PDIFFileWorkshop::PDIFKeywords> PDIFFileWorkshop::keyword_to_enum_value
    {
        // Имена контейнеров - разделы и подразделы.
        {"COMPONENT"sv, PDIFKeywords::PDIF_KEY_COMPONENT},      // Корень всей древовидной иерархии PDIF-базы данных.
        // Узлы "ENVIRONMENT", "USER", "DISPLAY", "DETAIL" и "SYMBOL" являются базисными секциями общего корня "COMPONENT" и содержат в
        // своих потомках всю фактическую информацию о проекте
        {"ENVIRONMENT"sv, PDIFKeywords::PDIF_KEY_ENVIRONMENT},  // Подраздел (координаты размещения - COMPONENT) с определением свойств "среды окружения".
        {"USER"sv, PDIFKeywords::PDIF_KEY_USER},                // Похожее поддерево (координаты размещения - COMPONENT) с определением пользовательских настроек.
        {"VIEW"sv, PDIFKeywords::PDIF_KEY_VIEW},                // Описание пользовательских настроек текущего вида схемы - окно, масштаб, свойства слоя (место размещения - USER).
        {"DETAIL"sv, PDIFKeywords::PDIF_KEY_DETAIL},            // Корень поддерева (размещение - COMPONENT), вмещающего содержательную информацию для проектов
                                                                // целостных устройств (схем или плат, режим DETL).
        {"SYMBOL"sv, PDIFKeywords::PDIF_KEY_SYMBOL},            // Корень поддерева (размещение - COMPONENT), вмещающего содержательную информацию для проекта
                                                                // отдельного компонента (его УГО или конструктива, режим SYMB).
        {"SUBCOMP"sv, PDIFKeywords::PDIF_KEY_SUBCOMP},          // Головной подузел всех данных описания встроенных подструктур (приборов и радиокомпонентов).
                                                                // Размещается в DETAIL.
        {"ANNOTATE"sv, PDIFKeywords::PDIF_KEY_ANNOTATE},        // Поддерево свободной (иллюстративной) графики проекта - графики, не связанной с каким-либо его
                                                                // элементом или компонентом. Размещается также в DETAIL.
        {"COMP_DEF"sv, PDIFKeywords::PDIF_KEY_COMPDEF}, // COMP_DEF - верхний узел подраздела описания встроенного радиокомпонента (расположение - DETAIL/SUBCOMP).
        {"NET_DEF"sv, PDIFKeywords::PDIF_KEY_NETDEF},   // NET_DEF - верхний узел секции определения проводящей цепи схемы или платы (поддерево узла DETAIL).
        {"PIN_DEF"sv, PDIFKeywords::PDIF_KEY_PINDEF},   // PIN_DEF - подсекция COMP_DEF описания выводов некоторого радиокомпонента.
        {"PIC"sv, PDIFKeywords::PDIF_KEY_PIC},          // PIC - контейнер-вместилище видимых графических примитивов, образующих начертание символа (его графику).
                                                        // Размещается в SYMBOL, DETAIL/PAD_STACK/PAD_DEF или DETAIL/SUBCOMP/COMP_DEF).
        {"ATR"sv, PDIFKeywords::PDIF_KEY_ATR},          // Головной узел, поддерево которого определяет атрибуты вмещающей сущности.
        {"IN"sv, PDIFKeywords::PDIF_KEY_IN},            // Атрибутное поддерево "внутренних" атрибутов (положение - ../ATR).
        {"EX"sv, PDIFKeywords::PDIF_KEY_EX},            // Атрибутное поддерево "внешних" атрибутов (положение такое же - ../ATR).
        {"I"sv, PDIFKeywords::PDIF_KEY_I},       // I - главный подраздел описания вставки экземпляра компонента (располагается в DETAIL/SUBCOMP).
        {"N"sv, PDIFKeywords::PDIF_KEY_N},       // N - главный подраздел с описанием токопроводящей цепи (размещается в DETAIL/NET_DEF).
        {"P"sv, PDIFKeywords::PDIF_KEY_P},       // P - подраздел раздела PIN_DEF с описанием отдельной "ножки" компонента (положение - DETAIL/SUBCOMP/COMP_DEF/PIN_DEF).
        {"DG"sv, PDIFKeywords::PDIF_KEY_DG},     // DG - подраздел раздела NET_DEF/N (описание видимых частей цепей).
        {"PKG"sv, PDIFKeywords::PDIF_KEY_PKG},   // PKG - Информация об упаковке компонента в составе УГО или схемы. Размещение - SYMBOL или DETAIL/SUBCOMP/COMP_DEF.
        {"SPKG"sv, PDIFKeywords::PDIF_KEY_SPKG}, // SPKG - Информация об упаковке компонента в составе конструктива или на плате.
                                                 // Размещение аналогично - SYMBOL или DETAIL/SUBCOMP/COMP_DEF.
        // Все упомянутые ниже дополнительные "вставочные" подблоки находятся по пути ../DETAIL/SUBCOMP/I.
        {"CN"sv, PDIFKeywords::PDIF_KEY_CN},     // CN - Подсекция I с информацией о соединении выводов вставленного экземпляра элемента с цепями схемы (или платы).
        {"IPT"sv, PDIFKeywords::PDIF_KEY_IPT},   // IPT - Подсекция I с данными об индивидуальных (для данной вставки) типах ножек (применяется только при описании плат).
        {"ASG"sv, PDIFKeywords::PDIF_KEY_ASG},   // ASG - Дополнительная упаковочная информация о свойствах вставленной копии компонента (применяется только для схем).
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
        // Характеристические терминалы, определяющие некоторые параметры отдельного вывода ("ножки") некоторого радиокомпонента.
        {"Pt"sv, PDIFKeywords::PDIF_KEY_PT},      // Pt - символьный (текстовый, для УГО символа) или номерной (для конструктива символа) тип данного вывода радиокомпонента.
        {"Lq"sv, PDIFKeywords::PDIF_KEY_LQ},      // Lq - код эквивалентности (взаимозаменяемости) данного вывода радиокомпонента.
        {"Ploc"sv, PDIFKeywords::PDIF_KEY_PLOC},  // Ploc - точка расположения вывода.
        // Составляющие проводящих цепей схем и печатных плат.
        {"W"sv, PDIFKeywords::PDIF_KEY_W},        // W - Проводящая ломаная (набор смежных проводящих прямолинейных отрезков).
        {"V"sv, PDIFKeywords::PDIF_KEY_V},        // V - Переходное отверстие (применяется только на печатных платах).
        {"Nn"sv, PDIFKeywords::PDIF_KEY_NN},      // Nn - Положение видимого имени цепи.
        // Атрибуты некоторой сущности.
        {"Org"sv, PDIFKeywords::PDIF_KEY_ORG},    // Org - Точка привязки символа.
        {"Ty"sv,  PDIFKeywords::PDIF_KEY_TY},     // Ty - Тип компонента.
        {"Smd"sv, PDIFKeywords::PDIF_KEY_SMD},    // Smd - Прибор, монтируемый на поверхность (только для печатной платы).
        {"Jmp"sv, PDIFKeywords::PDIF_KEY_JMP},    // Jmp - "Jumper" (только для печатной платы).
        {"Pl"sv, PDIFKeywords::PDIF_KEY_PL},      // Pl - Координаты местоположения.
        {"Sc"sv, PDIFKeywords::PDIF_KEY_SC},      // Sc - Масштабные коэффициенты по X и Y.
        {"Ro"sv, PDIFKeywords::PDIF_KEY_RO},      // Ro - Угол вращения.
        {"Mr"sv, PDIFKeywords::PDIF_KEY_MR},      // Mr -  Статус зеркальности (только для базы данных принципиальной схемы).
        {"Nl"sv, PDIFKeywords::PDIF_KEY_NL},      // Nl - Местоположение имени копии компонента.
        {"Ps"sv, PDIFKeywords::PDIF_KEY_PS},      // Ps - Сторона размещения компонентов (только для печатной платы).
        {"Pa"sv, PDIFKeywords::PDIF_KEY_PA},      // Pa - Угол установки компонента.
        {"Un"sv, PDIFKeywords::PDIF_KEY_UN},      // Un - Присваиваемое пользователем имя (только для PC-CAPS).
        {"At"sv, PDIFKeywords::PDIF_KEY_AT},      // At - текстовый пользовательский "внешний" атрибут.
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
        {"Tm"sv, PDIFKeywords::PDIF_KEY_TM},
        // Ключи особых свойств вставки радиокомпонента
        {"Rd"sv, PDIFKeywords::PDIF_KEY_RD},        // Rd - свойства конструкторского обозначения (текст и координаты местоположения) вставленного радиокомпонента.
        {"Pn"sv, PDIFKeywords::PDIF_KEY_PN},        // Pn - свойства некоторого отдельного вывода вставленной копии компонента (его наименование и координаты положения).
        // Ключевые слова узлов, описывающих упаковку компонента в корпусе (соответствие выводов радиокомпонента ножкам корпуса).
        // Ключи узлов команды SPKG (корпусировка конструктива).
        {"Sna"sv, PDIFKeywords::PDIF_KEY_SNA},      // Sna - имя секции (только для PC-CARDS).
        {"Sp"sv, PDIFKeywords::PDIF_KEY_SP},        // Sp - Карта ножек символа. Показывает соответствие выводов логического вентиля ножкам физического прибора.
        {"Apn"sv, PDIFKeywords::PDIF_KEY_APN},      // Apn - алфавитно-цифровой номер ножки.
        // Ключи узлов команды PKG (корпусировка УГО).
        {"Rdl"sv, PDIFKeywords::PDIF_KEY_RDL},      // Rdl - место показа позиционного обозначения.
        {"Pnl"sv, PDIFKeywords::PDIF_KEY_PNL},      // Pnl - места для показа номеров ножке.
        {"Sd"sv, PDIFKeywords::PDIF_KEY_SD},        // Sd - секция  упаковки (физического прибора и номера выводов.
        {"Pid"sv, PDIFKeywords::PDIF_KEY_PID}       // Pid - упаковочный ID (только для PC-CAPS).
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
                PDIFOps::PDIFOP_CONT_COMPONENT                  // Считывание внутреннего имени базы данных.
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
                PDIFOps::PDIFOP_CONT_COMPDEF             // Описание нового символа базы данных (УГО или конструктива).
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
        {PDIFKeywords::PDIF_KEY_ANNOTATE,
            {
                PDIFKeywords::PDIF_KEY_ANNOTATE,         // Ключ "ANNOTATE".
                false,                                   // Нетерминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе "COMPONENT/DETAIL".
                {PDIFKeywords::PDIF_KEY_COMPONENT, PDIFKeywords::PDIF_KEY_DETAIL},
                {},                                      // Не имеет аргументов.
                PDIFOps::PDIFOP_NOPE                     // "Чистый" подраздел.
            }
        },
        // SUBCOMP - раздел определения вложенных радиокомпонентов, их собственного описания и вставок их экземпляров в схему или на плату.
        {PDIFKeywords::PDIF_KEY_SUBCOMP,
            {
                PDIFKeywords::PDIF_KEY_SUBCOMP,          // Ключ "SUBCOMP".
                false,                                   // Нетерминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе "COMPONENT/DETAIL".
                {PDIFKeywords::PDIF_KEY_COMPONENT, PDIFKeywords::PDIF_KEY_DETAIL},
                {},                                      // Не имеет аргументов.
                PDIFOps::PDIFOP_NOPE                     // "Чистая" секция, не выполняет никаких операций.
            }
        },
        // NET_DEF - секция описания цепей - проводящих линий, соединяющих выводы радиокомпонентов в схеме или на плате.
        {PDIFKeywords::PDIF_KEY_NETDEF,
            {
                PDIFKeywords::PDIF_KEY_NETDEF,             // Ключ "NET_DEF".
                false,                                     // Нетерминал.
                false,                                     // Не является установочной операцией.
                // Находится в разделе "../DETAIL".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_DETAIL},
                {},                                        // Не имеет аргументов.
                PDIFOps::PDIFOP_NOPE                       // "Чистый раздел" без содержательных оперций.
            }
        },
        // COMP_DEF - секция описания одного вложенного радиокомпонента - его очертания, выводов, упаковки и атрибутов.
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
                PDIFOps::PDIFOP_CONT_COMPDEF               // Описание нового вложенного символа базы данных (УГО или конструктива,
                                                           // из которых набирается схема или устройство).
            }
        },
        // PIN_DEF - узел-дескриптор выводов радиокомпонента ("внешнего" или вложенного).
        {PDIFKeywords::PDIF_KEY_PINDEF,
            {
                PDIFKeywords::PDIF_KEY_PINDEF,           // Ключ "PIN_DEF".
                false,                                   // Нетерминал.
                false,                                   // Не является установочной операцией.
                // Вариант, который находится в разделе ".../SUBCOMP/COMP_DEF" (выводы вложенного символа).
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_SUBCOMP, PDIFKeywords::PDIF_KEY_COMPDEF},
                {},                                      // Не имеет аргументов.
                PDIFOps::PDIFOP_NOPE                     // "Чистый" подраздел.
            }
        },
        {PDIFKeywords::PDIF_KEY_PINDEF,
            {
                PDIFKeywords::PDIF_KEY_PINDEF,           // Ключ "PIN_DEF".
                false,                                   // Нетерминал.
                false,                                   // Не является установочной операцией.
                // Вариант, который находится в разделе ".../SYMBOL" (выводы внешнего символа).
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_SYMBOL},
                {},                                      // Не имеет аргументов.
                PDIFOps::PDIFOP_NOPE                     // "Чистый" подраздел.
            }
        },
        // PKG - упаковочные данные радиокомпонента (только для УГО, который также может быть внешним или вложенным).
        {PDIFKeywords::PDIF_KEY_PKG,
            {
                PDIFKeywords::PDIF_KEY_PKG,              // Ключ "PKG".
                false,                                   // Нетерминал.
                false,                                   // Не является установочной операцией.
                // Вариант, который находится в разделе ".../SUBCOMP/COMP_DEF" (упаковка для вложенного УГО символа).
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_SUBCOMP, PDIFKeywords::PDIF_KEY_COMPDEF},
                {},                                      // Не имеет аргументов.
                PDIFOps::PDIFOP_CONT_PKG                 // Создание и подготовка к работе блока данных с упаковочной информацией для УГО.
            }
        },
        {PDIFKeywords::PDIF_KEY_PKG,
            {
                PDIFKeywords::PDIF_KEY_PKG,              // Ключ "PKG".
                false,                                   // Нетерминал.
                false,                                   // Не является установочной операцией.
                // Вариант, который находится в разделе ".../SYMBOL" (упаковка внешнего УГО символа).
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_SYMBOL},
                {},                                      // Не имеет аргументов.
                PDIFOps::PDIFOP_CONT_PKG                 // Создание и подготовка к работе блока данных с упаковочной информацией для УГО.
            }
        },
        // SPKG - упаковочные данные радиокомпонента (этот вариант только для конструктива, который также может быть внешним или вложенным).
        {PDIFKeywords::PDIF_KEY_SPKG,
            {
                PDIFKeywords::PDIF_KEY_SPKG,             // Ключ "SPKG".
                false,                                   // Нетерминал.
                false,                                   // Не является установочной операцией.
                // Вариант, который находится в разделе ".../SUBCOMP/COMP_DEF" (упаковка для вложенного конструктива символа).
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_SUBCOMP, PDIFKeywords::PDIF_KEY_COMPDEF},
                {},                                      // Не имеет аргументов.
                PDIFOps::PDIFOP_CONT_SPKG                // Создание и подготовка к работе блока данных с упаковочной информацией конструктива.
            }
        },
        {PDIFKeywords::PDIF_KEY_SPKG,
            {
                PDIFKeywords::PDIF_KEY_SPKG,             // Ключ "SPKG".
                false,                                   // Нетерминал.
                false,                                   // Не является установочной операцией.
                // Вариант, который находится в разделе ".../SYMBOL" (упаковка внешнего конструктива символа).
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_SYMBOL},
                {},                                      // Не имеет аргументов.
                PDIFOps::PDIFOP_CONT_SPKG                // Создание и подготовка к работе блока данных с упаковочной информацией конструктива.
            }
        },
        // PIC-секция с графикой компонента. Может размещаться либо в "../SUBCOMP/COMP_DEF", либо в "../SYMBOL".
        {PDIFKeywords::PDIF_KEY_PIC,
            {
                PDIFKeywords::PDIF_KEY_PIC,              // Ключ "PIC".
                false,                                   // Нетерминал.
                false,                                   // Не является установочной операцией.
                // Вариант, который находится в разделе ".../SUBCOMP/COMP_DEF".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_SUBCOMP, PDIFKeywords::PDIF_KEY_COMPDEF},
                {},                                      // Не имеет аргументов.
                PDIFOps::PDIFOP_NOPE                     // "Чистый" подраздел.
            }
        },
        {PDIFKeywords::PDIF_KEY_PIC,
            {
                PDIFKeywords::PDIF_KEY_PIC,              // Ключ "PIC".
                false,                                   // Нетерминал.
                false,                                   // Не является установочной операцией.
                // Вариант, который находится в разделе ".../SYMBOL".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_SYMBOL},
                {},                                      // Не имеет аргументов.
                PDIFOps::PDIFOP_NOPE                     // "Чистый" подраздел.
            }
        },
        // I - подраздел описания отдельного включения в схему или установки на плату определённого радиоэлемента.
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
                PDIFOps::PDIFOP_CONT_I                     // Создаёт и подготавливает к работе новое описание вставки копии некоторого радиокомпонента.
            }
        },
        // Подсекция ASG общего раздела описания отдельной вставки компонента (раздела I). Она содержит некоторую информацию о вставленной копии компонента,
        // кроме той, которая размещается в узле CN и подразделе атрибутов вставки.
        {PDIFKeywords::PDIF_KEY_ASG,
            {
                PDIFKeywords::PDIF_KEY_ASG,                // Ключ "ASG".
                false,                                     // Нетерминал.
                false,                                     // Не является установочной операцией.
                // Находится в разделе "../DETAIL/SUBCOMP/I".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_DETAIL, PDIFKeywords::PDIF_KEY_SUBCOMP, PDIFKeywords::PDIF_KEY_I},
                {},                                        // Не имеет (обязательных) аргументов.
                PDIFOps::PDIFOP_NOPE                       // "Чистый" подраздел, сам по себе никаких операций не выполняет.
            }
        },
        // Подраздел описания некоторой проводящей цепи (как в схеме, так и на печатной плате).
        {PDIFKeywords::PDIF_KEY_N,
            {
                PDIFKeywords::PDIF_KEY_N,                  // Ключ "N".
                false,                                     // Нетерминал.
                false,                                     // Не является установочной операцией.
                // Находится в разделе "../DETAIL/NET_DEF".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_DETAIL, PDIFKeywords::PDIF_KEY_NETDEF},
                {},                                        // Не имеет аргументов.
                PDIFOps::PDIFOP_CONT_N                     // Создаёт и подготавливает к работе новое описание проводящей цепи схемы или платы.
            }
        },
        // Подсекция DG общего раздела описания единичной цепи (раздела N). Именно она содержит всю действительную информацию о цепи, кроме
        // той, которая размещается в подразделе атрибутов цепи.
        {PDIFKeywords::PDIF_KEY_DG,
            {
                PDIFKeywords::PDIF_KEY_DG,                 // Ключ "DG".
                false,                                     // Нетерминал.
                false,                                     // Не является установочной операцией.
                // Находится в разделе "../DETAIL/NET_DEF/N".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_DETAIL, PDIFKeywords::PDIF_KEY_NETDEF, PDIFKeywords::PDIF_KEY_N},
                {},                                        // Не имеет аргументов.
                PDIFOps::PDIFOP_NOPE                       // "Чистый" подраздел, сам по себе никаких операций не выполняет.
            }
        },
        // Подраздел описания свойств некоторого вывода ("ножки") радиокомпонента.
        {PDIFKeywords::PDIF_KEY_P,
            {
                PDIFKeywords::PDIF_KEY_P,                  // Ключ "P".
                false,                                     // Нетерминал.
                false,                                     // Не является установочной операцией.
                // Находится в разделе "../SUBCOMP/COMP_DEF/PIN_DEF".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_SUBCOMP, PDIFKeywords::PDIF_KEY_COMPDEF, PDIFKeywords::PDIF_KEY_PINDEF},
                {
                    // Один обязательный аргумент - имя данного вывода ("ножки").
                    {{ValueType::ARG_VALUE_STRING}, false}
                },
                PDIFOps::PDIFOP_CONT_P                     // Создаёт и подготавливает к работе новое описание вывода текущей радиодетали.
            }
        },
        // Ключ "ATR" - подраздел атрибутов текущей анализируемой сущности (вставки, символа или цепи).
        // Встака - копия (вставленный экземпляр) некоторого радиокомпонента. Символ - УГО или конструктив компонента (основного или вложенного).
        // Цепь - проводящая линия, имеющаяся в схеме или проложенная по плате.
        {PDIFKeywords::PDIF_KEY_ATR,                     // Атрибуты библиотечного символа (символа, описанного на верхнем уровне файла).
            {
                PDIFKeywords::PDIF_KEY_ATR,              // Ключ "ATR".
                false,                                   // Нетерминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе ".../SYMBOL".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_SYMBOL},
                {},                                      // Не имеет аргументов.
                PDIFOps::PDIFOP_NOPE                     // Только подраздел, содержит различные ("внутренние" и "внешние") атрибуты символа.
            }
        },
        {PDIFKeywords::PDIF_KEY_ATR,                     // Атрибуты копии (вставленного экземпляра) некоторого радиокомпонента.
            {
                PDIFKeywords::PDIF_KEY_ATR,              // Ключ "ATR".
                false,                                   // Нетерминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе ".../DETAIL/SUBCOMP/I".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_DETAIL, PDIFKeywords::PDIF_KEY_SUBCOMP, PDIFKeywords::PDIF_KEY_I},
                {},                                      // Не имеет аргументов.
                PDIFOps::PDIFOP_NOPE                     // Только подраздел, содержит различные ("внутренние" и "внешние") атрибуты отдельного
                                                         // подсоединения компонента (его включения в схему).
            }
        },
        {PDIFKeywords::PDIF_KEY_ATR,                     // Атрибуты вложенного символа (символа, описанного в списке вложений какой-либо схемы или платы).
            {
                PDIFKeywords::PDIF_KEY_ATR,              // Ключ "ATR".
                false,                                   // Нетерминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе ".../DETAIL/SUBCOMP/COMP_DEF".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_DETAIL, PDIFKeywords::PDIF_KEY_SUBCOMP, PDIFKeywords::PDIF_KEY_COMPDEF},
                {},                                      // Не имеет аргументов.
                PDIFOps::PDIFOP_NOPE                     // Только подраздел, содержит различные ("внутренние" и "внешние") атрибуты вложенного символа.
            }
        },
        {PDIFKeywords::PDIF_KEY_ATR,                     // Атрибуты электропроводящей цепи.
            {
                PDIFKeywords::PDIF_KEY_ATR,              // Ключ "ATR".
                false,                                   // Нетерминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе ".../DETAIL/NET_DEF/N".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_DETAIL, PDIFKeywords::PDIF_KEY_NETDEF, PDIFKeywords::PDIF_KEY_N},
                {},                                      // Не имеет аргументов.
                PDIFOps::PDIFOP_NOPE                     // Только подраздел, содержит различные ("внутренние" и "внешние") атрибуты отдельной цепи.
            }
        },
        // Два непосредственных потомка секции атрибутов ATR.
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
        {PDIFKeywords::PDIF_KEY_DBTYPE,
            {
                PDIFKeywords::PDIF_KEY_DBTYPE,           // Ключ "DBtype".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе "COMPONENT/ENVIRONMENT".
                {PDIFKeywords::PDIF_KEY_COMPONENT, PDIFKeywords::PDIF_KEY_ENVIRONMENT},
                {   // Обязательный параметр - типа проекта.
                    {{ValueType::ARG_VALUE_STRING}, false}
                },
                PDIFOps::PDIFOP_DBTYPE          // Установка типа проекта - схема или конструктив.
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
                {   // Установка локальной (для отдельного многоугольника) ширины апертуры для рисования границ полигона.
                    {{ValueType::ARG_VALUE_INT}, false}
                },
                PDIFOps::PDIFOP_POLYAP_LOC          // Местная установка ширины апертуры для рисовки данного полигона.
            }
        },
        // Команды рисования элементарных графических примитивов.
        // Многоугольник (полигон).
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
        // Графический примитив - дуга "старого" типа.
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
        // Примитив - дуга "нового" типа.
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
        // Графический примитив - окружность.
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
        // Графический примитив - ломаная линия из отрезков.
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
        // Графический примитив - прямоугольник без заполнения.
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
        // Графический примитив - текст.
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
        // Графический примитив - заполненный прямоугольник.
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
        // Узлы описания вставки компонентов в схему или на плату (разделы подсекции I).
        // CN - описатель соединений выводов радиокомпонента с токопроводящимми цепями схемы или платы.
        {PDIFKeywords::PDIF_KEY_CN,
            {
                PDIFKeywords::PDIF_KEY_CN,               // Ключ "CN".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе ".../DETAIL/SUBCOMP/I".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_DETAIL, PDIFKeywords::PDIF_KEY_SUBCOMP, PDIFKeywords::PDIF_KEY_I},
                { // Аргументы - последовательность строковых пар, первый член каждой из них - имя присоединяемого вывода, а второй - имя присоединяющей цепи.
                    {{ValueType::ARG_VALUE_STRING, ValueType::ARG_VALUE_STRING}, true}
                },
                PDIFOps::PDIFOP_INS_CN                   // Обработчик списка присоединений вставленного радиокомпонента.
            }
        },
        // Rd - задание позиционного (конструкторского) обозначения вставленного экземпляра радиокомпонента.
        {PDIFKeywords::PDIF_KEY_RD,
            {
                PDIFKeywords::PDIF_KEY_RD,               // Ключ "Rd".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе ".../DETAIL/SUBCOMP/I/ASG".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_DETAIL, PDIFKeywords::PDIF_KEY_SUBCOMP, PDIFKeywords::PDIF_KEY_I, PDIFKeywords::PDIF_KEY_ASG},
                { // Аргументы - (REFDES, (X, Y)) - текст позиционного обозначения и координаты его якорной точки.
                    {{ValueType::ARG_VALUE_STRING, ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT}, false}
                },
                PDIFOps::PDIFOP_INS_RD                   // Обработчик назначения позиционного обозначения вставленной копии компонента.
            }
        },
        // Pn - указание текста и размещения подписи вывода вставленного радиокомпонента.
        {PDIFKeywords::PDIF_KEY_PN,
            {
                PDIFKeywords::PDIF_KEY_PN,               // Ключ "Pn".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе ".../DETAIL/SUBCOMP/I/ASG".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_DETAIL, PDIFKeywords::PDIF_KEY_SUBCOMP, PDIFKeywords::PDIF_KEY_I, PDIFKeywords::PDIF_KEY_ASG},
                { // Аргументы - (PIN_TEXT, (X, Y)) - текст подписи вывода и координаты его якорной точки.
                    {{ValueType::ARG_VALUE_STRING, ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT}, false}
                },
                PDIFOps::PDIFOP_INS_PN                   // Обработчик указания положения и содержимого подписи конкретного вывода компонента (его "номера ножки").
            }
        },
        // Узлы описания проводящий цепей схемы или платы (разделы подсекции NET_DEF).
        // V - точка межсоединения или переходное отверстие в составе цепи.
        {PDIFKeywords::PDIF_KEY_V,
            {
                PDIFKeywords::PDIF_KEY_V,                // Ключ "V".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе ".../DETAIL/SUBCOMP/I/ASG".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_DETAIL, PDIFKeywords::PDIF_KEY_SUBCOMP, PDIFKeywords::PDIF_KEY_I, PDIFKeywords::PDIF_KEY_ASG},
                { // Аргументы - ((X, Y), pin_type) - координаты переходного отверстия и его тип (тип точки соединения или перехода).
                    {{ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT}, false}
                },
                PDIFOps::PDIFOP_NET_V                    // Обработчик создания точки межсоединения для схемы или переходного отверстия на плате.
            }
        },
        // W - последовательность проводящих сегментов в составе цепи (токопроводящая ломаная линия).
        {PDIFKeywords::PDIF_KEY_W,
            {
                PDIFKeywords::PDIF_KEY_W,                // Ключ "W".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе ".../DETAIL/SUBCOMP/I/ASG".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_DETAIL, PDIFKeywords::PDIF_KEY_SUBCOMP, PDIFKeywords::PDIF_KEY_I, PDIFKeywords::PDIF_KEY_ASG},
                { // Аргументы - (X, Y)... - список координат вершин участков ломаной проводящей линии.
                    {{ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT}, true}
                },
                PDIFOps::PDIFOP_NET_LINE                 // Создание ломаного участка токопроводящей цепи.
            }
        },
        // Nn - текстовое обозначение имени цепи (текстовый элемент, содержащий имя цепи).
        {PDIFKeywords::PDIF_KEY_NN,
            {
                PDIFKeywords::PDIF_KEY_NN,               // Ключ "Nn".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе ".../DETAIL/SUBCOMP/I/ASG".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_DETAIL, PDIFKeywords::PDIF_KEY_SUBCOMP, PDIFKeywords::PDIF_KEY_I, PDIFKeywords::PDIF_KEY_ASG},
                { // Аргументы - (X, Y)... - список координат якорных точек текстовых строк, содержащих имя данной цепи.
                    {{ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT}, true}
                },
                PDIFOps::PDIFOP_NET_NN                   // Создание множества строк-подписей с именем этой цепи.
            }
        },
        // Подузлы контейнера P с характеристиками параметров отдельного вывода ("ножки") некоторого радиокомпонента.
        // Pt - символьный (для УГО символа) или номерной (для конструктива символа) тип вывода радиокомпонента.
        {PDIFKeywords::PDIF_KEY_PT,
            {
                PDIFKeywords::PDIF_KEY_PT,               // Ключ "Pt".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе ".../SUBCOMP/COMP_DEF/PIN_DEF/P".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_SUBCOMP, PDIFKeywords::PDIF_KEY_COMPDEF, PDIFKeywords::PDIF_KEY_PINDEF, PDIFKeywords::PDIF_KEY_P},
                { // Аргументы - единственная строка - тип вывода (для конструктива трактуется как число).
                    {{ValueType::ARG_VALUE_STRING}, false}
                },
                PDIFOps::PDIFOP_PIN_PT                   // Указание типа (символьного для схем и числового для плат) некоторого вывода радиоэлемента.
            }
        },
        // Lq - код эквивалентности (взаимозаменяемости) данного вывода радиокомпонента.
        {PDIFKeywords::PDIF_KEY_LQ,
            {
                PDIFKeywords::PDIF_KEY_LQ,               // Ключ "Lq".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе ".../SUBCOMP/COMP_DEF/PIN_DEF/P".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_SUBCOMP, PDIFKeywords::PDIF_KEY_COMPDEF, PDIFKeywords::PDIF_KEY_PINDEF, PDIFKeywords::PDIF_KEY_P},
                { // Аргументы - единственное целочисленное значение - код взаимозаменяемости этого вывода.
                    {{ValueType::ARG_VALUE_INT}, false}
                },
                PDIFOps::PDIFOP_PIN_LQ                   // Код эквивалентности для некоторой совокупности взаимозаменяемых выводов.
            }
        },
        // Ploc - точка расположения вывода.
        {PDIFKeywords::PDIF_KEY_PLOC,
            {
                PDIFKeywords::PDIF_KEY_PLOC,             // Ключ "Ploc".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе ".../SUBCOMP/COMP_DEF/PIN_DEF/P".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_SUBCOMP, PDIFKeywords::PDIF_KEY_COMPDEF, PDIFKeywords::PDIF_KEY_PINDEF, PDIFKeywords::PDIF_KEY_P},
                { // Аргументы - (X, Y) - координаты точки размещения вывода.
                    {{ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT}, false}
                },
                PDIFOps::PDIFOP_PIN_PLOC                 // Назначение координат места, где будет помещён данный вывод элемента.
            }
        },
        // Конечные узлы задания атрибутов (расшифровка содержимого секции ATR).
        // Org -  точка привязки. Справочная реперная точка для исходного библиотечного компонента.
        {PDIFKeywords::PDIF_KEY_ORG,
            {
                PDIFKeywords::PDIF_KEY_ORG,              // Ключ "Org".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе ".../SYMBOL/ATR/IN".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_SYMBOL, PDIFKeywords::PDIF_KEY_ATR, PDIFKeywords::PDIF_KEY_IN},
                { // Аргументы - пара координат (X, Y) реперной точки символа.
                    {{ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT}, false}
                },
                PDIFOps::PDIFOP_ATR_IN_ORG               // Внутренний атрибут символа - точка привязки символа (реперная точка).
            }
        },
        // Ty - идентификационный код типа компонента (ID).
        {PDIFKeywords::PDIF_KEY_TY,
            {
                PDIFKeywords::PDIF_KEY_TY,               // Ключ "Ty".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе ".../ATR/IN".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_ATR, PDIFKeywords::PDIF_KEY_IN},
                { // Аргумент - целочисленный код типа.
                    {{ValueType::ARG_VALUE_INT}, false}
                },
                PDIFOps::PDIFOP_ATR_IN_TY               // Внутренний атрибут символа - идентификационный код его типа.
            }
        },
        // Smd - Прибор, монтируемый на поверхность (только для печатной платы).
        {PDIFKeywords::PDIF_KEY_SMD,
            {
                PDIFKeywords::PDIF_KEY_SMD,              // Ключ "Smd".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе ".../ATR/IN".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_ATR, PDIFKeywords::PDIF_KEY_IN},
                { // Аргумент - "Y" для деталей поаерхностного монтажа и "N" - для штыревых элементов.
                    {{ValueType::ARG_VALUE_STRING}, false}
                },
                PDIFOps::PDIFOP_ATR_IN_SMD               // Внутренний атрибут символа - его предназначение для поверхностного монтажа.
            }
        },
        // Jmp - "Jumper" (только для печатной платы).
        {PDIFKeywords::PDIF_KEY_JMP,
            {
                PDIFKeywords::PDIF_KEY_JMP,              // Ключ "Jmp".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе ".../ATR/IN".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_ATR, PDIFKeywords::PDIF_KEY_IN},
                { // Аргумент - "Y" для перемычек и "N" для всех прочих.
                    {{ValueType::ARG_VALUE_STRING}, false}
                },
                PDIFOps::PDIFOP_ATR_IN_JMP               // Внутренний атрибут символа - признак радиоэлемента-перемычки.
            }
        },
        // Pl - Координаты местоположения копии элемента.
        {PDIFKeywords::PDIF_KEY_PL,
            {
                PDIFKeywords::PDIF_KEY_PL,               // Ключ "Pl".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе ".../SUBCOMP/I/ATR/IN".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_SUBCOMP, PDIFKeywords::PDIF_KEY_I, PDIFKeywords::PDIF_KEY_ATR, PDIFKeywords::PDIF_KEY_IN},
                { // Аргументы - пара координат (X, Y) точки вставки копии символа.
                    {{ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT}, false}
                },
                PDIFOps::PDIFOP_ATR_IN_PL               // Внутренний атрибут вставки - якорная точка вставки экземпляра радиокомпонента.
            }
        },
        // Sc - Масштабные коэффициенты по X и Y.
        {PDIFKeywords::PDIF_KEY_SC,
            {
                PDIFKeywords::PDIF_KEY_SC,               // Ключ "Sc".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе ".../SUBCOMP/I/ATR/IN".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_SUBCOMP, PDIFKeywords::PDIF_KEY_I, PDIFKeywords::PDIF_KEY_ATR, PDIFKeywords::PDIF_KEY_IN},
                { // Аргументы - пара осевых масштабных коэффициентов (ScX, ScY) вставленной копии.
                    {{ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT}, false}
                },
                PDIFOps::PDIFOP_ATR_IN_SC               // Внутренний атрибут вставки - индивидуальные масштабные коэффициенты по осям вставленного экземпляра компонента.
            }
        },
        // Ro - Угол вращения.
        {PDIFKeywords::PDIF_KEY_RO,
            {
                PDIFKeywords::PDIF_KEY_RO,               // Ключ "Ro".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе ".../SUBCOMP/I/ATR/IN".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_SUBCOMP, PDIFKeywords::PDIF_KEY_I, PDIFKeywords::PDIF_KEY_ATR, PDIFKeywords::PDIF_KEY_IN},
                { // Аргумент - индекс (от 0 до 3) угла поворота копии вставленного компонента (0 - поворот на 0 градусов, 1 - 90 градусов, 2 - 180, 3 - 270 градусов по часовой стрелке).
                    {{ValueType::ARG_VALUE_INT}, false}
                },
                PDIFOps::PDIFOP_ATR_IN_RO               // Внутренний атрибут вставки - угол поворота вставленного компонента.
            }
        },
        // Mr - Статус зеркальности (только для базы данных принципиальной схемы).
        {PDIFKeywords::PDIF_KEY_MR,
            {
                PDIFKeywords::PDIF_KEY_MR,               // Ключ "Mr".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе ".../SUBCOMP/I/ATR/IN".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_SUBCOMP, PDIFKeywords::PDIF_KEY_I, PDIFKeywords::PDIF_KEY_ATR, PDIFKeywords::PDIF_KEY_IN},
                { // Аргумент - "Y" при наличии зеркального отражения для вставки и "N" - при его отсутствии.
                    {{ValueType::ARG_VALUE_STRING}, false}
                },
                PDIFOps::PDIFOP_ATR_IN_MR               // Внутренний атрибут вставки - признак зеркальности вставленной копии радиокомпонента принципиальной схемы.
            }
        },
        // Nl - Местоположение имени копии компонента.
        {PDIFKeywords::PDIF_KEY_NL,
            {
                PDIFKeywords::PDIF_KEY_NL,               // Ключ "Nl".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе ".../SUBCOMP/I/ATR/IN".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_SUBCOMP, PDIFKeywords::PDIF_KEY_I, PDIFKeywords::PDIF_KEY_ATR, PDIFKeywords::PDIF_KEY_IN},
                { // Аргументы - координаты имени (X, Y) вставленной копии.
                    {{ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT}, false}
                },
                PDIFOps::PDIFOP_ATR_IN_NL               // Внутренний атрибут вставки - координаты якорной точки имени экземпляра компонента.
            }
        },
        // Ps - Сторона размещения компонентов (только для печатной платы).
        {PDIFKeywords::PDIF_KEY_PS,
            {
                PDIFKeywords::PDIF_KEY_PS,               // Ключ "Ps".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе ".../SUBCOMP/I/ATR/IN".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_SUBCOMP, PDIFKeywords::PDIF_KEY_I, PDIFKeywords::PDIF_KEY_ATR, PDIFKeywords::PDIF_KEY_IN},
                { // Аргумент - "T" для помещения компонента на верхнюю сторону (сторону деталей) и "B" - при его размещении на нижней стороне (стороне проводников).
                    {{ValueType::ARG_VALUE_STRING}, false}
                },
                PDIFOps::PDIFOP_ATR_IN_PS               // Внутренний атрибут вставки - сторона размещения вставленного компоненита на печатной плате.
            }
        },
        // Pa - Угол установки компонента.
        {PDIFKeywords::PDIF_KEY_PA,
            {
                PDIFKeywords::PDIF_KEY_PA,               // Ключ "Pa".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе ".../SUBCOMP/I/ATR/IN".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_SUBCOMP, PDIFKeywords::PDIF_KEY_I, PDIFKeywords::PDIF_KEY_ATR, PDIFKeywords::PDIF_KEY_IN},
                { // Аргумент - величина угла установки копии вставленного компонента (от 1 до 89 градусов по часовой стрелке).
                    {{ValueType::ARG_VALUE_INT}, false}
                },
                PDIFOps::PDIFOP_ATR_IN_PA               // Внутренний атрибут вставки - угол установки вставленного компонента.
            }
        },
        // Un - Присваиваемое пользователем имя вставленного радиокомпонента (только для PC-CAPS).
        {PDIFKeywords::PDIF_KEY_UN,
            {
                PDIFKeywords::PDIF_KEY_UN,               // Ключ "Un".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе ".../SUBCOMP/I/ATR/IN".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_SUBCOMP, PDIFKeywords::PDIF_KEY_I, PDIFKeywords::PDIF_KEY_ATR, PDIFKeywords::PDIF_KEY_IN},
                { // Аргумент - строка с именем экземпляра радиокомпонента.
                    {{ValueType::ARG_VALUE_STRING}, false}
                },
                PDIFOps::PDIFOP_ATR_IN_UN_COMP           // Внутренний атрибут вставки - присвоенное имя вставленного экземпляра радиокомпонента.
            }
        },
        // Un - Присваиваемое пользователем имя проводящей цепи (только для PC-CAPS).
        {PDIFKeywords::PDIF_KEY_UN,
            {
                PDIFKeywords::PDIF_KEY_UN,               // Ключ "Un".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе ".../NET_DEF/N/ATR/IN".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_NETDEF, PDIFKeywords::PDIF_KEY_N, PDIFKeywords::PDIF_KEY_ATR, PDIFKeywords::PDIF_KEY_IN},
                { // Аргумент - строка с именем проводящей цепи.
                    {{ValueType::ARG_VALUE_STRING}, false}
                },
                PDIFOps::PDIFOP_ATR_IN_UN_NET           // Внутренний атрибут цепи - присвоенное ей имя.
            }
        },
        // At - Определяемый пользователем "внешний" текстовый атрибут.
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
                PDIFOps::PDIFOP_ATR_EX_AT                // Текстовый "внешний" атрибут компонента (радиодетали).
            }
        },
        // Обработчики узлов, описывающих упаковку компонента в корпусе (соответствие выводов радиокомпонента ножкам корпуса).
        // Правила обработки узлов команды SPKG (корпусировка конструктива).
        {PDIFKeywords::PDIF_KEY_SNA,                     // Sna - перечисление имен секций (только для PC-CARDS).
            {
                PDIFKeywords::PDIF_KEY_SNA,              // Ключ "Sna".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе ".../SPKG".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_SPKG},
                { // Список строковых аргументов - имена существующих секций конструктива.
                    {{ValueType::ARG_VALUE_STRING}, true}
                },
                PDIFOps::PDIFOP_PKG_SNA                  // Считывание списка доступных секций конструктива радиокомпонента.
            }
        },
        {PDIFKeywords::PDIF_KEY_SP,                      // Sp - Карта ножек символа.
            {
                PDIFKeywords::PDIF_KEY_SP,               // Ключ "Sp".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе ".../SPKG".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_SPKG},
                { // Список строковых аргументов - карта ножек символа.
                    {{ValueType::ARG_VALUE_STRING}, false},   // Имя логического вывода, к которому относится список ножек.
                    {{ValueType::ARG_VALUE_INT}, true}        // Список порядковых номеров ножек, соответствующих данному логическому
                                                              // выводу, по одному номеру для каждой секции физического прибора.
                },
                PDIFOps::PDIFOP_PKG_SP                   // Установка соответствия выводов логического вентиля ножкам физического прибора.
            }
        },
        {PDIFKeywords::PDIF_KEY_APN,                     // Apn - перечень "алфавитно-цифровых номеров" ножек конструктива.
            {
                PDIFKeywords::PDIF_KEY_APN,              // Ключ "Apn".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе ".../SPKG".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_SPKG},
                { // Список строковых аргументов - "алфавитно-цифровые номера" ножек.
                    {{ValueType::ARG_VALUE_STRING}, true}
                },
                PDIFOps::PDIFOP_PKG_APN                  // Переопределение "алфавитно-цифровых номеров" ножек.
            }
        },
        // Правила обработки  узлов команды PKG (корпусировка УГО).
        {PDIFKeywords::PDIF_KEY_RDL,                     // Rdl - место показа позиционного обозначения.
            {
                PDIFKeywords::PDIF_KEY_RDL,              // Ключ "Rdl".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе ".../PKG".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_PKG},
                { // Список аргументов - координатная пара условного положения конструкторского обозначения на УГО.
                    {{ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT}, false}
                },
                PDIFOps::PDIFOP_PKG_RDL                  // Указание "справочного" положения позиционного обозначения логического вентиля.
            }
        },
        {PDIFKeywords::PDIF_KEY_PNL,                     // Pnl - место для показа номера ножки в составе УГО радиокомпонента.
            {
                PDIFKeywords::PDIF_KEY_PNL,              // Ключ "Pnl".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе ".../PKG".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_PKG},
                { // Список аргументов - координатная пара условного положения "номерной" подписи очередного вывода секции радиокомпонента.
                    {{ValueType::ARG_VALUE_INT, ValueType::ARG_VALUE_INT}, false}
                },
                PDIFOps::PDIFOP_PKG_PNL                  // Указание "справочного" положения "номерной" подписи очередного вывода.
            }
        },
        {PDIFKeywords::PDIF_KEY_SD,                      // Sd - установка соответствия между выводами логической секции компонента и именами
                                                         // его физических выводов (ножек физического устройства).
            {
                PDIFKeywords::PDIF_KEY_SP,               // Ключ "Sd".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе ".../PKG".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_PKG},
                { // Список строковых аргументов - карта ножек символа.
                    {{ValueType::ARG_VALUE_STRING}, false},   // Имя секции (как правило, односимвольное A-Z), к которой относится список ножек.
                    {{ValueType::ARG_VALUE_STRING}, true}     // Список "алфавитно-цифровых" физических номеров, соответствующих логическим
                                                              // выводам данной секции физического прибора.
                },
                PDIFOps::PDIFOP_PKG_SD                   // Установка соответствия выводов логического вентиля ножкам физического прибора.
            }
        },
        {PDIFKeywords::PDIF_KEY_PID,                     // Pid - упаковочный ID (только для PC-CAPS).
            {
                PDIFKeywords::PDIF_KEY_PID,              // Ключ "Pid".
                true,                                    // Терминал.
                false,                                   // Не является установочной операцией.
                // Находится в разделе ".../PKG".
                {PDIFKeywords::PDIF_KEY_ANY, PDIFKeywords::PDIF_KEY_PKG},
                { // Единственный строковый аргумент - упаковочный идентификатор УГО компонента.
                  // Это имя файла, содержащего соответствующий ему конструктив.
                    {{ValueType::ARG_VALUE_STRING}, false}
                },
                PDIFOps::PDIFOP_PKG_PID                  // Назначение упаковочного идентификатора для УГО компонента.
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
        // Экземпляры для обслуживания контейнерных узлов.
        node_component_handler(this),      // COMPONENT - корневой узел всей древовидной структуры PDIF-документа.
        node_comp_def_handler(this),       // COMP_DEF - контейнер описания вложенного радиокомпонента.
        node_n_handler(this),              // N - контейнерное описание проводящей цепи схемы или платы.
        node_i_handler(this),              // I - контейнерное описание вставки радиокомпонента (его конкретной единичной копии).
        node_p_handler(this),              // P - контейнерное описание некоторого вывода радиокомпонента.
        node_pkg_handler(this),            // PKG - контейнер с упаковочной информацией для УГО радиокомпонента.
        node_spkg_handler(this),           // SPKG - контейнер с упаковочной информацией конструктива радиокомпонента.
        // Обработчики глобальных параметров PDIF-документа, принадлежащих к его среде.
        node_mode_handler(this),           // Выбор режима (подтипа) БД
        node_vw_handler(this),             // Vw - положение и размер окон отображения.
        node_pdif_vrev_handler(this),      // PDIFvrev - версия (ревизия) PDIF-формата.
        node_db_vrev_handler(this),        // DBvrev - версия (ревизия) формата двоичного исходника базы данных.
        node_db_unit_handler(this),        // DBunit - единица измерений координат, используемая в файле.
        node_lyrstr_handler(this),         // Lyrstr - таблица доступных слоёв - список слоёв, используемых компонентом.
        node_lyrphid_handler(this),        // Lyrphid - определение спаривания слоёв и их доступности для трассировкию
        node_ssymtbl_handler(this),        // Ssymtbl - задание свойств контактов, которые служат как переходные отверстия между различными слоями.
        node_polyap_glb_handler(this),     // Polyap(glob) - глобальная ширина апертуры для рисования границ полигона.
        node_polyap_loc_hanlder(this),     // Polyap(loc) - локальная ширина апертуры для рисования границ полигона.
        node_dbtype_handler(this),         // DBtype - тип базы данных - схемный ("Schematic") или конструктивный ("PC-Board") элемент.
        // Параметры выводов ("ножек") радиокомпонентов.
        node_pt_handler(this),             // Pt - назначение типа (строкового или числового) вывода.
        node_lq_handler(this),             // Lq - код эквивалентности выводов.
        node_ploc_handler(this),           // Ploc - координаты точки местоположения вывода.
        // Базовые элементы проводящих цепей.
        node_v_handler(this),              // V - переходное отверстие на печатной плате.
        node_nn_handler(this),             // Nn - Положение видимого имени цепи.
        // Конструкторы примитивных (простейших) графических объектов. Один и тот же обработчик обслуживает все команды создания соответствующего примитива в
        // любом контектсе (как свободные фигуры, так и привязанные).
        node_a_handler(this),              // A - Создание A-дуги.
        node_arc_handler(this),            // Arc - Создание ARC-дуги.
        node_circle_handler(this),         // C - Создание окружности.
        node_line_handler(this),           // L - Создание прямолинейного отрезка.
        node_rect_handler(this),           // R - Прямоугольник.
        node_text_handler(this),           // T - Текстовая строка.
        node_fill_rect_handler(this),      // Fr - Заполненный прямоугольник.
        node_poly_hanlder(this),           // Poly - команда рисования составного полигона.
        // Составляющие многоугольника.
        node_poly_ol_handler(this),        // Ol - описание внешнего контура (габарита) полигона.
        node_poly_pv_hanlder(this),        // Pv - многоугольная пустота полигона.
        node_poly_cv_handler(this),        // Cv - описание круглой выемки полигона.
        // Объекты-обработчики узлов параметров вставки радиокомпонент.
        node_cn_handler(this),             // CN - сведения о соединении выводов экземпляра компонента с цепями схемы или платы.
        node_ipt_handler(this),            // IPT - переназначение типов ножек для конкретного экземпляра установленного на плату радиокомпонента.
        node_rd_handler(this),             // Rd - позиционное обозначение экземпляра компонента - его текст и положение.
        node_pn_handler(this),             // Pn - обозначение вывода элемента - текст и координаты точки размещения.
        // Инфраструктура и образующий её набор обработчиков для получения и установки различных атрибутов, дополняющих описание
        // некоторых сущностей загружаемой базы PCAD-данных.
        node_org_handler(this),            // Org - Точка привязки символа.
        node_ty_handler(this),             // Ty - Тип компонента.
        node_smd_handler(this),            // Smd - Прибор, монтируемый на поверхность (только для печатной платы).
        node_jmp_handler(this),            // Jmp - "Jumper" (только для печатной платы).
        node_pl_handler(this),             // Pl - Координаты местоположения.
        node_sc_handler(this),             // Sc - Масштабные коэффициенты по X и Y.
        node_ro_handler(this),             // Ro - Угол вращения.
        node_mr_handler(this),             // Mr - Статус зеркальности (только для базы данных принципиальной схемы).
        node_nl_handler(this),             // Nl - Местоположение имени копии компонента.
        node_ps_handler(this),             // Ps - Сторона размещения компонентов (только для печатной платы).
        node_pa_handler(this),             // Pa - Угол установки компонента.
        node_un_comp_handler(this),        // Un - Присваиваемое пользователем имя вставленного компонента (только для PC-CAPS).
        node_un_net_handler(this),         // Un - Присваиваемое пользователем имя проводящей цепи (только для PC-CAPS).
        node_at_handler(this),             // At - текстовый атрибут пользователя.
        // Обработчики установочных команд (команд настройки атрибутов рисования).
        node_layer_select_handler(this),   // Ly - выбор слоя размещения примитивов.
        node_line_type_handler(this),      // Ls - назначение типа линии.
        node_line_width_handler(this),     // Wd - указание толщины линии.
        // Группа объектов-обработчиков команд настройки параметров текстового примитива.
        node_text_height_handler(this),    // Ts - высота текста (высота глифов используемого шрифта).
        node_text_justify_hanlder(this),   // Tj - выравнивание текста.
        node_text_rotation_handler(this),  // Tr - вращение (угол поворота) текста.
        node_text_mirror_handler(this),    // Tm - зеркальность текста.
        // Экземпляры объектов обработки данных упаковки или корпусировки конструктива (команды SPKG или PKG соответственно).
        // Вложенные узлы команды SPKG (корпусировка конструктива).
        node_sna_handler(this),            // Sna - имя секции (только для PC-CARDS).
        node_sp_handler(this),             // Sp - Карта ножек символа. Связывает вывод логического вентиля с ножками конструктива.
        node_apn_handler(this),            // Apn - алфавитно-цифровой номер ножки.
        // Экземпляры объектов обработки данных корпусировки УГО (команды PKG).
        node_rdl_handler(this),            // Rdl - место показа позиционного обозначения.
        node_pnl_handler(this),            // Pnl - места для показа номеров ножке.
        node_sd_handler(this),             // Sd - секция  упаковки  (физического прибора и номера выводов.
        node_pid_handler(this)             // Pid - упаковочный ID (только для PC-CAPS).
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
                // Ищем ближайший значащий (обязательно требующий наличие некоторого звена в маршруте, то есть не равный PDIF_KEY_ANY)
                // элемент трафарета.
                size_t next_meaning_pattern_pos = test_pos;
                for (; next_meaning_pattern_pos < pattern_path.size(); ++next_meaning_pattern_pos)
                {
                    if (pattern_path[next_meaning_pattern_pos] != PDIFKeywords::PDIF_KEY_ANY)
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
                for (size_t concrete_tail_try_pos = test_pos; concrete_tail_try_pos < concrete_path.size(); ++concrete_tail_try_pos)
                {
                    TreePathType concrete_tail{concrete_path.begin() + concrete_tail_try_pos, concrete_path.end()};
                    if (IsPDIFPathCorrespond(concrete_tail, pattern_tail))
                        // Хвост трафарета pattern_tail укладывается (соответствует) некоторому концевому участку конкрета (начиная от
                        // элемента с индексом concrete_tail_try_pos). Так что и в целом конкрет соответствует требованиям шаблона.
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
        auto layers_it = find_if
            (load_file_data.layers.begin(), load_file_data.layers.end(), [&find_layer_name](const LayerDesc& layer_desc) -> bool
            {
                return layer_desc.layer_name == find_layer_name;
            });
        return layers_it == load_file_data.layers.end() ? -1 : layers_it - load_file_data.layers.begin();
    }

    // Обнаружение и возврат порядкового индекса существующего в load_file_data.radio_components радиокомпонента с именем find_component_name.
    int PDIFFileWorkshop::FindComponentByName(const std::string& find_component_name) const
    {
        auto component_it = find_if
            (load_file_data.radio_components.begin(), load_file_data.radio_components.end(),
                [&find_component_name](const RadioComponentDesc& component_desc) -> bool
                {
                    return component_desc.GetName() == find_component_name;
                });
        return component_it == load_file_data.radio_components.end() ? -1 : component_it - load_file_data.radio_components.begin();
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
    bool PDIFFileWorkshop::IsPrevNodeContainer([[maybe_unused]] TreeNodeData* node_data, PDIFKeywords key) const
    {
        int prev_stack_index = static_cast<int>(node_stack.size()) - 2; // Индекс предпоследнего  узла, который нас вмещает.
        const TreeNodeData* prev_node_data = prev_stack_index >= 0 ? &(node_stack[prev_stack_index]) : nullptr;
        if (!prev_node_data || !prev_node_data->node_desc || prev_node_data->node_desc->is_terminal)
            return false;   // Предпоследнего узла не существует вовсе или это не контейнер.
        return key == PDIFKeywords::PDIF_KEY_ANY || key == prev_node_data->key;
    }

    // Проверка, можно ли предверхнему (то есть предпоследнему) элементу стека узлов назначать "внешние" атрибуты.
    bool PDIFFileWorkshop::IsPrevNodeAttributed([[maybe_unused]] TreeNodeData* node_data) const
    {
        int prev_stack_index = static_cast<int>(node_stack.size()) - 2; // Индекс предпоследнего  узла, который нас вмещает.
        const TreeNodeData* prev_node_data = prev_stack_index >= 0 ? &(node_stack[prev_stack_index]) : nullptr;
        if (!prev_node_data)
            return false;
        // Внешние атрибуты могут назначаться дескриптору радиокомпонента, описанию токопроводящей цепи, определителю вставки
        // компонента в схему или на плату, а также блоку описания отдельного вывода радиокомпонента.
        switch (prev_node_data->spec_info.type)
        {
            case NodeSpecType::NODE_TYPE_RADIO_COMPONENT:
                [[fallthrough]];
            case NodeSpecType::NODE_TYPE_NET_DESC:
                [[fallthrough]];
            case NodeSpecType::NODE_TYPE_COMP_INSERTION:
                [[fallthrough]];
            case NodeSpecType::NODE_TYPE_PIN_DEF:
                return true;
            default:
                return false;
        }
    }

    // Поиск в дереве узла с типом find_type, ближайшего к текущему анализируемому (верхнему в стеке node_stack).
    const PDIFFileWorkshop::TreeNodeData* PDIFFileWorkshop::FindNodeByType(NodeSpecType find_type) const
    {
        for (int stack_index = static_cast<int>(node_stack.size()) - 1; stack_index >= 0; --stack_index)
        {
            if (node_stack[stack_index].spec_info.type == find_type)
                return &node_stack[stack_index];
        }
        return nullptr;
    }

    PDIFFileWorkshop::TreeNodeData* PDIFFileWorkshop::FindNodeByType(NodeSpecType find_type)
    {
        return const_cast<TreeNodeData*>(const_cast<const PDIFFileWorkshop*>(this)->FindNodeByType(find_type));
    }

    // Проверка некоторого хвоста (суффикса) маршрута concrete_path на соответствие трафарету pattern_tail_path.
    bool PDIFFileWorkshop::CheckNodeStackTail(const TreePathType& concrete_path, const TreePathType& pattern_tail_path) const
    {
        for (int test_pos = static_cast<int>(std::max(concrete_path.size(), pattern_tail_path.size())) - 1; test_pos <= 0; --test_pos)
        {
            switch (pattern_tail_path[test_pos])
            {
            case PDIFKeywords::PDIF_KEY_ANY:
            {
                // Ищем ближайший предыдущий значащий (не равный PDIF_KEY_ANY) элемент трафарета.
                int prev_meaning_pattern_pos = test_pos;
                for (; prev_meaning_pattern_pos >= 0; --prev_meaning_pattern_pos)
                {
                    if (pattern_tail_path[prev_meaning_pattern_pos] != PDIFKeywords::PDIF_KEY_ANY)
                        break;
                }
                if (prev_meaning_pattern_pos < 0)
                    // Значащих элементов в голове трафарета более не содержится. Проверяемый маршрут, таким образом,
                    // удовлетворяет требованиям трафарета, завершаем проверку с истинным результатом.
                    return true;
                // Предыдущий значащий элемент трафарета находится по индексу prev_meaning_pattern_pos. Пробуем найти
                // положение головы (начальной части) трафарета в ещё непроверенной голове маршрута concrete_path
                // (заканчивается на звене с индексом test_pos).
                TreePathType new_pattern_tail{pattern_tail_path.begin(), pattern_tail_path.begin() + prev_meaning_pattern_pos};
                for (size_t concrete_end_try_pos = test_pos; concrete_end_try_pos >= 0; --concrete_end_try_pos)
                {
                    TreePathType concrete_head{concrete_path.begin(), concrete_path.begin() + concrete_end_try_pos};
                    if (CheckNodeStackTail(concrete_head, new_pattern_tail))
                        // Головной остаток трафарета new_pattern_tail укладывается (соответствует) некоторому начальному
                        // участку конкрета (этот подходящий найденный участок заканчивается элементом с индексом concrete_end_try_pos).
                        // Так что и в целом конкрет соответствует требованиям шаблона.
                        return true;
                }
                // Головы трафарета и проверяемого маршрута никак совместить не удалось, так что конкретный маршрут не
                // соответствует трафарету.
                return false;
            }
            case PDIFKeywords::PDIF_KEY_UNKNOWN:
                // Шаблонный заполнитель, соответствующий любому ключевому слову в данном элементе маршрута.
                // Так что здесь нам подходит любой тип текущего звена проверяемого маршрута concrete_path, никаких
                // специальных проверок на соответствие для данного элемента не требуется.
                break; // Данное звено маршрута соответствует требованиям трафарета, продолжаем проверку.
            default:
                if (pattern_tail_path[test_pos] != concrete_path[test_pos])
                    return false; // Выявлено несовпадение проверяемого маршрута трафарету.
                else
                    break;  // Данное звено маршрута совпадает с трафаретом, продолжаем проверку.
            }
        }
        // Весь шаблон pattern_tail_path соответствует некоторому суффиксу проверяемого маршрута concrete_path
        // (от символа с индексом test_pos до его конца). Так что общее условие проверки следует считать выполненным.
        return true;
    }

    // Загрузка значения типа логического вывода или ножки из строкового параметра узла. Целевой тип данных определяется видом
    // загружаемой базы данных.
    optional<PinType> PDIFFileWorkshop::LoadPinTypeFromParam(const std::string& str_pin_type) const
    {
        // Для схем тип узла строковый, для плат - числовой (целочисленный).
        if (load_file_data.file_values.file_flags & FileFlags::FILE_FLAG_EDITOR_PCCAPS)
        { // База данных описывает УГО или схему - тип узла строковый.
            return str_pin_type;
        }
        else
        { // База данных описывает конструктив радиоэлемента или плату - тип узла числовой.
            const char *conv_str_arg = str_pin_type.c_str();
            char *conv_text_end;
            int dig_pin_type = strtol(conv_str_arg, &conv_text_end, 10);
            if (conv_text_end - conv_str_arg == static_cast<int>(str_pin_type.size()))
                return dig_pin_type;
            else
                return {};
        }
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
        load_file_data.graph_objects.clear();
        load_file_data.layers.clear();
        load_file_data.cross_layers_pinholes.clear();
        load_file_data.radio_components.clear();
        load_file_data.radio_comp_inserts.clear();
        load_file_data.nets.clear();
        load_file_data.file_values = {};
        node_stack.clear();
        is_wait_keyword = false;
        additional_load_info.load_errors.clear();

        // Заполним поля файлового описания file_values, зависящие только от типа загружаемого файла.
        load_file_data.file_values.file_signature = "PDIF"; // Строка, кратко описывающая тип и содержание файла.
        load_file_data.file_values.file_workshop = this;    // Указатель на "цех файловой фабрики", открывший данный файл.
        load_file_data.file_values.pdif_encoding = additional_load_info.pdif_encoding;  // Кодировка загружаемого файла.

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
                // Настройки созданного узла (use_node.node_settings) будем наследовать от ближайшего контейнера, в который созданный узел вложен.
                if (node_stack.size() > 1)
                    use_node.node_settings = node_stack[node_stack.size() - 2].node_settings;
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
                // сверить истинный и надлежащий тип очередного параметра.
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

        // Разбор загружаемого файла закончен успешно. Конструируем и возвращаем PCAD-документ на основе
        // полученной в процессе информации.
        return {PCADFile(move(load_file_data), additional_load_info.aperture_provider), PCADLoadError::LOAD_FILE_NO_ERROR};
    }

    // Определения методов конкретных классов-обработчиов командных узлов дерева PDIF-базы данных.

    // Сначала следует группа обработчиков контейнерных подразделов общего дерева PDIF-файла.
    // COMPONENT - корневой узел документа, содержит его основное имя.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeCOMPONENTHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeCOMPONENTHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        if (node_data->args.size() != 1)
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}}; // Должен быть строго один строковый параметр.
        if (!holds_alternative<string>(node_data->args[0]))
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};   // Параметр оказался не строковым.

        const string& database_name = get<string>(node_data->args[0]);  // Внутреннее имя документа (базы данных).
        GetWorkshop()->load_file_data.file_values.database_name = wxString(database_name.c_str(), wxConvUTF8);

        return {};
    }

    // SYMBOL или COMP_DEF - определение единственного базового или очередного встроенного в базу данных радиокомпонента.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeCOMPDEFHandler::HandleOpenNode(TreeNodeData* node_data)
    { // Создаём в node_data->spec_info.handler_spec_data описатель очередного внутреннего радиокомпонента загружаемого файла. Его описание
      // будет постепенно формироваться при дальнейшем разборе файла.
        string component_name;
        if (node_data->key == PDIFKeywords::PDIF_KEY_COMPDEF)
        { // COMP_DEF - начало описания очередного встроенного компонента, используемого в радиосхеме или печатной плате, содержащейся в загружаемой базе данных.
            if (node_data->args.size() != 1)
                return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}}; // Должен существовать только один параметр - имя создаваемого радиокомпонента.
            if (!holds_alternative<string>(node_data->args[0]))
                return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};      // Это имя должно быть строкой.

            component_name = get<string>(node_data->args[0]);
        }
        else if (node_data->key == PDIFKeywords::PDIF_KEY_SYMBOL)
        { // SYMBOL - определение единственного радиокомпонента, описанию которого и посвящена загружаемая база данных, если она является
          // символьной и описывает УГО или конструктив некоторого единственного радиокомпонента.
            component_name = GetWorkshop()->load_file_data.file_values.database_name.ToStdString();
        }
        else
        {
            return {{PCADLoadError::LOAD_FILE_COMMAND_UNACCEPTABLE_HERE, "COMPDEF"}};
        }

        RadioComponentDesc::SourceData* component_data = new (nothrow) RadioComponentDesc::SourceData;
        if (!component_data)
            return {{PCADLoadError::LOAD_FILE_MEMORY_ERROR, {}}};
        // Память под блок специфической информации, накопительно описывающей новую создаваемую радиодеталь, успешно выделена.
        component_data->comp_name = component_name;
        node_data->spec_info.handler_spec_data.reset(component_data);
        node_data->spec_info.type = NodeSpecType::NODE_TYPE_RADIO_COMPONENT;
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeCOMPDEFHandler::HandleCloseNode(TreeNodeData* node_data)
    { // Секция .../DETAIL/SUBCOMP/COMP_DEF закрыта, формирование описания нового радиокомпонента завершено.
        assert(node_data->spec_info.type == NodeSpecType::NODE_TYPE_RADIO_COMPONENT);
        RadioComponentDesc::SourceData* component_data = (RadioComponentDesc::SourceData*)(node_data->spec_info.handler_spec_data.get());
        GetWorkshop()->load_file_data.radio_components.push_back(RadioComponentDesc(move(*component_data)));
        return {};
    }

    // N - контейнерное описание проводящей цепи схемы или платы.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeNHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        if (node_data->args.size() != 1)
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}}; // Должен существовать только один параметр - имя создаваемой цепи.
        if (!holds_alternative<string>(node_data->args[0]))
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};      // Это имя должно быть строкой.

        // Создаём для новой подготавливаемой цепи структуру её дескриптора.
        NetDefDesc::SourceData* net_data = new (nothrow) NetDefDesc::SourceData;
        if (!net_data)
            return {{PCADLoadError::LOAD_FILE_MEMORY_ERROR, {}}};
        // Память под блок специфической информации, накопительно описывающей новую создаваемую цепь, успешно выделена.
        net_data->net_name = get<string>(node_data->args[0]);
        node_data->spec_info.handler_spec_data.reset(net_data);
        node_data->spec_info.type = NodeSpecType::NODE_TYPE_NET_DESC;
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeNHandler::HandleCloseNode(TreeNodeData* node_data)
    {   // Описание цепи завершено помещаем её в состав формируемого документа.
        assert(node_data->spec_info.type == NodeSpecType::NODE_TYPE_NET_DESC);
        NetDefDesc::SourceData* net_data = (NetDefDesc::SourceData*)(node_data->spec_info.handler_spec_data.get());
        GetWorkshop()->load_file_data.nets.push_back(NetDefDesc(move(*net_data)));
        return {};
    }

    // I - контейнерное описание вставки радиокомпонента (его конкретной единичной копии).
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeIHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        if (node_data->args.size() != 2)
            // Должны наличествовать строго два параметр - библиотечное имя вставляемого компонента и его собственное имя (имя экземпляра).
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}};
        if (!holds_alternative<string>(node_data->args[0]) || !holds_alternative<string>(node_data->args[1]))
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};      // Оба имени обязательно строки.

        // Создаём для новой создаваемой вставки блок для описывающей её информации.
        NodeIHandler::IHelper* insertion_data = new (nothrow) NodeIHandler::IHelper;
        if (!insertion_data)
            return {{PCADLoadError::LOAD_FILE_MEMORY_ERROR, {}}};
        // Память под блок специфической информации, накопительно описывающей новую вставку радиокомпонента, успешно выделена.
        insertion_data->comp_name = get<string>(node_data->args[0]);
        insertion_data->insertion_name = get<string>(node_data->args[1]);
        node_data->spec_info.handler_spec_data.reset(insertion_data);
        node_data->spec_info.type = NodeSpecType::NODE_TYPE_COMP_INSERTION;
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeIHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        assert(node_data->spec_info.type == NodeSpecType::NODE_TYPE_COMP_INSERTION);
        NodeIHandler::IHelper* insertion_data = (NodeIHandler::IHelper*)(node_data->spec_info.handler_spec_data.get());
        RadioComponentInsertion::SourceData* src_insertion_data = static_cast<RadioComponentInsertion::SourceData*>(insertion_data);
        GetWorkshop()->load_file_data.radio_comp_inserts.push_back(RadioComponentInsertion(move(*src_insertion_data)));
        return {};
    }

    // P - контейнерное описание некоторого вывода радиокомпонента.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodePHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        if (node_data->args.size() != 1)
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}}; // Должен существовать только один параметр - имя создаваемого вывода.
        if (!holds_alternative<string>(node_data->args[0]))
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};      // Это имя должно быть строкой.

        // Создаём для новой подготавливаемой цепи структуру её дескриптора.
        ComponentPinDef* pin_def_data = new (nothrow) ComponentPinDef;
        if (!pin_def_data)
            return {{PCADLoadError::LOAD_FILE_MEMORY_ERROR, {}}};
        // Память под описатель вывода радиокомпонента успешно выделена.
        pin_def_data->pin_al_number = get<string>(node_data->args[0]);
        pin_def_data->pin_name = get<string>(node_data->args[0]);
        pin_def_data->layer_number = node_data->node_settings.layer_number;
        node_data->spec_info.handler_spec_data.reset(pin_def_data);
        node_data->spec_info.type = NodeSpecType::NODE_TYPE_PIN_DEF;
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodePHandler::HandleCloseNode(TreeNodeData* node_data)
    { // Специфицирование вывода закончено, можно поместить его в принадлежащее ему место.
        assert(node_data->spec_info.type == NodeSpecType::NODE_TYPE_PIN_DEF);
        ComponentPinDef* pin_def_data = (ComponentPinDef*)(node_data->spec_info.handler_spec_data.get());
        // Выделяем определитель радиокомпонента, которому принадлежит данный вывод.
        PDIFFileWorkshop::TreeNodeData* current_component_node = GetWorkshop()->FindNodeByType(NodeSpecType::NODE_TYPE_RADIO_COMPONENT);
        assert(current_component_node);
        RadioComponentDesc::SourceData* component_data =
            (RadioComponentDesc::SourceData*)(current_component_node->spec_info.handler_spec_data.get());
        // Добавляем вновь сформированный вывод в список имеющихся выводов текущего анализируемого радиокомпонента.
        component_data->pins.push_back(move(*pin_def_data));
        return {};
    }

    // PKG - контейнер с упаковочной информацией для УГО радиокомпонента.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodePKGHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        // Создаём блок хранения упаковочной информации конструируемого в данный момент радиокомпонента.
        NodePKGHandler::PKGHelper* pkg_sect_data = new (nothrow) NodePKGHandler::PKGHelper;
        if (!pkg_sect_data)
            return {{PCADLoadError::LOAD_FILE_MEMORY_ERROR, {}}};

        node_data->spec_info.handler_spec_data.reset(pkg_sect_data);
        node_data->spec_info.type = NodeSpecType::NODE_TYPE_PKG;
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodePKGHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        assert(node_data->spec_info.type == NodeSpecType::NODE_TYPE_PKG);
        NodePKGHandler::PKGHelper* pkg_sect_data = (NodePKGHandler::PKGHelper*)(node_data->spec_info.handler_spec_data.get());
        // Выделяем определитель радиокомпонента, с которым связана эта упаковка.
        PDIFFileWorkshop::TreeNodeData* current_component_node = GetWorkshop()->FindNodeByType(NodeSpecType::NODE_TYPE_RADIO_COMPONENT);
        assert(current_component_node);
        RadioComponentDesc::SourceData* component_data =
            (RadioComponentDesc::SourceData*)(current_component_node->spec_info.handler_spec_data.get());
        // Добавляем вновь сформированный вывод в список имеющихся выводов текущего анализируемого радиокомпонента.
        component_data->sections_def = ComponentSectDef{move(*static_cast<ComponentPKGSectDef*>(pkg_sect_data))};
        return {};
    }

    // SPKG - контейнер с упаковочной информацией конструктива радиокомпонента.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeSPKGHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        // Создаём блок хранения упаковочной информации конструируемого в данный момент радиокомпонента.
        NodeSPKGHandler::SPKGHelper* spkg_sect_data = new (nothrow) NodeSPKGHandler::SPKGHelper;
        if (!spkg_sect_data)
            return {{PCADLoadError::LOAD_FILE_MEMORY_ERROR, {}}};

        node_data->spec_info.handler_spec_data.reset(spkg_sect_data);
        node_data->spec_info.type = NodeSpecType::NODE_TYPE_SPKG;
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeSPKGHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        assert(node_data->spec_info.type == NodeSpecType::NODE_TYPE_SPKG);
        NodeSPKGHandler::SPKGHelper* spkg_sect_data = (NodeSPKGHandler::SPKGHelper*)(node_data->spec_info.handler_spec_data.get());
        // Выделяем определитель радиокомпонента, с которым связана эта упаковка.
        PDIFFileWorkshop::TreeNodeData* current_component_node = GetWorkshop()->FindNodeByType(NodeSpecType::NODE_TYPE_RADIO_COMPONENT);
        assert(current_component_node);
        RadioComponentDesc::SourceData* component_data =
            (RadioComponentDesc::SourceData*)(current_component_node->spec_info.handler_spec_data.get());
        // Добавляем очередную полученную порцию упаковочной информации в структуру описания текущего анализируемого радиокомпонента.
        component_data->sections_def = ComponentSectDef{move(*static_cast<ComponentSPKGSectDef*>(spkg_sect_data))};
        return {};
    }

    // Далее расположены определения методов классов-обработчиков терминальных узлов PDIF-дерева.
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
            GetWorkshop()->load_file_data.file_values.file_flags |= FileFlags::FILE_FLAG_DETL;
        else if (mode_name == "SYMB"s)
            GetWorkshop()->load_file_data.file_values.file_flags &= (~FileFlags::FILE_FLAG_DETL);
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

        GetWorkshop()->load_file_data.file_values.current_window_center =
            {get<int64_t>(node_data->args[0]), get<int64_t>(node_data->args[1])};   // Центр рабочего окна.
        GetWorkshop()->load_file_data.file_values.current_window_scale =
            get<double>(node_data->args[2]);                                        // Его масштаб.

        for (size_t current_window = 0, arg_number = 3; current_window < window_count; ++current_window, arg_number += 5)
        {
            int64_t real_window_number = get<int64_t>(node_data->args[arg_number]),
                    dlx = get<int64_t>(node_data->args[arg_number + 1]),
                    dly = get<int64_t>(node_data->args[arg_number + 2]),
                    urx = get<int64_t>(node_data->args[arg_number + 3]),
                    ury = get<int64_t>(node_data->args[arg_number + 4]);

            int64_t ulx = dlx, uly = ury, win_width = urx - dlx, win_height = ury - dly;

            vector<wxRect>& uwin = GetWorkshop()->load_file_data.file_values.user_windows;
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
            GetWorkshop()->load_file_data.file_values.file_flags |= FileFlags::FILE_FLAG_UNIT_INCHES;
        else if (db_unit_name == "CMM"s)
            GetWorkshop()->load_file_data.file_values.file_flags &= (~FileFlags::FILE_FLAG_UNIT_INCHES);
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

            GetWorkshop()->load_file_data.layers.push_back(move(new_layer));
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

        int max_layer_number = static_cast<int>(GetWorkshop()->load_file_data.layers.size()) - 1;  // Максимальный индекс существующего слоя.
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
                LayerDesc& use_layer_desc = GetWorkshop()->load_file_data.layers[use_layer_number.value()];
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

        int max_layer_number = static_cast<int>(GetWorkshop()->load_file_data.layers.size()) - 1;  // Максимальный индекс существующего слоя.
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
            GetWorkshop()->load_file_data.cross_layers_pinholes.push_back(move(new_cross_pinhole));
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
            GetWorkshop()->load_file_data.file_values.polyap = use_poly_aperture.value();
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
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};      // Этот параметр (ширина апертуры) должен быть целым.

        if (optional<int> use_poly_aperture = CheckIntValue(get<int64_t>(node_data->args[0]), 1024); use_poly_aperture)
        {
            PDIFFileWorkshop::TreeNodeData* current_polygone_node = GetWorkshop()->FindNodeByType(NodeSpecType::NODE_TYPE_POLYGONE);
            if (!current_polygone_node)
                return {{PCADLoadError::LOAD_FILE_COMMAND_UNACCEPTABLE_HERE, "Polyap"}};

            NodePolyHandler::PolyNodeSpecInfo* poly_spec_info =
                (NodePolyHandler::PolyNodeSpecInfo*)(current_polygone_node->spec_info.handler_spec_data.get());
            poly_spec_info->polyap = use_poly_aperture.value();
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
            GetWorkshop()->load_file_data.file_values.file_flags &= (~FileFlags::FILE_FLAG_EDITOR_PCCAPS);
            GetWorkshop()->load_file_data.file_values.DBU_in_measure_unit = 100; // Конструктив - 100 DBU на метрическую единицу (дюйм или миллиметр).
        }
        else if (db_type_name == "Schematic"s)
        {
            GetWorkshop()->load_file_data.file_values.file_flags |= FileFlags::FILE_FLAG_EDITOR_PCCAPS;
            GetWorkshop()->load_file_data.file_values.DBU_in_measure_unit = 10;  // Схема - 10 DBU на метрическую единицу (дюйм или миллиметр).
        }
        else
        {
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_VALUE, {}}};   // Недопустмое значение параметра (не Schematic и не PC-Board).
        }
        return {};
    }

    // Poly - команда рисования составного полигона.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodePolyHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        NodePolyHandler::PolyNodeSpecInfo* poly_spec_info = new NodePolyHandler::PolyNodeSpecInfo;
        if (!poly_spec_info)
            return {{PCADLoadError::LOAD_FILE_MEMORY_ERROR, {}}};

        // Память под блок специфической информации, накопительно описывающей полигон, успешно выделена.
        node_data->spec_info.handler_spec_data.reset(poly_spec_info);
        node_data->spec_info.type = NodeSpecType::NODE_TYPE_POLYGONE;
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodePolyHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        // Извлекаем указатель на блок информации с данными нашего полигона, анализ которого завершается. Этот блок
        // к текущему моменту времени должен быть уже полностью заполнен.
        NodePolyHandler::PolyNodeSpecInfo* poly_spec_info =
            (NodePolyHandler::PolyNodeSpecInfo*)(node_data->spec_info.handler_spec_data.get());
        FillType poly_fill_type = poly_spec_info->contour.is_hatched ? FillType::POLY_FILL_HATCH : FillType::POLY_FILL_SOLID;
        // Конструируем графический объект полигона на основе полученной ранее информации.
        GraphObj* graph_obj_ptr = new ObjPoly
            (node_data->node_settings.layer_number, poly_fill_type, poly_spec_info->polyap,
             poly_spec_info->contour.points, poly_spec_info->circ_voids, poly_spec_info->poly_voids);
        // Сохраняем полученный объект по назначению.
        if (node_data->node_desc->opcode == PDIFOps::PDIFOP_PIC_POLY)
        { // Мы находимся внутри описания встроенного радиокомпонента и наш многоугольник принадлежит ему.
            PDIFFileWorkshop::TreeNodeData* current_component_node = GetWorkshop()->FindNodeByType(NodeSpecType::NODE_TYPE_RADIO_COMPONENT);
            assert(current_component_node);
            RadioComponentDesc::SourceData* radio_component_info =
                (RadioComponentDesc::SourceData*)(current_component_node->spec_info.handler_spec_data.get());
            radio_component_info->graph_objects.push_back(graph_obj_ptr);
        }
        else if (node_data->node_desc->opcode == PDIFOps::PDIFOP_NET_POLY)
        {
            PDIFFileWorkshop::TreeNodeData* current_net_node = GetWorkshop()->FindNodeByType(NodeSpecType::NODE_TYPE_NET_DESC);
            assert(current_net_node);
            NetDefDesc::SourceData* net_desc_data =
                (NetDefDesc::SourceData*)(current_net_node->spec_info.handler_spec_data.get());
            net_desc_data->net_parts.push_back(graph_obj_ptr);
        }
        else
        { // Свободный (иллюстративный) многоугольник прямо добавляется к массиву графических примитивов документа.
            GetWorkshop()->load_file_data.graph_objects.push_back(graph_obj_ptr);
        }
        return {};
    }

    // Описание внешнего контура многоугольника.
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
            return {{PCADLoadError::LOAD_FILE_COMMAND_UNACCEPTABLE_HERE, "Poly::Ol"}};

        if (node_data->spec_info.handler_spec_data)
        {
            NodePolyHandler::PolyNodeSpecInfo* poly_spec_info =
                (NodePolyHandler::PolyNodeSpecInfo*)(node_data->spec_info.handler_spec_data.get());
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

    // Описание полигональной выемки многоугольника.
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
            return {{PCADLoadError::LOAD_FILE_COMMAND_UNACCEPTABLE_HERE, "Poly::Pv"}};

        if (node_data->spec_info.handler_spec_data)
        {
            NodePolyHandler::PolyNodeSpecInfo* poly_spec_info =
                (NodePolyHandler::PolyNodeSpecInfo*)(node_data->spec_info.handler_spec_data.get());
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

    // Описание круглой пустоты многоугольника.
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
            return {{PCADLoadError::LOAD_FILE_COMMAND_UNACCEPTABLE_HERE, "Poly::Cv"}};

        if (node_data->spec_info.handler_spec_data)
        {
            NodePolyHandler::PolyNodeSpecInfo* poly_spec_info =
                (NodePolyHandler::PolyNodeSpecInfo*)(node_data->spec_info.handler_spec_data.get());
            // Создаем новую круговую пустоту и сохранем её в описатель многоугольника.
            wxPoint log_void_center =
                GetWorkshop()->ConvPntToLog(get<int64_t>(node_data->args[0]), get<int64_t>(node_data->args[1]));
            ObjCVoid new_circ_void(log_void_center, get<int64_t>(node_data->args[2]));
            poly_spec_info->circ_voids.push_back(move(new_circ_void));
        }
        return {};
    }

    // A - Создание A-дуги (как свободной, так и присоединённой).
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeAHandler::HandleOpenNode(TreeNodeData* node_data)
    {
       return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeAHandler::HandleCloseNode(TreeNodeData* node_data)
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
        // Конструируем рассчитанный примитив дуги.
        GraphObj* graph_obj_ptr = new ObjArc
            (node_data->node_settings.layer_number, node_data->node_settings.line_type,
             node_data->node_settings.line_width,
             arc_center_log, arc_begin_log, arc_end_log);
        // Сохраняем её в целевой контейнер.
        if (node_data->node_desc->opcode == PDIFOps::PDIFOP_PIC_A_ARC)
        { // Мы находимся внутри описания встроенного радиокомпонента и наша дуга принадлежит ему.
            PDIFFileWorkshop::TreeNodeData* current_component_node = GetWorkshop()->FindNodeByType(NodeSpecType::NODE_TYPE_RADIO_COMPONENT);
            assert(current_component_node);
            RadioComponentDesc::SourceData* component_poly_spec_info =
                (RadioComponentDesc::SourceData*)(current_component_node->spec_info.handler_spec_data.get());
            component_poly_spec_info->graph_objects.push_back(graph_obj_ptr);
        }
        else
        { // Свободная (иллюстративная) дуга прямо добавляется к массиву графических примитивов документа.
            GetWorkshop()->load_file_data.graph_objects.push_back(graph_obj_ptr);
        }
        return {};
    }

    // Arc - Создание ARC-дуги (аналогично, может быть свободной или включённой в радиокомпонент).
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeArcHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeArcHandler::HandleCloseNode(TreeNodeData* node_data)
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
        // Конструируем рассчитанную дугу.
        GraphObj* graph_obj_ptr = new ObjArc
            (node_data->node_settings.layer_number, node_data->node_settings.line_type,
             node_data->node_settings.line_width, arc_center_log, arc_begin_log, arc_end_log);
        // Сохраняем её в целевой контейнер.
        if (node_data->node_desc->opcode == PDIFOps::PDIFOP_PIC_ARC_ARC)
        { // Мы находимся внутри описания встроенного радиокомпонента и наша дуга принадлежит ему.
            PDIFFileWorkshop::TreeNodeData* current_component_node = GetWorkshop()->FindNodeByType(NodeSpecType::NODE_TYPE_RADIO_COMPONENT);
            assert(current_component_node);
            RadioComponentDesc::SourceData* component_poly_spec_info =
                (RadioComponentDesc::SourceData*)(current_component_node->spec_info.handler_spec_data.get());
            component_poly_spec_info->graph_objects.push_back(graph_obj_ptr);
        }
        else
        { // Свободная (иллюстративная) дуга прямо добавляется к массиву графических примитивов документа.
            GetWorkshop()->load_file_data.graph_objects.push_back(graph_obj_ptr);
        }
        return {};
    }

    // C - Создание графического примитива окружности.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeCircleHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeCircleHandler::HandleCloseNode(TreeNodeData* node_data)
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
        // Создание объекта окружности с параметрами, полученными выше.
        GraphObj* graph_obj_ptr = new ObjCirc
            (node_data->node_settings.layer_number, node_data->node_settings.line_type,
             node_data->node_settings.line_width, circle_center_log, circle_radius);
        // Сохраняем его в место назначения.
        if (node_data->node_desc->opcode == PDIFOps::PDIFOP_PIC_CIRCLE)
        { // Мы находимся внутри описания встроенного радиокомпонента и окружность принадлежит ему.
            PDIFFileWorkshop::TreeNodeData* current_component_node = GetWorkshop()->FindNodeByType(NodeSpecType::NODE_TYPE_RADIO_COMPONENT);
            assert(current_component_node);
            RadioComponentDesc::SourceData* component_poly_spec_info =
                (RadioComponentDesc::SourceData*)(current_component_node->spec_info.handler_spec_data.get());
            component_poly_spec_info->graph_objects.push_back(graph_obj_ptr);
        }
        else
        { // Свободная окружность прямо добавляется к массиву графических примитивов документа.
            GetWorkshop()->load_file_data.graph_objects.push_back(graph_obj_ptr);
        }
        return {};
    }

    // L - Создание ломаной линии, являющейся цепью прямолинейных отрезков.
    // Также этим обработчиком обслуживается и узел W - проводящая ломаная линия, включаемая в состав общей проводящей цепи.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeLineHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeLineHandler::HandleCloseNode(TreeNodeData* node_data)
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
        PDIFFileWorkshop::TreeNodeData* current_component_node = nullptr;
        if (node_data->node_desc->opcode == PDIFOps::PDIFOP_PIC_LINE)
        {
            current_component_node = GetWorkshop()->FindNodeByType(NodeSpecType::NODE_TYPE_RADIO_COMPONENT);
            assert(current_component_node);
        }
        else if (node_data->node_desc->opcode == PDIFOps::PDIFOP_NET_LINE)
        {
            current_component_node = GetWorkshop()->FindNodeByType(NodeSpecType::NODE_TYPE_NET_DESC);
            assert(current_component_node);
        }

        for (size_t span_index = 0; span_index < span_count; ++span_index)
        {
            size_t param_index = span_index * 2;
            int end_span_vertex_x = get<int64_t>(node_data->args[param_index]),
                end_span_vertex_y = get<int64_t>(node_data->args[param_index + 1]);
            wxPoint end_span_vertex_log = GetWorkshop()->ConvPntToLog(end_span_vertex_x, end_span_vertex_y);
            // Конструируем очередной сегмент ломаной.
            GraphObj* graph_obj_ptr = new ObjLine
                (node_data->node_settings.layer_number, node_data->node_settings.line_type,
                 node_data->node_settings.line_width, begin_span_vertex_log, end_span_vertex_log);
            // Сохраняем его в место назначения.
            if (node_data->node_desc->opcode == PDIFOps::PDIFOP_PIC_LINE)
            { // Мы находимся внутри описания встроенного радиокомпонента и ломаная линия принадлежит ему.
                RadioComponentDesc::SourceData* component_data =
                    (RadioComponentDesc::SourceData*)(current_component_node->spec_info.handler_spec_data.get());
                component_data->graph_objects.push_back(graph_obj_ptr);
            }
            else if (node_data->node_desc->opcode == PDIFOps::PDIFOP_NET_LINE)
            { // Мы находимся внутри описания токопроводящей цепи и ломаный проводник относится именно к ней.
                NetDefDesc::SourceData* net_desc_data =
                    (NetDefDesc::SourceData*)(current_component_node->spec_info.handler_spec_data.get());
                net_desc_data->net_parts.push_back(graph_obj_ptr);
            }
            else
            { // Ломаная непосредственно вносится в массив графических примитивов документа.
                GetWorkshop()->load_file_data.graph_objects.push_back(graph_obj_ptr);
            }
            // Переходим к построению следующего отрезка ломаной.
            begin_span_vertex_log = end_span_vertex_log;
        }

        return {};
    }

    // R - прямоугольник.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeRectangleHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeRectangleHandler::HandleCloseNode(TreeNodeData* node_data)
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
        // Создание прямоугольного примитива с извлечёнными из параметров характеристиками.
        GraphObj* graph_obj_ptr = new ObjRect
            (node_data->node_settings.layer_number, node_data->node_settings.line_type,
             node_data->node_settings.line_width, rect_log);
        // Сохраняем его в место назначения.
        if (node_data->node_desc->opcode == PDIFOps::PDIFOP_PIC_RECT)
        { // Мы находимся внутри описания встроенного радиокомпонента и прямоугольник принадлежит ему.
            PDIFFileWorkshop::TreeNodeData* current_component_node = GetWorkshop()->FindNodeByType(NodeSpecType::NODE_TYPE_RADIO_COMPONENT);
            assert(current_component_node);
            RadioComponentDesc::SourceData* component_poly_spec_info =
                (RadioComponentDesc::SourceData*)(current_component_node->spec_info.handler_spec_data.get());
            component_poly_spec_info->graph_objects.push_back(graph_obj_ptr);
        }
        else
        { // Прямоугольник непосредственно добавляется к концу массива графических примитивов документа.
            GetWorkshop()->load_file_data.graph_objects.push_back(graph_obj_ptr);
        }
        return {};
    }

    // T - текстовая строка.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeTextHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeTextHandler::HandleCloseNode(TreeNodeData* node_data)
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
        // Конструирование текстового графического объекта.
        GraphObj* graph_obj_ptr = new ObjText
            (node_data->node_settings.layer_number, text_anchor_log,
             node_data->node_settings.text_orient, node_data->node_settings.text_height,
             node_data->node_settings.text_align, text_text);
        // Сохраняем его в место назначения.
        if (node_data->node_desc->opcode == PDIFOps::PDIFOP_PIC_TEXT)
        { // Мы находимся внутри описания встроенного радиокомпонента и, следовательно, текст принадлежит ему.
            PDIFFileWorkshop::TreeNodeData* current_component_node = GetWorkshop()->FindNodeByType(NodeSpecType::NODE_TYPE_RADIO_COMPONENT);
            assert(current_component_node);
            RadioComponentDesc::SourceData* component_poly_spec_info =
                (RadioComponentDesc::SourceData*)(current_component_node->spec_info.handler_spec_data.get());
            component_poly_spec_info->graph_objects.push_back(graph_obj_ptr);
        }
        else
        { // Текстовая надпись свободна, она добавляется к концу массива графических примитивов документа.
            GetWorkshop()->load_file_data.graph_objects.push_back(graph_obj_ptr);
        }
        return {};
    }

    // Fr - заполненный прямоугольник.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeFillRectHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeFillRectHandler::HandleCloseNode(TreeNodeData* node_data)
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
        unsigned char rect_fill_color =
            node_data->node_settings.layer_number < static_cast<int>(GetWorkshop()->load_file_data.layers.size()) ?
            GetWorkshop()->load_file_data.layers[node_data->node_settings.layer_number].layer_color : 0;
        // Создание примитива закрашенногог прямоугольника.
        GraphObj* graph_obj_ptr = new ObjFillRect
            (node_data->node_settings.layer_number, rect_fill_color, rect_log);
        // Сохраняем его в место назначения.
        if (node_data->node_desc->opcode == PDIFOps::PDIFOP_PIC_FILL_RECT)
        { // Мы находимся внутри описания встроенного радиокомпонента и, следовательно, фигура принадлежит ему.
            PDIFFileWorkshop::TreeNodeData* current_component_node = GetWorkshop()->FindNodeByType(NodeSpecType::NODE_TYPE_RADIO_COMPONENT);
            assert(current_component_node);
            RadioComponentDesc::SourceData* component_poly_spec_info =
                (RadioComponentDesc::SourceData*)(current_component_node->spec_info.handler_spec_data.get());
            component_poly_spec_info->graph_objects.push_back(graph_obj_ptr);
        }
        else
        { // Прямоугольник не имеет ассоциации с каким-либо контейнером, так что он добавляется к концу массива графических
          // примитивов документа.
            GetWorkshop()->load_file_data.graph_objects.push_back(graph_obj_ptr);
        }
        return {};
    }

    // Прочие элементы проводящих цепей, не соответствующие каким-либо другим типам графических элементов.
    // V - точечный проводящий элемент (межсоединения на схеме или переходные отверстия на плате).
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeVHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeVHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        if (node_data->args.size() != 2 && node_data->args.size() != 3)
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}};  // Количество параметров должны быть 2 или 3.

        // Все аргументы должны быть целочисленными (пара координат точечного объекта и возможный его числовой тип).
        for (size_t i = 0; i < node_data->args.size(); ++i)
        {
            if (!holds_alternative<int64_t>(node_data->args[i]))
                return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};
        }

        int point_center_x = get<int64_t>(node_data->args[0]),
            point_center_y = get<int64_t>(node_data->args[1]);
        int point_type = node_data->args.size() == 3 ? get<int64_t>(node_data->args[2]) : 0;
        wxPoint point_center_log = GetWorkshop()->ConvPntToLog(point_center_x, point_center_y);
        int via_radius = GetWorkshop()->load_file_data.file_values.file_flags & FileFlags::FILE_FLAG_DETL ?
            PDIFFileWorkshop::VIA_POINT_RADIUS_PCB : PDIFFileWorkshop::VIA_POINT_RADIUS_SCH;

        PDIFFileWorkshop::TreeNodeData* current_net_desc_node = GetWorkshop()->FindNodeByType(NodeSpecType::NODE_TYPE_NET_DESC);
        assert(current_net_desc_node);
        if (!current_net_desc_node)
            return {{PCADLoadError::LOAD_FILE_COMMAND_UNACCEPTABLE_HERE, "V"}};

        NetDefDesc::SourceData* net_desc_data =
            (NetDefDesc::SourceData*)(current_net_desc_node->spec_info.handler_spec_data.get());
        // Конструируем точечный объект с указанными в аргументах свойствами.
        GraphObj* graph_obj_ptr = new ObjPoint
            (node_data->node_settings.layer_number, point_center_log, via_radius, true,
             ObjPoint::PointCategory::POINT_CAT_VIA, point_type);
        // Сохраняем его в место назначения - в состав той цепи, которой она принадлежит.
        net_desc_data->net_parts.push_back(graph_obj_ptr);
        return {};
    }

    // Nn - видимые текстовые этикетки с именем цепи.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeNnHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeNnHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        if (node_data->args.size() > 0 && (node_data->args.size() % 2 == 0))
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}};  // Количество параметров должны быть четным.

        // Все аргументы должны быть целочисленными (набор из координатных пар якорных точек этикеток).
        for (size_t i = 0; i < node_data->args.size(); ++i)
        {
            if (!holds_alternative<int64_t>(node_data->args[i]))
                return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};
        }

        size_t label_count = node_data->args.size() / 2;
        PDIFFileWorkshop::TreeNodeData* current_net_desc_node = GetWorkshop()->FindNodeByType(NodeSpecType::NODE_TYPE_NET_DESC);
        assert(current_net_desc_node);
        if (!current_net_desc_node)
            return {{PCADLoadError::LOAD_FILE_COMMAND_UNACCEPTABLE_HERE, "Nn"}};
        NetDefDesc::SourceData* net_desc_data =
            (NetDefDesc::SourceData*)(current_net_desc_node->spec_info.handler_spec_data.get());

        for (size_t label_index = 0; label_index < label_count; ++label_index)
        {
            size_t param_index = label_index * 2;
            int label_anchor_x = get<int64_t>(node_data->args[param_index]),
                label_anchor_y = get<int64_t>(node_data->args[param_index + 1]);
            wxPoint label_anchor_log = GetWorkshop()->ConvPntToLog(label_anchor_x, label_anchor_y);
            // Конструируем очередную текстовую этикетку.
            GraphObj* graph_obj_ptr = new ObjText
                (node_data->node_settings.layer_number, label_anchor_log,
                 node_data->node_settings.text_orient, node_data->node_settings.text_height,
                 node_data->node_settings.text_align, net_desc_data->net_name);
            // Сохраняем её в место назначения - в состав той цепи, которой она принадлежит.
            net_desc_data->net_parts.push_back(graph_obj_ptr);
        }

        return {};
    }

    // Терминалы подраздела определения вывода ("ножки") некоторого радиокомпонента.
    // Pt - символьный (для УГО символа) или номерной (для конструктива символа) тип вывода радиокомпонента.
    std::optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodePtHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        return {};
    }

    std::optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodePtHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        size_t argc = node_data->args.size();
        if (argc != 1)
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}};  // Параметр этого узла всегда только один.
        // Этот параметр - тип вывода - должен быть строковым.
        if (!holds_alternative<string>(node_data->args[0]))
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};
        string str_pin_type = get<string>(node_data->args[0]);

        // Узел Pt допускается только в составе описания вывода какого-либо радиокомпонента (тип NodeSpecType::NODE_TYPE_PIN_DEF).
        PDIFFileWorkshop::TreeNodeData* current_pin_def_node = GetWorkshop()->FindNodeByType(NodeSpecType::NODE_TYPE_PIN_DEF);
        if (!current_pin_def_node)
            return {{PCADLoadError::LOAD_FILE_COMMAND_UNACCEPTABLE_HERE, "Pt"}};
        ComponentPinDef* pin_def_info = (ComponentPinDef*)(current_pin_def_node->spec_info.handler_spec_data.get());
        // Для схем тип узла строковый, для плат - числовой (целочисленный).
        if (optional<PinType> opt_pin_type = GetWorkshop()->LoadPinTypeFromParam(str_pin_type); opt_pin_type)
            pin_def_info->pin_type = opt_pin_type.value();
        else
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_VALUE, {}}};

        return {};
    }

    // Lq - код эквивалентности (взаимозаменяемости) данного вывода радиокомпонента.
    std::optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeLqHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        return {};
    }

    std::optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeLqHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        if (node_data->args.size() != 1)
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}};  // Параметр этого узла всегда только один.
        // Этот параметр - код эквивалентности вывода - должен быть числом.
        if (!holds_alternative<int64_t>(node_data->args[0]))
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};

        // Узел Lq допускается только в составе описания вывода какого-либо радиокомпонента (тип NodeSpecType::NODE_TYPE_PIN_DEF).
        PDIFFileWorkshop::TreeNodeData* current_pin_def_node = GetWorkshop()->FindNodeByType(NodeSpecType::NODE_TYPE_PIN_DEF);
        if (!current_pin_def_node)
            return {{PCADLoadError::LOAD_FILE_COMMAND_UNACCEPTABLE_HERE, "Lq"}};
        ComponentPinDef* pin_def_info = (ComponentPinDef*)(current_pin_def_node->spec_info.handler_spec_data.get());

        if (optional<int> checked_val = CheckIntValue(get<int64_t>(node_data->args[0]), 0x7FFF); checked_val)
            pin_def_info->equive_code = checked_val.value(); // Код эквивалентности допустим по величине - продолжаем.
        else
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_VALUE, {}}};  // Код эквивалентности вне допустимых пределов.

        return {};
    }

    // Ploc - точка расположения вывода.
    std::optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodePlocHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        return {};
    }

    std::optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodePlocHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        if (node_data->args.size() != 2)
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}};  // Параметр этого узла - двузначная координатная пара.
        // Эти параметры - координаты местоположения вывода - должны быть числовыми.
        if (!holds_alternative<int64_t>(node_data->args[0]) || !holds_alternative<int64_t>(node_data->args[1]))
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};

        // Узел Ploc допускается только в составе описания вывода какого-либо радиокомпонента (тип NodeSpecType::NODE_TYPE_PIN_DEF).
        PDIFFileWorkshop::TreeNodeData* current_pin_def_node = GetWorkshop()->FindNodeByType(NodeSpecType::NODE_TYPE_PIN_DEF);
        if (!current_pin_def_node)
            return {{PCADLoadError::LOAD_FILE_COMMAND_UNACCEPTABLE_HERE, "Ploc"}};
        ComponentPinDef* pin_def_info = (ComponentPinDef*)(current_pin_def_node->spec_info.handler_spec_data.get());

        int pin_center_x = get<int64_t>(node_data->args[0]),
            pin_center_y = get<int64_t>(node_data->args[1]);
        pin_def_info->pin_coords = GetWorkshop()->ConvPntToLog(pin_center_x, pin_center_y);
        return {};
    }

    // At - "внешний" текстовый атрибут пользователя. Он всегда привязанный, принадлежащий какому-то элементу
    // (символу, компоненту, цепи) базы данных.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeAtHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        if (GetWorkshop()->IsPrevNodeAttributed(node_data))
            return {};
        else
            return {{PCADLoadError::LOAD_FILE_COMMAND_UNACCEPTABLE_HERE, "At"}};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeAtHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        if (node_data->args.size() != 4)
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}};  // Спецификатор внешнего атрибута имеет ровно 4 параметра.
        // Первые два - строки ключа и значения создаваемого атрибута. Вторые два - координатная пара (X, Y) точки видимого отображения
        // атрибута на чертеже.
        if (!holds_alternative<string>(node_data->args[0]) || !holds_alternative<string>(node_data->args[1]) ||
            !holds_alternative<int64_t>(node_data->args[2]) || !holds_alternative<int64_t>(node_data->args[3]))
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};

        int attr_anchor_x = get<int64_t>(node_data->args[2]),
            attr_anchor_y = get<int64_t>(node_data->args[3]);
        wxPoint attr_anchor_log = GetWorkshop()->ConvPntToLog(attr_anchor_x, attr_anchor_y);

        // Получаем вышележащий элемент стека объектов, которому нужно назначить этот "внешний" атрибут.
        PDIFFileWorkshop* my_workshop = GetWorkshop();
        int prev_stack_index = static_cast<int>(my_workshop->node_stack.size()) - 2;
        if (prev_stack_index < 0)   // Стек узлов пуст, а команда употреблена в корневом контексте - это, разумеется, недопустимо.
            return {{PCADLoadError::LOAD_FILE_COMMAND_UNACCEPTABLE_HERE, "At"}};

        const TreeNodeData* prev_node_data = &(my_workshop->node_stack[prev_stack_index]);
        ExAttrsCollection* attr_collection;
        // Внешние атрибуты могут назначаться дескриптору радиокомпонента, описанию токопроводящей цепи, определителю вставки
        // компонента в схему или на плату, а также блоку описания отдельного вывода радиокомпонента.
        switch (prev_node_data->spec_info.type)
        {
            case NodeSpecType::NODE_TYPE_RADIO_COMPONENT:
                attr_collection = &(reinterpret_cast<RadioComponentDesc::SourceData*>(node_data->spec_info.handler_spec_data.get())->ex_attr_collection);
                break;
            case NodeSpecType::NODE_TYPE_NET_DESC:
                attr_collection = &(reinterpret_cast<NetDefDesc::SourceData*>(node_data->spec_info.handler_spec_data.get())->ex_attr_collection);
                break;
            case NodeSpecType::NODE_TYPE_COMP_INSERTION:
                attr_collection = &(reinterpret_cast<RadioComponentInsertion::SourceData*>(node_data->spec_info.handler_spec_data.get())->ex_attr_collection);
                break;
            case NodeSpecType::NODE_TYPE_PIN_DEF:
                attr_collection = static_cast<ExAttrsCollection*>((ComponentPinDef*)(node_data->spec_info.handler_spec_data.get()));
                break;
            default:    // Здесь указание внешнего атрибута не допускается, так как нет владеющего им узла.
                return {{PCADLoadError::LOAD_FILE_COMMAND_UNACCEPTABLE_HERE, "At"}};
        }
        // Целевая коллекция обнаружена, на неё сейчас указывает attr_collection.
        (*attr_collection)[get<string>(node_data->args[0])] =
            TextExtAttr{.key = get<string>(node_data->args[0]), .value = get<string>(node_data->args[1]), .attr_coords = attr_anchor_log};
        return {};
    }

    // Ly - выбор слоя размещения примитивов.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeLyHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        if (GetWorkshop()->IsPrevNodeContainer(node_data))
            return {};
        else
            return {{PCADLoadError::LOAD_FILE_COMMAND_UNACCEPTABLE_HERE, "Ly"}};
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
            return {{PCADLoadError::LOAD_FILE_COMMAND_UNACCEPTABLE_HERE, "Ls"}};
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
            return {{PCADLoadError::LOAD_FILE_COMMAND_UNACCEPTABLE_HERE, "Wd"}};
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
            return {{PCADLoadError::LOAD_FILE_COMMAND_UNACCEPTABLE_HERE, "Ts"}};
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
            return {{PCADLoadError::LOAD_FILE_COMMAND_UNACCEPTABLE_HERE, "Tj"}};
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
            return {{PCADLoadError::LOAD_FILE_COMMAND_UNACCEPTABLE_HERE, "Tr"}};
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
            return {{PCADLoadError::LOAD_FILE_COMMAND_UNACCEPTABLE_HERE, "Tm"}};
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

    // Терминальные узлы - подсекции раздела PKG - упаковочная информация УГО радиокомпонента.
    // Rdl - место показа позиционного обозначения.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeRdlHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        if (GetWorkshop()->IsPrevNodeContainer(node_data, PDIFKeywords::PDIF_KEY_PKG))
            return {};
        else
            return {{PCADLoadError::LOAD_FILE_COMMAND_UNACCEPTABLE_HERE, "Rdl"}};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeRdlHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        if (node_data->args.size() != 2)
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}};  // Параметр этого узла - двузначная координатная пара.
        // Эти параметры - координаты местоположения позиционного (конструкторского) обозначения компонента - должны быть числовыми.
        if (!holds_alternative<int64_t>(node_data->args[0]) || !holds_alternative<int64_t>(node_data->args[1]))
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};

        // Получаем вышележащий элемент стека объектов, описывающий загружаемый в данный момент радиокомпонент (основной или встроенный).
        PDIFFileWorkshop::TreeNodeData* current_component_node = GetWorkshop()->FindNodeByType(NodeSpecType::NODE_TYPE_RADIO_COMPONENT);
        if (!current_component_node)
            return {{PCADLoadError::LOAD_FILE_COMMAND_UNACCEPTABLE_HERE, "Rdl"}};
        RadioComponentDesc::SourceData* component_data =
            (RadioComponentDesc::SourceData*)(current_component_node->spec_info.handler_spec_data.get());

        // Устанавливаем "справочное" опорное положение конструкторского обозначения радиоэлемента.
        int refdes_anchor_x = get<int64_t>(node_data->args[0]),
            refdes_anchor_y = get<int64_t>(node_data->args[1]);
        component_data->refdes.pos = GetWorkshop()->ConvPntToLog(refdes_anchor_x, refdes_anchor_y);
        return {};
    }

    // Pnl - места для показа номера (текстовой подписи) ножки.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodePnlHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        if (GetWorkshop()->IsPrevNodeContainer(node_data, PDIFKeywords::PDIF_KEY_PKG))
            return {};
        else
            return {{PCADLoadError::LOAD_FILE_COMMAND_UNACCEPTABLE_HERE, "Pnl"}};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodePnlHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        if (node_data->args.size() != 2)
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}};  // Параметр этого узла - двузначная координатная пара.
        // Эти параметры - координаты якорной точки текстовой этикетки очередного вывода - должны быть числовыми.
        if (!holds_alternative<int64_t>(node_data->args[0]) || !holds_alternative<int64_t>(node_data->args[1]))
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};

        // Получаем вышележащий элемент стека объектов, описывающий загружаемый в данный момент радиокомпонент (основной или встроенный).
        PDIFFileWorkshop::TreeNodeData* current_component_node = GetWorkshop()->FindNodeByType(NodeSpecType::NODE_TYPE_RADIO_COMPONENT);
        if (!current_component_node)
            return {{PCADLoadError::LOAD_FILE_COMMAND_UNACCEPTABLE_HERE, "Pnl"}};
        RadioComponentDesc::SourceData* component_data =
            (RadioComponentDesc::SourceData*)(current_component_node->spec_info.handler_spec_data.get());
        // Далее извлекаем указатель на служебную структуру, поддерживаемую внутри подраздела PKG.
        NodePKGHandler::PKGHelper* pkg_spec_info = (NodePKGHandler::PKGHelper*)(node_data->spec_info.handler_spec_data.get());
        size_t use_pnl_index = pkg_spec_info->current_pnl_index++;
        if (use_pnl_index >= component_data->pins.size())
            // Очередного вывода, которому нужно назначить положение подписи, не существует.
            return {{PCADLoadError::INDEX_OUT_LIMIT, {}}};
        // Вывод со следующим по порядку индексом существует. Выберем его для настройки.
        PinLabelDef& use_pin_label = component_data->pins[use_pnl_index].pin_label;
        int pinname_anchor_x = get<int64_t>(node_data->args[0]),
            pinname_anchor_y = get<int64_t>(node_data->args[1]);
        use_pin_label.pin_name_coords = GetWorkshop()->ConvPntToLog(pinname_anchor_x, pinname_anchor_y);
        use_pin_label.layer_number = node_data->node_settings.layer_number;
        use_pin_label.text_height = node_data->node_settings.text_height;
        use_pin_label.text_orient = node_data->node_settings.text_orient;
        use_pin_label.text_align = node_data->node_settings.text_align;
        return {};
    }

    // Sd - секция упаковки (физического прибора и номера выводов.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeSdHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        if (GetWorkshop()->IsPrevNodeContainer(node_data, PDIFKeywords::PDIF_KEY_PKG))
            return {};
        else
            return {{PCADLoadError::LOAD_FILE_COMMAND_UNACCEPTABLE_HERE, "Sd"}};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeSdHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        // Получаем вышележащий элемент стека объектов, описывающий загружаемый в данный момент радиокомпонент (основной или встроенный).
        PDIFFileWorkshop::TreeNodeData* current_component_node = GetWorkshop()->FindNodeByType(NodeSpecType::NODE_TYPE_RADIO_COMPONENT);
        if (!current_component_node)
            return {{PCADLoadError::LOAD_FILE_COMMAND_UNACCEPTABLE_HERE, "Pnl"}};
        RadioComponentDesc::SourceData* component_data =
            (RadioComponentDesc::SourceData*)(current_component_node->spec_info.handler_spec_data.get());
        // Набор параметров узла должен содержать имя секции и столько прочих параметров, сколько выводов определено
        // в данном радиокомпоненте.
        if (node_data->args.size() != component_data->pins.size() + 1)
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}};
        // Все параметры - строки.
        for (size_t param_index = 0; param_index < node_data->args.size(); ++param_index)
            if (!holds_alternative<string>(node_data->args[param_index]))
                return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};
        // Далее извлекаем указатель на служебную структуру, поддерживаемую внутри подраздела PKG.
        NodePKGHandler::PKGHelper* pkg_spec_info = (NodePKGHandler::PKGHelper*)(node_data->spec_info.handler_spec_data.get());
        // Получаем из параметров узла и заполняем список "алфавитно-цифровых" номеров ножек физического устройства
        // для секции, идентифицируемой первым аргументом (то есть с индексом 0) нашего узла.
        PinNameToALNumber load_pin_to_al_number;
        for (size_t param_index = 1; param_index < node_data->args.size(); ++param_index)
            // Для вывода с индексом param_index выбираем из component_data->pins его логическое имя, а затем устанавливаем для него
            // упаковочное соответствие "алфавитно-цифровому" номеру некоторой ножки прибора, заданному очередным параметром узла.
            load_pin_to_al_number[component_data->pins[param_index].pin_name] = get<string>(node_data->args[param_index]);
        // Ассоциативный массив упаковочной связи для секции с именем node_data->args[0] полностью сформирован.
        pkg_spec_info->pin_pkg_data[get<string>(node_data->args[0])] = move(load_pin_to_al_number);
        return {};
    }

    // Pid - упаковочный ID (только для PC-CAPS).
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodePidHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        if (GetWorkshop()->IsPrevNodeContainer(node_data, PDIFKeywords::PDIF_KEY_PKG))
            return {};
        else
            return {{PCADLoadError::LOAD_FILE_COMMAND_UNACCEPTABLE_HERE, "Pid"}};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodePidHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        if (node_data->args.size() != 1)
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}};  // Параметр этого узла единствен.
        // Он есть строка и указывает упаковочный идентификатор - имя файла, содержащего конструктив для данного символа
        // (в том случае, если в PDIF-базе описан его УГО).
        if (!holds_alternative<string>(node_data->args[0]))
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};

        // Получаем указатель на загружаемый в данный момент радиокомпонент.
        PDIFFileWorkshop::TreeNodeData* current_component_node = GetWorkshop()->FindNodeByType(NodeSpecType::NODE_TYPE_RADIO_COMPONENT);
        if (!current_component_node)
            return {{PCADLoadError::LOAD_FILE_COMMAND_UNACCEPTABLE_HERE, "Pid"}};
        RadioComponentDesc::SourceData* component_data =
            (RadioComponentDesc::SourceData*)(current_component_node->spec_info.handler_spec_data.get());
        // Назначаем ему упаковочный ID.
        component_data->package_id = get<string>(node_data->args[0]);
        return {};
    }

    // Терминальные узлы - подсекции раздела SPKG - упаковочная информация физического конструктива радиокомпонента.
    // Sna - перечисление имён имеющихся секций в составе конструктива (только для PC-CARDS).
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeSnaHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        if (GetWorkshop()->IsPrevNodeContainer(node_data, PDIFKeywords::PDIF_KEY_SPKG))
            return {};
        else
            return {{PCADLoadError::LOAD_FILE_COMMAND_UNACCEPTABLE_HERE, "Sna"}};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeSnaHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        // Извлекаем указатель на упаковочные данные в формате SPKG, которые в настоящий момент заполняются.
        NodeSPKGHandler::SPKGHelper* spkg_spec_info = (NodeSPKGHandler::SPKGHelper*)(node_data->spec_info.handler_spec_data.get());
        spkg_spec_info->sect_names.clear();
        // Создаем и заполняем словарь статьями, соответствующими секциям прибора, перечисленным в операндах данного узла.
        SectNameToPackInfo new_sects_pack_group;
        for (size_t section_index = 0; section_index < node_data->args.size(); ++section_index)
        {
            if (!holds_alternative<string>(node_data->args[section_index]))
                return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};   // Все аргументы должны быть строковыми.
            // Создаём словарную статью для секции с именем node_data->args[section_index].
            const string& next_section_name = get<string>(node_data->args[section_index]);
            new_sects_pack_group[next_section_name];
            // Также внесём весь полученный список имён секций в spkg_spec_info->sect_names, чтобы сохранить порядок их
            // следования в команде Sna на будущее.
            spkg_spec_info->sect_names.push_back(next_section_name);
        }
        // Переносим заготовку упаковочного словаря очередной группы секций в новый элемент массива-накопителя sect_spkg_data.
        // С данного момента именно эта группа секций становится активной и далее будет заполняться именно она.
        spkg_spec_info->sect_spkg_data.push_back(move(new_sects_pack_group));
        return {};
    }

    // Sp - карта ножек символа. Показывает соответствие выводов логического вентиля ножкам физического прибора.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeSpHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        if (GetWorkshop()->IsPrevNodeContainer(node_data, PDIFKeywords::PDIF_KEY_SPKG))
            return {};
        else
            return {{PCADLoadError::LOAD_FILE_COMMAND_UNACCEPTABLE_HERE, "Sp"}};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeSpHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        // Сначала получим указатель на текущий создаваемый радиокомпонент.
        PDIFFileWorkshop::TreeNodeData* current_component_node = GetWorkshop()->FindNodeByType(NodeSpecType::NODE_TYPE_RADIO_COMPONENT);
        if (!current_component_node)
            return {{PCADLoadError::LOAD_FILE_COMMAND_UNACCEPTABLE_HERE, "Sp"}};
        RadioComponentDesc::SourceData* component_data =
            (RadioComponentDesc::SourceData*)(current_component_node->spec_info.handler_spec_data.get());
        // Далее извлекаем указатель на упаковочные данные в формате SPKG, которые в настоящий момент заполняются.
        NodeSPKGHandler::SPKGHelper* spkg_spec_info = (NodeSPKGHandler::SPKGHelper*)(node_data->spec_info.handler_spec_data.get());
        // Выбираем для работы последнюю существующую группу однородных секций, которая сейчас как раз и заполняется.
        if (spkg_spec_info->sect_spkg_data.empty())
            return {{PCADLoadError::LOAD_FILE_COMMAND_UNACCEPTABLE_HERE, "Sp"}};
        SectNameToPackInfo& sect_name_to_pack = spkg_spec_info->sect_spkg_data.back();
        size_t section_count = sect_name_to_pack.size();   // Количество секций радиокомпонента, заданное ранее узлом "Sna".
        // Аргументы узла включают логическое имя вывода, карта которого заключена в параметрах данного узла, а также список порядковых
        // номеров (базированных к единице) в количестве, равным числу секций section_count в последней их гомогенной группе.
        if (node_data->args.size() != section_count + 1)
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}};  // Количество параметров узла неверное.

        if (!holds_alternative<string>(node_data->args[0]))    // Логическое имя вывода должно быть строкой.
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};
        const string& pin_logical_name = get<string>(node_data->args[0]);
        // max_pin_ordinal - максимальный ординал ножки (базированный к единице её порядковый номер).
        int max_pin_ordinal = static_cast<int>(component_data->pins.size());
        for (size_t pin_ordinal_index = 1; pin_ordinal_index < node_data->args.size(); ++pin_ordinal_index)
        {
            if (!holds_alternative<int64_t>(node_data->args[pin_ordinal_index]))
                // Все прочие параметры (порядковые номера ножек) должны быть целочисленными.
                return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};

            optional<int> checked_pin_ordinal = CheckIntValue(get<int64_t>(node_data->args[pin_ordinal_index]), max_pin_ordinal);
            if (!checked_pin_ordinal || checked_pin_ordinal.value() == 0)
                return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_VALUE, {}}};  // Ножки с таким ординалом не существует.
            // Выделим информацию о физическом выводе ("ножке") с ординалом checked_pin_ordinal.
            ComponentPinDef& component_pin_data = component_data->pins[checked_pin_ordinal.value() - 1];
            // Доформируем элемент упаковочной информации, который будет проекцировать логический вывод pin_logical_name на ножку
            // component_pin_data для секции next_section_name.
            const string& next_section_name = spkg_spec_info->sect_names[pin_ordinal_index - 1];
            sect_name_to_pack[next_section_name][pin_logical_name] = component_pin_data.pin_al_number;
        }
        return {};
    }

    // Apn - алфавитно-цифровой номер ножки.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeApnHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        if (GetWorkshop()->IsPrevNodeContainer(node_data, PDIFKeywords::PDIF_KEY_SPKG))
            return {};
        else
            return {{PCADLoadError::LOAD_FILE_COMMAND_UNACCEPTABLE_HERE, "Apn"}};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeApnHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        // Получаем вышележащий элемент стека объектов, описывающий загружаемый в данный момент радиокомпонент (основной или встроенный).
        PDIFFileWorkshop::TreeNodeData* current_component_node = GetWorkshop()->FindNodeByType(NodeSpecType::NODE_TYPE_RADIO_COMPONENT);
        if (!current_component_node)
            return {{PCADLoadError::LOAD_FILE_COMMAND_UNACCEPTABLE_HERE, "Apn"}};
        RadioComponentDesc::SourceData* component_data =
            (RadioComponentDesc::SourceData*)(current_component_node->spec_info.handler_spec_data.get());
        // Корректный узел содержит столько параметров, сколько определено ножек для данного радиокомпонента.
        if (node_data->args.size() != component_data->pins.size())
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}};
        for (size_t param_index = 0; param_index < node_data->args.size(); ++param_index)
        {
            // Все параметры - строки. Каждый из них - "алфавитно-цифровой номер" некоторой ножки, которые следуют тут в том же порядке,
            // в котором они определялись узлами P.
            if (!holds_alternative<string>(node_data->args[param_index]))
                return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};
            // Заменяем "алфавитно-цифровой номер" ножек на их новые значения, имена же оставляем прежними.
            component_data->pins[param_index].pin_al_number = get<string>(node_data->args[param_index]);
        }
        return {};
    }

    // Объекты-обработчики узлов параметров вставки радиокомпонент.
    // CN - сведения о соединении выводов экземпляра компонента с цепями схемы или платы.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeCNHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        if (GetWorkshop()->IsPrevNodeContainer(node_data, PDIFKeywords::PDIF_KEY_I))
            return {};   // CN - непосредственная подсекция I.
        else
            return {{PCADLoadError::LOAD_FILE_COMMAND_UNACCEPTABLE_HERE, "CN"}};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeCNHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        // Команда имеет два формата.
        // Для первого её формата узел должен содержать столько пар строковых параметров, сколько ножек имеет вставляемый радиокомпонент.
        // Каждая пара состоит из имени контакта (первый член пары - имя вывода или ножки) и имени токопроводящей цепи, к которой этот
        // контакт подсоединён. При отсутствии соединения в качестве имени цепи используется знак вопроса '?'.
        // Для второго формата имена контактов не приводятся, параметры представляют собой просто список соединённых с ними цепей.
        // В обоих случаях порядок и количество элементов (пар или строк) совпадают с порядком и количеством выводов компонента.

        // Получаем указатель на дескриптор блока "вставочной" информации, который мы, собственно, в данный момент и формируем.
        PDIFFileWorkshop::TreeNodeData* current_insertion_node = GetWorkshop()->FindNodeByType(NodeSpecType::NODE_TYPE_COMP_INSERTION);
        assert(current_insertion_node);
        if (!current_insertion_node)
            return {{PCADLoadError::LOAD_FILE_COMMAND_UNACCEPTABLE_HERE, "CN"}};
        NodeIHandler::IHelper* insertion_data = (NodeIHandler::IHelper*)(current_insertion_node->spec_info.handler_spec_data.get());

        // Отыскиваем определитель радиокомпонента, который требуется вставить в схему или на плату.
        int insert_component_index = GetWorkshop()->FindComponentByName(insertion_data->comp_name);
        if (insert_component_index < 0)
            return {{PCADLoadError::LOAD_FILE_COMPONENT_NOT_FOUND , "CN : "s + insertion_data->comp_name}};
        RadioComponentDesc& component_data = GetWorkshop()->load_file_data.radio_components[insert_component_index];

        // Проверим корректность количества аргументов узла, а также выясним формат команды.
        bool is_full_format = true;
        if (node_data->args.size() == component_data.pins_size())
            is_full_format = false; // Это второй (сокращённый) формат команды.
        else if (node_data->args.size() == component_data.pins_size() * 2)
            is_full_format = true; // Это первый (полный) формат команды.
        else    // Недопустимое количество аргументов узла.
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}};

        // Все параметры должны быть строковыми.
        for (size_t param_index = 0; param_index < node_data->args.size(); ++param_index)
            if (!holds_alternative<string>(node_data->args[param_index]))
                return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};

        // Форма всех аргументов верная, формат также допустим - переходим к его разбору.
        if (is_full_format)
        { // Анализ полного формата команды.
            for (size_t param_index = 0, proc_pin_index = 0; param_index < node_data->args.size(); param_index += 2, ++proc_pin_index)
            {
                string pin_name = get<string>(node_data->args[param_index]);
                const string& net_name = get<string>(node_data->args[param_index + 1]);

                if (net_name == "?")
                    continue;   // Вывод не подсоединён к какой-либо цепи - пропускаем этот терм.
                if (pin_name != "*")
                { // Проверим наличие вывода с таким именем среди существующих для данного радиокомпонента.
                    auto test_pin_it = component_data.pins_begin();
                    for (; test_pin_it != component_data.pins_end(); ++test_pin_it)
                    {
                        if (test_pin_it->pin_name == pin_name)
                            break;
                    }
                    if (test_pin_it == component_data.pins_end())
                        return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_VALUE, "Invalid_pin_name : "s + pin_name}};
                    proc_pin_index = test_pin_it - component_data.pins_begin();
                }
                else
                { // Явно имя подсоединяемого  вывода не задано, используем текущий вывод по порядку.
                    if (proc_pin_index >= component_data.pins_size())
                        return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_VALUE, "Invalid_pin_number : "s + to_string(proc_pin_index)}};
                    pin_name = (component_data.pins_begin() + proc_pin_index)->pin_name;
                }
                // Все параметры пары корректны, создаём описывающий её элемент в списке соединений.
                insertion_data->connect_info.push_back({move(pin_name), net_name});
            }
        }
        else
        { // Анализ краткого формата команды.
            for (size_t param_index = 0; param_index < node_data->args.size(); ++param_index)
            {
                const string& net_name = get<string>(node_data->args[param_index]);
                const string& component_pin_name = (component_data.pins_begin() + param_index)->pin_name;

                if (net_name == "?")
                    continue;   // Вывод не подсоединён к какой-либо цепи - пропускаем этот терм.
                insertion_data->connect_info.push_back({component_pin_name, net_name});
            }
        }
        return {};
    }

    // IPT - переназначение типов ножек для конкретного экземпляра установленного на плату радиокомпонента.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeIPTHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        if (GetWorkshop()->IsPrevNodeContainer(node_data, PDIFKeywords::PDIF_KEY_I))
            return {};  // IPT - непосредственная подсекция I.
        else
            return {{PCADLoadError::LOAD_FILE_COMMAND_UNACCEPTABLE_HERE, "IPT"}};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeIPTHandler::HandleCloseNode(TreeNodeData* node_data)
    { // Переопределение типов контакта для всех ножек данного экземпляра компонента.
        // Команда может иметь два формата - с указанием имён ножек (полный формат) и без его указания (краткий).
        // Получаем указатель на дескриптор блока "вставочной" информации, который мы, собственно, в данный момент и формируем.
        PDIFFileWorkshop* use_workshop = GetWorkshop();
        PDIFFileWorkshop::TreeNodeData* current_insertion_node = use_workshop->FindNodeByType(NodeSpecType::NODE_TYPE_COMP_INSERTION);
        assert(current_insertion_node);
        if (!current_insertion_node)
            return {{PCADLoadError::LOAD_FILE_COMMAND_UNACCEPTABLE_HERE, "IPT"}};
        NodeIHandler::IHelper* insertion_data = (NodeIHandler::IHelper*)(current_insertion_node->spec_info.handler_spec_data.get());

        // Отыскиваем определитель радиокомпонента, который требуется вставить в схему или на плату.
        int insert_component_index = use_workshop->FindComponentByName(insertion_data->comp_name);
        if (insert_component_index < 0)
            return {{PCADLoadError::LOAD_FILE_COMPONENT_NOT_FOUND , "IPT : "s + insertion_data->comp_name}};
        RadioComponentDesc& component_data = use_workshop->load_file_data.radio_components[insert_component_index];

        // Проверим корректность количества аргументов узла, а также выясним формат команды.
        bool is_full_format = true;
        if (node_data->args.size() == component_data.pins_size())
            is_full_format = false; // Это второй (сокращённый) формат команды.
        else if (node_data->args.size() == component_data.pins_size() * 2)
            is_full_format = true; // Это первый (полный) формат команды.
        else    // Недопустимое количество аргументов узла.
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}};

        // Все параметры должны быть строковыми.
            for (size_t param_index = 0; param_index < node_data->args.size(); ++param_index)
                if (!holds_alternative<string>(node_data->args[param_index]))
                    return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};

        // Форма всех аргументов верная, формат также допустим - переходим к его разбору.
        if (is_full_format)
        { // Анализ полного формата команды.
            for (size_t param_index = 0; param_index < node_data->args.size(); param_index += 2)
            {
                const string& pin_name = get<string>(node_data->args[param_index]);
                const string& str_pin_type = get<string>(node_data->args[param_index + 1]);
                // Проверим наличие вывода с таким именем среди существующих для данного радиокомпонента.
                auto test_pin_it = component_data.pins_begin();
                for (; test_pin_it != component_data.pins_end(); ++test_pin_it)
                {
                    if (test_pin_it->pin_name == pin_name)
                        break;
                }
                if (test_pin_it == component_data.pins_end())
                    return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_VALUE, "Invalid_pin_name : "s + pin_name}};

                if (optional<PinType> opt_pin_type = use_workshop->LoadPinTypeFromParam(str_pin_type); opt_pin_type)
                    insertion_data->pin_type_info.push_back({pin_name, opt_pin_type.value()});
                else
                    return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_VALUE, {}}};
            }
        }
        else
        { // Анализ краткого формата команды.
            for (size_t param_index = 0; param_index < node_data->args.size(); ++param_index)
            {
                const string& str_pin_type = get<string>(node_data->args[param_index]);
                // Выясним имя ножки с порядковым индексом param_index.
                if (param_index >= component_data.pins_size())
                    return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_VALUE, "Invalid_pin_number : "s + to_string(param_index)}};
                const string& pin_name = (component_data.pins_begin() + param_index)->pin_name;

                if (optional<PinType> opt_pin_type = use_workshop->LoadPinTypeFromParam(str_pin_type); opt_pin_type)
                    insertion_data->pin_type_info.push_back({pin_name, opt_pin_type.value()});
                else
                    return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_VALUE, {}}};
            }
        }
        return {};
    }

    // Rd - позиционное обозначение экземпляра вставленного экземпляра компонента - его текст и положение.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeRdHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeRdHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        // Параметры этого узла - текст конструкторского обозначения и двузначная координатная пара его реперной точки.
        if (node_data->args.size() != 3)
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}};
        // Сначала следует строка (текст позиционного обозначения), затем - числовая координатная пара.
        if (!holds_alternative<string>(node_data->args[0]) ||
            !holds_alternative<int64_t>(node_data->args[1]) || !holds_alternative<int64_t>(node_data->args[2]))
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};

        // Получаем указатель на дескриптор блока "вставочной" информации, который в данный момент формируется.
        PDIFFileWorkshop* use_workshop = GetWorkshop();
        PDIFFileWorkshop::TreeNodeData* current_insertion_node = use_workshop->FindNodeByType(NodeSpecType::NODE_TYPE_COMP_INSERTION);
        assert(current_insertion_node);
        if (!current_insertion_node)
            return {{PCADLoadError::LOAD_FILE_COMMAND_UNACCEPTABLE_HERE, "IPT"}};
        NodeIHandler::IHelper* insertion_data = (NodeIHandler::IHelper*)(current_insertion_node->spec_info.handler_spec_data.get());

        const string& position_designator_text = get<string>(node_data->args[0]);
        int text_anchor_x = get<int64_t>(node_data->args[1]),
            text_anchor_y = get<int64_t>(node_data->args[2]);
        wxPoint text_anchor_log = GetWorkshop()->ConvPntToLog(text_anchor_x, text_anchor_y);

        // Конструируем графический объект текстовой надписи с конструкторским обозначением данной копии радиокомпонента.
        insertion_data->refdes_obj = new ObjText
            (node_data->node_settings.layer_number, text_anchor_log,
             node_data->node_settings.text_orient, node_data->node_settings.text_height,
             node_data->node_settings.text_align, move(position_designator_text));
        return {};
    }

    // Pn - обозначение вывода элемента для его отдельной копии - текст и координаты точки размещения.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodePnHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodePnHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        // Параметры этого узла - текст подписи к ножке и двузначная координатная пара её якорной точки.
        if (node_data->args.size() != 3)
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}};
        // Сначала следует строка (текст подписи), затем - числовая координатная пара.
        if (!holds_alternative<string>(node_data->args[0]) ||
            !holds_alternative<int64_t>(node_data->args[1]) || !holds_alternative<int64_t>(node_data->args[2]))
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};

        // Получаем указатель на дескриптор блока "вставочной" информации, который в данный момент формируется.
        PDIFFileWorkshop* use_workshop = GetWorkshop();
        PDIFFileWorkshop::TreeNodeData* current_insertion_node = use_workshop->FindNodeByType(NodeSpecType::NODE_TYPE_COMP_INSERTION);
        assert(current_insertion_node);
        if (!current_insertion_node)
            return {{PCADLoadError::LOAD_FILE_COMMAND_UNACCEPTABLE_HERE, "Pn"}};
        NodeIHandler::IHelper* insertion_data = (NodeIHandler::IHelper*)(current_insertion_node->spec_info.handler_spec_data.get());

        const string& pin_label_text = get<string>(node_data->args[0]);
        int text_anchor_x = get<int64_t>(node_data->args[1]),
            text_anchor_y = get<int64_t>(node_data->args[2]);
        wxPoint text_anchor_log = GetWorkshop()->ConvPntToLog(text_anchor_x, text_anchor_y);

        // Конструируем графический текстовый объект очередной текстовой надписи с конструкторским обозначением данной копии радиокомпонента.
        insertion_data->pin_labels.push_back
            (new ObjText
                (node_data->node_settings.layer_number, text_anchor_log,
                 node_data->node_settings.text_orient, node_data->node_settings.text_height,
                 node_data->node_settings.text_align, move(pin_label_text)));
        ++insertion_data->current_pn_index;

        return {};
    }

    // ------ Группа обработчиков назначения "внутренних" атрибутов для различных сущностей PCAD-документа
    // (терминалы внутри подсекции ../ATR/IN).

    // Org - Точка привязки символа.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeOrgHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeOrgHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        // Параметры этого узла - двузначная координатная пара якорной точки радиокомпонента.
        if (node_data->args.size() != 2)
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}};
        // Координатная пара состоит из двух целых чисел.
        if (!holds_alternative<int64_t>(node_data->args[0]) || !holds_alternative<int64_t>(node_data->args[1]))
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};

        // Выделяем определитель радиокомпонента, с которым связана эта упаковка.
        PDIFFileWorkshop::TreeNodeData* current_component_node = GetWorkshop()->FindNodeByType(NodeSpecType::NODE_TYPE_RADIO_COMPONENT);
        assert(current_component_node);
        if (!current_component_node)
            return {{PCADLoadError::LOAD_FILE_COMMAND_UNACCEPTABLE_HERE, "Org"}};
        RadioComponentDesc::SourceData* component_data =
            (RadioComponentDesc::SourceData*)(current_component_node->spec_info.handler_spec_data.get());
        // Извлекаем из параметров узла и назначаем текущему библиотечному компоненту якорную точку привязки.
        int component_anchor_x = get<int64_t>(node_data->args[0]),
            component_anchor_y = get<int64_t>(node_data->args[1]);
        component_data->org_pos = GetWorkshop()->ConvPntToLog(component_anchor_x, component_anchor_y);
        return {};
    }

    // Ty - Тип компонента.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeTyHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeTyHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        // Параметрами такой узловой команды является одно единственное целое число - идентификационный код типа компонента (ID).
        if (node_data->args.size() != 1)
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}};
        // Идентификационный код типа компонента - целочисленный.
        if (!holds_alternative<int64_t>(node_data->args[0]))
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};

        // Выделяем определитель радиокомпонента, с которым связана эта упаковка.
        PDIFFileWorkshop::TreeNodeData* current_component_node = GetWorkshop()->FindNodeByType(NodeSpecType::NODE_TYPE_RADIO_COMPONENT);
        assert(current_component_node);
        if (!current_component_node)
            return {{PCADLoadError::LOAD_FILE_COMMAND_UNACCEPTABLE_HERE, "Ty"}};
        RadioComponentDesc::SourceData* component_data =
            (RadioComponentDesc::SourceData*)(current_component_node->spec_info.handler_spec_data.get());

        component_data->ty_id = get<int64_t>(node_data->args[0]);
        return {};
    }

    // Smd - Прибор, монтируемый на поверхность (только для печатной платы).
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeSmdHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeSmdHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        // Здесь параметр единственный.
        if (node_data->args.size() != 1)
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}};
        // И он - строка.
        if (!holds_alternative<string>(node_data->args[0]))
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};

        // Выделяем определитель радиокомпонента, с которым связана эта упаковка.
        PDIFFileWorkshop::TreeNodeData* current_component_node = GetWorkshop()->FindNodeByType(NodeSpecType::NODE_TYPE_RADIO_COMPONENT);
        assert(current_component_node);
        if (!current_component_node)
            return {{PCADLoadError::LOAD_FILE_COMMAND_UNACCEPTABLE_HERE, "Smd"}};
        RadioComponentDesc::SourceData* component_data =
            (RadioComponentDesc::SourceData*)(current_component_node->spec_info.handler_spec_data.get());
        string smd_flag = get<string>(node_data->args[0]);
        if (smd_flag == "Y")
            component_data->is_smd = true;
        else if (smd_flag == "N")
            component_data->is_smd = false;
        else
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_VALUE, smd_flag}};

        return {};
    }

    // Jmp - "Jumper" (только для печатной платы).
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeJmpHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeJmpHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        // Здесь параметр единственный.
        if (node_data->args.size() != 1)
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}};
        // И он - строка.
        if (!holds_alternative<string>(node_data->args[0]))
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};

        // Выделяем определитель радиокомпонента, с которым связана эта упаковка.
        PDIFFileWorkshop::TreeNodeData* current_component_node = GetWorkshop()->FindNodeByType(NodeSpecType::NODE_TYPE_RADIO_COMPONENT);
        assert(current_component_node);
        if (!current_component_node)
            return {{PCADLoadError::LOAD_FILE_COMMAND_UNACCEPTABLE_HERE, "Jmp"}};
        RadioComponentDesc::SourceData* component_data =
            (RadioComponentDesc::SourceData*)(current_component_node->spec_info.handler_spec_data.get());
        string jumper_flag = get<string>(node_data->args[0]);
        if (jumper_flag == "Y")
            component_data->is_jumper = true;
        else if (jumper_flag == "N")
            component_data->is_jumper = false;
        else
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_VALUE, jumper_flag}};

        return {};
    }

    // Pl - Координаты местоположения вставленного экземпляра радиокомпонента.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodePlHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodePlHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        // Параметры этого узла - двузначная координатная пара якорной точки радиокомпонента.
        if (node_data->args.size() != 2)
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}};
        // Координатная пара состоит из двух целых чисел.
        if (!holds_alternative<int64_t>(node_data->args[0]) || !holds_alternative<int64_t>(node_data->args[1]))
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};

        // Получаем указатель на дескриптор блока "вставочной" информации, который в данный момент формируется.
        PDIFFileWorkshop::TreeNodeData* current_insertion_node = GetWorkshop()->FindNodeByType(NodeSpecType::NODE_TYPE_COMP_INSERTION);
        assert(current_insertion_node);
        if (!current_insertion_node)
            return {{PCADLoadError::LOAD_FILE_COMMAND_UNACCEPTABLE_HERE, "Pn"}};
        NodeIHandler::IHelper* insertion_data = (NodeIHandler::IHelper*)(current_insertion_node->spec_info.handler_spec_data.get());

        // Извлекаем из параметров узла и назначаем экземпляру указанную точку местоположения.
        int insert_place_x = get<int64_t>(node_data->args[0]),
            insert_place_y = get<int64_t>(node_data->args[1]);
        insertion_data->place_pos = GetWorkshop()->ConvPntToLog(insert_place_x, insert_place_y);
        return {};
    }

    // Sc - Масштабные коэффициенты вставки компонента по осям X и Y.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeScHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeScHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        // Параметры этого узла - два масштабных коэффициента, определяющих относительный размер вставленного экземпляра
        // радиокомпонента относительно его библиотечного прототипа.
        if (node_data->args.size() != 2)
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}};
        // Оба масштабных коэффициента - целые числа, заданные в процентах.
        if (!holds_alternative<int64_t>(node_data->args[0]) || !holds_alternative<int64_t>(node_data->args[1]))
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};

        // Получаем указатель на дескриптор блока "вставочной" информации, который в данный момент формируется.
        PDIFFileWorkshop::TreeNodeData* current_insertion_node = GetWorkshop()->FindNodeByType(NodeSpecType::NODE_TYPE_COMP_INSERTION);
        assert(current_insertion_node);
        if (!current_insertion_node)
            return {{PCADLoadError::LOAD_FILE_COMMAND_UNACCEPTABLE_HERE, "Sc"}};
        NodeIHandler::IHelper* insertion_data = (NodeIHandler::IHelper*)(current_insertion_node->spec_info.handler_spec_data.get());
        // Назначаем вставке масштабы, указанные в процентах в параметрах данной узловой команды.
        insertion_data->scale_x = static_cast<double>(get<int64_t>(node_data->args[0])) / 100.0;  // Горизонтальный масштаб вставленной копии xкомпонента.
        insertion_data->scale_y = static_cast<double>(get<int64_t>(node_data->args[1])) / 100.0;  // Вертикальный масштаб вставленной копии компонента.
        return {};
    }

    // Ro - Угол вращения.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeRoHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeRoHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        // Параметр такого узла единственный.
        if (node_data->args.size() != 1)
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}};
        // И это целочисленный коэффициент нормального поворота в диапазоне от 0 до 3.
        if (!holds_alternative<int64_t>(node_data->args[0]))
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};

        // Получаем указатель на дескриптор блока "вставочной" информации, который в данный момент формируется.
        PDIFFileWorkshop::TreeNodeData* current_insertion_node = GetWorkshop()->FindNodeByType(NodeSpecType::NODE_TYPE_COMP_INSERTION);
        assert(current_insertion_node);
        if (!current_insertion_node)
            return {{PCADLoadError::LOAD_FILE_COMMAND_UNACCEPTABLE_HERE, "Sc"}};
        NodeIHandler::IHelper* insertion_data = (NodeIHandler::IHelper*)(current_insertion_node->spec_info.handler_spec_data.get());

        if (optional<int> checked_ro_val = CheckIntValue(get<int64_t>(node_data->args[0]), 3); checked_ro_val)
             insertion_data->rotate_factor = checked_ro_val.value();
        else
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_VALUE, {}}};   // Фактор нормального поворота вне допустимых пределов.

        return {};
    }

    // Mr -  Статус зеркальности (только для базы данных принципиальной схемы).
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeMrHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeMrHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        // Здесь параметр единственный.
        if (node_data->args.size() != 1)
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}};
        // И он - строка ("Y" или "N").
        if (!holds_alternative<string>(node_data->args[0]))
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};

        // Получаем указатель на дескриптор блока "вставочной" информации, который в данный момент формируется.
        PDIFFileWorkshop::TreeNodeData* current_insertion_node = GetWorkshop()->FindNodeByType(NodeSpecType::NODE_TYPE_COMP_INSERTION);
        assert(current_insertion_node);
        if (!current_insertion_node)
            return {{PCADLoadError::LOAD_FILE_COMMAND_UNACCEPTABLE_HERE, "Mr"}};
        NodeIHandler::IHelper* insertion_data = (NodeIHandler::IHelper*)(current_insertion_node->spec_info.handler_spec_data.get());

        string mirror_flag = get<string>(node_data->args[0]);
        if (mirror_flag == "Y")
            insertion_data->is_mirror = true;
        else if (mirror_flag == "N")
            insertion_data->is_mirror = false;
        else
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_VALUE, mirror_flag}};

        return {};
    }

    // Nl - Местоположение имени копии компонента.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeNlHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeNlHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        // Параметры этого узла - двузначная координатная пара якорной точки видимого изображения частного имени копии компонента.
        if (node_data->args.size() != 2)
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}};
        // Координатная пара состоит из двух целых чисел.
        if (!holds_alternative<int64_t>(node_data->args[0]) || !holds_alternative<int64_t>(node_data->args[1]))
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};

        // Получаем указатель на дескриптор блока "вставочной" информации, который в данный момент формируется.
        PDIFFileWorkshop::TreeNodeData* current_insertion_node = GetWorkshop()->FindNodeByType(NodeSpecType::NODE_TYPE_COMP_INSERTION);
        assert(current_insertion_node);
        if (!current_insertion_node)
            return {{PCADLoadError::LOAD_FILE_COMMAND_UNACCEPTABLE_HERE, "Nl"}};
        NodeIHandler::IHelper* insertion_data = (NodeIHandler::IHelper*)(current_insertion_node->spec_info.handler_spec_data.get());

        // Извлекаем из параметров узла и назначаем экземпляру указанную реперную точку видимой надписи, содержащей его частное имя.
        int insert_name_anchor_x = get<int64_t>(node_data->args[0]),
            insert_name_anchor_y = get<int64_t>(node_data->args[1]);
        insertion_data->ins_name_pos = GetWorkshop()->ConvPntToLog(insert_name_anchor_x, insert_name_anchor_y);
        return {};
    }

    // Ps - Сторона размещения компонентов (только для печатной платы).
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodePsHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodePsHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        // Параметр узла единственный и строковый (один символ - "T" или "B").
        if (node_data->args.size() != 1)
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}};
        if (!holds_alternative<string>(node_data->args[0]))
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};

        // Получаем указатель на дескриптор блока "вставочной" информации, который в данный момент формируется.
        PDIFFileWorkshop::TreeNodeData* current_insertion_node = GetWorkshop()->FindNodeByType(NodeSpecType::NODE_TYPE_COMP_INSERTION);
        assert(current_insertion_node);
        if (!current_insertion_node)
            return {{PCADLoadError::LOAD_FILE_COMMAND_UNACCEPTABLE_HERE, "Ps"}};
        NodeIHandler::IHelper* insertion_data = (NodeIHandler::IHelper*)(current_insertion_node->spec_info.handler_spec_data.get());

        string pcb_side_flag = get<string>(node_data->args[0]);
        if (pcb_side_flag == "T")
            insertion_data->on_top_side = true;
        else if (pcb_side_flag == "B")
            insertion_data->on_top_side = false;
        else
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_VALUE, pcb_side_flag}};

        return {};
    }

    // Pa - Угол установки компонента.
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodePaHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodePaHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        // Параметр такого узла единственный.
        if (node_data->args.size() != 1)
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}};
        // И это целочисленный угол установки радиокомпонента в диапазоне от 0 до 89 градусов по часовой стрелке.
        if (!holds_alternative<int64_t>(node_data->args[0]))
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};

        // Получаем указатель на дескриптор блока "вставочной" информации, который в данный момент формируется.
        PDIFFileWorkshop::TreeNodeData* current_insertion_node = GetWorkshop()->FindNodeByType(NodeSpecType::NODE_TYPE_COMP_INSERTION);
        assert(current_insertion_node);
        if (!current_insertion_node)
            return {{PCADLoadError::LOAD_FILE_COMMAND_UNACCEPTABLE_HERE, "Pa"}};
        NodeIHandler::IHelper* insertion_data = (NodeIHandler::IHelper*)(current_insertion_node->spec_info.handler_spec_data.get());

        if (optional<int> checked_pa_val = CheckIntValue(get<int64_t>(node_data->args[0]), 90); checked_pa_val)
             insertion_data->set_angle = checked_pa_val.value();
        else
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_VALUE, {}}};   // Угол установки вне допустимых пределов.

        return {};
    }

    // Un - Присваиваемое пользователем имя (для вставки компонента, только для PC-CAPS).
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeUnCompHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeUnCompHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        // Параметр узла единственный и строковый (один символ - "Y" или "N").
        if (node_data->args.size() != 1)
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}};
        if (!holds_alternative<string>(node_data->args[0]))
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};

        // Получаем указатель на дескриптор блока "вставочной" информации, который в данный момент формируется.
        PDIFFileWorkshop::TreeNodeData* current_insertion_node = GetWorkshop()->FindNodeByType(NodeSpecType::NODE_TYPE_COMP_INSERTION);
        assert(current_insertion_node);
        if (!current_insertion_node)
            return {{PCADLoadError::LOAD_FILE_COMMAND_UNACCEPTABLE_HERE, "Un"}};
        NodeIHandler::IHelper* insertion_data = (NodeIHandler::IHelper*)(current_insertion_node->spec_info.handler_spec_data.get());

        string user_name_flag = get<string>(node_data->args[0]);
        if (user_name_flag == "Y")
            insertion_data->is_user_ins_name = true;
        else if (user_name_flag == "N")
            insertion_data->is_user_ins_name = false;
        else
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_VALUE, user_name_flag}};

        return {};
    }

    // Un - Присваиваемое пользователем имя (для проводящей цепи, только для PC-CAPS).
    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeUnNetHandler::HandleOpenNode(TreeNodeData* node_data)
    {
        return {};
    }

    optional<FileWorkshop::ErrorInfo> PDIFFileWorkshop::NodeUnNetHandler::HandleCloseNode(TreeNodeData* node_data)
    {
        // Параметр узла единственный и строковый (один символ - "Y" или "N").
        if (node_data->args.size() != 1)
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAMS_QUANTITY, {}}};
        if (!holds_alternative<string>(node_data->args[0]))
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_TYPE, {}}};

        // Получаем указатель на формируемый в данный момент дескриптор токопроводящей цепи.
        PDIFFileWorkshop::TreeNodeData* current_net_node = GetWorkshop()->FindNodeByType(NodeSpecType::NODE_TYPE_NET_DESC);
        assert(current_net_node);
        if (!current_net_node)
            return {{PCADLoadError::LOAD_FILE_COMMAND_UNACCEPTABLE_HERE, "Un"}};
        NetDefDesc::SourceData* net_desc_data = (NetDefDesc::SourceData*)(current_net_node->spec_info.handler_spec_data.get());

        string user_name_flag = get<string>(node_data->args[0]);
        if (user_name_flag == "Y")
            net_desc_data->is_user_net_name = true;
        else if (user_name_flag == "N")
            net_desc_data->is_user_net_name = false;
        else
            return {{PCADLoadError::LOAD_FILE_INCORRECT_PARAM_VALUE, user_name_flag}};

        return {};
    }
} // namespace HandlerPDIF
