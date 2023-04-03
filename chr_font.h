#pragma once

#include "PCADViewDraw.h"
#include "wx/xrc/xmlres.h"
#include <string>
#include <filesystem>

#define MAX_COPYRIGHT_LEN 254     // Предельная длина информационной строкив CHR-заголовке

namespace chr
{
    class CHRFontXmlHandler : public wxXmlResourceHandler
    {
    public:
        CHRFontXmlHandler(const wxString& resource_filename) :
            resource_filename_(resource_filename)
        {}
        virtual wxObject* DoCreateResource();
        virtual bool CanHandle(wxXmlNode *node);

    private:
        wxString resource_filename_;
    };

    // CHR font file headers
    #pragma pack(push, 1)
    struct CHRNameHeader
    {
        uint16_t header_offset;
        char     font_name[4];
        uint16_t font_size;
        char     font_version[3];
    };

    struct CHRParHeader
    {
        unsigned char     par_prefix;
        uint16_t          chars_count;
        char              reserved1;
        unsigned char     first_char;
        uint16_t          data_offset;
        unsigned char     fill_flag;
        signed char       upper_margin;
        unsigned char     reserved2;
        signed char       lower_margin;
        char              reserved3[5];
    };
    #pragma pack(pop)

    enum class GlyphCmdType
    {
        GLYPH_CMD_END_GLYPH = 0,
        GLYPH_CMD_MOVE_PEN,
        GLYPH_CMD_DRAW_LINE,
        GLYPH_CMD_UNKNOWN = 255
    };

    struct GlyphCmdDesc
    {
        GlyphCmdType cmd_type;
        wxPoint target_point;
    };

    struct SymbolDescType
    {
        unsigned char          symbol_code;
        int                    symbol_width;
        int                    start_symbol_cmd; // Номер элемента массива symbol_glyphs,
                                                 // содержащий первую команду описания начертания символа.
        int                    cmd_count;  // Количество команд в описании начертания
    };

    struct CHRFontInfo
    {
        char                        font_name[4];
        wxString                    file_name;
        std::string                 copyright_text;
        int                         chars_count = 0; // Количество символов, описанных в шрифте
        unsigned char               first_char = 0;  // Код первого символа, описанного шрифтом
        int                         upper_margin; // предельная верхняя граница символов
        int                         lower_margin; // предельная нижняя граница символов
        int                         average_width; // Средняя ширина символа
        std::vector<int>            width_table; // Таблица ширин символов
        std::vector<SymbolDescType> symbols_desc; // Массив с определениями символов шрифта
        std::vector<GlyphCmdDesc>   cmd_glyphs; // Массив, хранящий команды описания очертаний символов
    };

    enum class CHRLoadErrCode
    {
        CHR_ERR_NO_ERROR = 0,
        CHR_ERR_INVALID_FILE_FORMAT,
        CHR_ERR_FILE_OPEN_ERROR,
        CHR_ERR_FILE_READ_ERROR
    };

    struct SetFontDesc
    {
        int font_number = -1;
        wxFont font; // Поле, хранящее параметры отрисовки шрифта.
                     // Пока только размер (высоту) символа и возможно, его ширину.
    };

    class CHRProcessor
    {
    public:
        friend class iterator;
        struct ExtentResult
        {
            bool is_valid = false;
            wxSize extent = wxSize(0, 0);
            double vert_scale = 1;
            double hor_scale = 1;
            const SetFontDesc* font_desc = nullptr;
            wxSize font_pix_size = wxSize(0, 0);
            const CHRFontInfo* font_info = nullptr;
        };

        CHRProcessor() = default;
        CHRProcessor(std::initializer_list<std::string> filename_list)
        {
            for (const std::string& load_filename : filename_list)
                FontLoader(wxString(load_filename.c_str(), wxConvUTF8));
        }

        CHRProcessor(std::initializer_list<wxString> filename_list)
        {
            for (const wxString& load_filename : filename_list)
                FontLoader(load_filename);
        }

        static wxString GetTextErrMessage(CHRLoadErrCode err_code);
        std::pair<int, CHRLoadErrCode> LoadFont(const wxString& font_path)
        {
            return FontLoader(font_path);
        }

        std::pair<int, CHRLoadErrCode> LoadFont(wxFSFile* font_file_ptr)
        {
            return FontLoader(font_file_ptr);
        }

        std::pair<int, CHRLoadErrCode> LoadFont(const std::string& str_font_path)
        {
            return FontLoader(wxString(str_font_path.c_str(), wxConvUTF8));
        }

        void UnloadFont(int font_number);
        void Clear();
        int GetMaxFontNumber() const
        {
            return fonts_info_.size() - 1;
        }

        const CHRFontInfo* GetFontInfo(int font_number) const
        {
            if (font_number < 0 || font_number >= static_cast<int>(fonts_info_.size()) ||
                !fonts_info_[font_number].chars_count)
                return nullptr;

            return &fonts_info_[font_number];
        }

        CHRProcessor& SetDCFontNumber(wxDC* wxdc_ptr, int font_number)
        {
            if (font_number < 0 || font_number >= static_cast<int>(fonts_info_.size()) ||
                !fonts_info_[font_number].chars_count)
                font_number = active_font_number_;
            SetFontDesc& font_desc = CreateNewFontSetMap(wxdc_ptr);
            font_desc.font_number = font_number;
            return *this;
        }

        CHRProcessor& SetDCPixelSize(wxDC* wxdc_ptr, const wxSize& pixelSize)
        {
            SetFontDesc& font_desc = CreateNewFontSetMap(wxdc_ptr);
            font_desc.font.SetPixelSize(pixelSize);
            return *this;
        }

        wxSize GetTextExtent(wxDC* wxdc_ptr, const wxString& wx_text) const
        {
            return Extenter(wxdc_ptr, wx_text, true).extent;
        }

        wxRect GetRectOrientedText(wxDC* wxdc_ptr, const wxString& wx_text, const wxPoint& text_point,
                                   TextOrientation text_orientation) const
        {
            return Drawer(wxdc_ptr, wx_text, text_point, text_orientation, false);
        }

        wxRect DrawOrientedText(wxDC* wxdc_ptr, const wxString& wx_text, const wxPoint& text_point,
                                TextOrientation text_orientation) const
        {
            return Drawer(wxdc_ptr, wx_text, text_point, text_orientation, true);
        }

        int GetActiveFontNumber() const
        {
            return active_font_number_;
        }

        CHRProcessor& SetActiveFontNumber(int new_active_font_number)
        {
            if (new_active_font_number < 0 ||
                new_active_font_number >= static_cast<int>(fonts_info_.size()) ||
                !fonts_info_[new_active_font_number].chars_count)
                return *this;
            active_font_number_ = new_active_font_number;
            return *this;
        }

    private:
        std::vector<CHRFontInfo> fonts_info_;
        int active_fonts_count_ = 0;
        int active_font_number_ = -1;
        std::unordered_map <wxDC*, SetFontDesc> font_set_map_;

        std::pair<int, CHRLoadErrCode> FontLoader(const wxString& font_path);
        std::pair<int, CHRLoadErrCode> FontLoader(wxFSFile* font_file_ptr);
        ExtentResult Extenter(wxDC* wxdc_ptr, const wxString& wx_text,
                              bool is_count_extent) const;
        wxRect Drawer(wxDC* wxdc_ptr, const wxString& wx_text, const wxPoint& text_point,
                      TextOrientation text_orientation, bool is_draw) const;
        SetFontDesc& CreateNewFontSetMap(wxDC* wxdc_ptr)
        {
            if (font_set_map_.count(wxdc_ptr))
            {
                return font_set_map_[wxdc_ptr];
            }
            else
            {
                SetFontDesc new_font_desc;
                new_font_desc.font_number = active_font_number_;
                new_font_desc.font.SetPixelSize(wxSize(10, 10));
                return (font_set_map_[wxdc_ptr] = std::move(new_font_desc));
            }
        }

    public:
        size_t size() const
        {
            return active_fonts_count_;
        }

        class iterator : public std::iterator<std::bidirectional_iterator_tag, CHRFontInfo>
        {
        public:
            friend class CHRProcessor;
            explicit iterator(CHRProcessor& parent, bool is_create_end_iter) : parent_(parent)
            {
                if (is_create_end_iter)
                {
                    current_font_info_iter_ = parent_.fonts_info_.end();
                }
                else
                {
                    current_font_info_iter_ = parent_.fonts_info_.begin();
                    while (current_font_info_iter_ != parent_.fonts_info_.end() &&
                           current_font_info_iter_->chars_count == 0)
                        ++current_font_info_iter_;
                }
            }

            iterator& operator++()
            {
                while (current_font_info_iter_ != parent_.fonts_info_.end() &&
                       current_font_info_iter_->chars_count == 0)
                    ++current_font_info_iter_;
                return *this;
            }

            iterator operator++(int)
            {
                iterator retval = *this;
                ++(*this);
                return retval;
            }

            iterator& operator--()
            {
                auto save_old_iter = current_font_info_iter_;
                while (current_font_info_iter_ != parent_.fonts_info_.begin())
                {
                    --current_font_info_iter_;
                    if (current_font_info_iter_->chars_count)
                        return *this;
                }
                current_font_info_iter_ = save_old_iter;
                return *this;
            }

            iterator operator--(int)
            {
                iterator retval = *this;
                --(*this);
                return retval;
            }

            bool operator==(iterator other) const
            {
                return current_font_info_iter_ == other.current_font_info_iter_;
            }

            bool operator!=(iterator other) const
            {
                return !(*this == other);
            }

            reference operator*() const
            {
                return *current_font_info_iter_;
            }

            pointer operator->() const
            {
                return &(*current_font_info_iter_);
            }

        private:
            CHRProcessor& parent_;
            std::vector<CHRFontInfo>::iterator current_font_info_iter_;
        };

        iterator begin()
        {
            return iterator(*this, false);
        }

        iterator end()
        {
            return iterator(*this, true);
        }
    };
} // namespace chr
