
#include <string>
#include <sstream>

#include "ApertureFile.h"

using namespace std;
using namespace aperture;

namespace
{
    struct AperSignatureDef
    {
        string signat;
        ApertureFileType aper_file_type;
    };

    static const int APR_FIRST_SECT_IDS[] = {6, 7};
    static const int APR_SECOND_SECT_IDS[] = {66, 67};
    enum APRIdsContent
    {
        ROUND_SHAPE_VAL = 0,
        SQUARE_SHAPE_VAL = 1
    };

    void DetectAPRApertureDCode(FlashDesc& cur_flash_desc)
    {
        size_t d_code_number = 0;
        if (cur_flash_desc.flash_number >= 1 && cur_flash_desc.flash_number <= 10)
            d_code_number = cur_flash_desc.flash_number + 9;
        else if (cur_flash_desc.flash_number >= 11 && cur_flash_desc.flash_number <= 12)
            d_code_number = cur_flash_desc.flash_number + 59;
        else if (cur_flash_desc.flash_number >= 13 && cur_flash_desc.flash_number <= 22)
            d_code_number = cur_flash_desc.flash_number + 7;
        else if (cur_flash_desc.flash_number >= 23 && cur_flash_desc.flash_number <= 24)
            d_code_number = cur_flash_desc.flash_number + 49;
        else if (cur_flash_desc.flash_number >= 25 && cur_flash_desc.flash_number <= 64)
            d_code_number = cur_flash_desc.flash_number + 5;
        else
            d_code_number = cur_flash_desc.flash_number + 9;
        cur_flash_desc.d_code = "D" + to_string(d_code_number);
    }

    bool ProcessAPRApertureString(string& aper_string, bool is_gerber32,
                                  FlashDesc& cur_flash_desc)
    {
            int round_shape_val, square_shape_val;
            if (is_gerber32)
            {
                round_shape_val = APR_FIRST_SECT_IDS[ROUND_SHAPE_VAL];
                square_shape_val = APR_FIRST_SECT_IDS[SQUARE_SHAPE_VAL];
            }
            else
            {
                round_shape_val = APR_SECOND_SECT_IDS[ROUND_SHAPE_VAL];
                square_shape_val = APR_SECOND_SECT_IDS[SQUARE_SHAPE_VAL];
            }

            int aper_num, aper_size_mil, aper_shape_val, aper_type_val;
            istringstream cur_string_istream(aper_string);
            cur_string_istream >> aper_num;
            cur_string_istream.ignore(32767, ',');
            cur_string_istream >> aper_size_mil;
            cur_string_istream.ignore(32767, ',');
            cur_string_istream >> aper_shape_val;
            cur_string_istream.ignore(32767, ',');
            cur_string_istream >> aper_type_val;
            if(cur_string_istream.fail())
                return false;

            cur_flash_desc.flash_number = aper_num;
            DetectAPRApertureDCode(cur_flash_desc);
            if (aper_type_val == 0)
                cur_flash_desc.flash_type = FlashType::FLASH_LINER;
            else if (aper_type_val == 1)
                cur_flash_desc.flash_type = FlashType::FLASH_FLASHER;
            else
                cur_flash_desc.flash_type = FlashType::FLASH_UNKNOWN;

            if (aper_shape_val == round_shape_val)
                cur_flash_desc.flash_shape = FlashShape::FLASH_ROUND;
            else if (aper_shape_val == square_shape_val)
                cur_flash_desc.flash_shape = FlashShape::FLASH_SQUARE;
            else
                cur_flash_desc.flash_shape = FlashShape::FLASH_UNKNOWN; //Апертура специальной формы

            cur_flash_desc.flash_size_inch = static_cast<double>(aper_size_mil) / 1000;
            cur_flash_desc.flash_size_mm = static_cast<double>(aper_size_mil) * 2.54 / 100;
            return true;
    }

    void PushAPRFlashDescToVector(bool is_gerber32, FlashDesc& cur_flash_desc, FlashDescCommon& result_flashes)
    {
        int flash_pos = cur_flash_desc.flash_number - 1;
        if (flash_pos < 0)
            return;

        vector<FlashDesc>* flash_vector_ptr;
        if (is_gerber32)
            flash_vector_ptr = &result_flashes.gerber32;
        else
            flash_vector_ptr = &result_flashes.gerber_laser;
        if (!cur_flash_desc.d_code.size())
            DetectAPRApertureDCode(cur_flash_desc);

        if (flash_pos < static_cast<int>(flash_vector_ptr->size()))
        {
            (*flash_vector_ptr)[flash_pos] = cur_flash_desc;
        }
        else
        {
            for (int i = flash_vector_ptr->size(); i < flash_pos; ++i)
            {
                FlashDesc temp_fd;
                temp_fd.flash_number = i + 1;
                DetectAPRApertureDCode(temp_fd);
                flash_vector_ptr->push_back(temp_fd);
            }
            flash_vector_ptr->push_back(cur_flash_desc);
        }
    }

    const AperSignatureDef sign_arr[] = {{"! GAPFile Version 1.0", ApertureFileType::APERTURE_FILE_GAP},
                                         {"V004", ApertureFileType::APERTURE_FILE_APR}};
}

namespace aperture
{
    ApertureFileType DetectApertureFileType(std::istream& aper_istr)
    {
        vector<bool> sign_flag(ArraySize(sign_arr), false);
        int symbol_pos = 0;

        while (aper_istr)
        {
            int c = aper_istr.get();
            if (!aper_istr || c < 0 || c > 255)
                break;
            bool is_any_signat_remain = false;
            for (int i = 0; i < static_cast<int>(sign_flag.size()); ++i)
            {
                if (!sign_flag[i])
                {
                    if (sign_arr[i].signat[symbol_pos] != c)
                    {
                        sign_flag[i] = true;
                    }
                    else
                    {
                        is_any_signat_remain = true;
                        if (symbol_pos == static_cast<int>(sign_arr[i].signat.size()) - 1)
                            return sign_arr[i].aper_file_type;
                    }
                }
            }
            if (!is_any_signat_remain)
                break;
            ++symbol_pos;
        }
        return ApertureFileType::APERTURE_FILE_NONE;
    }

    FlashDescCommon LoadAPRApertureFile(istream& aper_istr, PCADLoadError& error_code)
    {
        FlashDescCommon result_flashes;
        FlashDesc cur_flash_desc;
        string read_str;
        int block_num = 0, str_num = 0;
        error_code = PCADLoadError::LOAD_FILE_NO_ERROR;
        // Сбрасываем остаток первой строки после опеределения сигнатуры файла в функции DetectApertureFileType.
        getline(aper_istr, read_str);
        ++block_num;

        while (!aper_istr.eof())
        {
            getline(aper_istr, read_str);
            if (aper_istr.eof())
                break;
            if (aper_istr.fail())
            {
                error_code = PCADLoadError::LOAD_FILE_READ_ERROR;
                return result_flashes;
            }
            //Очередная строка считана успешно
            ++str_num;
            read_str = TrimString(read_str);
            if (!read_str.size())
                continue; // Считанная строка пуста, пропускаем её и переходим к следующей
            switch (block_num)
            {
                case 0: //Первый однострочный блок содержит сигнатуру файла, проверим ее правильность
                    if (read_str != "V004"s)
                    {
                        error_code = PCADLoadError::LOAD_FILE_BAD_FILE_FORMAT;
                        return result_flashes;
                    }
                    ++block_num;
                    break;
                case 1: //Во втором (как правило, однострочном) блоке содержится имя, под которым этот файл
                        //сохранен в программе PCPHOTO. Оно нам не нужно. Пропустим его, ожидая поступления
                        //строки правильного формата.
                    if (ProcessAPRApertureString(read_str, true, cur_flash_desc))
                        ++block_num;  // Формат строки правильный - началась секция апертур
                    else
                        break;
                case 2: //Первая секция описания апертур. Содержит апертуры для графопостроителя Gerber 32.
                        //Строки описания апертур имеют следующий вид: 1,32,66,1
                    if (ProcessAPRApertureString(read_str, true, cur_flash_desc))
                    { // Формат  строки верный, сведения об очередной апертуре получены
                        if (cur_flash_desc.flash_number < 1 || cur_flash_desc.flash_number > 255)
                        {
                            error_code = PCADLoadError::LOAD_FILE_BAD_FIELD_FORMAT;
                            return result_flashes;
                        }
                        PushAPRFlashDescToVector(true, cur_flash_desc, result_flashes);
                    }
                    else
                    { // Формат  строки неверный, секция с описаниями апертур для Gerber32 закончена
                        ++block_num;
                    }
                    break;
                case 3: //Проскакиваем также секцию текстовых описаний нестандартных апертур для Gerber 32.
                        //Там находятся строки, словесно описывающие тип и назначение нестандартной засветки.
                    if (ProcessAPRApertureString(read_str, false, cur_flash_desc))
                        ++block_num; // Формат строки правильный - началась следующая секция апертур
                    else
                        break;
                case 4: //Секция описания апертур для графопостроителя Gerber Laser.
                    if (ProcessAPRApertureString(read_str, false, cur_flash_desc))
                    { // Формат  строки верный, сведения об очередной апертуре получены
                        if (cur_flash_desc.flash_number < 1 || cur_flash_desc.flash_number > 255)
                        {
                            error_code = PCADLoadError::LOAD_FILE_BAD_FIELD_FORMAT;
                            return result_flashes;
                        }
                        PushAPRFlashDescToVector(false, cur_flash_desc, result_flashes);
                    }
                    else
                    { // Формат  строки неверный, секция с описаниями апертур Gerber Laser закончена
                        ++block_num;
                        error_code = PCADLoadError::LOAD_FILE_NO_ERROR;
                        return result_flashes;
                    }
                    break;
            }
        }
        error_code = PCADLoadError::LOAD_FILE_NO_ERROR;
        return result_flashes;
    }

    FlashDescCommon LoadGAPApertureFile(istream& aper_istr, PCADLoadError& error_code)
    {
        FlashDescCommon result_flashes;
        string read_str;
        int str_num = 0, aper_count = 0;
        error_code = PCADLoadError::LOAD_FILE_NO_ERROR;
        // Сбрасываем остаток первой строки после опеределения сигнатуры файла в функции DetectApertureFileType.
        getline(aper_istr, read_str);
        ++str_num;

        while (!aper_istr.eof())
        {
            getline(aper_istr, read_str);
            if (aper_istr.eof())
                break;
            if (aper_istr.fail())
            {
                error_code = PCADLoadError::LOAD_FILE_READ_ERROR;
                return result_flashes;
            }
            //Очередная строка считана успешно
            ++str_num;
            read_str = TrimString(read_str);
            if (!read_str.size())
                continue; // Считанная строка пуста, пропускаем её и переходим к следующей
            switch (str_num)
            {
                case 1: //Первая строка содержит сигнатуру файла, проверим ее правильность
                if (read_str != "! GAPFile Version 1.0"s)
                {
                    error_code = PCADLoadError::LOAD_FILE_BAD_FILE_FORMAT;
                    return result_flashes;
                }
                break;
            case 2:
                break; //Во второй строке название программы-генератора файла - пропускаем его.
            case 3: //Еще одна строка сигнатуры - проверяем
                if (read_str != "GAPFile   Version 1.0    (c) 1995 Advanced CAM Technologies, Inc."s)
                {
                    error_code = PCADLoadError::LOAD_FILE_BAD_FILE_FORMAT;
                    return result_flashes;
                }
                break;
            default: //Все остальные строки описывают апертуры - обрабатываем...
                //Строки имеют следующий вид:
                //D10 ROUND      32.00 0.813  0.00 0.000   0   0.00  0.00  D10   0   0.00  0.00 D0
                istringstream cur_string_istream(read_str);
                string aper_id, aper_type_name;
                double aper_size_mil, aper_size_mm;
                cur_string_istream >> aper_id >> aper_type_name >> aper_size_mil >> aper_size_mm;
                if (cur_string_istream.fail())
                {
                    error_code = PCADLoadError::LOAD_FILE_READ_ERROR;
                    return result_flashes;
                }
                ++aper_count;
                FlashDesc cur_flash_desc;
                cur_flash_desc.flash_number = aper_count;
                cur_flash_desc.d_code = aper_id;
                cur_flash_desc.flash_type = FlashType::FLASH_UNKNOWN;
                cur_flash_desc.flash_size_inch = aper_size_mil / 1000;
                cur_flash_desc.flash_size_mm = aper_size_mm;

                if (aper_type_name == "ROUND"s)
                    cur_flash_desc.flash_shape = FlashShape::FLASH_ROUND;
                else if (aper_type_name == "SQUARE"s)
                    cur_flash_desc.flash_shape = FlashShape::FLASH_SQUARE;
                else
                    cur_flash_desc.flash_shape = FlashShape::FLASH_UNKNOWN; //Апертура неизвестной формы

                if (aper_count <= 24)
                    result_flashes.gerber32.push_back(cur_flash_desc);
                result_flashes.gerber_laser.push_back(cur_flash_desc);
            }
        }
        error_code = PCADLoadError::LOAD_FILE_NO_ERROR;
        return result_flashes;
    }
} // namespace aperture
