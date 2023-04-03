
#include "chr_font.h"

#include <wx/filesys.h>
#include <iostream>
#include <fstream>

using namespace std;
using namespace std::filesystem;

namespace chr
{
    wxObject* CHRFontXmlHandler::DoCreateResource()
    {
        wxString font_file_name = m_node->GetNodeContent();
        wxFSFile* font_file_ptr = wxFileSystem().
            OpenFile(resource_filename_ + wxT("#zip:") + font_file_name);
        return font_file_ptr;
    }

    bool CHRFontXmlHandler::CanHandle(wxXmlNode *node)
    {
        return IsOfClass(node, wxT("fontCHR"));
    }

    void CHRProcessor::UnloadFont(int font_number)
    {
        if (font_number < 0 || font_number >= static_cast<int>(fonts_info_.size()) ||
            !fonts_info_[font_number].chars_count)
            return;

        CHRFontInfo& font_info = fonts_info_[font_number];
        font_info.chars_count = 0; // Нулевое значение этого поля будет считать флагом пустой записи о шрифте
        font_info.width_table.clear();
        font_info.symbols_desc.clear();
        font_info.cmd_glyphs.clear();
        --active_fonts_count_;
        // Если выгружается текущий шрифт, попытаемся найти какой-либо уже загруженный и сделать его текущим
        if (active_font_number_ == font_number)
        {
            active_font_number_ = -1;
            for (size_t i = 0; i < fonts_info_.size(); ++i)
            {
                if (fonts_info_[i].chars_count)
                {
                    active_font_number_ = i;
                    break;
                }
            }
        }
        // Далее нужно удалить все ссылки на выгружаемый шрифт из словаря font_set_map_.
        std::vector <wxDC*> erase_font_set_list;
        for (auto& font_set_map_pair : font_set_map_)
            if (font_set_map_pair.second.font_number == font_number)
                erase_font_set_list.push_back(font_set_map_pair.first);

        for (wxDC* erase_dc : erase_font_set_list)
            font_set_map_.erase(erase_dc);
    }

    void CHRProcessor::Clear()
    {
        fonts_info_.clear();
        active_fonts_count_ = 0;
        active_font_number_ = -1;
        font_set_map_.clear();
    }

    pair<int, CHRLoadErrCode> CHRProcessor::FontLoader(const wxString& font_path)
    {
        wxFSFile* font_file_ptr = wxFileSystem().OpenFile(font_path);
        if (font_file_ptr)
        {
            pair<int, CHRLoadErrCode> result = FontLoader(font_file_ptr);
            delete font_file_ptr;
            return result;
        }
        else
        {
            return {-1, CHRLoadErrCode::CHR_ERR_FILE_OPEN_ERROR};
        }
    }

    pair<int, CHRLoadErrCode> CHRProcessor::FontLoader(wxFSFile* font_file_ptr)
    {
        wxInputStream* font_file_stream = font_file_ptr->GetStream();
        if (!font_file_stream || !font_file_stream->IsOk())
            return {-1, CHRLoadErrCode::CHR_ERR_FILE_OPEN_ERROR};

        string file_signat(4, 0);
        font_file_stream->Read(file_signat.data(), 4);
        if (file_signat != "PK\b\b")
            return {-1, CHRLoadErrCode::CHR_ERR_INVALID_FILE_FORMAT};

        string copyright_sign;
        while (true)
        {
            char c = font_file_stream->GetC();
            if (!font_file_stream->IsOk())
                return {-1, CHRLoadErrCode::CHR_ERR_FILE_READ_ERROR};
            copyright_sign += c;
            if (copyright_sign.size() > MAX_COPYRIGHT_LEN)
                return {-1, CHRLoadErrCode::CHR_ERR_INVALID_FILE_FORMAT};
            if (c == 0x1a)
                break;
        }

        CHRNameHeader name_header;
        font_file_stream->Read(&name_header, sizeof(name_header));
        if (!font_file_stream->IsOk())
            return {-1, CHRLoadErrCode::CHR_ERR_FILE_READ_ERROR};

        CHRParHeader par_header;
        font_file_stream->SeekI(name_header.header_offset);
        font_file_stream->Read(&par_header, sizeof(par_header));
        if (!font_file_stream->IsOk() || par_header.par_prefix != '+' ||  par_header.chars_count == 0)
            return {-1, CHRLoadErrCode::CHR_ERR_INVALID_FILE_FORMAT};

        vector<uint16_t> offsets(par_header.chars_count, 0);
        font_file_stream->Read(offsets.data(), par_header.chars_count * 2);
        vector<unsigned char> widths(par_header.chars_count, 0);
        font_file_stream->Read(widths.data(), par_header.chars_count);
        if (!font_file_stream->IsOk())
            return {-1, CHRLoadErrCode::CHR_ERR_FILE_READ_ERROR};

        // Наконец приступаем к формированию и заполнению описателей символов шрифта
        CHRFontInfo font_info;
        font_info.font_name[0] = name_header.font_name[0];
        font_info.font_name[1] = name_header.font_name[1];
        font_info.font_name[2] = name_header.font_name[2];
        font_info.font_name[3] = name_header.font_name[3];
        font_info.file_name = font_file_ptr->GetLocation();
        font_info.copyright_text = copyright_sign;
        font_info.chars_count = par_header.chars_count;
        font_info.first_char = par_header.first_char;
        font_info.upper_margin = par_header.upper_margin;
        font_info.lower_margin = par_header.lower_margin;
        // max_command_offset - покомандное смещение, указывающее _за_ последнюю команду.
        int max_command_offset = (name_header.font_size - sizeof(par_header) - par_header.chars_count * 3) / 2;
        // Теперь рассчитаем это же максимально возможное смещение, но исходя из размера файла
        int max_command_offset_by_file = (font_file_stream->GetSize() - name_header.header_offset -
                                          sizeof(par_header) - par_header.chars_count * 3) / 2;
        if (max_command_offset > max_command_offset_by_file)
            max_command_offset = max_command_offset_by_file;
        if (max_command_offset <= 0)
            return {-1, CHRLoadErrCode::CHR_ERR_INVALID_FILE_FORMAT};
        // Пересчитываем смещения из байтовых в покомандные. Смещение до первой команды хранится в байтах,
        // а сами команды двухбайтовые.
        for (uint16_t& cur_offset : offsets)
        {
            cur_offset >>= 1;
            if (cur_offset >= max_command_offset)
                cur_offset = 0; // Все недопустимые (указывающие за конец шрифта) смещения зануляем
        }

        unsigned int total_width;
        for (int i = 0; i < par_header.chars_count; ++i)
        { // Формируем дескрипторы для всех описываемых в шрифте символов
            SymbolDescType symbol_desc;

            symbol_desc.symbol_code = par_header.first_char + i;
            symbol_desc.symbol_width = widths[i];
            symbol_desc.start_symbol_cmd = offsets[i];
            if (i < par_header.chars_count - 1)
                symbol_desc.cmd_count = offsets[i + 1] - offsets[i];
            else
                symbol_desc.cmd_count = max_command_offset - offsets[i];

            total_width += symbol_desc.symbol_width;
            font_info.width_table.push_back(symbol_desc.symbol_width);
            font_info.symbols_desc.push_back(symbol_desc);
        }
        font_info.average_width = round(total_width / par_header.chars_count);

        // Переходим к считыванию блока команд рисовки символов
        font_file_stream->SeekI(name_header.header_offset + par_header.data_offset);
        while (font_file_stream->IsOk() && !font_file_stream->Eof())
        {
            uint16_t next_cmd_val;
            font_file_stream->Read(&next_cmd_val, sizeof(next_cmd_val));
            GlyphCmdDesc cmd_desc;
            if (next_cmd_val & 0x8000)
            {
                if (next_cmd_val & 0x0080)
                    cmd_desc.cmd_type = GlyphCmdType::GLYPH_CMD_DRAW_LINE;
                else
                    cmd_desc.cmd_type = GlyphCmdType::GLYPH_CMD_UNKNOWN;
            }
            else
            {
                if (next_cmd_val & 0x0080)
                    cmd_desc.cmd_type = GlyphCmdType::GLYPH_CMD_MOVE_PEN;
                else
                    cmd_desc.cmd_type = GlyphCmdType::GLYPH_CMD_END_GLYPH;
            }
            int target_x = next_cmd_val & 0x007f;
            int target_y = (next_cmd_val >> 8) & 0x007f;
            if (target_y > 63)
                target_y = target_y - 128;
            cmd_desc.target_point = wxPoint(target_x, target_y);
            font_info.cmd_glyphs.push_back(cmd_desc);
        }

        ++active_fonts_count_;
        size_t fonts_info_num = 0;
        // Ищем свободную ячейку для вновь загруженного шрифта
        for (; fonts_info_num < fonts_info_.size(); ++fonts_info_num)
        {
            if (!fonts_info_[fonts_info_num].chars_count)
            { // Свободная ячейка нашлась, занимаем её.
                fonts_info_[fonts_info_num] = move(font_info);
                break;
            }
        }
        if (fonts_info_num >= fonts_info_.size())
        { // Все элементы массива fonts_info_ заняты, добавляем новый и занимаем его.
            fonts_info_.push_back(move(font_info));
            fonts_info_num = fonts_info_.size() - 1;
        }

        if (active_font_number_ < 0)
            active_font_number_ = fonts_info_num;
        return {fonts_info_num, CHRLoadErrCode::CHR_ERR_NO_ERROR};
    }

    CHRProcessor::ExtentResult CHRProcessor::Extenter(wxDC* wxdc_ptr, const wxString& wx_text,
                                                      bool is_count_extent) const
    { // Функция выбирает из текущей шрифтовой базы параметры нужного шрифта, а также рассчитывает габаритные
      // размеры (длину и высоту) выводимой строки (если это заказно установкой is_count_extent в true).
        ExtentResult result;
        if (!font_set_map_.count(wxdc_ptr))
            return result;

        result.font_desc = &font_set_map_.at(wxdc_ptr);
        if (result.font_desc->font_number < 0 ||
            result.font_desc->font_number >= static_cast<int>(fonts_info_.size()) ||
            !fonts_info_[result.font_desc->font_number].chars_count)
            return result;

        result.font_pix_size = result.font_desc->font.GetPixelSize(); // Заказанный пользователем размер рисовки
        result.font_info = &fonts_info_[result.font_desc->font_number]; // ссылка на используемый шрифт
        // Сначала рассчитаем заказанные коэффициенты масштабирования по вертикали и горизонтали.
        // Для этого вычислим соотношения затребованных размеров рисования с исходными размерами шрифта.
        // В качестве исходных размеров примем его максимальную действительную высоту
        // (upper_margin - lower_margin + 1) и среднюю ширину символа average_width.
        result.vert_scale = static_cast<double>(result.font_pix_size.GetHeight()) /
                            (result.font_info->upper_margin - result.font_info->lower_margin + 1);
        if (result.font_pix_size.GetWidth())
            result.hor_scale = static_cast<double>(result.font_pix_size.GetWidth()) / result.font_info->average_width;
        else // Если требуемый горизонтальный размер шрифта не задан, положим масштаб по горизонтали равным масштабу по вертикали.
            result.hor_scale = result.vert_scale;
        result.is_valid = true;
        if (!is_count_extent)
            return result; // Рассчитывать габариты не нужно - всё сделано, выходим.

        // Теперь определяем габаритные размеры надписи (длину и высоту).
        int hor_ext = 0;
        for (wxChar wx_c : wx_text) // Сначала рассчитаем протяжённость строки в "шрифтовых точках"
        { // Рисоваться будут только символы в пределах диапазона кодов, описанных в шрифте
            if (wx_c >= result.font_info->first_char &&
                wx_c < result.font_info->first_char + result.font_info->chars_count)
                hor_ext += result.font_info->width_table[wx_c - result.font_info->first_char];
        }
        // Теперь вычислим высоту строки в шрифтовых точках. В качестве такой выберем максимальную полную высоту символов шрифта.
        int vert_ext = result.font_info->upper_margin - result.font_info->lower_margin + 1;
        // На данный момент мы имеем длину строки hor_ext и высоту строки vert_ext в "точках шрифта". Далее преобразуем
        // их в действительные экранные пиксели, учитывая ранее рассчитанные масштабы рисования текста.
        result.extent = wxSize(hor_ext * result.hor_scale, vert_ext * result.vert_scale);
        return result;
    }

    wxRect CHRProcessor::Drawer(wxDC* wxdc_ptr, const wxString& wx_text, const wxPoint& text_point,
                                TextOrientation text_orientation, bool is_draw) const
    {
        // Получим данные выбранного для wxdc_ptr шрифта и выполним расчётов масшатбов рисовки.
        ExtentResult rs = Extenter(wxdc_ptr, wx_text, false);
        if (!rs.is_valid)
            return {};
        wxRect result_rect;
        int pen_halfwidth = wxdc_ptr->GetPen().GetWidth() / 2;
        wxPoint start_symbol_point(text_point.x, text_point.y);
        // Ну а теперь переходим непосредственно к рисованию
        for (wxChar wx_c : wx_text)
        { // Рисоваться будут только символы в пределах диапазона кодов, описанных в шрифте
            if (wx_c >= rs.font_info->first_char && wx_c < rs.font_info->first_char + rs.font_info->chars_count)
            {
                wxPoint current_symbol_point = start_symbol_point; // Начинаем рисовать следующий символ там,
                                                                   // где закончился предыдущий.
                wxRect current_symbol_point_rect = wxRect(current_symbol_point, wxSize(1, 1));
                current_symbol_point_rect.Inflate(pen_halfwidth, pen_halfwidth);
                int current_cmd_number =
                    rs.font_info->symbols_desc[wx_c - rs.font_info->first_char].start_symbol_cmd;

                while (current_cmd_number >= 0)
                {
                    const GlyphCmdDesc& current_cmd = rs.font_info->cmd_glyphs[current_cmd_number];
                    double target_x = current_cmd.target_point.x * rs.hor_scale;
                    double target_y = current_cmd.target_point.y * rs.vert_scale;
                    switch (text_orientation)
                    {
                    case TextOrientation::TEXT_LEFT_RIGHT: //
                        target_y = -target_y;
                        break;
                    case TextOrientation::TEXT_DOWN_UP: //
                        swap(target_x, target_y);
                        target_x = -target_x;
                        target_y = -target_y;
                        break;
                    case TextOrientation::TEXT_RIGHT_LEFT: //
                        target_x = -target_x;
                        break;
                    case TextOrientation::TEXT_UP_DOWN: //
                        swap(target_x, target_y);
                        break;
                    case TextOrientation::TEXT_LEFT_RIGHT_MIRROR: //
                        target_x = -target_x;
                        target_y = -target_y;
                        break;
                    case TextOrientation::TEXT_DOWN_UP_MIRROR: //
                        swap(target_x, target_y);
                        target_x = -target_x;
                        break;
                    case TextOrientation::TEXT_RIGHT_LEFT_MIRROR: //
                        break;
                    case TextOrientation::TEXT_UP_DOWN_MIRROR: //
                        swap(target_x, target_y);
                        target_y = -target_y;
                        break;
                    default:
                        break;
                    }

                    wxPoint target_point = wxPoint(target_x + start_symbol_point.x,
                                                   target_y + start_symbol_point.y);
                    wxRect target_point_rect = wxRect(target_point, wxSize(1, 1));
                    target_point_rect.Inflate(pen_halfwidth, pen_halfwidth);

                    switch (current_cmd.cmd_type)
                    {
                    case GlyphCmdType::GLYPH_CMD_DRAW_LINE:
                        if (is_draw)
                            wxdc_ptr->DrawLine(current_symbol_point, target_point);
                        [[fallthrough]];
                    case GlyphCmdType::GLYPH_CMD_MOVE_PEN:
                        result_rect.Union(current_symbol_point_rect);
                        result_rect.Union(target_point_rect);
                        current_symbol_point = target_point;
                        current_symbol_point_rect = target_point_rect;
                        ++current_cmd_number;
                        break;
                    case GlyphCmdType::GLYPH_CMD_END_GLYPH:
                        [[fallthrough]];
                    default:
                        start_symbol_point = current_symbol_point;
                        current_cmd_number = -1;
                        break;
                    }
                }
            }
        }

        return result_rect;
    }

    wxString CHRProcessor::GetTextErrMessage(CHRLoadErrCode err_code)
    {
        switch (err_code)
        {
        case CHRLoadErrCode::CHR_ERR_NO_ERROR:
            return _("Полный порядок");
        case CHRLoadErrCode::CHR_ERR_FILE_OPEN_ERROR:
            return _("Ошибка открытия файла");
        case CHRLoadErrCode::CHR_ERR_FILE_READ_ERROR:
            return _("Ошибка чтения файла");
        case CHRLoadErrCode::CHR_ERR_INVALID_FILE_FORMAT:
            return _("Неверный формат файла шрифта");
        default:
            return _("Неизвестная ошибка");
        }
    }

} // namespace chr
