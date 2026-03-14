#pragma once

#include <iostream>

#include <string>
#include <string_view>
#include <vector>
#include <set>

#include "PCADFile.h"

// Класс, обслуживающий загрузку и сохранения описаний инструментов сверления в файлы формата PC-DRILL (*.TBL).
class TBLFileServer
{
public:
    // Константы, служащие для формирования наполнения секции поясняющего текста таблицы интсрументов при хранении ее в ini-файле.
    static constexpr char ANNOTATION_SECTNAME[] = "Annotation";
    static constexpr char ANNOT_STRING_INDEX_PREFIX[] = "Text";
    //
    static constexpr char DRILL_BIT_SECTNAME_PREFIX[] = "DrillBit";
    static constexpr char DRILL_BIT_DIAMETER_PARAMNAME[] = "Diameter";
    //
    static constexpr char PIN_DRILL_BIT_SECTNAME[] = "PinDrillBit";
    static constexpr char PIN_INDEX_PREFIX[] = "Pin";
    //
    static constexpr char PROPERTIES_SECTNAME[] = "Properties";
    static constexpr char DRILL_SORTING_PARAMNAME[] = "DrillSorting";
    static constexpr char PCAD4_TBL_STRICT_PARAMNAME[] = "PCAD4TBLStrict";
    static constexpr char TEXT_ENCODING_PARAMNAME[] = "Encoding";

    // Максимально допустимый условный номер сверла при соблюдении ограничений формата  данных PC-DRILL.
    static constexpr int DRILL_BIT_MAX_NUMBER = 9999;
    // Максимально возможный диаметер сверла, допускаемый для указания
    static constexpr double DRILL_BIT_MAX_DIAMETER = 99.999;
    // Названия различных текстовых кодировок, которые мы будем использовать для выбора между ними.
    static constexpr char UTF8_ENCODING_NAME[] = "UTF-8";
    static constexpr char CP866_ENCODING_NAME[] = "CP-866 (DOS)";
    static constexpr char CP1251_ENCODING_NAME[] = "CP-1251 (Windows)";

    // Структура, служащая для настройки некоторых режимов и особенностей функционирования данного сервера.
    struct DatabaseOptions
    {
        bool is_strict_tbl_format = false;
        int fixed__pin_count = -1;
        int fixed_drill_bit_count = -1;
        bool m_is_insert_timestamp_text = false;    // Требование вставки специальной строки в состав пояснительного текста,
                                                    // содержащей время и дату создания TBL-файла при его сохранении.
    };

    // Структура, указывающая назначение сверла с условным номером drill_bit отверстию под вывод с конструктивным номером pin.
    struct PinDrillBit
    {
        int pin = 0;        // Конструктивный номер вывода.
        int drill_bit = 0;  // Условный (порядковый) номер сверла, назначенного выводу pin.

        bool operator<(const PinDrillBit& other) const noexcept
        {
            return pin < other.pin;
        }
    };

    // Параметры сверла с номером drill_bit (пока только его диаметр).
    struct DrillBitParams
    {
        int drill_bit = -1;      // Условный (порядковый) номер сверла.
        double diameter = 0.0;  // Диаметр сверла drill_bit в миллиметрах.

        bool operator<(const DrillBitParams& other) const noexcept
        {
            return drill_bit < other.drill_bit;
        }
    };
    // Прочие зависимые типы, используемые для взаимообмена информацией с сервером.
    // Вектора, которые возможно применять для передачи/получения от/в сервера сразу всей имеющейся информации некоторого вида.
    using PinDrillBitV = std::vector<PinDrillBit>;
    using DrillBitParamsV = std::vector<DrillBitParams>;
    // Множества аналогичного назначения.
    using PinDrillBitS = std::set<PinDrillBit>;
    using DrillBitParamsS = std::set<DrillBitParams>;

    void ClearChangeFlag()
    {
        m_is_changed_data = false;
    }
    bool IsDataChanged() const
    {
        return m_is_changed_data;
    }

    // Получение всей имеющейся информации разом.
    std::pair<PinDrillBitV, DrillBitParamsV> GetAsVectors() const;
    const std::pair<const PinDrillBitS&, const DrillBitParamsS&> GetAsSets() const
    {
        return {m_pin_drill_bit_set, m_drill_bit_params_set};
    }
    // Получение имеющейся у сервера информации по частям (покомпонентно).
    size_t GetPinDrillBitCount() const
    {
        return m_pin_drill_bit_set.size();
    }
    PinDrillBitV GetPinDrillBitVector() const;
    const PinDrillBitS& GetPinDrillBitSet() const
    {
        return m_pin_drill_bit_set;
    }
    const PinDrillBit* GetPinDrillBit(int pin) const;

    size_t GetDrillBitParamsCount() const
    {
        return m_drill_bit_params_set.size();
    }
    DrillBitParamsV GetDrillBitParamsVector() const;
    const DrillBitParamsS& GetDrillBitParamsSet() const
    {
        return m_drill_bit_params_set;
    }
    const DrillBitParams* GetDrillBitParams(int drill_bit) const;

    // Установка (загрузка) данных в сервер комплексно, сразу полным комплектом.
    std::vector<FileWorkshop::ErrorInfo> SetAsVectors
        (std::pair<PinDrillBitV, DrillBitParamsV>&& input_vectors_pair, bool is_clear_previous = true);
    void SetAsSets(std::pair<PinDrillBitS, DrillBitParamsS>&& input_sets_pair, bool is_clear_previous = true)
    {
        m_is_changed_data = !is_clear_previous;
        SetPinDrillBitSet(std::move(input_sets_pair.first), is_clear_previous);
        SetDrillBitParamsSet(std::move(input_sets_pair.second), is_clear_previous);
    }
    // Установка (загрузка) информации о параметрах сверловки в сервер по частям.
    std::vector<FileWorkshop::ErrorInfo> SetPinDrillBitVector
        (PinDrillBitV&& pin_drill_bit_vector, bool is_clear_previous = true, bool is_check_drill_exists = true);
    void SetPinDrillBitSet(PinDrillBitS&& pin_drill_bit_set, bool is_clear_previous = true)
    {
        m_is_changed_data = !is_clear_previous;
        if (is_clear_previous)
            m_pin_drill_bit_set = std::move(pin_drill_bit_set);
        else
            m_pin_drill_bit_set.merge(pin_drill_bit_set);

    }
    std::vector<FileWorkshop::ErrorInfo> SetDrillBitParamsVector(DrillBitParamsV&& drill_bit_params_vector, bool is_clear_previous = true);
    void SetDrillBitParamsSet(DrillBitParamsS&& drill_bit_params_set, bool is_clear_previous = true)
    {
        m_is_changed_data = !is_clear_previous;
        if (is_clear_previous)
            m_drill_bit_params_set = std::move(drill_bit_params_set);
        else
            m_drill_bit_params_set.merge(drill_bit_params_set);
    }
    // Операции над отдельными элементами информационных таблиц сверловки.
    // Добавление (или изменение, если запись для такого типа вывода уже существует) информации в таблицу сопоставления типов выводов
    // и номеров инструментов.
    std::pair<FileWorkshop::ErrorInfo, bool> SetPinDrillBit
        (PinDrillBit&& pin_drill_bit, bool is_assign = true, bool is_check_drill_exists = true);
    FileWorkshop::ErrorInfo RemovePinDrillBit(int pin);
    void ClearPinDrillBit()
    {
        m_is_changed_data = false;
        m_pin_drill_bit_set.clear();
    }
    // Добавление (или изменение, если запись для такого сверла уже существует) записи с параметрами некоторого сверла.
    std::pair<FileWorkshop::ErrorInfo, bool> SetDrillBitParams(DrillBitParams&& drill_bit_params, bool is_assign = true);
    std::pair<FileWorkshop::ErrorInfo, std::vector<int>> RemoveDrillBitParams
        (int drill_bit, bool is_correct_pin_drill_bit = true, int new_drill_bit = 0);
    void ClearDrillBitParams(bool is_correct_pin_drill_bit = true);
    // Вспомогательная функция поиска наименьшего/наибольшего номера сверла, которое ещё не числится в базе.
    int GetUnusedPinDrillBitNumber(bool is_find_max_value = false);

    // Загрузка/сохранения данных из/в абстрактных потоков ввода/вывода в TBL-формате.
    FileWorkshop::ErrorInfo LoadTBLData(std::istream& tbl_input_stream);
    FileWorkshop::ErrorInfo SaveTBLData(std::ostream& tbl_output_stream) const;
    // Загрузка/сохранения данных из/в TBL-файлы.
    FileWorkshop::ErrorInfo LoadTBLData(const std::filesystem::path& tbl_input_file);
    FileWorkshop::ErrorInfo SaveTBLData(const std::filesystem::path& tbl_output_file) const;
    // Загрузка/сохранения данных из/в абстрактных потоков ввода/вывода в INI-формате.
    FileWorkshop::ErrorInfo LoadINIData(std::istream& ini_input_stream);
    FileWorkshop::ErrorInfo SaveINIData(std::ostream& ini_output_stream) const;
    // Загрузка/сохранения данных из/в INI-файлы.
    FileWorkshop::ErrorInfo LoadINIData(const std::filesystem::path& ini_input_file);
    FileWorkshop::ErrorInfo SaveINIData(const std::filesystem::path& ini_output_file) const;

    // Методы работы с пояснительным текстом базы данных сверловки.
    // Извлекатели и установщики пояснительного текста, хранимого вместе с данными таблицы сверловки.
    size_t GetExplainDataSize() const
    {
        return m_file_annotation_texts.size();
    }
    std::string GetExplainDataText(size_t expline_line_index) const
    {
        if (expline_line_index < m_file_annotation_texts.size())
            return m_file_annotation_texts[expline_line_index];
        else
            return {};
    }
    const std::vector<std::string>& GetExplainDataText() const
    {
        return m_file_annotation_texts;
    }
    std::string GetExplainDataTextAsString(const std::string& line_divider = "\r\n") const;
    void ClearExplainDataText()
    {
        m_file_annotation_texts.clear();
    }
    void SetExplainDataText(std::vector<std::string>&& explain_strings)
    {
        m_file_annotation_texts = move(explain_strings);
    }
    void SetExplainDataText(const std::string& explain_text);
    void AddExplainDataText(const std::string& explain_text_line)
    {
        m_file_annotation_texts.push_back(explain_text_line);
    }

    // Структура, содержащая сведения о свойствах и правилах хранения и обработки данных, связанных с определением сверлильных
    // инструментов, сопоставлением этих инструментов и назначенных им целевых объектах, а также непосредственной генерации
    // выходного файла сверловки.
    enum class DrillFileStringEncoding
    {
        ENCODE_TEXT_UNKNOWN = 0,
        ENCODE_TEXT_UTF8,
        ENCODE_TEXT_CP866,
        ENCODE_TEXT_CP1251,
        ENCODE_TEXT_MAX = ENCODE_TEXT_CP1251
    };

    struct DrillDatabasePropertiesSaver
    {
        DrillFileStringEncoding text_encoding = DrillFileStringEncoding::ENCODE_TEXT_UTF8;
        bool is_PCAD4_TBL_strict_format = false;
        bool is_sorting_during_generate = true;
    };

    const DrillDatabasePropertiesSaver& GetDrillDatabaseProperties() const
    {
        return m_database_properties;
    }

    void SetDrillDatabaseProperties(const DrillDatabasePropertiesSaver& properties_saver)
    {
        m_database_properties = properties_saver;
    }

    static std::string DatabaseTextEncodingToString(DrillFileStringEncoding database_text_encoding);
    static DrillFileStringEncoding DatabaseTextEncodingFromString(const std::string& encoding_name);

private:

    // Описание формата внутренних структур TBL-файла, из которых он состоит.
    static constexpr char CRLF[] = "\r\n";
    static constexpr char CTRLZ[] = "\0x1a";
    //
    static constexpr uint16_t MOD_ORDINAL_MIN = 0x5;
    static constexpr uint16_t MOD_ORDINAL_MAX = 0x14;
    static constexpr int FIRST_HALF_LIMIT = 127;    // Максимальный номер индекса записи, принадлежащей к "первой половине" секции.
    static constexpr uint16_t FIRST_HALF_FALG_VALUE = 0xa;
    static constexpr uint16_t SECOND_HALF_FALG_VALUE = 0x27;
    static constexpr double DRILL_BIT_DIAMETER_TOLERANCE = 0.0005;

    #pragma pack(push, 1)
    // Заголовок фиксированного формата, следующей за текстовой секцией и предваряющий
    // секцию записей описания и назначения инструментов.
    struct FixedHeader
    {
        char        signature[3] = {0xc8, 0x00, 0x3f};     // Сигнатура формата файла.
    };

    // Запись, описывающая сверло, назначенное определённому выводу, конструктивный номер которого соответствует
    // порядковому индексу данной записи в файле.
    struct PinDrillRecordData
    {
        uint16_t    mod_ordinal = 0x5;          // "Порядковый остаток" номера записи (см. спецификацию TBL-формата).
        uint16_t    half_flag = 0x0a;           // "Флаг половины" (детально также описан в спецификации).
        uint32_t    reserved_1 = 0;             // Зарезервировано.
        uint16_t    drill_bit_number = 0;       // Условный номер сверла, назначенного выводу, которому соответствует
                                                // данная запись в файле.
        float       drill_bit_diameter = 0.0;   // Диаметер сверла с условным номером drill_bit_number в миллиметрах.
    };
    #pragma pack(pop)

    bool m_is_changed_data = false;
    DatabaseOptions m_options;
    // Поля, реализующие хранилище базы данных инструментов сверловки, обслуживаемой в данный момент сервером.
    std::vector<std::string> m_file_annotation_texts;   // Строки, составляющие текстовый поясняющий заголовок TBL-файла.
    PinDrillBitS m_pin_drill_bit_set;          // Таблица с данными соответствия конструктивного номера (кода) вывода и назначенного ему сверла.
    DrillBitParamsS m_drill_bit_params_set;    // Таблица, хранящая параметры доступных сверлящих инструментов.
    DrillDatabasePropertiesSaver m_database_properties; // Запись с настроечными параметрами операций над базой данных сверловки.

    std::vector<int> ResetPinDefinitions(int drill_bit, int new_drill_bit = 0);
};
