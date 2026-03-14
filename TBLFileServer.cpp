
#include "TBLFileServer.h"
#include <fstream>
#include <charconv>
#include <array>
#include <algorithm>

#include "redefine_.h"

using namespace std;
using namespace std::literals;

string ErrMsgDrillBitNotFound(int test_drill_bit)
{
    return (_("Сверло с номером ") + wxString::FromCDouble(test_drill_bit, 0) + _(" не описано в базе")).ToStdString();
}

string ErrMsgPinAlreadyExists(int test_pin, int drill_bit)
{
    return (_("Выводу ") + wxString::FromCDouble(test_pin, 0) + _(" уже назначено сверло ") + wxString::FromCDouble(drill_bit, 0)).ToStdString();
}

string ErrMsgDrillBitAlreadyExists(int test_drill_bit)
{
    return (_("Сверло с номером ") + wxString::FromCDouble(test_drill_bit, 0) + _(" уже описано в базе")).ToStdString();
}

// Получение всей имеющейся информации разом.
pair<TBLFileServer::PinDrillBitV, TBLFileServer::DrillBitParamsV> TBLFileServer::GetAsVectors() const
{
    return {GetPinDrillBitVector(), GetDrillBitParamsVector()};
}

// Получение имеющейся у сервера информации по частям (покомпонентно).
TBLFileServer::PinDrillBitV TBLFileServer::GetPinDrillBitVector() const
{
    PinDrillBitV result_pin_drill_bit_vec;
    for (const PinDrillBit& scan_pin_drill_bit : m_pin_drill_bit_set)
        result_pin_drill_bit_vec.push_back(scan_pin_drill_bit);
    return result_pin_drill_bit_vec;
}

const TBLFileServer::PinDrillBit* TBLFileServer::GetPinDrillBit(int pin) const
{
    if (auto pin_drill_bit_it = m_pin_drill_bit_set.find({pin, 0});
        pin_drill_bit_it != m_pin_drill_bit_set.end())
        return &(*pin_drill_bit_it);
    else
        return nullptr;
}

TBLFileServer::DrillBitParamsV TBLFileServer::GetDrillBitParamsVector() const
{
    DrillBitParamsV result_drill_bit_param_vec;
    for (const DrillBitParams& scan_drill_bit_params : m_drill_bit_params_set)
        result_drill_bit_param_vec.push_back(scan_drill_bit_params);

    return result_drill_bit_param_vec;
}

const TBLFileServer::DrillBitParams* TBLFileServer::GetDrillBitParams(int drill_bit) const
{
    if (auto drill_bit_params_it = m_drill_bit_params_set.find({drill_bit, 0.0});
        drill_bit_params_it != m_drill_bit_params_set.end())
        return &(*drill_bit_params_it);
    else
        return nullptr;
}

// Установка (загрузка) данных в сервер комплексно, сразу полным комплектом.
vector<FileWorkshop::ErrorInfo> TBLFileServer::SetAsVectors
    (pair<PinDrillBitV, DrillBitParamsV>&& input_vectors_pair, bool is_clear_previous)
{
    vector<FileWorkshop::ErrorInfo> err_vec_1 =
        SetDrillBitParamsVector(std::move(input_vectors_pair.second), is_clear_previous);
    vector<FileWorkshop::ErrorInfo> err_vec_2 =
        SetPinDrillBitVector(std::move(input_vectors_pair.first), is_clear_previous);

    err_vec_1.insert(err_vec_1.begin(), make_move_iterator(err_vec_2.begin()), make_move_iterator(err_vec_2.end()));
    return err_vec_1;
}

// Установка (загрузка) информации о параметрах сверловки в сервер по частям.
vector<FileWorkshop::ErrorInfo> TBLFileServer::SetPinDrillBitVector
    (PinDrillBitV&& pin_drill_bit_vector, bool is_clear_previous, bool is_check_drill_exists)
{
    vector<FileWorkshop::ErrorInfo> result_errs;
    if (is_clear_previous)
        m_pin_drill_bit_set.clear();

    for (PinDrillBit& scan_pin_drill_bit : pin_drill_bit_vector)
    {
        int scan_pin = scan_pin_drill_bit.pin,
            scan_drill_bit = scan_pin_drill_bit.drill_bit;
        if (is_check_drill_exists && m_drill_bit_params_set.find({scan_drill_bit, 0.0}) == m_drill_bit_params_set.end())
            result_errs.push_back({PCADLoadError::VALUE_NOT_FOUND, ErrMsgDrillBitNotFound(scan_drill_bit)});
        else if (!m_pin_drill_bit_set.insert(move(scan_pin_drill_bit)).second)
            result_errs.push_back({PCADLoadError::VALUE_ALREADY_EXISTS, ErrMsgPinAlreadyExists(scan_pin, scan_drill_bit)});
    }

    return result_errs;
}

vector<FileWorkshop::ErrorInfo> TBLFileServer::SetDrillBitParamsVector(DrillBitParamsV&& drill_bit_params_vector, bool is_clear_previous)
{
    vector<FileWorkshop::ErrorInfo> result_errs;
    if (is_clear_previous)
        m_drill_bit_params_set.clear();

    for (DrillBitParams& scan_drill_bit_params: drill_bit_params_vector)
    {
        int scan_drill_bit = scan_drill_bit_params.drill_bit;
        if (!m_drill_bit_params_set.insert(move(scan_drill_bit_params)).second)
            result_errs.push_back({PCADLoadError::VALUE_ALREADY_EXISTS, ErrMsgDrillBitAlreadyExists(scan_drill_bit)});
    }

    return result_errs;
}

// Операции над отдельными элементами информационных таблиц сверловки.
// Добавление (или изменение, если запись для такого типа вывода уже существует) информации в таблицу соспоставления типов выводов
// и номеров инструментов.
pair<FileWorkshop::ErrorInfo, bool> TBLFileServer::SetPinDrillBit
    (PinDrillBit&& pin_drill_bit, bool is_assign, bool is_check_drill_exists)
{
    if (is_check_drill_exists)
    {
        if (m_drill_bit_params_set.find({pin_drill_bit.drill_bit, 0.0}) == m_drill_bit_params_set.end())
            return {{PCADLoadError::VALUE_NOT_FOUND, ErrMsgDrillBitNotFound(pin_drill_bit.drill_bit)}, false};
    }

    bool is_dublicate = m_pin_drill_bit_set.count(pin_drill_bit);
    if (is_dublicate)
    {
        if (!is_assign)
            return {{PCADLoadError::VALUE_ALREADY_EXISTS, {}}, false};
        else
            m_pin_drill_bit_set.erase(pin_drill_bit);
    }
    m_pin_drill_bit_set.insert(move(pin_drill_bit));
    return {{PCADLoadError::LOAD_FILE_NO_ERROR, {}}, is_dublicate};
}

FileWorkshop::ErrorInfo TBLFileServer::RemovePinDrillBit(int pin)
{
    if (m_pin_drill_bit_set.erase({pin, 0}))
        return {PCADLoadError::LOAD_FILE_NO_ERROR, {}};
    else
        return {PCADLoadError::VALUE_NOT_FOUND, {}};
}

// Добавление (или изменение, если запись для такого сверла уже существует) записи с параметрами некоторого сверла.
pair<FileWorkshop::ErrorInfo, bool> TBLFileServer::SetDrillBitParams(DrillBitParams&& drill_bit_params, bool is_assign)
{
    bool is_dublicate = m_drill_bit_params_set.count(drill_bit_params);
    if (is_dublicate)
    {
        if (!is_assign)
            return {{PCADLoadError::VALUE_ALREADY_EXISTS, {}}, false};
        else
            m_drill_bit_params_set.erase(drill_bit_params);
    }
    m_drill_bit_params_set.insert(move(drill_bit_params));
    return {{PCADLoadError::LOAD_FILE_NO_ERROR, {}}, is_dublicate};
}

vector<int> TBLFileServer::ResetPinDefinitions(int drill_bit, int new_drill_bit)
{
    vector<int> corrected_pins;
    while (true)
    {
        auto pin_drill_bit_it = find_if(m_pin_drill_bit_set.begin(), m_pin_drill_bit_set.end(),
                [drill_bit, &corrected_pins](const PinDrillBit& test_pin_drill_bit) -> bool
                {
                    if (test_pin_drill_bit.drill_bit == drill_bit)
                    {
                        corrected_pins.push_back(test_pin_drill_bit.pin);
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                });
        if (pin_drill_bit_it == m_pin_drill_bit_set.end())
            break;
        // Обнуляем сверло, назначенное найденному выводу pin_drill_bit_it, которому ранее было назначено сверло drill_bit.
        auto pin_drill_bit_handle = m_pin_drill_bit_set.extract(pin_drill_bit_it);
        pin_drill_bit_handle.value().drill_bit = new_drill_bit;
        m_pin_drill_bit_set.insert(move(pin_drill_bit_handle));
    }
    return corrected_pins;
}

// Метод удаляет из инструментальной таблицы данные о сверле с номером drill_bit. Кроме того, функция обеспечивает дополнительный
// функционал - замена всех ссылок на удаляемое сверло drill_bit в таблице назначений на новое сверло с номером new_drill_bit.
pair<FileWorkshop::ErrorInfo, vector<int>> TBLFileServer::RemoveDrillBitParams
    (int drill_bit, bool is_correct_pin_drill_bit, int new_drill_bit)
{
    vector<int> corrected_pins;

    if (m_drill_bit_params_set.erase({drill_bit, 0.0}))
    {
        m_is_changed_data = true;
        if (is_correct_pin_drill_bit)
            corrected_pins = ResetPinDefinitions(drill_bit, new_drill_bit);
        return {{PCADLoadError::LOAD_FILE_NO_ERROR, {}}, corrected_pins};
    }
    else
    {
        return {{PCADLoadError::VALUE_NOT_FOUND, {}}, corrected_pins};
    }
}

void TBLFileServer::ClearDrillBitParams(bool is_correct_pin_drill_bit)
{
    if (is_correct_pin_drill_bit)
    {
        PinDrillBitS new_pin_drill_bit_set;
        for_each(m_pin_drill_bit_set.begin(), m_pin_drill_bit_set.end(),
                 [&new_pin_drill_bit_set](const PinDrillBit& test_pin_drill_bit)
                 {
                    new_pin_drill_bit_set.emplace(test_pin_drill_bit.pin, 0);
                 });
        m_pin_drill_bit_set = move(new_pin_drill_bit_set);
    }
    m_is_changed_data = false;
    m_drill_bit_params_set.clear();
}

// Метод поиска свободного (пока неиспользуемого в базе данных инструментов сврловки) номера сверла.
int TBLFileServer::GetUnusedPinDrillBitNumber(bool is_find_max_value)
{
    if (m_drill_bit_params_set.empty())
        return 1; // Единица - наименьший допустимый индекс сверла.
    int max_drill_bit_number = (--m_drill_bit_params_set.end())->drill_bit;
    DrillBitParams test_drill_params;
    if (is_find_max_value)
    {
        for (int test_drill_bit = max_drill_bit_number; test_drill_bit > 0; --test_drill_bit)
        { // Проверяем все номера свёрл от старшего, имеющегося в базе, до единичного (нулевой номер сверла не допускается).
            test_drill_params.drill_bit = test_drill_bit;
            if (!m_drill_bit_params_set.count(test_drill_params))
                return test_drill_bit;
        }
    }
    else
    {
        for (int test_drill_bit = 1; test_drill_bit <= max_drill_bit_number; ++test_drill_bit)
        { // Проверяем все номера свёрл от 1 до старшего, имеющегося в базе.
            test_drill_params.drill_bit = test_drill_bit;
            if (!m_drill_bit_params_set.count(test_drill_params))
                return test_drill_bit;
        }
    }
    // Массив занятых номеров сверл плотный, пропусков не имеет, так что возвращаем номер, следующий за максимальным
    // задействованным.
    return ++max_drill_bit_number;

}

string TBLFileServer::GetExplainDataTextAsString(const std::string& line_divider) const
{
    string result;
    for (const string& annot_string : m_file_annotation_texts)
        result += annot_string + line_divider;
    return result;
}

void TBLFileServer::SetExplainDataText(const string& explain_text)
{
    m_file_annotation_texts.clear();
    size_t symb_pos = 0;
    while (symb_pos < explain_text.size())
    {
        size_t end_str_pos = explain_text.find('\n', symb_pos);
        if (end_str_pos == string::npos)
            end_str_pos = explain_text.size();
        string next_explain_line = explain_text.substr(symb_pos, end_str_pos - symb_pos);
        if (next_explain_line.back() == '\r')   // Удаляем концевой CR, если он есть.
            next_explain_line = next_explain_line.substr(0, next_explain_line.size() - 1);

        m_file_annotation_texts.push_back(move(next_explain_line));
        symb_pos = end_str_pos + 1;
    }
}

// Загрузка/сохранения данных из/в абстрактных потоков ввода/вывода в TBL-формате.
FileWorkshop::ErrorInfo TBLFileServer::LoadTBLData(istream& tbl_input_stream)
{
    // Обрабатываем блок поясняющих текстовых строк файла.
    vector<string> loaded_annotation_texts;
    while (true)
    {
        if (tbl_input_stream.peek() == *CTRLZ)
        { // Найден конец разъяснительной секции - выходим.
            tbl_input_stream.get();
            break;
        }

        string new_annot_line;
        getline(tbl_input_stream, new_annot_line);
        if (!tbl_input_stream)
            return {PCADLoadError::LOAD_FILE_READ_ERROR, {}};
        loaded_annotation_texts.push_back(move(new_annot_line));
    }

    // Загружаем и проверяем на соответствие сигнатуру формата.
    FixedHeader loaded_fix_header, orig_fix_header;
    tbl_input_stream.read((char*)&loaded_fix_header, sizeof(loaded_fix_header));
    if (!tbl_input_stream)
        return {PCADLoadError::LOAD_FILE_READ_ERROR, {}};
    if (memcmp(loaded_fix_header.signature, orig_fix_header.signature, size(loaded_fix_header.signature)) != 0)
        return {PCADLoadError::LOAD_FILE_BAD_FILE_FORMAT, {}};

    // Сигнатуры совпали, переходим к загрузке основной информационной части файла.

    uint16_t mod_ordinal = MOD_ORDINAL_MIN;
    int load_record_index = -1;
    FileWorkshop::ErrorInfo result_error = {PCADLoadError::LOAD_FILE_NO_ERROR, {}};
    PinDrillBitS loaded_pin_drill_bit_set;
    DrillBitParamsS loaded_drill_bit_params_set;

    while (true)
    {
        PinDrillRecordData loaded_data_record;
        ++load_record_index;
        tbl_input_stream.read((char*)&loaded_data_record, sizeof(loaded_data_record));
        if (!tbl_input_stream)
        {
            if (tbl_input_stream.gcount() == 0)
                return result_error;    // Всё прочитано и обработано успешно - выходим.
            if (tbl_input_stream.bad())
                return {PCADLoadError::LOAD_FILE_READ_ERROR, {}};   // Аппаратная ошибка чтения файла.
            // Запись считана без аппаратных ошибок, но не полностью. Это ошибка формата потока.
            result_error = {PCADLoadError::LOAD_FILE_BAD_FILE_FORMAT, {}};
            break;
        }

        if (loaded_data_record.mod_ordinal != mod_ordinal)
        {
            result_error = {PCADLoadError::LOAD_FILE_BAD_FILE_FORMAT, {}};
            continue;
        }

        uint16_t correct_half_flag = load_record_index <= FIRST_HALF_LIMIT ? FIRST_HALF_FALG_VALUE : SECOND_HALF_FALG_VALUE;
        if (loaded_data_record.half_flag != correct_half_flag)
        {
            result_error = {PCADLoadError::LOAD_FILE_BAD_FILE_FORMAT, {}};
            continue;
        }

        // Поля прочитанной записи, однозначно зависящие от порядкового номера записи, имеют правильные значения.
        // Приступаем к формированию записей базы данных сверловки и вставки их в массивы хранилища.
        PinDrillBit new_pin_drill_bit;
        new_pin_drill_bit.pin = load_record_index;
        new_pin_drill_bit.drill_bit = loaded_data_record.drill_bit_number;
        loaded_pin_drill_bit_set.insert(move(new_pin_drill_bit));

        DrillBitParams new_drill_bit_params;
        new_drill_bit_params.drill_bit = loaded_data_record.drill_bit_number;
        new_drill_bit_params.diameter = loaded_data_record.drill_bit_diameter;
        // Необльшой тест на согласованность данных. Проверим наличие в накопителе loaded_drill_bit_params_set уже
        // возможно хранящегося там описания параметров сверла loaded_data_record.drill_bit_number. Если оно там
        // уже есть, его диаметр должен совпадать сloaded_data_record.drill_bit_diameter. Если же это не так, на
        // выход выставим ошибку.
        if (auto drill_bit_param_it = loaded_drill_bit_params_set.find({loaded_data_record.drill_bit_number, 0.0});
            drill_bit_param_it != loaded_drill_bit_params_set.end())
        { // Ранее свердло с таким идентификационным номером уже описывалось.
            if (fabs(drill_bit_param_it->diameter - loaded_data_record.drill_bit_diameter) > DRILL_BIT_DIAMETER_TOLERANCE)
                // В предыдущем описании того жн сверла для него был указан другой диаметр.
                result_error = {PCADLoadError::VALUE_DUPLICATES, {}};
            loaded_drill_bit_params_set.erase(drill_bit_param_it);
        }

        loaded_drill_bit_params_set.insert(move(new_drill_bit_params));
    }

    return result_error;
}

FileWorkshop::ErrorInfo TBLFileServer::SaveTBLData(ostream& tbl_output_stream) const
{
    // Сначала формируем секцию пояснительных текстовых строк.
    for(const string& annot_line : m_file_annotation_texts)
    {
        tbl_output_stream.write(annot_line.data(), annot_line.size());
        tbl_output_stream.write(CRLF, sizeof(CRLF) - 1);
    }
    if (m_options.m_is_insert_timestamp_text)
    {

    }
    tbl_output_stream.write(CTRLZ, sizeof(CTRLZ) - 1);
    if (!tbl_output_stream)
        return {PCADLoadError::LOAD_FILE_WRITE_ERROR, {}};

    // Далее - заголовк фиксированной длины (фактически, просто сигнатура формата).
    FixedHeader fix_header;
    tbl_output_stream.write((char*)&fix_header, sizeof(fix_header));
    if (!tbl_output_stream)
        return {PCADLoadError::LOAD_FILE_WRITE_ERROR, {}};

    // И наконец, последовательность записей с описанием одновременно параметров сверл и их привязки к типам выводов.
    uint16_t mod_ordinal = MOD_ORDINAL_MIN;
    int drill_bit_rec_index = 0;
    FileWorkshop::ErrorInfo result_error = {PCADLoadError::LOAD_FILE_NO_ERROR, {}};

    for (const PinDrillBit& pin_drill_bit : m_pin_drill_bit_set)
    {
        PinDrillRecordData new_pin_drill_record;

        new_pin_drill_record.mod_ordinal = mod_ordinal;
        if (++mod_ordinal > MOD_ORDINAL_MAX)
            mod_ordinal = MOD_ORDINAL_MIN;
        new_pin_drill_record.half_flag =
            (drill_bit_rec_index++) <= FIRST_HALF_LIMIT ? FIRST_HALF_FALG_VALUE : SECOND_HALF_FALG_VALUE;
        new_pin_drill_record.drill_bit_number = static_cast<uint16_t>(pin_drill_bit.drill_bit);

        if (pin_drill_bit.drill_bit > 0)
        {
            if (auto drill_bit_params_it = m_drill_bit_params_set.find({pin_drill_bit.drill_bit, 0.0});
                drill_bit_params_it != m_drill_bit_params_set.end())
                new_pin_drill_record.drill_bit_diameter = drill_bit_params_it->diameter;
            else
                result_error = {PCADLoadError::VALUE_NOT_FOUND, "Сверло - " + to_string(pin_drill_bit.drill_bit)};
        }
        tbl_output_stream.write((char*)&new_pin_drill_record, sizeof(new_pin_drill_record));
    }

    if (!tbl_output_stream)
        result_error = {PCADLoadError::LOAD_FILE_WRITE_ERROR, {}};
    return result_error;
}

// Загрузка/сохранения данных из/в TBL-файлы.
FileWorkshop::ErrorInfo TBLFileServer::LoadTBLData(const filesystem::path& tbl_input_file)
{
    ifstream tbl_load_stream(tbl_input_file, ios_base::binary);
    if (tbl_load_stream)
        return LoadTBLData(tbl_load_stream);
    else
        return {PCADLoadError::LOAD_FILE_OPEN_ERROR, {}};
}

FileWorkshop::ErrorInfo TBLFileServer::SaveTBLData(const filesystem::path& tbl_output_file) const
{
    ofstream tbl_save_stream(tbl_output_file, ios_base::binary);
    if (tbl_save_stream)
        return SaveTBLData(tbl_save_stream);
    else
        return {PCADLoadError::LOAD_FILE_CREATE_ERROR, {}};
}

// Загрузка/сохранения данных из/в абстрактных потоков ввода/вывода в INI-формате.
FileWorkshop::ErrorInfo TBLFileServer::LoadINIData(istream& ini_input_stream)
{
    enum class SECTION_TYPE
    {
        SECTION_NONE = 0,
        SECTION_ANNOTATION,
        SECTION_DRILL_BITS_PARAMS,
        SECTION_PIN_DRILL_BIT,
        SECTION_DATABASE_PROPERTIES
    };

    DrillBitParams new_drill_bit;
    SECTION_TYPE sect_type = SECTION_TYPE::SECTION_NONE;

    // Сохраним страховочные копии базы данных на случай необходимости их восстановления при ошибке загрузки.
    vector<string> bk_file_annotation_texts = move(m_file_annotation_texts);
    PinDrillBitS bk_pin_drill_bit_set = move(m_pin_drill_bit_set);
    DrillBitParamsS bk_drill_bit_params_set = move(m_drill_bit_params_set);
    //
    m_file_annotation_texts.clear();
    m_drill_bit_params_set.clear();
    m_pin_drill_bit_set.clear();
    m_database_properties = {};

    while (ini_input_stream.good())
    {
        std::string current_line;
        getline(ini_input_stream, current_line);
        if (!ini_input_stream)
            break;
        if (size_t comment_pos = current_line.find(';'); comment_pos != string::npos)
            // Отрезаем найденный комментарий от очередной считанной строки ini-файла.
            current_line = current_line.substr(0, comment_pos);
        current_line = TrimString(current_line);
        if (current_line.empty())
            continue;
        // Обнаруживаем строки-признаки начала очередной секции ini-файла.
        if (current_line[0] == '[' && current_line.back() == ']')
        {
            // Завершаем предыдущую секцию.
            switch (sect_type)
            {
            case SECTION_TYPE::SECTION_DRILL_BITS_PARAMS:
                // Завершаем секцию параметров сверла - сохраняем его полностью сформированное описание в хранилище.
                SetDrillBitParams(move(new_drill_bit));
                new_drill_bit = {};
                break;
            default:
                break;
            }

            // Открываем последующую, определяя ее тип и возможный индекс.
            string section_prefix;
            int section_index = -1;
            current_line = TrimString(current_line.substr(1, current_line.size() - 2));
            if (size_t underline_sign_pos = current_line.find('_'); underline_sign_pos != string::npos)
            {
                section_prefix = TrimString(current_line.substr(0, underline_sign_pos));
                section_index = atoi(current_line.substr(underline_sign_pos + 1).c_str());
            }
            else
            {
                section_prefix = current_line;
            }
            // Определяем тип открывающейся секции и выполняем начальные настройки ее разбора.
            if (current_line == ANNOTATION_SECTNAME)
            {
                sect_type = SECTION_TYPE::SECTION_ANNOTATION;
            }
            else if (current_line == PIN_DRILL_BIT_SECTNAME)
            {
                sect_type = SECTION_TYPE::SECTION_PIN_DRILL_BIT;
            }
            else if (current_line == PROPERTIES_SECTNAME)
            {
                sect_type = SECTION_TYPE::SECTION_DATABASE_PROPERTIES;
            }
            else if (section_prefix == DRILL_BIT_SECTNAME_PREFIX && section_index > 0)
            {
                sect_type = SECTION_TYPE::SECTION_DRILL_BITS_PARAMS;
                new_drill_bit.drill_bit = section_index;
            }
            else
            {
                sect_type = SECTION_TYPE::SECTION_NONE;
            }
        }
        else
        {
            std::string current_key, current_value, key_prefix;
            int key_index = -1;
            // Выделим из считанной ini-строки ее ключевую (слева от знака '=') и значащую части (справа от него)
            if (size_t equal_sign_pos = current_line.find('='); equal_sign_pos != string::npos)
            {
                current_key = TrimString(current_line.substr(0, equal_sign_pos));
                current_value = TrimString(current_line.substr(equal_sign_pos + 1));
            }
            else
            { // Строка не имеет правильной присваивающей структуры, пропустим ее.
                continue;
            }
            // Разделим также ключ на префиксную и индексную его части, если он содержит их разделитель '_'.
            if (size_t underline_sign_pos = current_key.find('_'); underline_sign_pos != string::npos)
            {
                key_prefix = TrimString(current_key.substr(0, underline_sign_pos));
                key_index = atoi(current_key.substr(underline_sign_pos + 1).c_str());
            }
            else
            {
                key_prefix = current_key;
            }

            // Обрабатываем очередную считанную ini-строку в соответствии с типом текущей секции.
            switch (sect_type)
            {
            case SECTION_TYPE::SECTION_ANNOTATION:
                if (key_prefix == ANNOT_STRING_INDEX_PREFIX)
                    m_file_annotation_texts.push_back(current_value);
                break;
            case SECTION_TYPE::SECTION_DRILL_BITS_PARAMS:
                if (current_key == DRILL_BIT_DIAMETER_PARAMNAME)
                    new_drill_bit.diameter = atof(current_value.c_str());
                break;
            case SECTION_TYPE::SECTION_PIN_DRILL_BIT:
                if (key_prefix == PIN_INDEX_PREFIX)
                { // Добавляем в базу ещё одно сопоставление между номером вывода key_index и назначенным ему сверлом current_value.
                    PinDrillBit new_pin_drill_bit;
                    new_pin_drill_bit.pin = key_index;
                    new_pin_drill_bit.drill_bit = atoi(current_value.c_str());
                    SetPinDrillBit(move(new_pin_drill_bit));
                }
                break;
            case SECTION_TYPE::SECTION_DATABASE_PROPERTIES:
                if (current_key == DRILL_SORTING_PARAMNAME)
                    m_database_properties.is_sorting_during_generate = atoi(current_value.c_str());
                else if (current_key == PCAD4_TBL_STRICT_PARAMNAME)
                    m_database_properties.is_PCAD4_TBL_strict_format = atoi(current_value.c_str());
                else if (current_key == TEXT_ENCODING_PARAMNAME)
                    m_database_properties.text_encoding = DatabaseTextEncodingFromString(current_value);
                break;
            default:
                break;
            }
        }
    }

    if (ini_input_stream.good() || ini_input_stream.eof())
    {
        return {PCADLoadError::LOAD_FILE_NO_ERROR, {}};
    }
    else
    {   // При загрузке возникла ошибка. Восстановим содержимое базы из страхкопии и выходим с ошибкой.
        m_file_annotation_texts = move(bk_file_annotation_texts);
        m_drill_bit_params_set = move(bk_drill_bit_params_set);
        m_pin_drill_bit_set = move(bk_pin_drill_bit_set);
        return {PCADLoadError::LOAD_FILE_READ_ERROR, {}};
    }
}

FileWorkshop::ErrorInfo TBLFileServer::SaveINIData(ostream& ini_output_stream) const
{
    array<char, 32> chars_buf;

    // Создаем секцию со строками аннотации.
    ini_output_stream << '[' + string(ANNOTATION_SECTNAME) + ']' << endl;
    size_t annot_string_count = 0;
    for (const string& annot_string : m_file_annotation_texts)
    {
        ini_output_stream << string(ANNOT_STRING_INDEX_PREFIX) + '_' + to_string(++annot_string_count)
                          << "=" << annot_string << endl;
    }

    // Далее набор секций с параметрами сверл.
    for (const DrillBitParams& drill_bit_params : m_drill_bit_params_set)
    {
        if (auto [ptr, ec] = to_chars(chars_buf.data(), chars_buf.data() + chars_buf.size(), drill_bit_params.diameter, chars_format::fixed, 3);
            ec == errc())
        {
            *ptr = '\0';
            ini_output_stream << '[' + string(DRILL_BIT_SECTNAME_PREFIX) + '_' + to_string(drill_bit_params.drill_bit) + ']' << endl;
            ini_output_stream << DRILL_BIT_DIAMETER_PARAMNAME << "=" << chars_buf.data() << endl;
        }
    }

    // Тут размещается секция сопоставления номеров сверл и типов выводов.
    ini_output_stream << '[' + string(PIN_DRILL_BIT_SECTNAME) + ']' << endl;
    for (const PinDrillBit& pin_drill_bit : m_pin_drill_bit_set)
    {
        ini_output_stream << string(PIN_INDEX_PREFIX) + '_' + to_string(pin_drill_bit.pin)
                          << "=" << pin_drill_bit.drill_bit << endl;
    }

    // Наконец, в завершение следует секция с описанием дополнительных параметров хранения и обработки информации из этой базы данных.
    ini_output_stream << '[' + string(PROPERTIES_SECTNAME) + ']' << endl;
    ini_output_stream << noboolalpha << string(PCAD4_TBL_STRICT_PARAMNAME) << "="
                      << m_database_properties.is_PCAD4_TBL_strict_format << endl;
    ini_output_stream << string(DRILL_SORTING_PARAMNAME) << "="
                      << m_database_properties.is_sorting_during_generate << endl;
    ini_output_stream << string(TEXT_ENCODING_PARAMNAME) << "="
                      << DatabaseTextEncodingToString(m_database_properties.text_encoding) << endl;

    if (ini_output_stream)
        return {PCADLoadError::LOAD_FILE_NO_ERROR, {}};
    else
        return {PCADLoadError::LOAD_FILE_WRITE_ERROR, {}};
}

// Загрузка/сохранения данных из/в INI-файлы.
FileWorkshop::ErrorInfo TBLFileServer::LoadINIData(const filesystem::path& ini_input_file)
{
    ifstream ini_load_stream(ini_input_file);
    if (ini_load_stream)
        return LoadINIData(ini_load_stream);
    else
        return {PCADLoadError::LOAD_FILE_OPEN_ERROR, {}};
}

FileWorkshop::ErrorInfo TBLFileServer::SaveINIData(const filesystem::path& ini_output_file) const
{
    ofstream ini_save_stream(ini_output_file);
    if (ini_save_stream)
        return SaveINIData(ini_save_stream);
    else
        return {PCADLoadError::LOAD_FILE_CREATE_ERROR, {}};
}

std::string TBLFileServer::DatabaseTextEncodingToString(DrillFileStringEncoding database_text_encoding)
{
    switch (database_text_encoding)
    {
    case DrillFileStringEncoding::ENCODE_TEXT_UTF8:
        return UTF8_ENCODING_NAME;
    case DrillFileStringEncoding::ENCODE_TEXT_CP866:
        return CP866_ENCODING_NAME;
    case DrillFileStringEncoding::ENCODE_TEXT_CP1251:
        return CP1251_ENCODING_NAME;
    default:
        return {};
    }
}

TBLFileServer::DrillFileStringEncoding TBLFileServer::DatabaseTextEncodingFromString(const std::string& encoding_name)
{
    if (encoding_name == UTF8_ENCODING_NAME)
        return DrillFileStringEncoding::ENCODE_TEXT_UTF8;
    else if (encoding_name == UTF8_ENCODING_NAME)
        return DrillFileStringEncoding::ENCODE_TEXT_CP866;
    else if (encoding_name == UTF8_ENCODING_NAME)
        return DrillFileStringEncoding::ENCODE_TEXT_CP1251;
    else
        return DrillFileStringEncoding::ENCODE_TEXT_UNKNOWN;
}
