#pragma once

#include "ApertureFile.h"
#include <filesystem>

namespace aperture
{
    class ApertureProvider
    {
    public:
        ApertureProvider();
        ApertureProvider(const ApertureProvider& other) = default;
        ApertureProvider& operator=(const ApertureProvider& other) = default;

        PCADLoadError LoadApertureFile(const ApertureLoadlInfo& load_info);
        UsingApertureType GetApertureType() const
        {
            return using_aperture_type_;
        }

        void SetApertureType(UsingApertureType using_aperture_type) const
        {
            using_aperture_type_ = using_aperture_type;
        }

        size_t size() const;
        std::vector<FlashDesc>::const_iterator begin() const;
        std::vector<FlashDesc>::const_iterator end() const;
        std::vector<FlashDesc>::const_reverse_iterator rbegin() const;
        std::vector<FlashDesc>::const_reverse_iterator rend() const;
        FlashDesc GetFlashDesc(int flash_num) const;
        FlashDesc GetFlashDesc(const std::string& scan_d_code) const;
        aperture::ApertureFileType GetApertureFileType() const
        {
            return aperture_filetype_;
        }

        wxString GetApertureFileName() const
        {
            return wxString(aperture_filepath_.string().c_str(), wxConvUTF8);
        }

        std::filesystem::path GetApertureFilePath() const
        {
            return aperture_filepath_;
        }

    private:
        FlashDescCommon flashes_; // Описатели апертур, используемых изображением
        mutable UsingApertureType using_aperture_type_;
        aperture::ApertureFileType aperture_filetype_;
        std::filesystem::path aperture_filepath_;
        static constexpr int GERBER32_MAX_APERTURE_NUMBER = 24;
    };
}
