#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <variant>
#include <any>

#include "PCADViewDraw.h"
#include "ApertureProvider.h"

enum LayerAttributesValues
{
    LAYER_ON_AVL_ACT_OFF = 0, // Слой выключен (невидим)
    LAYER_ON_AVL_ACT_ON = 1, // Слой включён (отображается)
    LAYER_ON_AVL_ACT_AVL = 2, // Слой активен (доступен для редактирования)
    LAYER_ON_AVL_ACT_ACT = 3 // Рабочий слой (выбран для редактирования)
};

struct LayerAttributes
{
    unsigned layer_on_avl_act : 2;
    operator int()
    {
        return layer_on_avl_act;
    }
};

struct LayerDesc
{
    std::string layer_name;
    int layer_number; // Номер слоя в списке VLYR
    int layer_color; // Индекс в палитре, которым должна выводиться графика слоя
    wxColor layer_wx_color; // Прямой цвет, который будет применяться для рисования графики слоя
    mutable LayerAttributes layer_attributes; // Флаги включения и активности слоя
};

enum FileFlags
{
    FILE_FLAG_UNIT_INCHES = 1, // Единица измерения в файле - дюймы, иначе - миллиметры.
                               // Этот флаг имеет смысл в сочетании с полем DBU_in_measure_unit
                               // структуры FileDefValues, указывающим плотность DBU в файле, т. е.
                               // количество DBU на дюйм или миллиметр.
    FILE_FLAG_EDITOR_PCCAPS = 2 // Файл создан в редакторе PCCAPS, иначе - в PCCARDS или PCPLACE.
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
    uint32_t file_flags;  // Поле битовых флагов, опеределяющих отдельные
                          // бинарные признаки изображения из группы FileFlags.
    int DBU_in_measure_unit = 100; // Количество координатных единиц DBU в основной
                                   // измерительной единице изображения (миллиметры
                                   // или дюймы, что определяется флагом FILE_FLAG_INCHES).
    std::filesystem::path picture_filepath; // Путь к файлу загруженной картинки
};

class PCADFile
{
private:
    void PCADFileClear();

    std::vector<GraphObj*> graph_objects_; // Массив графических элементов файла
    std::vector<LayerDesc> layers_; // Массив описателей слоёв изображения
    aperture::ApertureProvider& aperture_provider_; // Ссылка на диспетчер апертур
    FileDefValues file_values_; // Общее описание характеристик файла

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
    using LoadFileResult = std::pair<PCADFile, PCADLoadError>;
    struct AdditionalLoadInfo
    {
        std::filesystem::path picture_file_path;
        // Сслыка на поставщика информации об апертурах, который будет использоваться загружаемым документом.
        aperture::ApertureProvider& aperture_provider;
        // Указывает загрузчику, следует ли ему перезагружать или переключать текущее множество засветок,
        // если этого требует формат загружаемого файла. При значении "ложь" загрузчику запрещается каким-либо
        // образом изменять текущий набор апертур, а нужно приспособиться к его имеющемуся состоянию.
        bool is_control_aperture_data = false;
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
