#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <variant>
#include <any>

#include "macroses.h"
#include "PCADViewDraw.h"
#include "ApertureProvider.h"

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

struct LayerDesc
{
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

struct CrossLayerPinhole
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
    wxString file_signature; // Строка, кратко описывающая тип и содержание файла
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
};

class PCADFile
{
private:
    void PCADFileClear();

    std::vector<GraphObj*> graph_objects_;                  // Массив графических элементов файла
    std::vector<LayerDesc> layers_;                         // Массив описателей слоёв изображения
    std::vector<CrossLayerPinhole> cross_layers_pinholes_;  // Массив со свойствами межслойных переходных отверстий.
    aperture::ApertureProvider& aperture_provider_;         // Ссылка на диспетчер апертур
    FileDefValues file_values_;                             // Общее описание характеристик файла

public:
    PCADFile(aperture::ApertureProvider& aperture_provider);
    PCADFile(const PCADFile& other) = delete;
    PCADFile(PCADFile&& other);
    PCADFile(std::vector<GraphObj*> graph_objects, std::vector<LayerDesc> layers,
             aperture::ApertureProvider& aperture_provider, FileDefValues file_values);
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
            return {"", -1, -1, {}};
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
        case PCADLoadError::LOAD_FILE_COMAMND_UNACCEPTABLE_HERE:
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
