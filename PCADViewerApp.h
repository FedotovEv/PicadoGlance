/***************************************************************
 * Name:      PCADViewerApp.h
 * Author:    Dichrograph (dichrograph@rambler.ru)
 * Created:   2022-04-01
 * Copyright: Dichrograph ()
 * License:   GPL-3.0-or-later
 **************************************************************/

#pragma once

#include <fstream>
#include <filesystem>

#include <wx/app.h>
#include <wx/cshelp.h>
#include <wx/html/helpctrl.h>
#include <wx/cmdline.h>
#include <wx/print.h>
#include <wx/filesys.h>
#include <wx/fs_zip.h>
#include <wx/intl.h>

#include "PCADFile.h"
#include "svg.h"

struct VaryCanvasEditorStatus
{
    wxSize canvas_size = wxSize(210, 297);
    bool is_metric = true;
    bool is_fixed_ratio = false;
    std::string canvas_name;
};

enum class GraphExportType
{
    GRAPH_EXPORT_UNKNOWN = 0,
    GRAPH_EXPORT_FILE,
    GRAPH_EXPORT_CLIPBOARD,
    GRAPH_EXPORT_SVG,
    GRAPH_EXPORT_PRINTER
};

enum class ExportErrorType
{
    EXPORT_NO_ERROR = 0,
    EXPORT_CREATE_FILE_ERROR,
    EXPORT_WRITE_FILE_ERROR,
    EXPORT_CATCH_CLIPBOARD_ERROR
};

struct GraphExportContext
{
    GraphExportType export_type = GraphExportType::GRAPH_EXPORT_CLIPBOARD;
    wxString filename;
    wxBitmapType bitmap_type = wxBITMAP_TYPE_BMP;
    wxDC* draw_dc_ptr = nullptr;
    bool is_canvas_export = false;
    bool is_selected_export = false;
    bool is_contour_export = false;
    svg::Document* svg_document_ptr = nullptr;
};

struct SizePositionData
{
    // Размер и положение отображаемой части рисунка в области исходного изображения
    wxRect source_draw_part;
    // Размер и положение холста в области (на поверхности) исходного изображения
    wxRect source_canvas;
};

enum class AdditionalScaleModeType
{
    ADDITIONAL_SCALE_NONE = 0,
    ADDITIONAL_SCALE_RUSSIAN_INCH,
    ADDITIONAL_SCALE_ANY
};

struct OptionsData
{
    // Параметры для загрузки файла апертур (вспышек)
    wxString aperture_filename; // Полное имя нужного файла апертур
    bool is_aperture_gerber_laser = false; // == true, если следует использовать апертуры типа GERBER LASER
    // Флаг необходимости использования корректирующих подмасштабов при рисовании на экране
    bool is_use_screen_subscale = false;
    // Используемый вид единиц измерения размеров (только для целей ввода-вывода размеров)
    MeasureUnitTypeData measure_unit_type = MeasureUnitTypeData::MEASURE_UNIT_DBU;
    // Параметры режима "дополнительный масштаб изображения"
    AdditionalScaleModeType additional_scale_mode = AdditionalScaleModeType::ADDITIONAL_SCALE_NONE;
    double additional_scale_x = 1;
    double additional_scale_y = 1;
    wxString picture_filename; // Имя рисунка для загрузки, переданного через командную строку
};

enum WhatStatusType
{
    SET_STATUS_CURSOR_INFO = 1,
    SET_STATUS_PICTURE_INFO = 2,
    SET_STATUS_VIEW_AREA_INFO = 4,
    SET_STATUS_WORKSCALE_INFO = 8
};

enum class SelectOpType
{
    SELECT_OP_SELECT = 1,
    SELECT_OP_UNSELECT,
    SELECT_OP_INVERT
};

struct LanguageDescriptType
{
    wxLanguage language_identifier;
    wxString language_name;
    std::filesystem::path language_path;
    std::filesystem::path mo_file_path;
    long menu_item_id;
    wxMenuItem* menu_item_ptr = nullptr;
};

class PCADViewerFrame;
class MyPrintout : public wxPrintout
{
public:
    MyPrintout(PCADViewerFrame* viewer_frame_ptr, GraphExportContext& export_context,
               const wxString& title, bool is_preview):
        wxPrintout(title), viewer_frame_ptr_(viewer_frame_ptr),
        export_context_(export_context), title_(title), is_preview_(is_preview)
    {}

    virtual ~MyPrintout()
    {}

    virtual void GetPageInfo(int* min_page, int* max_page, int* page_from, int* page_to) override
    {
        *min_page = 1;
        *max_page = 1;
        *page_from = 1;
        *page_to = 1;
    }

    virtual bool HasPage(int page_num) override
    {
        return page_num == 1;
    }

    virtual bool IsPreview() const override
    {
        return is_preview_;
    }

    virtual wxString GetTitle()	const override
    {
        return title_;
    }

    virtual bool OnPrintPage(int page_num) override;

private:
    PCADViewerFrame* viewer_frame_ptr_;
    GraphExportContext export_context_;
    wxString title_;
    bool is_preview_;
};

class PCADViewerApp : public wxApp
{
    public:
        // Глобальные данные программы, описывающие её состояние
        int return_code = 0;
        wxFontData font_data;
        PCADFileFactory file_factory;
        // Значения параметров настройки программы (которые можно указать в диалоге настройки)
        OptionsData options_data;
        // Единый диспетчер апетур, который будет использоваться всеми просматриваемыми файлами
        aperture::ApertureProvider aperture_provider;
        // Загруженный просматриваемый PCAD-файл
        PCADFile pcad_file = PCADFile(aperture_provider);
        // Пределы допустимых масштабов, которые можно установить полосами прокрутки
        double min_scale_x, max_scale_x;
        double min_scale_y, max_scale_y;
        bool is_equal_scales = false;
        // Текущие рабочие масштабы вывода изображения
        double current_x_scale = 1, current_y_scale = 1;
        // Текущие корректирующие подмасштабы для вывода изображения на экран
        double current_x_screen_subscale = 1, current_y_screen_subscale = 1;
        // Контекст, который использовался для последнего обновления просматриваемого изображения
        DrawContext last_draw_context;
        // Текущее состояние выделяющего контура
        SelectContourData select_contour;
        // Структура и массив, описывающие внутреннее содержание пункта меню "Загрузить как..."
        struct MenuLoadAsItemDesc
        {
            std::string menu_item_name;
            wxMenuItem* menu_item_ptr = nullptr;
            long menu_item_id = 0;
            FileWorkshop* file_workshop_ptr = nullptr;
        };
        std::vector<MenuLoadAsItemDesc> menu_load_as_desc;
        // Актуальное состояние холста-подложки
        CanvasContext canvas_context;
        // Состояние диалога редактирования переменного размера подложки
        VaryCanvasEditorStatus vary_canvas_status;
        std::vector<VaryCanvasEditorStatus> vary_canvas_list;
        std::vector<MenuLoadAsItemDesc> menu_vary_canvas_select_desc;
        // Массив поддерживаемых (русский плюс найденные при сканировании) программой языков
        std::vector<LanguageDescriptType> languages_descs;
        wxHtmlHelpController HtmlHelp;
        // Указатель на используемую нами локаль
        wxLocale* m_locale = nullptr;

        inline double GetEffScaleX()
        {
            return current_x_scale * current_x_screen_subscale *
                (options_data.additional_scale_mode != AdditionalScaleModeType::ADDITIONAL_SCALE_NONE ?
                 options_data.additional_scale_x : 1.0);
        }

        inline double GetEffScaleY()
        {
            return current_y_scale * current_y_screen_subscale *
                (options_data.additional_scale_mode != AdditionalScaleModeType::ADDITIONAL_SCALE_NONE ?
                 options_data.additional_scale_y : 1.0);
        }

        virtual void GetInstalledLanguages();
        void RecreateGUI();
        virtual bool OnInit();
        virtual void OnInitCmdLine(wxCmdLineParser& parser);
        virtual bool OnCmdLineParsed(wxCmdLineParser& parser);
};
