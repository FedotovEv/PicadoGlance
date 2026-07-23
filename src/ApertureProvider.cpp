
#include "ApertureProvider.h"

using namespace std;
using namespace std::filesystem;
using namespace aperture;

namespace
{
    struct InternalFlashStruct
    {
        double flash_size_points; // Размер апертуры (засветки) в сотых дюйма (точках)
        double flash_size_mm; // Размер апертуры в миллиметрах
        FlashShape flash_shape; // Форма засветки
        FlashType flash_type; // Способ применения апертуры
        int d_code; // Числовая часть D-кода апертуры, используемого для ссылки на неё в GERBER-файлах.
    };

    const InternalFlashStruct internal_flashes[] =
    {
        {32.00, 0.813, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 10},
        {36.00, 0.914, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 11},
        {40.00, 1.016, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 12},
        {44.00, 1.118, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 13},
        {48.00, 1.219, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 14},
        {52.00, 1.321, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 15},
        {56.00, 1.422, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 16},
        {60.00, 1.524, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 17},
        {64.00, 1.626, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 18},
        {68.00, 1.727, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 19},
        {72.00, 1.829, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 70},
        {76.00, 1.930, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 71},
        {80.00, 2.032, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 20},
        {84.00, 2.134, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 21},
        {88.00, 2.235, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 22},
        {92.00, 2.337, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 23},
        {96.00, 2.438, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 24},
        {100.00, 2.540, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 25},
        {104.00, 2.642, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 26},
        {108.00, 2.743, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 27},
        {112.00, 2.845, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 28},
        {116.00, 2.946, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 29},
        {120.00, 3.048, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 72},
        {124.00, 3.150, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 73},
        {128.00, 3.251, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 30},
        {132.00, 3.353, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 31},
        {136.00, 3.454, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 32},
        {140.00, 3.556, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 33},
        {144.00, 3.658, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 34},
        {148.00, 3.759, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 35},
        {152.00, 3.861, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 36},
        {156.00, 3.962, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 37},
        {160.00, 4.064, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 38},
        {164.00, 4.166, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 39},
        {168.00, 4.267, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 40},
        {172.00, 4.369, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 41},
        {176.00, 4.470, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 42},
        {180.00, 4.572, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 43},
        {184.00, 4.674, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 44},
        {188.00, 4.775, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 45},
        {192.00, 4.877, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 46},
        {196.00, 4.978, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 47},
        {200.00, 5.080, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 48},
        {204.00, 5.182, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 49},
        {208.00, 5.283, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 50},
        {212.00, 5.385, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 51},
        {216.00, 5.486, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 52},
        {220.00, 5.588, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 53},
        {224.00, 5.690, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 54},
        {228.00, 5.791, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 55},
        {232.00, 5.893, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 56},
        {236.00, 5.994, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 57},
        {240.00, 6.096, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 58},
        {244.00, 6.198, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 59},
        {248.00, 6.299, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 60},
        {252.00, 6.401, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 61},
        {256.00, 6.502, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 62},
        {260.00, 6.604, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 63},
        {264.00, 6.706, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 64},
        {268.00, 6.807, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 65},
        {272.00, 6.909, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 66},
        {276.00, 7.010, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 67},
        {280.00, 7.112, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 68},
        {284.00, 7.214, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 69},
        {288.00, 7.315, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 74},
        {292.00, 7.417, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 75},
        {296.00, 7.518, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 76},
        {300.00, 7.620, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 77},
        {304.00, 7.722, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 78},
        {308.00, 7.823, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 79},
        {312.00, 7.925, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 80},
        {316.00, 8.026, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 81},
        {320.00, 8.128, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 82},
        {324.00, 8.230, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 83},
        {328.00, 8.331, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 84},
        {332.00, 8.433, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 85},
        {336.00, 8.534, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 86},
        {340.00, 8.636, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 87},
        {344.00, 8.738, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 88},
        {348.00, 8.839, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 89},
        {4.00, 0.102, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 90},
        {5.00, 0.127, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 91},
        {6.00, 0.152, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 92},
        {7.00, 0.178, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 93},
        {8.00, 0.203, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 94},
        {9.00, 0.229, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 95},
        {10.00, 0.254, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 96},
        {11.00, 0.279, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 97},
        {12.00, 0.305, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 98},
        {13.00, 0.330, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 99},
        {14.00, 0.356, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 100},
        {15.00, 0.381, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 101},
        {16.00, 0.406, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 102},
        {17.00, 0.432, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 103},
        {18.00, 0.457, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 104},
        {19.00, 0.483, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 105},
        {20.00, 0.508, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 106},
        {22.00, 0.559, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 107},
        {24.00, 0.610, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 108},
        {26.00, 0.660, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 109},
        {28.00, 0.711, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 110},
        {30.00, 0.762, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 111},
        {32.00, 0.813, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 112},
        {34.00, 0.864, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 113},
        {36.00, 0.914, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 114},
        {38.00, 0.965, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 115},
        {40.00, 1.016, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 116},
        {42.00, 1.067, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 117},
        {44.00, 1.118, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 118},
        {46.00, 1.168, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 119},
        {48.00, 1.219, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 120},
        {50.00, 1.270, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 121},
        {52.00, 1.321, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 122},
        {54.00, 1.372, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 123},
        {56.00, 1.422, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 124},
        {58.00, 1.473, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 125},
        {60.00, 1.524, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 126},
        {62.00, 1.575, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 127},
        {64.00, 1.626, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 128},
        {68.00, 1.727, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 129},
        {72.00, 1.829, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 130},
        {76.00, 1.930, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 131},
        {80.00, 2.032, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 132},
        {84.00, 2.134, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 133},
        {88.00, 2.235, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 134},
        {92.00, 2.337, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 135},
        {96.00, 2.438, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 136},
        {100.00, 2.540, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 137},
        {104.00, 2.642, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 138},
        {108.00, 2.743, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 139},
        {112.00, 2.845, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 140},
        {116.00, 2.946, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 141},
        {120.00, 3.048, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 142},
        {124.00, 3.150, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 143},
        {128.00, 3.251, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 144},
        {132.00, 3.353, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 145},
        {136.00, 3.454, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 146},
        {140.00, 3.556, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 147},
        {144.00, 3.658, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 148},
        {148.00, 3.759, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 149},
        {152.00, 3.861, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 150},
        {158.00, 4.013, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 151},
        {164.00, 4.166, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 152},
        {170.00, 4.318, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 153},
        {176.00, 4.470, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 154},
        {182.00, 4.623, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 155},
        {188.00, 4.775, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 156},
        {194.00, 4.928, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 157},
        {200.00, 5.080, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 158},
        {206.00, 5.232, FlashShape::FLASH_ROUND, FlashType::FLASH_FLASHER, 159},
        {32.00, 0.813, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 160},
        {36.00, 0.914, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 161},
        {40.00, 1.016, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 162},
        {44.00, 1.118, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 163},
        {48.00, 1.219, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 164},
        {52.00, 1.321, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 165},
        {56.00, 1.422, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 166},
        {60.00, 1.524, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 167},
        {64.00, 1.626, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 168},
        {68.00, 1.727, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 169},
        {72.00, 1.829, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 170},
        {76.00, 1.930, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 171},
        {80.00, 2.032, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 172},
        {84.00, 2.134, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 173},
        {88.00, 2.235, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 174},
        {92.00, 2.337, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 175},
        {96.00, 2.438, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 176},
        {100.00, 2.540, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 177},
        {104.00, 2.642, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 178},
        {108.00, 2.743, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 179},
        {112.00, 2.845, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 180},
        {116.00, 2.946, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 181},
        {120.00, 3.048, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 182},
        {124.00, 3.150, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 183},
        {128.00, 3.251, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 184},
        {132.00, 3.353, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 185},
        {136.00, 3.454, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 186},
        {140.00, 3.556, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 187},
        {144.00, 3.658, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 188},
        {148.00, 3.759, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 189},
        {152.00, 3.861, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 190},
        {156.00, 3.962, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 191},
        {160.00, 4.064, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 192},
        {164.00, 4.166, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 193},
        {168.00, 4.267, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 194},
        {172.00, 4.369, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 195},
        {176.00, 4.470, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 196},
        {180.00, 4.572, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 197},
        {184.00, 4.674, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 198},
        {188.00, 4.775, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 199},
        {192.00, 4.877, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 200},
        {196.00, 4.978, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 201},
        {200.00, 5.080, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 202},
        {204.00, 5.182, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 203},
        {208.00, 5.283, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 204},
        {212.00, 5.385, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 205},
        {216.00, 5.486, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 206},
        {220.00, 5.588, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 207},
        {224.00, 5.690, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 208},
        {228.00, 5.791, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 209},
        {232.00, 5.893, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 210},
        {236.00, 5.994, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 211},
        {240.00, 6.096, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 212},
        {244.00, 6.198, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 213},
        {248.00, 6.299, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 214},
        {252.00, 6.401, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 215},
        {256.00, 6.502, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 216},
        {260.00, 6.604, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 217},
        {264.00, 6.706, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 218},
        {268.00, 6.807, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 219},
        {272.00, 6.909, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 220},
        {276.00, 7.010, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 221},
        {280.00, 7.112, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 222},
        {284.00, 7.214, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 223},
        {288.00, 7.315, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 224},
        {292.00, 7.417, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 225},
        {296.00, 7.518, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 226},
        {300.00, 7.620, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 227},
        {304.00, 7.722, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 228},
        {308.00, 7.823, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 229},
        {312.00, 7.925, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 230},
        {316.00, 8.026, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 231},
        {320.00, 8.128, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 232},
        {324.00, 8.230, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 233},
        {328.00, 8.331, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 234},
        {332.00, 8.433, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 235},
        {336.00, 8.534, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 236},
        {340.00, 8.636, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 237},
        {344.00, 8.738, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 238},
        {348.00, 8.839, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 239},
        {4.00, 0.102, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 240},
        {6.00, 0.152, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 241},
        {8.00, 0.203, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 242},
        {10.00, 0.254, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 243},
        {12.00, 0.305, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 244},
        {14.00, 0.356, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 245},
        {16.00, 0.406, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 246},
        {18.00, 0.457, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 247},
        {20.00, 0.508, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 248},
        {22.00, 0.559, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 249},
        {26.00, 0.660, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 250},
        {30.00, 0.762, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 251},
        {34.00, 0.864, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 252},
        {38.00, 0.965, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 253},
        {42.00, 1.067, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 254},
        {46.00, 1.168, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 255},
        {50.00, 1.270, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 256},
        {54.00, 1.372, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 257},
        {58.00, 1.473, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 258},
        {62.00, 1.575, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 259},
        {66.00, 1.676, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 260},
        {70.00, 1.778, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 261},
        {74.00, 1.880, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 262},
        {78.00, 1.981, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 263},
        {82.00, 2.083, FlashShape::FLASH_SQUARE, FlashType::FLASH_FLASHER, 264}
    };
} // namespace

namespace aperture
{
    ApertureProvider::ApertureProvider()
    {
        LoadApertureFile(ApertureLoadlInfo{});
    }

    PCADLoadError ApertureProvider::LoadApertureFile(const ApertureLoadlInfo& load_info)
    {
        PCADLoadError process_error_code = PCADLoadError::LOAD_FILE_NO_ERROR;
        flashes_.gerber32.clear();
        flashes_.gerber_laser.clear();

        if (load_info.aper_file_stream_ptr &&
            load_info.aper_file_type != ApertureFileType::APERTURE_FILE_NONE)
        {
            ApertureFileType aper_file_type = DetectApertureFileType(*load_info.aper_file_stream_ptr);
            if (aper_file_type == ApertureFileType::APERTURE_FILE_GAP)
                flashes_ = LoadGAPApertureFile(*load_info.aper_file_stream_ptr, process_error_code);
            else if (aper_file_type == ApertureFileType::APERTURE_FILE_APR)
                flashes_ = LoadAPRApertureFile(*load_info.aper_file_stream_ptr, process_error_code);
            if (process_error_code == PCADLoadError::LOAD_FILE_NO_ERROR)
            {
                aperture_filetype_ = aper_file_type;
                aperture_filepath_ = load_info.aperture_file_path;
            }
        }

        if (!load_info.aper_file_stream_ptr ||
            load_info.aper_file_type == ApertureFileType::APERTURE_FILE_NONE ||
            process_error_code != PCADLoadError::LOAD_FILE_NO_ERROR)
        {  // Затребована загрузка вспышек по умолчанию или загрузка файла с описанием
           // апертур не удалась. В обоих случаях используем предопределённый набор апертур.
            size_t i = 0;
            for (const InternalFlashStruct& current_flash : internal_flashes)
            {
                FlashDesc fd_flash;
                fd_flash.flash_number = ++i;
                fd_flash.flash_size_inch = current_flash.flash_size_points / 100;
                fd_flash.flash_size_mm = current_flash.flash_size_mm;
                fd_flash.flash_shape = current_flash.flash_shape;
                fd_flash.flash_type = current_flash.flash_type;
                fd_flash.d_code = 'D' + to_string(current_flash.d_code);

                if (i <= GERBER32_MAX_APERTURE_NUMBER)
                    flashes_.gerber32.push_back(fd_flash);
                flashes_.gerber_laser.push_back(fd_flash);
            }
            aperture_filetype_ = ApertureFileType::APERTURE_FILE_NONE;
            aperture_filepath_ = path();
        }
        return process_error_code;
    }

    FlashDesc ApertureProvider::GetFlashDesc(const string& scan_d_code) const
    {
        FlashDesc fd;
        fd.d_code = scan_d_code;

        if (using_aperture_type_ == UsingApertureType::USING_APERTURE_GERBER_LASER)
        {
            for (const FlashDesc& current_fd : flashes_.gerber_laser)
            {
                if (current_fd.d_code == scan_d_code)
                {
                    fd = current_fd;
                    break;
                }
            }
        }
        else
        {
            for (const FlashDesc& current_fd : flashes_.gerber32)
            {
                if (current_fd.d_code == scan_d_code)
                {
                    fd = current_fd;
                    break;
                }
            }
        }

        return fd;
    }

    FlashDesc ApertureProvider::GetFlashDesc(int flash_num) const
    { // Номер апертуры flash_num должен лежать в диапазоне от 1 до максимального номера среди загруженных апертур.
      // Для Gerber 32, как правило, доступны 24 диафрагмы с номерами от 1 до 24, а для Gerber Laser - 255 апертур
      // с номерами от 1 до 255.
        int flash_pos = flash_num - 1;
        if (using_aperture_type_ == UsingApertureType::USING_APERTURE_GERBER32)
        {
            if (flash_pos >= 0 && flash_pos < static_cast<int>(flashes_.gerber32.size()))
                return flashes_.gerber32[flash_pos];
        }
        else if (using_aperture_type_ == UsingApertureType::USING_APERTURE_GERBER_LASER)
        {
            if (flash_pos >= 0 && flash_pos < static_cast<int>(flashes_.gerber_laser.size()))
                return flashes_.gerber_laser[flash_pos];
        }
        // Если нужной загруженной вспышки нет, возвращаем апертуру по умолчанию с нужным номером
        FlashDesc fd;
        fd.flash_number = flash_num;
        return fd;
    }

    size_t ApertureProvider::size() const
    {
        if (using_aperture_type_ == UsingApertureType::USING_APERTURE_GERBER32)
            return flashes_.gerber32.size();
        else if (using_aperture_type_ == UsingApertureType::USING_APERTURE_GERBER_LASER)
            return flashes_.gerber_laser.size();
        else
            return 0;
    }

    vector<FlashDesc>::const_iterator ApertureProvider::begin() const
    {
        if (using_aperture_type_ == UsingApertureType::USING_APERTURE_GERBER32)
            return flashes_.gerber32.cbegin();
        else if (using_aperture_type_ == UsingApertureType::USING_APERTURE_GERBER_LASER)
            return flashes_.gerber_laser.cbegin();
        else
            return flashes_.gerber32.cend();
    }

    vector<FlashDesc>::const_iterator ApertureProvider::end() const
    {
        if (using_aperture_type_ == UsingApertureType::USING_APERTURE_GERBER32)
            return flashes_.gerber32.cend();
        else if (using_aperture_type_ == UsingApertureType::USING_APERTURE_GERBER_LASER)
            return flashes_.gerber_laser.cend();
        else
            return flashes_.gerber32.cend();
    }

    vector<FlashDesc>::const_reverse_iterator ApertureProvider::rbegin() const
    {
        if (using_aperture_type_ == UsingApertureType::USING_APERTURE_GERBER32)
            return flashes_.gerber32.crbegin();
        else if (using_aperture_type_ == UsingApertureType::USING_APERTURE_GERBER_LASER)
            return flashes_.gerber_laser.crbegin();
        else
            return flashes_.gerber32.crend();
    }

    vector<FlashDesc>::const_reverse_iterator ApertureProvider::rend() const
    {
        if (using_aperture_type_ == UsingApertureType::USING_APERTURE_GERBER32)
            return flashes_.gerber32.crend();
        else if (using_aperture_type_ == UsingApertureType::USING_APERTURE_GERBER_LASER)
            return flashes_.gerber_laser.crend();
        else
            return flashes_.gerber32.crend();
    }
} // namespace aperture
