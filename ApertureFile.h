#pragma once

#include <iostream>
#include <vector>
#include <filesystem>
#include <any>
#include "macroses.h"

#define ArraySize(x) (sizeof(x) / sizeof((x)[0]))

namespace aperture
{
    enum class ApertureFileType
    {
        APERTURE_FILE_NONE = 0,
        APERTURE_FILE_AUTO = 1,
        APERTURE_FILE_GAP,
        APERTURE_FILE_APR,
    };

    enum class UsingApertureType
    {
        USING_APERTURE_UNKNOWN = 0,
        USING_APERTURE_GERBER32 = 1,
        USING_APERTURE_GERBER_LASER
    };

    enum class FlashShape
    {
        FLASH_UNKNOWN = 0,
        FLASH_ROUND,
        FLASH_SQUARE,
        FLASH_ANOTHER
    };

    enum class FlashType
    {
        FLASH_UNKNOWN = -1,
        FLASH_LINER = 0,
        FLASH_FLASHER
    };

    struct FlashDesc
    {
        int flash_number = 1;
        double flash_size_inch = 0.04; // Размер апертуры (засветки) в дюймах
        double flash_size_mm = 1.016; // Размер апертуры в миллиметрах
        FlashShape flash_shape = FlashShape::FLASH_ROUND; // Форма засветки
        FlashType flash_type = FlashType::FLASH_FLASHER; // Способ применения апертуры
        std::string d_code = "D12"; // D-Код апертуры, используемый для ссылки на неё в GERBER-файлах.
    };

    struct ApertureLoadlInfo
    {
        ApertureFileType aper_file_type = ApertureFileType::APERTURE_FILE_NONE;
        std::istream* aper_file_stream_ptr = nullptr;
        std::filesystem::path aperture_file_path;
        UsingApertureType using_aperture_type = UsingApertureType::USING_APERTURE_GERBER32;
    };

    struct FlashDescCommon
    { // Данные этой структуры описывают используемые изображением апертуры (вспышки)
        std::vector<FlashDesc> gerber32; // Массив описателей апертур типа Gerber32
        std::vector<FlashDesc> gerber_laser; // Массив описателей апертур типа Gerber Laser
    };

    ApertureFileType DetectApertureFileType(std::istream& aper_istr);
    FlashDescCommon LoadAPRApertureFile(std::istream& aper_istr, PCADLoadError& error_code);
    FlashDescCommon LoadGAPApertureFile(std::istream& aper_istr, PCADLoadError& error_code);
} // namespace aperture
