#pragma once

//��� ����� ��������� (������ ��������� � ������ �� ������������ �����)
#define APPLICATION_NAME "PCADViewer"
//��������� ��� �������� ����� ������ ��������� ����� � ������
#define POINTS_IN_MILLIMETER 3.94
#define MILS_IN_MILLIMETER 39.4
#define POINTS_IN_INCH 100
#define MILLIMETERS_IN_INCH 25.4
// �������������� �������, ����������� ��� ������ � "������� ������"
// (���� ��� ������ � �������� ����������� ������� ���� ������ ����� 2.5 ��).
#define RUSSIAN_INCH_SCALE 0.984252
//������������ �������� ��� ����� ��������� � ���������.
#define DEFAULT_SCROLL_RANGE_VALUE 1000
//��������� ����������� �����
#define SOLID_LINE_MASK  0xffffffff //����� �������� �����
#define DOTTED_LINE_MASK 0xf0f0f0f0 //����� ��������� �����
#define DOTTED_RAPID_LINE_MASK 0x55555555 //����� ��������� ��������� �����
#define DASHED_LINE_MASK 0xf860f860 //����� ���������� �����
#define COLOR_24BITS_MASK 0xFFFFFF // ����� ��������� 24-�������� ����� �� 32-�������� ��������
//��������� �������� ��� ��������� �� ��������� ������� �����
#define ZERO_TOLERANCE 0.0000001
#define DOT_SIZE 10.0
#define DOT_SIZE_INT 10
#define LIMIT_RAY_LENGTH 100.0
#define SELECT_CONTOUR_WIDTH 6 // ������� ����������� �������
#define MAX_VARY_CANVASES 20 // ������������ ���������� ���������� (�������������) �������
#define VARY_CANVASES_MAX_NAME_LEN 20 // ������������ ����� ����� ����������������� ������

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

enum class AdditionalScaleModeType
{
    ADDITIONAL_SCALE_NONE = 0,
    ADDITIONAL_SCALE_RUSSIAN_INCH,
    ADDITIONAL_SCALE_ANY
};

#include <string>
#include <wx/string.h>

struct OptionsData
{
    // ��������� ��� �������� ����� ������� (�������)
    wxString aperture_filename; // ������ ��� ������� ����� �������
    bool is_aperture_gerber_laser = false; // == true, ���� ������� ������������ �������� ���� GERBER LASER
    // ���� ������������� ������������� �������������� ������������ ��� ��������� �� ������
    bool is_use_screen_subscale = false;
    // ������ ��������� ����������� chr ���������� ������ ��� ������� ��������� ����������
    bool is_use_chr_text_engine = false;
    // ������������ ��� ������ ��������� �������� (������ ��� ����� �����-������ ��������)
    MeasureUnitTypeData measure_unit_type = MeasureUnitTypeData::MEASURE_UNIT_DBU;
    // ��������� ������ "�������������� ������� �����������"
    AdditionalScaleModeType additional_scale_mode = AdditionalScaleModeType::ADDITIONAL_SCALE_NONE;
    double additional_scale_x = 1;
    double additional_scale_y = 1;
    wxString picture_filename; // ��� ������� ��� ��������, ����������� ����� ��������� ������
};

std::string UpcaseString(const std::string& arg_string);
std::string TrimString(const std::string& arg_string);
