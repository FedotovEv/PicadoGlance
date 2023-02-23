#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <any>

#include "macroses.h"
#include "PCADFile.h"
#include "PCADViewDraw.h"
#include "ApertureFile.h"

namespace HandlerGERBER
{
    class GERBERFileWorkshop : public FileWorkshop
    {
    public:
        GERBERFileWorkshop() = default;
        virtual std::string GetFileWorkshopDescription() const override
        {
            return "GERBER (Gerber 32 и Gerber Laser) файлы";
        }

        virtual std::vector<std::pair<std::string, std::string>> GetFileExtensions() const override
        {
            using namespace std;

            pair<string, string> extension_pair1 = {"Файлы изображений GERBER 32(*.gbr)"s, "*.gbr"s};
            pair<string, string> extension_pair2 = {"Файлы изображений GERBER LASER(*.lgr)"s, "*.lgr"s};
            return {move(extension_pair1), move(extension_pair2)};
        }

        virtual FileWorkshop::LoadFileResult LoadPCADFile(std::istream& istr,
                                                          FileWorkshop::AdditionalLoadInfo& addit_load_info) override;

    private:
        static constexpr int DBU_IN_MEASURE_UNIT = 1000; // В файлах Gerber координатная единица всегда равна 1/1000 дюйма
    };

    class EXCELLONFileWorkshop : public FileWorkshop
    {
    public:
        EXCELLONFileWorkshop() = default;
        virtual std::string GetFileWorkshopDescription() const override
        {
            return "EXCELLON файл сверловки";
        }

        virtual std::vector<std::pair<std::string, std::string>> GetFileExtensions() const override
        {
            using namespace std;

            pair<string, string> extension_pair = {"Файлы сверловки EXCELLON (*.drl)"s, "*.drl"s};
            return {move(extension_pair)};
        }

        virtual FileWorkshop::LoadFileResult LoadPCADFile(std::istream& istr,
                                                          FileWorkshop::AdditionalLoadInfo& addit_load_info) override;

    private:
        // В файлах Excellon при работе в дюймовой системе координатная единица равна 1/10000 дюйма
        static constexpr int INCH_DBU_IN_MEASURE_UNIT = 10000;
        // При работе же в метрической системе примем координатную единицу равной 1/100 миллиметра
        static constexpr int MM_DBU_IN_MEASURE_UNIT = 100;
        static constexpr double TO_MM_SCALE_FACTOR = 100.0 / 101.6;
    };
} // namespace HandlerGERBER
