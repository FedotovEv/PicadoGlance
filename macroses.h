#pragma once

//Имя нашей программы (должен совпадать с именем ее исполняемого файла)
#define APPLICATION_NAME "PCADViewer"
//Константы для перевода одних единиц измерения длины в другие
#define POINTS_IN_MILLIMETER 3.94
#define MILS_IN_MILLIMETER 39.4
#define POINTS_IN_INCH 100
#define MILLIMETERS_IN_INCH 25.4
// Дополнительный масштаб, необходимый для работы с "русским дюймом"
// (если при работе в дюймовых координатах принять дюйм равным точно 2.5 мм).
#define RUSSIAN_INCH_SCALE 0.984252
//Максимальное значение для полос прокрутки в программе.
#define DEFAULT_SCROLL_RANGE_VALUE 1000
//Некоторые графические маски
#define SOLID_LINE_MASK  0xffffffff //Маска сплошной линии
#define DOTTED_LINE_MASK 0xf0f0f0f0 //Маска штриховой линии
#define DOTTED_RAPID_LINE_MASK 0x55555555 //Маска учащённой штриховой линии
#define DASHED_LINE_MASK 0xf860f860 //Маска пунктирной линии
#define COLOR_24BITS_MASK 0xFFFFFF // Маска выделения 24-битового цвета из 32-битового значения
//Граничное значение для сравнения на равенство дробных чисел
#define ZERO_TOLERANCE 0.0000001
#define DOT_SIZE 10.0
#define DOT_SIZE_INT 10
#define LIMIT_RAY_LENGTH 100.0
#define SELECT_CONTOUR_WIDTH 6 // Толщина выделяющего контура
#define MAX_VARY_CANVASES 20 // Максимальное количество переменных (нестандартных) холстов
#define VARY_CANVASES_MAX_NAME_LEN 20 // Максимальная длина имени пользовательского холста

enum class MeasureUnitTypeData
{
    MEASURE_UNIT_DBU = 1,
    MEASURE_UNIT_MM,
    MEASURE_UNIT_INCH
};

enum class TextExportStyleType
{
    TEXT_EXPORT_PRECISION_STYLE = 1,
    TEXT_EXPORT_CONDENSED_STYLE
};

enum class ContourSideDetectType
{
    CONTOUR_NOT_INTERSECT = 0,
    CONTOUR_INTERSECT_LEFT,
    CONTOUR_INTERSECT_RIGHT,
    CONTOUR_INTERSECT_TOP,
    CONTOUR_INTERSECT_BOTTOM,
    CONTOUR_INTERSECT_LEFT_TOP,
    CONTOUR_INTERSECT_LEFT_BOTTOM,
    CONTOUR_INTERSECT_RIGHT_TOP,
    CONTOUR_INTERSECT_RIGHT_BOTTOM
};

enum class PCADLoadError
{
    LOAD_FILE_NO_ERROR = 0,
    LOAD_FILE_READ_ERROR,
    LOAD_FILE_SEEK_ERROR,
    LOAD_FILE_BAD_FILE_FORMAT,
    LOAD_FILE_BAD_FILE_HEADER,
    LOAD_FILE_BAD_RECORD_FORMAT,
    LOAD_FILE_BAD_FIELD_FORMAT,
    LOAD_FILE_MEMORY_ERROR
};

#include <string>

std::string UpcaseString(const std::string& arg_string);
std::string TrimString(const std::string& arg_string);
