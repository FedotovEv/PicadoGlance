
#include <iostream>
#include <string>
#include <vector>
#include <any>
#include <filesystem>
#include <cctype>

#include "gerber.h"
#include "PCADFile.h"
#include "PCADViewDraw.h"
#include "ApertureFile.h"
#include <wx/colour.h>
#include <wx/gdicmn.h>

using namespace std;
using namespace aperture;

namespace
{
    struct ComplexParam
    {
        char symbol = 0;
        bool is_param_int = false;
        bool is_param_double = false;
        union
        {
            int param_int;
            double param_double;
        };
    };

    enum class GerberOpCode
    {
        GERBER_OPCODE_NOP = 0,
        GERBER_LIGHT_ON,
        GERBER_LIGHT_OFF,
        GERBER_MAKE_FLASH,
        GERBER_MOVE_HEAD_X,
        GERBER_MOVE_HEAD_Y,
        GERBER_MOVE_HEAD_XY,
        GERBER_SET_APERTURE,
        GERBER_WORK_END,
        GERBER_ERROR
    };

    struct GerberOpDescription
    {
        GerberOpCode opcode;
        wxPoint point;
        string d_code;
    };

    struct ValueParamData
    {
        bool is_param_int = false;
        bool is_param_double = false;
        size_t position_after = string::npos;
        union
        {
            int param_int;
            double param_double;
        };
    };

    bool is_numeric_symb(char c)
    {
        return isdigit(c) || c == '.' || c == '+' || c == '-';
    }

    ValueParamData ExtractValueParam(const string& str_arg, size_t digital_param_start)
    { // Функция выделяет числовой параметр команды из строки str_arg, начиная с позиции digital_param_start.
        size_t digital_param_end = digital_param_start;
        while (digital_param_end < str_arg.size())
        {
            char next_char = str_arg[digital_param_end];
            if (!is_numeric_symb(next_char))
                break;
            ++digital_param_end;
        }

        ValueParamData result;
        result.position_after = digital_param_end;
        if (digital_param_end > digital_param_start)
        {
            string param_str = str_arg.substr(digital_param_start, digital_param_end - digital_param_start);
            try
            {
                if (param_str.find('.') != string::npos)
                {
                    result.param_double = stod(param_str);
                    result.is_param_double = true;
                }
                else
                {
                    result.param_int = stoi(param_str);
                    result.is_param_int = true;
                }
            }
            catch (exception&)
            {}
        }
        return result;
    }

    vector<ComplexParam> SplitStringToComplexParams(const string& arg_str)
    { // Функция разделяет строку-аргумент на параметрические блоки типа ComplexParam, каждый из
      // которых состоит из первой кодовой латинской буквы и следующего за ней числового параметра.
        vector<ComplexParam> result;
        size_t current_position = 0;

        while (current_position < arg_str.size())
        {
            ComplexParam next_param;
            char current_symbol = arg_str[current_position++];
            if (isalpha(current_symbol))
            {
                next_param.symbol = current_symbol;
                ValueParamData value_param_def = ExtractValueParam(arg_str, current_position);
                current_position = value_param_def.position_after;
                if (value_param_def.is_param_int)
                {
                    next_param.is_param_int = true;
                    next_param.param_int = value_param_def.param_int;
                }
                else if (value_param_def.is_param_double)
                {
                    next_param.is_param_double = true;
                    next_param.param_double = value_param_def.param_double;
                }
                if (next_param.is_param_int || next_param.is_param_double)
                    result.push_back(next_param);
                else
                    break;
            }
            else
            {
                break;
            }
        }

        return result;
    }

    vector<GerberOpDescription> DecodeGerberCommandPacket(const string& gerber_command)
    { // Функция разделяет кадр GERBER-команд gerber_command на составляющие его отдельные
      // команды типа GerberOpDescription.
        vector<GerberOpDescription> result;
        bool is_light_onoff_command = false;

        vector<ComplexParam> params = SplitStringToComplexParams(gerber_command);
        for (size_t i = 0; i < params.size(); ++i)
        {
            const ComplexParam& current_param = params[i];
            GerberOpDescription next_op;
            if (!current_param.is_param_int)
            {
                next_op.opcode = GerberOpCode::GERBER_ERROR;
            }
            else
            {
                switch (toupper(current_param.symbol))
                {
                case 'D':
                    switch (current_param.param_int)
                    {
                    case 1:
                        next_op.opcode = GerberOpCode::GERBER_LIGHT_ON;
                        break;
                    case 2:
                        next_op.opcode = GerberOpCode::GERBER_LIGHT_OFF;
                        break;
                    case 3:
                        next_op.opcode = GerberOpCode::GERBER_MAKE_FLASH;
                        break;
                    default:
                        next_op.opcode = GerberOpCode::GERBER_ERROR;
                        break;
                    }
                    break;
                case 'M':
                    if (current_param.param_int == 2)
                        next_op.opcode = GerberOpCode::GERBER_WORK_END;
                    else
                        next_op.opcode = GerberOpCode::GERBER_ERROR;
                    break;
                case 'G':
                    if (current_param.param_int == 54 || i >= params.size())
                    {
                        ComplexParam next_param = params[++i];
                        if (toupper(next_param.symbol) == 'D' && next_param.is_param_int)
                        {
                            next_op.opcode = GerberOpCode::GERBER_SET_APERTURE;
                            next_op.d_code = 'D' + to_string(next_param.param_int);
                        }
                        else
                        {
                            next_op.opcode = GerberOpCode::GERBER_ERROR;
                        }
                    }
                    else
                    {
                        next_op.opcode = GerberOpCode::GERBER_ERROR;
                    }
                    break;
                case 'X':
                    next_op.opcode = GerberOpCode::GERBER_MOVE_HEAD_X;
                    next_op.point.x = current_param.param_int;
                    break;
                case 'Y':
                    next_op.opcode = GerberOpCode::GERBER_MOVE_HEAD_Y;
                    next_op.point.y = current_param.param_int;
                    break;
                default:
                    next_op.opcode = GerberOpCode::GERBER_ERROR;
                    break;
                }
            }

            // Заносим выделенную из пакета команду в выходной массив result.
            if (next_op.opcode == GerberOpCode::GERBER_LIGHT_ON ||
                next_op.opcode == GerberOpCode::GERBER_LIGHT_OFF)
            { // Команды включения/выключения света выполняем перед командами движения головки в кадре
                if (is_light_onoff_command)
                    result[0] = next_op;
                else
                    result.insert(result.begin(), next_op);
                is_light_onoff_command = true;
            }
            else if (next_op.opcode == GerberOpCode::GERBER_MAKE_FLASH)
            {
                GerberOpDescription light_off_op;
                light_off_op.opcode = GerberOpCode::GERBER_LIGHT_OFF;
                if (is_light_onoff_command)
                    result[0] = light_off_op;
                else
                    result.insert(result.begin(), light_off_op);
                is_light_onoff_command = true;
                result.push_back(next_op);
            }
            else if (next_op.opcode == GerberOpCode::GERBER_MOVE_HEAD_X)
            { // Команду смены координат головки возможно требуется объединить с предыдущей, уже находящейся в result
                if (result.size())
                {
                    GerberOpDescription previous_op = result.back();
                    if (previous_op.opcode == GerberOpCode::GERBER_MOVE_HEAD_X)
                    {
                        result.pop_back();
                    }
                    else if (previous_op.opcode == GerberOpCode::GERBER_MOVE_HEAD_Y ||
                             previous_op.opcode == GerberOpCode::GERBER_MOVE_HEAD_XY)
                    {
                        result.pop_back();
                        next_op.point.y = previous_op.point.y;
                        next_op.opcode = GerberOpCode::GERBER_MOVE_HEAD_XY;
                    }
                }
                result.push_back(next_op);
            }
            else if (next_op.opcode == GerberOpCode::GERBER_MOVE_HEAD_Y)
            {
                if (result.size())
                {
                    GerberOpDescription previous_op = result.back();
                    if (previous_op.opcode == GerberOpCode::GERBER_MOVE_HEAD_X ||
                        previous_op.opcode == GerberOpCode::GERBER_MOVE_HEAD_XY)
                    {
                        result.pop_back();
                        next_op.point.x = previous_op.point.x;
                        next_op.opcode = GerberOpCode::GERBER_MOVE_HEAD_XY;
                    }
                    else if (previous_op.opcode == GerberOpCode::GERBER_MOVE_HEAD_Y)
                    {
                        result.pop_back();
                    }
                }
                result.push_back(next_op);
            }
            else if (next_op.opcode == GerberOpCode::GERBER_ERROR)
            {
                result.insert(result.begin(), next_op);
                break;
            }
            else
            {
                result.push_back(next_op);
            }
        }

        return result;
    }

    enum class ExcellonOpCode
    {
        EXCELLON_OPCODE_NOP = 0,
        EXCELLON_SERVICE,
        EXCELLON_SET_UNIT_MM,
        EXCELLON_SET_UNIT_INCH,
        EXCELLON_SET_DRILL_BIT,
        EXCELLON_PROCEED_DRILL,
        EXCELLION_WORK_END,
        EXCELLON_ERROR
    };

    struct DrillBitIdent
    {
        int number;
        double diameter;
    };

    struct ExcellonOpDescription
    {
        ExcellonOpCode opcode = ExcellonOpCode::EXCELLON_OPCODE_NOP;
        wxPoint point;
        bool is_x_specified = false;
        bool is_y_specified = false;
        DrillBitIdent drill_bit;
    };

    enum class WhatWait
    {
        WAIT_NOTHING = 0,
        WAIT_DRILLBIT_DIAMETER,
        WAIT_DRILL_COORD_X,
        WAIT_DRILL_COORD_Y,
        WAIT_DONE
    };

    ExcellonOpDescription DecodeExcellonCommand(const string& excellon_command)
    {
        ExcellonOpDescription result;

        if (!excellon_command.size())
            return result;

        if (excellon_command[0] == '/')
        {
            result.opcode = ExcellonOpCode::EXCELLON_SERVICE;
            return result;
        }

        WhatWait what_wait = WhatWait::WAIT_NOTHING;
        for (const ComplexParam& current_param : SplitStringToComplexParams(excellon_command))
        {
            if (result.opcode == ExcellonOpCode::EXCELLON_ERROR)
                break;
            switch (toupper(current_param.symbol))
            {
            case 'M':
                if (result.opcode != ExcellonOpCode::EXCELLON_OPCODE_NOP || !current_param.is_param_int)
                {
                    result.opcode = ExcellonOpCode::EXCELLON_ERROR;
                    break;
                }
                switch (current_param.param_int)
                {
                case 30:
                    result.opcode = ExcellonOpCode::EXCELLION_WORK_END;
                    break;
                case 71:
                    result.opcode = ExcellonOpCode::EXCELLON_SET_UNIT_MM;
                    break;
                case 72:
                    result.opcode = ExcellonOpCode::EXCELLON_SET_UNIT_INCH;
                    break;
                default:
                    result.opcode = ExcellonOpCode::EXCELLON_ERROR;
                    break;
                }
                what_wait = WhatWait::WAIT_DONE;
                break;
            case 'T':
                if (result.opcode != ExcellonOpCode::EXCELLON_OPCODE_NOP || !current_param.is_param_int)
                {
                    result.opcode = ExcellonOpCode::EXCELLON_ERROR;
                    break;
                }
                result.opcode = ExcellonOpCode::EXCELLON_SET_DRILL_BIT;
                result.drill_bit.number = current_param.param_int;
                result.drill_bit.diameter = -1;
                what_wait = WhatWait::WAIT_DRILLBIT_DIAMETER;
                break;
            case 'C':
                if (result.opcode != ExcellonOpCode::EXCELLON_SET_DRILL_BIT ||
                    what_wait != WhatWait::WAIT_DRILLBIT_DIAMETER)
                {
                    result.opcode = ExcellonOpCode::EXCELLON_ERROR;
                    break;
                }
                if (current_param.is_param_int)
                    result.drill_bit.diameter = current_param.param_int;
                else if (current_param.param_double)
                    result.drill_bit.diameter = current_param.param_double;
                what_wait = WhatWait::WAIT_DONE;
                break;
            case 'X':
                if (result.opcode == ExcellonOpCode::EXCELLON_OPCODE_NOP && current_param.is_param_int)
                {
                    result.opcode = ExcellonOpCode::EXCELLON_PROCEED_DRILL;
                    result.point.x = current_param.param_int;
                    result.is_x_specified = true;
                    result.is_y_specified = false;
                    what_wait = WhatWait::WAIT_DRILL_COORD_Y;
                }
                else if (result.opcode == ExcellonOpCode::EXCELLON_PROCEED_DRILL &&
                         what_wait == WhatWait::WAIT_DRILL_COORD_X && current_param.is_param_int)
                {
                    result.point.x = current_param.is_param_int;
                    result.is_x_specified = true;
                    what_wait = WhatWait::WAIT_DONE;
                }
                else
                {
                    result.opcode = ExcellonOpCode::EXCELLON_ERROR;
                }
                break;
            case 'Y':
                if (result.opcode == ExcellonOpCode::EXCELLON_OPCODE_NOP && current_param.is_param_int)
                {
                    result.opcode = ExcellonOpCode::EXCELLON_PROCEED_DRILL;
                    result.is_x_specified = false;
                    result.point.y = current_param.param_int;
                    result.is_y_specified = true;
                    what_wait = WhatWait::WAIT_DRILL_COORD_X;
                }
                else if (result.opcode == ExcellonOpCode::EXCELLON_PROCEED_DRILL &&
                         what_wait == WhatWait::WAIT_DRILL_COORD_Y && current_param.is_param_int)
                {
                    result.point.y = current_param.param_int;
                    result.is_y_specified = true;
                    what_wait = WhatWait::WAIT_DONE;
                }
                else
                {
                    result.opcode = ExcellonOpCode::EXCELLON_ERROR;
                }
                break;
            default:
                result.opcode = ExcellonOpCode::EXCELLON_ERROR;
                break;
            }
        }

        return result;
    }

    FileWorkshop::LoadFileResult ProcessExitData(vector<GraphObj*> graph_objects, vector<LayerDesc> layers,
             FileWorkshop::AdditionalLoadInfo& addit_load_info, FileDefValues file_values, PCADLoadError load_error)
    {
        int shift_y = file_values.frame_rect.GetBottom() + file_values.frame_rect.GetTop();
        PCADFile temp_pcad_doc(move(graph_objects), move(layers),
                               addit_load_info.aperture_provider, move(file_values));
        temp_pcad_doc.ShiftDocument(0, -shift_y);
        return {move(temp_pcad_doc), load_error};
    }
}

namespace HandlerGERBER
{
    FileWorkshop::LoadFileResult GERBERFileWorkshop::LoadPCADFile(istream& istr,
                                FileWorkshop::AdditionalLoadInfo& addit_load_info)
    {
        vector<GraphObj*> graph_objects; // Массив графических элементов файла
        vector<LayerDesc> layers; // Массив описателей слоёв изображения
        FileDefValues file_values;

        // Заполним частично заголовок file_values
        file_values.DBU_in_measure_unit = DBU_IN_MEASURE_UNIT; // 1000 DBU на дюйм
        file_values.file_flags = FILE_FLAG_UNIT_INCHES;
        file_values.picture_filepath = addit_load_info.picture_file_path;
        if (UpcaseString(addit_load_info.picture_file_path.extension().string()) == ".LGR"s)
        {
            file_values.file_signature = wxT("GERBER LASER");
            if (addit_load_info.is_control_aperture_data)
                addit_load_info.aperture_provider.SetApertureType(UsingApertureType::USING_APERTURE_GERBER_LASER);
        }
        else
        {
            file_values.file_signature = wxT("GERBER 32");
            if (addit_load_info.is_control_aperture_data)
                addit_load_info.aperture_provider.SetApertureType(UsingApertureType::USING_APERTURE_GERBER32);
        }
        file_values.file_workshop = this;
        // Gerber-файл не имеет внутренней слоевой структуры, поэтому создаём единственный слой, на котором и
        // будет размещаться весь рисунок.
        LayerDesc layer_desc;
        layer_desc.layer_number = 1;
        layer_desc.layer_name = "GERBER";
        layer_desc.layer_color = 15;
        layer_desc.layer_wx_color = *wxWHITE;
        layer_desc.layer_attributes = LayerAttributes{0};
        layers.push_back(layer_desc);
        // Ну а теперь считываем содержащиеся в файле команды, преобразуя их в два типа
        // примитивов - линии (отрезки) и вспышки (апертуры).
        aperture::FlashDesc current_aperture = addit_load_info.aperture_provider.GetFlashDesc(0);
        wxPoint current_point(0, 0);
        bool is_light_on = false;

        while (istr.good())
        {
            GraphObj* graph_obj_ptr = nullptr;
            // Считываем очередной кадр команд, отделённый символами '*'
            string gerber_command;
            getline(istr, gerber_command, '*');
            gerber_command = TrimString(gerber_command);
            if (!gerber_command.size())
                continue;
            for (GerberOpDescription& command : DecodeGerberCommandPacket(gerber_command))
            {
                switch (command.opcode)
                {
                case GerberOpCode::GERBER_LIGHT_ON:
                    is_light_on = true;
                    break;
                case GerberOpCode::GERBER_LIGHT_OFF:
                    is_light_on = false;
                    break;
                case GerberOpCode::GERBER_MAKE_FLASH:
                    graph_obj_ptr = new ObjFlash(0, wxPoint(current_point.x, -current_point.y),
                                                 FlashOrientation::FLASH_NORMAL,
                                                 current_aperture.flash_number, file_values,
                                                 addit_load_info.aperture_provider);
                    if (!graph_obj_ptr)
                        return ProcessExitData(move(graph_objects), move(layers), addit_load_info,
                                               move(file_values), PCADLoadError::LOAD_FILE_MEMORY_ERROR);
                    file_values.frame_rect.Union(graph_obj_ptr->GetFrameRect());
                    is_light_on = false;
                    break;
                case GerberOpCode::GERBER_MOVE_HEAD_X:
                case GerberOpCode::GERBER_MOVE_HEAD_Y:
                case GerberOpCode::GERBER_MOVE_HEAD_XY:
                    if (command.opcode == GerberOpCode::GERBER_MOVE_HEAD_X)
                        command.point.y = current_point.y;
                    else if (command.opcode == GerberOpCode::GERBER_MOVE_HEAD_Y)
                        command.point.x = current_point.x;
                    if (is_light_on)
                    {
                        graph_obj_ptr = new ObjLine(0, LineType::LINE_SOLID,
                                                    current_aperture.flash_size_inch * file_values.DBU_in_measure_unit,
                                                    wxPoint(current_point.x, -current_point.y),
                                                    wxPoint(command.point.x, -command.point.y));
                        if (!graph_obj_ptr)
                            return ProcessExitData(move(graph_objects), move(layers), addit_load_info,
                                                   move(file_values), PCADLoadError::LOAD_FILE_MEMORY_ERROR);
                        file_values.frame_rect.Union(graph_obj_ptr->GetFrameRect());
                    }
                    current_point = command.point;
                    break;
                case GerberOpCode::GERBER_SET_APERTURE:
                    current_aperture = addit_load_info.aperture_provider.GetFlashDesc(command.d_code);
                    break;
                case GerberOpCode::GERBER_WORK_END:
                    return ProcessExitData(move(graph_objects), move(layers), addit_load_info,
                                           move(file_values), PCADLoadError::LOAD_FILE_NO_ERROR);
                case GerberOpCode::GERBER_OPCODE_NOP:
                    break;
                case GerberOpCode::GERBER_ERROR:
                default:
                    return ProcessExitData(move(graph_objects), move(layers), addit_load_info,
                                           move(file_values), PCADLoadError::LOAD_FILE_BAD_RECORD_FORMAT);
                }
                if (graph_obj_ptr)
                    // Успешно сформирован очередной графический примитив - добавим его в список примитивов чертежа.
                    graph_objects.push_back(graph_obj_ptr);
            }

            if (istr.bad() || (istr.fail() && !istr.eof()))
                return ProcessExitData(move(graph_objects), move(layers), addit_load_info,
                                       move(file_values), PCADLoadError::LOAD_FILE_READ_ERROR);
        }

        return ProcessExitData(move(graph_objects), move(layers), addit_load_info,
                               move(file_values), PCADLoadError::LOAD_FILE_NO_ERROR);
    }

    FileWorkshop::LoadFileResult EXCELLONFileWorkshop::LoadPCADFile(std::istream& istr,
                                        FileWorkshop::AdditionalLoadInfo& addit_load_info)
    {
        vector<GraphObj*> graph_objects; // Массив графических элементов файла
        vector<LayerDesc> layers; // Массив описателей слоёв изображения
        FileDefValues file_values;

        // Заполним некоторые поля заголовока file_values
        file_values.file_signature = wxT("EXCELLON");
        file_values.file_workshop = this;
        file_values.file_flags = 0;
        file_values.picture_filepath = addit_load_info.picture_file_path;
        file_values.DBU_in_measure_unit = -1; // Пока выставим признак неизвестной системы мер
        // Excellon-файл не имеет внутренней слоевой структуры, поэтому создаём единственный слой, на котором и
        // будет размещаться весь рисунок сверловки.
        LayerDesc layer_desc;
        layer_desc.layer_number = 1;
        layer_desc.layer_name = "EXCELLON";
        layer_desc.layer_color = 10;
        layer_desc.layer_wx_color = *wxRED;
        layer_desc.layer_attributes = LayerAttributes{0};
        layers.push_back(layer_desc);

        // Ну а теперь считываем содержащиеся в файле команды, преобразуя их в единственный тип
        // примитивов - "закрашенный круг" - круг, толщина линии которого обеспечивает его полное закрашивание.
        unordered_map<int, double> bit_number_to_diameter;
        wxPoint last_drill_point(0, 0);
        DrillBitIdent current_drill_bit{1, 1.0};

        while (istr.good())
        {
            GraphObj* graph_obj_ptr = nullptr;
            double work_drill_diameter;
            // Считываем очередной кадр команд, отделённый символами '*'
            string excellon_command;
            getline(istr, excellon_command);
            excellon_command = TrimString(excellon_command);
            if (!excellon_command.size())
                continue;

            ExcellonOpDescription command = DecodeExcellonCommand(excellon_command);
            switch (command.opcode)
            {
            case ExcellonOpCode::EXCELLON_SERVICE:
                break;
            case ExcellonOpCode::EXCELLON_SET_UNIT_MM:
                if (file_values.DBU_in_measure_unit > 0)
                    return ProcessExitData(move(graph_objects), move(layers), addit_load_info,
                                           move(file_values), PCADLoadError::LOAD_FILE_BAD_RECORD_FORMAT);

                file_values.DBU_in_measure_unit = MM_DBU_IN_MEASURE_UNIT; // 100 DBU на миллиметр
                break;
            case ExcellonOpCode::EXCELLON_SET_UNIT_INCH:
                if (file_values.DBU_in_measure_unit > 0)
                    return ProcessExitData(move(graph_objects), move(layers), addit_load_info,
                                           move(file_values), PCADLoadError::LOAD_FILE_BAD_RECORD_FORMAT);

                file_values.DBU_in_measure_unit = INCH_DBU_IN_MEASURE_UNIT; // 10000 DBU на дюйм
                file_values.file_flags = FILE_FLAG_UNIT_INCHES;
                break;
            case ExcellonOpCode::EXCELLON_SET_DRILL_BIT:
                if (command.drill_bit.diameter > 0)
                {
                    bit_number_to_diameter[command.drill_bit.number] = command.drill_bit.diameter;
                    current_drill_bit = command.drill_bit;
                }
                else if (bit_number_to_diameter.count(command.drill_bit.number))
                {
                    current_drill_bit.number = command.drill_bit.number;
                    current_drill_bit.diameter = bit_number_to_diameter[command.drill_bit.number];
                }
                else
                {
                    current_drill_bit.number = command.drill_bit.number;
                    current_drill_bit.diameter = 1;
                }
                break;
            case ExcellonOpCode::EXCELLON_PROCEED_DRILL:
                if (file_values.DBU_in_measure_unit <= 0)
                    return ProcessExitData(move(graph_objects), move(layers), addit_load_info,
                                           move(file_values), PCADLoadError::LOAD_FILE_BAD_RECORD_FORMAT);
                if (!command.is_x_specified)
                    command.point.x = last_drill_point.x;
                if (!command.is_y_specified)
                    command.point.y = last_drill_point.y;
                last_drill_point = command.point;

                if (file_values.file_flags & FILE_FLAG_UNIT_INCHES)
                { // Целевая система координат - дюймовая (целевая единица 1/10000 дюйма)
                  // Диаметр сверла также переводим в десятитысячные дюйма
                    work_drill_diameter = current_drill_bit.diameter / MILLIMETERS_IN_INCH * 10000;
                }
                else
                { // Целевая система координат - метрическая (целевая единица 1/100 миллиметра)
                  // Операционная система метрическая, нужно скорректировать точку command.point до сотых миллиметра
                    command.point.x *= TO_MM_SCALE_FACTOR;
                    command.point.y *= TO_MM_SCALE_FACTOR;
                    // Диаметр сверла также переводим в сотые миллиметра
                    work_drill_diameter = current_drill_bit.diameter * 100;
                }
                // На данный момент имеем координаты центра отверстия command.point и диаметр сверла work_drill_diameter
                // в целевой системе координат.
                graph_obj_ptr = new ObjCirc(0, LineType::LINE_SOLID, work_drill_diameter / 2,
                                            wxPoint(command.point.x, -command.point.y), work_drill_diameter / 4);
                if (!graph_obj_ptr)
                    return ProcessExitData(move(graph_objects), move(layers), addit_load_info,
                                           move(file_values), PCADLoadError::LOAD_FILE_MEMORY_ERROR);
                file_values.frame_rect.Union(graph_obj_ptr->GetFrameRect());
                break;
            case ExcellonOpCode::EXCELLION_WORK_END:
                return ProcessExitData(move(graph_objects), move(layers), addit_load_info,
                                       move(file_values), PCADLoadError::LOAD_FILE_NO_ERROR);
            case ExcellonOpCode::EXCELLON_OPCODE_NOP:
                break;
            case ExcellonOpCode::EXCELLON_ERROR:
            default:
                return ProcessExitData(move(graph_objects), move(layers), addit_load_info,
                                       move(file_values), PCADLoadError::LOAD_FILE_BAD_RECORD_FORMAT);
            }

            if (graph_obj_ptr)
                // Успешно сформирован очередной графический примитив - добавим его в список примитивов чертежа.
                graph_objects.push_back(graph_obj_ptr);

            if (istr.bad() || (istr.fail() && !istr.eof()))
                return ProcessExitData(move(graph_objects), move(layers), addit_load_info,
                                       move(file_values), PCADLoadError::LOAD_FILE_READ_ERROR);
        }

        return ProcessExitData(move(graph_objects), move(layers), addit_load_info,
                               move(file_values), PCADLoadError::LOAD_FILE_NO_ERROR);
    }
} // namespace HandlerGERBER
